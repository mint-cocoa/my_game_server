#pragma once
#include <memory>
#include "SocketUtil.h"
#include "SocketAddress.h"
#include <sys/epoll.h>

class Session;

using SessionPtr = std::shared_ptr<Session>;
class Session {
public:
    Session(int epollFD, TCPSocketPtr& clientSocket);
    virtual ~Session();
    void Start();
    void HandleEvent(uint32_t events);
private:
    void OnRead();
    void OnWrite();

    int _epollFD;
    TCPSocketPtr _clientSocket;
    char _buffer[1024];
};