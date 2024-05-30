#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <fcntl.h>
#include <iostream>
#include "SocketAddress.h"

class UDPSocket;
using UDPSocketPtr = std::shared_ptr<UDPSocket>;

class UDPSocket {
public:
    ~UDPSocket();

    int BindSocket(const SocketAddress &inToAddress);
    int SendTo(const void *data, int length, const SocketAddress &address);
    int ReceiveFrom(char *buffer, int length, SocketAddress &address);
    bool SetNonBlockingMode(bool shouldBeNonBlocking);

private:
    friend class SocketUtil;
    explicit UDPSocket(int inSocket) : mSocket(inSocket) {}

    int mSocket;
};

