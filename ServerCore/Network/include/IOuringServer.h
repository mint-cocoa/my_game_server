#pragma once
#include <vector>
#include <memory>
#include "SocketUtil.h"
#include "SocketAddress.h"
#include "Session.h"
#include <liburing.h>

#define QUEUE_DEPTH 256
#define BUFFER_SIZE 8192
#define EVENT_TYPE_ACCEPT       0
#define EVENT_TYPE_READ         1
#define EVENT_TYPE_WRITE        2


class IOuringServer {

public:
    IOuringServer(SocketAddress &address);
    ~IOuringServer();
    
    bool start();   

private:
    void add_accept_request();

    struct io_uring _ring;
    io_uring_sqe *sqe;
    io_uring_cqe *cqe;
    TCPSocketPtr _serverSocket;

};
