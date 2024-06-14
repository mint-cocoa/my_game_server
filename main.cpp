#include "ServerCore/Network/include/SocketUtil.h"
#include "ServerCore/Network/include/EPollServer.h"

int main() {
    // Bind the receive socket to the local address
    SocketAddressPtr  bindAddress = SocketUtil::CreateIPv4FromString("127.0.0.1:8080");
    if (!bindAddress) {
        std::cerr << "Failed to create bind address." << std::endl;
        return 1;
    }

    EpollTcpServer server(*bindAddress);

    server.start();
    return 0;
}
 