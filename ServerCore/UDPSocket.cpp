#include "UDPSocket.h"
#include "SocketUtil.h"
UDPSocket::~UDPSocket() {
    shutdown(mSocket, SHUT_RDWR);
}

int UDPSocket::BindSocket(const SocketAddress &inToAddress) {
    int err = bind(mSocket, &inToAddress.mSockAddr, inToAddress.GetSize());
    if (err == 0) {
        return err;
    }
    return -1;
}

int UDPSocket::SendTo(const void *data, int length, const SocketAddress &address) {
    int byteSentCount = sendto(
            mSocket,
            static_cast<const char *>(data),
            length,
            0,
            &address.mSockAddr,
            address.GetSize());
    if (byteSentCount >= 0) {
        return byteSentCount;
    }
    return -1;
}

int UDPSocket::ReceiveFrom(char *buffer, int length, SocketAddress &address) {
    socklen_t fromLength = address.GetSize();
    int byteReceivedCount = recvfrom(
            mSocket,
            static_cast<char *>(buffer),
            length,
            0,
            &address.mSockAddr,
            &fromLength);
    if (byteReceivedCount >= 0) {
        return byteReceivedCount;
    }
    return -1;
}

bool UDPSocket::SetNonBlockingMode(bool shouldBeNonBlocking) {
    int flags = fcntl(mSocket, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags." << std::endl;
        return false;
    }
    flags = shouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    if (fcntl(mSocket, F_SETFL, flags) == -1) {
        std::cerr << "Failed to set non-blocking mode." << std::endl;
        return false;
    }
    return true;
}
