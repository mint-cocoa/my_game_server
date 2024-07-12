#include "include/Session.h"

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
    _socket = SocketUtil::CreateTCPSocket(INET);
}

Session::~Session()
{
    _socket.close(); // not implemented
}

void Session::Send(SendBufferRef sendBuffer)
{
    if (IsConnected() == false)
        return;

    bool registerSend = false;

    {
        WRITE_LOCK;

        _sendQueue.push(sendBuffer);

        if (_sendRegistered.exchange(true) == false)
            registerSend = true;
    }
    
    if (registerSend)
        RegisterSend();
}

bool Session::Connect()
{
    return RegisterConnect();
}

void Session::Disconnect(const char* cause)
{
    if (_connected.exchange(false) == false)
        return;

    cout << "Disconnect : " << cause << endl;

    RegisterDisconnect();
}

void Session::Dispatch(UringObject* event, int32_t bufferId)
{
    switch (event->type)
    {
    case ContextType::Connect:
        ProcessConnect();

        break;
    case ContextType::Disconnect:
        ProcessDisconnect();
        break;
    case ContextType::Read:
        ProcessRecv(bufferId);
        break;
    case ContextType::Write:
        ProcessSend(bufferId);
        break;
    default:
        break;
    }
}

bool Session::RegisterConnect()
{
    if (IsConnected())
        return false;

    if (GetService()->GetServiceType() != ServiceType::Client)
        return false;

    io_uring_sqe* sqe = io_uring_get_sqe(_ring);
    io_uring_prep_connect(sqe, _socket, &GetService()->GetNetAddress().GetSockAddr(), sizeof(sockaddr_in));
    UringObject* ctx = new UringObject();
    ctx->type = ContextType::Connect;
    ctx->session = shared_from_this();
    io_uring_sqe_set_data(sqe, ctx);

    return true;
}

bool Session::RegisterDisconnect()
{
    io_uring_sqe* sqe = io_uring_get_sqe(_ring);
    io_uring_prep_close(sqe, _socket);
    UringObject* ctx = new UringObject();
    ctx->type = ContextType::Disconnect;
    ctx->session = shared_from_this();
    io_uring_sqe_set_data(sqe, ctx);

    return true;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false)
        return;

    io_uring_sqe* sqe = io_uring_get_sqe(_ring);
    io_uring_prep_recv(sqe, _socket, _recvBuffer.WritePos(), _recvBuffer.FreeSize(), 0);
    UringObject* ctx = new UringObject();
    ctx->type = ContextType::Read;
    ctx->session = shared_from_this();
    io_uring_sqe_set_data(sqe, ctx);
}

void Session::RegisterSend()
{
    if (IsConnected() == false)
        return;

    io_uring_sqe* sqe = io_uring_get_sqe(_ring);

    {
        WRITE_LOCK;

        int32 writeSize = 0;
        while (_sendQueue.empty() == false)
        {
            SendBufferRef sendBuffer = _sendQueue.front();
            writeSize += sendBuffer->WriteSize();
            _sendQueue.pop();
        }

        io_uring_prep_send(sqe, _socket, sendBuffer->Buffer(), writeSize, 0);
    }

    UringObject* ctx = new UringObject();
    ctx->type = ContextType::Write;
    ctx->session = shared_from_this();
    io_uring_sqe_set_data(sqe, ctx);
}

void Session::ProcessConnect()
{
    _connected.store(true);

    GetService()->AddSession(GetSessionRef());

    OnConnected();

    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    OnDisconnected();
    GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32_t bufferId)
{
    auto buffferAddr;
    if (numOfBytes == 0)
    {
        Disconnect("Recv 0");
        return;
    }

    if (_recvBuffer.OnWrite(numOfBytes) == false)
    {
        Disconnect("OnWrite Overflow");
        return;
    }

    int32 dataSize = _recvBuffer.DataSize();
    int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
    if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
    {
        Disconnect("OnRead Overflow");
        return;
    }
    
    _recvBuffer.Clean();

    RegisterRecv();
}

void Session::ProcessSend(int32 bufferId)
{
    if (numOfBytes == 0)
    {
        Disconnect("Send 0");
        return;
    }

    OnSend(numOfBytes);

    WRITE_LOCK;
    if (_sendQueue.empty())
        _sendRegistered.store(false);
    else
        RegisterSend();
}

void Session::HandleError(int32 errorCode)
{
    switch (errorCode)
    {
    case ECONNRESET:
    case ECONNABORTED:
        Disconnect("HandleError");
        break;
    default:
        cout << "Handle Error : " << errorCode << endl;
        break;
    }
}

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
    int32 processLen = 0;

    while (true)
    {
        int32 dataSize = len - processLen;
        if (dataSize < sizeof(PacketHeader))
            break;

        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
        if (dataSize < header.size)
            break;

        OnRecvPacket(&buffer[processLen], header.size);

        processLen += header.size;
    }

    return processLen;
}