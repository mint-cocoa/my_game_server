#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <string>
#include "SocketUtil.h"
#include "SocketAddressFactory.h"
class EpollTcpServer {
public:
    EpollTcpServer(int port, int maxEvents = 10, int maxClients = 10, SocketAddressPtr serverAddr = nullptr);

    EpollTcpServer(int port, int maxEvents, int maxClients, auto ServerAddr);

    ~EpollTcpServer();
    void start();

private:
    int m_serverFd;
    int m_epollFd;
    TCPSocketPtr ListenSocket;
    SocketAddressPtr m_serverAddr;
    int m_maxEvents;
    int m_maxClients;
    std::vector<struct epoll_event> m_events;

    void handleConnection();
    static void handleClient(int clientFd); // static으로 변경
};

#endif // EPOLLTCPSERVER_H
