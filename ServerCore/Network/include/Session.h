#pragma once
#include <memory>
#include "SocketUtil.h"
#include "SocketAddress.h"
#include <sys/epoll.h>

class Session;

using SessionPtr = std::shared_ptr<Session>;
class Session {
public:
    Session();
    virtual ~Session();


private:
    void OnRead();
    void OnWrite();

    int _epollFD;
    TCPSocketPtr _clientSocket;
    char _buffer[1024];
};