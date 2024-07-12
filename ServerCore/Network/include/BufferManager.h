#pragma once
#include <liburing.h>
#include <cstdint>
#include <vector>
#include "RecvBuffer.h"
#include "SendBuffer.h"

class BufferManager {
public:
    BufferManager(io_uring& ring, uint32_t bufferSize, uint32_t bufferCount);
    ~BufferManager();

    RecvBuffer* GetRecvBuffer();
    SendBufferRef Open(uint32_t size);

private:
    void InitializeBufferRing();

    io_uring& _ring;
    io_uring_buf_ring* _recvBufRing;
    io_uring_buf_ring* _sendBufRing;
    io_uring_cqe *_cqe;
    io_uring_sqe *_sqe;
    uint8_t* _baseAddr;
    size_t _bufRingSize;

    std::vector<RecvBuffer> _recvBuffers;
    std::vector<SendBufferChunkRef> _sendBufferChunks;

    static constexpr int BUFFER_GROUP_ID = 1;
};