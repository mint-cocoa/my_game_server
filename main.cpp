#include "ServerCore/SocketUtil.h"
#include "ServerCore/SocketAddressFactory.h"



int main() {
    // Create UDP sockets for sending and receiving
    UDPSocketPtr udpSendSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
    UDPSocketPtr udpReceiveSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

    if (!udpSendSocket || !udpReceiveSocket) {
        std::cerr << "Failed to create UDP sockets." << std::endl;
        return 1;
    }

    // Bind the receive socket to the local address
    auto bindAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
    if (!bindAddress) {
        std::cerr << "Failed to create bind address." << std::endl;
        return 1;
    }

    if (udpReceiveSocket->BindSocket(*bindAddress) != NO_ERROR) {
        std::cerr << "Failed to bind receive socket." << std::endl;
        return 1;
    }

    udpReceiveSocket->SetNonBlockingMode(true);

    // Send data to the bound address
    const char* sendData = "Hello, UDP!";
    auto sendAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
    if (!sendAddress) {
        std::cerr << "Failed to create send address." << std::endl;
        return 1;
    }

    int bytesSent = udpSendSocket->SendTo(sendData, strlen(sendData), *sendAddress);
    if (bytesSent < 0) {
        std::cerr << "Failed to send data." << std::endl;
        return 1;
    }
    
    char *receiveBuffer = new char[1024];
    while(1){
    int bytesReceived = udpReceiveSocket->ReceiveFrom(receiveBuffer, sizeof(receiveBuffer) - 1, *bindAddress);
                                                          i
    if (bytesReceived > 0) {
        receiveBuffer[bytesReceived] = '\0';
        std::cout << "Received: " << receiveBuffer << std::endl;
    } else {
        std::cerr << "No data received." << std::endl;
    }
}
    return 0;
}
 