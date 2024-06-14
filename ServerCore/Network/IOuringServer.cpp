#include "include/IOuringServer.h"

IOuringServer::IOuringServer(SocketAddress &address) {
    // io_uring 초기화
    if (io_uring_queue_init(32, &_ring, 0) < 0) {
        std::cerr << "Failed to initialize io_uring" << std::endl;
        return;
    }

    // 소켓 생성
    _serverSocket = SocketUtil::CreateTCPSocket(INET);
    if (!_serverSocket) {
        std::cerr << "Failed to create TCP socket" << std::endl;
        return;
    }

    // 소켓 바인딩
    if (!_serverSocket->Bind(address)) {
        std::cerr << "Failed to bind socket" << std::endl;
        return;
    }

    // 소켓 리슨
    if (!_serverSocket->Listen()) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return;
    }

    // SQE 준비
    sqe = io_uring_get_sqe(&_ring);
    if (!sqe) {
        std::cerr << "Failed to get SQE for accept" << std::endl;
        return;
    }

    socklen_t addressLen = address.GetSize();
    io_uring_prep_accept(sqe,
                         _serverSocket->getFD(),
                         (struct sockaddr *) &address.mSockAddr,
                         &addressLen,
                         0);

    io_uring_sqe_set_data(sqe, &_serverSocket);
    io_uring_submit(&_ring);
}

IOuringServer::~IOuringServer() {
    io_uring_queue_exit(&_ring);
}

bool IOuringServer::start() {
    while (true) {
        int ret = io_uring_wait_cqe(&_ring, &cqe);

        if (ret < 0) {
            std::cerr << "Error in io_uring_wait_cqe" << std::endl;
            return false;
        }

        TCPSocketPtr socketPtr = *(TCPSocketPtr*)cqe->user_data;

        if (cqe->res < 0) {
            std::cerr << "Error in io_uring_wait_cqe" << std::endl;
            return false;
        }

        if (socketPtr == _serverSocket) {
            TCPSocketPtr newSocket = SocketUtil::CreateTCPSocket(INET);
            if (newSocket) {
                std::cout << "Accepted new connection" << std::endl;
                sqe = io_uring_get_sqe(&_ring);
                if (!sqe) {
                    std::cerr << "Failed to get SQE for accept" << std::endl;
                    return false;
                }
                SocketAddress newClientAddr(INADDR_ANY, 48000);
                socklen_t addressLen = newClientAddr.GetSize();
                io_uring_prep_accept(sqe,
                                     _serverSocket->getFD(),
                                     (struct sockaddr *) &newClientAddr.mSockAddr,
                                     &addressLen,
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