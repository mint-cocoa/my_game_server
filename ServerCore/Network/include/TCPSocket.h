#pragma once
class TCPSocket;
using TCPSocketPtr = std::shared_ptr<TCPSocket>;
class TCPSocket {
public:
    TCPSocket(TCPSocketPtr sharedPtr);

    ~TCPSocket();

    int Connect(const SocketAddress& inAddress);
    int Bind(const SocketAddress& inToAddress);
    int Listen(int inBacklog = 32);
    TCPSocketPtr Accept(SocketAddress& inFromAddress);
    int Send(const void* inData, int inLen);
    int Receive(void* inBuffer, int inLen);
    bool SetNonBlockingMode(bool shouldBeNonBlocking);
    int getFD() const;
private:
    friend class SocketUtil;
    explicit TCPSocket(int inSocket) : mSocket(inSocket) {}
    int mSocket;
};

