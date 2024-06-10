#include "include/Session.h"
#include <unistd.h>
#include <iostream>

Session::Session(int epollFD, TCPSocketPtr clientSocket)
        : _epollFD(epollFD), _clientSocket(clientSocket) {}

void Session::Start() {
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = this;
    if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _clientSocket->getFD(), &event) == -1) {
        std::cerr << "Failed to add client socket to epoll instance." << std::endl;
    }
}

void Session::HandleEvent(uint32_t events) {
    if (events & EPOLLIN) {
        OnRead();
    }
    if (events & EPOLLOUT) {
        OnWrite();
    }
}

void Session::OnRead() {
    int bytesRead = read(_clientSocket->getFD(), _buffer, sizeof(_buffer));
    if (bytesRead <= 0) {
        if (bytesRead == 0 || (bytesRead == -1 && errno != EAGAIN)) {
            close(_clientSocket->getFD());
            epoll_ctl(_epollFD, EPOLL_CTL_DEL, _clientSocket->getFD(), nullptr);
        }
        return;
    }
    // Process data...
    std::cout << "Received data: " << std::string(_buffer, bytesRead) << std::endl;
}

void Session::OnWrite() {

}