#ifndef URING_ECHO_SERVER_H
#define URING_ECHO_SERVER_H

#include <liburing.h>
#include <netinet/in.h>
#include <cstdint>
#include "SocketUtil.h"
#include "SendBuffer.h"

#pragma once

/*----------------
	IocpObject
-----------------*/
class UringObject;
using UringObjectPtr  = std::shared_ptr<UringObject>;
class UringObject
{
public:
    virtual io_uring GetUring() abstract;
    virtual void Dispatch(class UringEvent* uringEvent, int32_t bufferId) abstract;
};



class UringCore {
public:
    UringCore();
    ~UringCore();

    static constexpr unsigned NUM_SUBMISSION_QUEUE_ENTRIES = 1024;
    static constexpr unsigned NUM_WAIT_ENTRIES = 1;
    static constexpr unsigned CQE_BATCH_SIZE = 16;

    bool		Register(UringObjectPtr uringObject);
    bool		Dispatch(uint32_t timeoutMs = INFINITE);


    
private:

    io_uring uring_;
};

#endif // URING_ECHO_SERVER_H
