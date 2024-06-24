#include "include/UringBuffer.h"
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <error.h>
#include "include/UringCore.h"

constexpr int EXIT_ERROR = 1;
constexpr int BUFFER_GROUP_ID = 1;

UringBuffer::UringBuffer(io_uring& ring)
    : ring_(ring), buf_ring_(nullptr), io_buffers_base_addr_(nullptr), buf_ring_size_(0) {
    constexpr size_t ring_size = (IOuringServer::IO_BUFFER_SIZE + sizeof(io_uring_buf)) * IOuringServer::NUM_IO_BUFFERS;
    buf_ring_size_ = ring_size;

    SendBufferManager*
    void* ring_addr =
    if (ring_addr == MAP_FAILED) {
        error(EXIT_ERROR, 0, "mmap ring");
    }

    io_uring_buf_reg reg{};
    memset(&reg, 0, sizeof(reg));
    reg.ring_addr = reinterpret_cast<__u64>(ring_addr);
    reg.ring_entries = IOuringServer::NUM_IO_BUFFERS;
    reg.bgid = BUFFER_GROUP_ID;

    const unsigned flags = 0;
    const int register_buf_ring_result = io_uring_register_buf_ring(&ring_, &reg, flags);
    if (register_buf_ring_result != 0) {
        error(EXIT_ERROR, -register_buf_ring_result, "io_uring_register_buf_ring");
    }

    buf_ring_ = reinterpret_cast<io_uring_buf_ring*>(ring_addr);
    io_uring_buf_ring_init(buf_ring_);

    io_buffers_base_addr_ = (uint8_t*)ring_addr + (sizeof(io_uring_buf) * IOuringServer::NUM_IO_BUFFERS);

    for (uint16_t buffer_idx = 0u; buffer_idx < IOuringServer::NUM_IO_BUFFERS; ++buffer_idx) {
        io_uring_buf_ring_add(buf_ring_, get_buffer_addr(buffer_idx),
                              IOuringServer::IO_BUFFER_SIZE, buffer_idx,
                              io_uring_buf_ring_mask(IOuringServer::NUM_IO_BUFFERS),
                              buffer_idx);
    }

    io_uring_buf_ring_advance(buf_ring_, IOuringServer::NUM_IO_BUFFERS);
}

UringBuffer::~UringBuffer() {
    if (buf_ring_ != nullptr) {
        munmap(buf_ring_, buf_ring_size_);
    }
}

uint8_t* UringBuffer::get_base_addr() const {
    return io_buffers_base_addr_;
}

uint8_t* UringBuffer::get_buffer_addr(uint16_t idx) const {
    return io_buffers_base_addr_ + (idx << log2<IOuringServer::IO_BUFFER_SIZE>());
}

void UringBuffer::recycle_buffer(uint16_t idx) {
    io_uring_buf_ring_add(buf_ring_, get_buffer_addr(idx), IOuringServer::IO_BUFFER_SIZE, idx,
                          io_uring_buf_ring_mask(IOuringServer::NUM_IO_BUFFERS), 0);
    io_uring_buf_ring_advance(buf_ring_, 1);
}
