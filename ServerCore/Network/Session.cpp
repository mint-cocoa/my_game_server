#include "include/Session.h"
#include <unistd.h>
#include <iostream>

Session::Session(int epollFD, TCPSocketPtr& clientSocket)
        : _epollFD(epollFD), _clientSocket(clientSocket) {}

void Session::Start() {
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = this;
    if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _clientSocket->getFD(), &event) == -1) {
        std::cerr << "Failed to add client socket to epoll instance." << std::endl;
    }
}

void add_accept(int listening_socket) {
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        std::cerr << "Unable to get submission queue entry" << std::endl;
        exit(1);
    }

    io_uring_prep_multishot_accept(sqe, listening_socket, (sockaddr*)&client_addr, &client_addr_len, 0);
    io_uring_sqe_set_data(sqe, (void*)(intptr_t)listening_socket);
}

void Session::Dispatch(UringObject* event, int numOfBytes) {
    switch (event->type) {
        case ContextType::Accept:
            HandleAccept(event);
            break;
        case ContextType::Close:
            // No-op
            LOG_INFO("Closed: %d\n", ctx.client_fd);
            break;
        case ContextType::Read:
            HandleRead(cqe, ctx.client_fd);
            break;
        case ContextType::Write:
            HandleWrite(ctx);
            break;
        default:
            error(EXIT_ERROR, 0, "context type not handled: %d", static_cast<int>(ctx.type));
            break;
    }
}

void Session::HandleAccept(UringObject* event) {
    int clientFD = event->clientFD;
    if (clientFD == -1) {
        std::cerr << "Failed to accept client connection." << std::endl;
        return;
    }

    // Create a new session for the client
    TCPSocketPtr clientSocket = std::make_shared<TCPSocket>(clientFD);
    SessionPtr session = std::make_shared<Session>(_epollFD, clientSocket);
    session->Start();
}

void Session::OnWrite() {

}