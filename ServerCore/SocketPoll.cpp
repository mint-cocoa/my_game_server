#include "SocketPoll.h"
#include "SocketUtil.h"
#include <vector>
void SocketPoll::SelectTCPLoop() {
    TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(INET);
    SocketAddress reciveAddress(INADDR_ANY, 48000);

    if (listenSocket->Bind(reciveAddress) != NO_ERROR) {
        return;
    }
    
    std::vector <TCPSocketPtr> readBlock;
    readBlock.push_back(listenSocket);
    std::vector<TCPSocketPtr> 

}