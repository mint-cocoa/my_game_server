#include "include/SocketUtil.h"

TCPSocket::~TCPSocket() {
    shutdown(mSocket, SHUT_RDWR);
}

int TCPSocket::Connect(const SocketAddress& inAddress) {
    int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());
    if (err >= 0) {
        return NO_ERROR;
    }
    SocketUtil::ReportError("TCPSocket::Connect");
    return -SocketUtil::GetLastError();
}

int TCPSocket::Bind(const SocketAddress& inToAddress) {
    int err = bind(mSocket, &inToAddress.mSockAddr, inToAddress.GetSize());
    if (err >= 0) {
        return shared_from_this();
    }
    SocketUtil::ReportError("TCPSocket::Bind");
    return -SocketUtil::GetLastError();
}

int TCPSocket::Listen(int inBacklog) {
    int err = listen(mSocket, inBacklog);
    if (err >= 0) {
        return NO_ERROR;
    }
    SocketUtil::ReportError("TCPSocket::Listen");
    return -SocketUtil::GetLastError();
}

TCPSocketPtr TCPSocket::Accept(SocketAddress& inFromAddress) {
    socklen_t length = inFromAddress.GetSize();
    int newSocket = accept(mSocket, &inFromAddress.mSockAddr, &length);
    if (newSocket != -1) {
        return TCPSocketPtr(new TCPSocket(newSocket));
    }

    SocketUtil::ReportError("TCPSocket::Accept");
    return nullptr;
}

int TCPSocket::Send(const void* inData, int inLen) {
    int bytesSent = send(mSocket, static_cast<const char*>(inData), inLen, 0);
    if (bytesSent >= 0) {
        return bytesSent;
    }
    SocketUtil::ReportError("TCPSocket::Send");
    return -SocketUtil::GetLastError();
}

int TCPSocket::Receive(void* inBuffer, int inLen) {
    int bytesReceived = recv(mSocket, static_cast<char*>(inBuffer), inLen, 0);
    if (bytesReceived >= 0) {
        return bytesReceived;
    }
    SocketUtil::ReportError("TCPSocket::Receive");
    return -SocketUtil::GetLastError();


}

bool TCPSocket::SetNonBlockingMode(bool shouldBeNonBlocking) {
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
