#include <sys/socket.h>

class SocketPoll
{
public:
    static void SelectTCPLoop();
    void epollTCPLoop();
};