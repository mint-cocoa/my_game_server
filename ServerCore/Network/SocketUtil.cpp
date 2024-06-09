#include "include/SocketUtil.h"

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily inFamily) {
    int s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
    if (s != -1)
    {
        return UDPSocketPtr(new UDPSocket(s));
    }
    // 오류 처리
}

TCPSocketPtr SocketUtil::CreateTCPSocket(SocketAddressFamily inFamily) {
    int s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);
    if (s != -1)
    {
        return TCPSocketPtr(new TCPSocket(s));
    }
    // 오류 처리
}

SocketAddressPtr SocketUtil::CreateIPv4FromString(const std::string& inString) {
    auto pos = inString.find_last_of(':');
    std::string host, service;

    if (pos != std::string::npos) {
        host = inString.substr(0, pos);
        service = inString.substr(pos + 1);
    } else {
        host = inString;
        service = "0";
    }

    addrinfo hint{};
    hint.ai_family = AF_INET;
    addrinfo* result = nullptr;

    int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
    if (error != 0 || result == nullptr) {
        if (result != nullptr) {
            freeaddrinfo(result);
        }
        return nullptr;
    }

    for (addrinfo* res = result; res != nullptr; res = res->ai_next) {
        if (res->ai_addr != nullptr) {
            auto toRet = std::make_shared<SocketAddress>(*res->ai_addr);
            freeaddrinfo(result);
            return toRet;
        }
    }

    freeaddrinfo(result);
    return nullptr;
}

void SocketUtil::ReportError(const char* inOperationDesc) {
    std::cerr << inOperationDesc << " failed: " << strerror(errno) << std::endl;
}



int SocketUtil::GetLastError() {
    return errno;
}


