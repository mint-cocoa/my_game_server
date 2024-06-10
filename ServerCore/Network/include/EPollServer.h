#pragma once
#include <vector>
#include <memory>
#include "SocketUtil.h"
#include "SocketAddress.h"
#include "Session.h"
#include <sys/epoll.h>


class EpollServer {
public:

    EpollServer(int maxEvents, SocketAddress &address);

    void Start();

private:
    int _epollFD;
    int _maxEvents;

    TCPSocketPtr _serverSocket;
    std::vector<epoll_event> _events;
};