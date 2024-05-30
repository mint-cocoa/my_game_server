#pragma once
#include "SocketAddress.h"
#include <string>
#include <netdb.h>

class SocketAddressFactory {
public:
    static SocketAddressPtr CreateIPv4FromString(const std::string& inString) {
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
};

