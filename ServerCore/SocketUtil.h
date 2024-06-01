#include "UDPSocket.h"
#include "TCPSocket.h"
#include <sys/socket.h>
#include <netdb.h>

#define NO_ERROR 0
enum SocketAddressFamily {
    INET = AF_INET,
    INET6 = AF_INET6
};

class SocketUtil {
public:
    static UDPSocketPtr CreateUDPSocket(SocketAddressFamily inFamily) {
        int s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
        if (s != -1)
        {
            return UDPSocketPtr(new UDPSocket(s));
        }
        
    }
    static TCPSocketPtr CreateTCPSocket(SocketAddressFamily inFamily) {
        int  s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);
        if (s != -1)
        {
            return TCPSocketPtr(new TCPSocket(s));
        }
        
    }

    static void ReportError(const char* inOperationDesc) {
        std::cerr << inOperationDesc << " failed: " << strerror(errno) << std::endl;
    }

    static int GetLastError() {
        return errno;
    }
};