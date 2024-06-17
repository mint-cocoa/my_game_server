#include "include/EPollServer.h"
#include <unistd.h>
#include <iostream>

EpollServer::EpollServer(int maxEvents, SocketAddress& address) : _maxEvents(maxEvents), _events(maxEvents) {
    _serverSocket = SocketUtil::CreateTCPSocket(INET);
    int serverFD = _serverSocket->getFD();
    _epollFD = epoll_create1(0);
    if (_epollFD == -1) {
        std::cerr << "Failed to create epoll instance." << std::endl;
        close(serverFD);
    }

    epoll_event event;
    event.events = EPOLLIN;
    event.data.ptr = &_serverSocket;

    SocketAddress receivingAddress(INADDR_ANY, 48000); // bind to all interfaces
    _serverSocket->Bind(receivingAddress);
    _serverSocket->Listen();

    if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, serverFD, &event) == -1) {
        std::cerr << "Failed to add server socket to epoll instance." << std::endl;
        close(serverFD);
        close(_epollFD);
    }
}

void EpollServer::Start() {
    while (true) {
        int numEvents = epoll_wait(_epollFD, _events.data(), _maxEvents, -1);
        if (numEvents == -1) {
            throw std::runtime_error("epoll_wait failed.");
        }

        for (int i = 0; i < numEvents; ++i) {
            if (_events[i].data.ptr == &_serverSocket) {
                // new client connection request
                SocketAddress newClientAddr(INADDR_ANY, 48000);
                auto newSocket = _serverSocket->Accept(newClientAddr);
                int newSocketFD = newSocket->getFD( );

            } else {
                // handle client data
                auto socket = static_cast<TCPSocket*>(_events[i].data.ptr); // cast to TCPSocket
                char buffer[1024];
                int bytesRead = socket->Receive(buffer, 1024);
                if (bytesRead == 0) {
                    // client disconnected
                    close(socket->getFD());
                    epoll_ctl(_epollFD, EPOLL_CTL_DEL, socket->getFD(), nullptr);
                } else {
                    // echo data back to client
                    socket->Send(buffer, bytesRead);
                }

            }
        }
    }
}