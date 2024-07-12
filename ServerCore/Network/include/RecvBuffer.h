#pragma once
#include <liburing.h>
#include <cstdint>

class BufferObject {



}

class BufferRing {
    enum { BUFFER_COUNT = 10 , IO_BUFFER_SIZE = 4096 };
public:
    BufferRing(uint32_t bufferSize);
    ~BufferRing();

    constexpr unsigned buffer_ring_size() {
        return (_bufferSize + sizeof(io_uring_buf_ring)) * BUFFER_COUNT;
    }

    uint8_t* GetBufferRingBaseAddr(void* ring_addr) {
        return (uint8_t*)ring_addr + (sizeof(io_uring_buf) * BUFFER_COUNT);
    }

    constexpr uint8_t* GetBufferRingAddr(uint8_t* base_addr, uint16_t idx) {
        return base_addr + (idx << log2<IO_BUFFER_SIZE>());
    }
    struct io_uring_buf_ring* setup(struct io_uring* ring);

    void BufRingAdvance(){ io_uring_buf_ring_advance(br, 1); }

    int32_t DataSize() { return _writePos - _readPos; }
    int32_t FreeSize() { return _capacity - _writePos; }

private:
    int32_t _capacity = 0;
    int32_t _bufferSize = 0;


    struct io_uring_buf_ring* br;
};
