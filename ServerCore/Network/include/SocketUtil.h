#pragma once

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "SocketAddress.h"
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include "UringCore.h"

#define NO_ERROR 0

enum SocketAddressFamily {
    INET = AF_INET,
    INET6 = AF_INET6
};

class SocketUtil {
public:
    static UDPSocketPtr CreateUDPSocket(SocketAddressFamily inFamily);
    static TCPSocketPtr CreateTCPSocket(SocketAddressFamily inFamily);
    static SocketAddressPtr CreateIPv4FromString(const std::string& inString);
    static void ReportError(const char* inOperationDesc);
    static int GetLastError();
    static IOuringServerPtr CreateIOuringServer();
    
};
        