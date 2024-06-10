#include "include/IOuringServer.h"


IOuringServer::IOuringServer(SocketAddress &address) {
    _serverSocket = SocketUtil::CreateTCPSocket(INET);
    _serverSocket->Bind(address);
    _serverSocket->Listen();
    sqe = io_uring_get_sqe(&_ring);

    if (!sqe) {
        std::cerr << "Failed to get SQE for accept" << std::endl;
        return;
    }
    auto addressLen = reinterpret_cast<socklen_t*>(address.GetSize());
    io_uring_prep_accept(sqe,
                         _serverSocket->getFD(),
                         (struct sockaddr *) &address.mSockAddr,
                         addressLen,
                         0);

    io_uring_sqe_set_data(sqe, &_serverSocket);
    io_uring_submit(&_ring);
}


bool IOuringServer::start() {
    struct io_uring_cqe *cqe;
    while (true) {

       int ret =  io_uring_wait_cqe(&_ring, &cqe);

        if (ret < 0)
        {
            std::cerr << "Error in io_uring_wait_cqe" << std::endl;
            return false;
        }

        TCPSocketPtr socketPtr = *(TCPSocketPtr*)cqe->user_data;

        if (cqe->res < 0) {
            std::cerr << "Error in io_uring_wait_cqe" << std::endl;
            return false;
        }

        if(socketPtr == _serverSocket) {
            TCPSocketPtr newSocket = SocketUtil::CreateTCPSocket(INET);
            if (newSocket) {
                std::cout << "Accepted new connection" << std::endl;
                sqe = io_uring_get_sqe(&_ring);
                if (!sqe) {
                    std::cerr << "Failed to get SQE for accept" << std::endl;
                    return false;
                }
                SocketAddress newClientAddr(INADDR_ANY, 48000);
                auto addressLen = reinterpret_cast<socklen_t*>(newClientAddr.GetSize());
                io_uring_prep_accept(sqe,
                                     _serverSocket->getFD(),
                                     (struct sockaddr *) &newClientAddr.mSockAddr,
                                     addressLen,
                                     0);

                io_uring_sqe_set_data(sqe, &_serverSocket);
                io_uring_submit(&_ring);
            }
        } else {
            std::cout << "Received data from client" << std::endl;

        }

        io_uring_cqe_seen(&_ring, cqe);
    }
}


