#include "SocketPoll.h"
#include <unistd.h>
#include <iostream>
#include "SocketUtil.h"


EpollTcpServer::EpollTcpServer(int port, int maxEvents, int maxClients, auto ServerAddr)
        : m_maxEvents(maxEvents), m_maxClients(maxClients), m_events(maxEvents), m_serverAddr(ServerAddr) {

    // 소켓 생성 및 설정
    ListenSocket = SocketUtil::CreateTCPSocket(INET);

    ListenSocket->Bind(*m_serverAddr);
    ListenSocket->Listen(m_maxClients);

    // epoll 인스턴스 생성 및 서버 소켓 등록
    m_epollFd = epoll_create1(0);
    if (m_epollFd == -1) {
        close(m_serverFd);
        throw std::runtime_error("Failed to create epoll instance.");
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = m_serverFd;
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_serverFd, &event) == -1) {
        close(m_serverFd);
        close(m_epollFd);
        throw std::runtime_error("Failed to add server socket to epoll.");
    }
}

EpollTcpServer::~EpollTcpServer() {
    close(m_epollFd);
    close(m_serverFd);
}

void EpollTcpServer::start() {
    std::cout << "Server started. Listening on port "<< std::endl;

    while (true) {
        int numEvents = epoll_wait(m_epollFd, m_events.data(), m_maxEvents, -1);
        if (numEvents == -1) {
            throw std::runtime_error("epoll_wait failed.");
        }

        for (int i = 0; i < numEvents; ++i) {
            if (m_events[i].data.fd == m_serverFd) {
                handleConnection();
            } else {
                // 클라이언트 데이터 처리를 위한 스레드 생성
                std::thread(handleClient, m_events[i].data.fd).detach();
            }
        }
    }
}

void EpollTcpServer::handleConnection() {
    SocketAddressPtr newClientAddress;
    ListenSocket->Accept(*newClientAddress);
}
void EpollTcpServer::handleClient(int clientFd) { // static 메소드로 변경
    // 클라이언트 데이터 처리 (생략 - 기존 코드와 동일)
}
