
#include <memory>
#include <pcap/socket.h>
class TCPSocket;
using TCPSocketPtr = std::shared_ptr<TCPSocket>;
class TCPSocket {
public:
    ~TCPSocket();

    int Connect(const SocketAddress& inAddress);
    int Bind(const SocketAddress& inToAddress);
    int Listen(int inBacklog = 32);
    TCPSocketPtr Accept(SocketAddress& inFromAddress);
    int Send(const void* inData, int inLen);
    int Receive(void* inBuffer, int inLen);
    bool SetNonBlockingMode(bool shouldBeNonBlocking);

private:
    friend class SocketUtil;
    explicit TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
    SOCKET mSocket;
};

