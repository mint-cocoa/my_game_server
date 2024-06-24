#ifndef URING_ECHO_SERVER_H
#define URING_ECHO_SERVER_H

#include <liburing.h>
#include <netinet/in.h>
#include <cstdint>
#include "SocketUtil.h"
#include "UringBuffer.h"
#include "SendBuffer.h"



enum ContextType : uint8_t { Accept, Close, Read, Write };
struct UringObject{
public:
    int32_t client_fd;
    ContextType type;

};

class UringCore {
public:
    UringCore();
    ~UringCore();



    static constexpr unsigned NUM_SUBMISSION_QUEUE_ENTRIES = 1024;
    static constexpr unsigned NUM_WAIT_ENTRIES = 1;
    static constexpr unsigned CQE_BATCH_SIZE = 16;


    void Register(UringObject obj);
    void Dispatch();
    
private:

    io_uring_sqe* sqe_;
    std::vector<io_uring_cqe*> cqes;
    io_uring uring_;
};

#endif // URING_ECHO_SERVER_H
