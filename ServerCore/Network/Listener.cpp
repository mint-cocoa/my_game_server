#include "Listener.h"
#include "SocketUtil.h"
#include "AcceptEvent.h"
#include "Session.h"
#include "Service.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <liburing.h>

Listener::~Listener()
{
    CloseSocket();

    for (AcceptEvent* acceptEvent : _acceptEvents)
    {
        delete acceptEvent;
    }

    if (_ring)
    {
        io_uring_queue_exit(_ring);
        delete _ring;
    }
}

bool Listener::StartAccept(ServerServiceRef service)
{
    _service = service;
    if (_service == nullptr)
        return false;

    _socket = SocketUtil::CreateTCPSocket();
    if (_socket == nullptr)
        return false;

    if (_socket->Bind(_service->GetNetAddress()) == false)
        return false;

    if (_socket->Listen() == false)
        return false;

    _ring = new struct io_uring;
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));

    if (io_uring_queue_init_params(256, _ring, &params) < 0)
        return false;

    const int32 acceptCount = _service->GetMaxSessionCount();
    for (int32 i = 0; i < acceptCount; i++)
    {
        AcceptEvent* acceptEvent = new AcceptEvent();
        acceptEvent->owner = shared_from_this();
        _acceptEvents.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }

    return true;
}

void Listener::CloseSocket()
{
    if (_socket != -1)
    {
        close(_socket);
        _socket = -1;
    }
}

void Listener::Dispatch(struct io_uring_cqe* cqe)
{
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(io_uring_cqe_get_data(cqe));
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    SessionRef session = _service->CreateSession();

    acceptEvent->Init();
    acceptEvent->session = session;

    struct io_uring_sqe* sqe = io_uring_get_sqe(_ring);
    io_uring_sqe_set_data(sqe, acceptEvent);
    io_uring_prep_accept(sqe, _socket->getFD(), nullptr, nullptr, 0);
    io_uring_submit(_ring);
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    SessionRef session = acceptEvent->session;

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket < 0)
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetSocket(clientSocket);
    session->SetNetAddress(NetAddress(clientAddr));
    session->ProcessConnect();

    RegisterAccept(acceptEvent);
}