#include "BufferManager.h"
#include <sys/mman.h>
#include <cstring>
#include <stdexcept>

SendBuffer::SendBuffer(io_uring& ring, uint32_t bufferSize, uint32_t bufferCount)
        : _ring(ring), _bufRing(nullptr), _baseAddr(nullptr)
{
    _bufRingSize = (bufferSize + sizeof(io_uring_buf)) * bufferCount;

    InitializeBufferRing();

    _recvBuffers.reserve(bufferCount);
    for (uint32_t i = 0; i < bufferCount; ++i) {
        _recvBuffers.emplace_back(bufferSize);
    }

    _sendBufferChunks.reserve(bufferCount);
    for (uint32_t i = 0; i < bufferCount; ++i) {
        _sendBufferChunks.push_back(std::make_shared<SendBufferChunk>());
    }
}

SendBuffer::~SendBuffer() {
    if (_bufRing != nullptr) {
        munmap(_bufRing, _bufRingSize);
    }
}

void SendBuffer::InitializeBufferRing() {
    void* ringAddr = mmap(nullptr, _bufRingSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ringAddr == MAP_FAILED) {
        throw std::runtime_error("Failed to mmap buffer ring");
    }

    io_uring_buf_reg reg = {};
    reg.ring_addr = reinterpret_cast<__u64>(ringAddr);
    reg.ring_entries = _recvBuffers.size();
    reg.bgid = BUFFER_GROUP_ID;

    if (io_uring_register_buf_ring(&_ring, &reg, 0) != 0) {
        throw std::runtime_error("Failed to register buffer ring");
    }

    _bufRing = reinterpret_cast<io_uring_buf_ring*>(ringAddr);
    io_uring_buf_ring_init(_bufRing);

    _baseAddr = static_cast<uint8_t*>(ringAddr) + (sizeof(io_uring_buf) * _recvBuffers.size());

    for (uint16_t i = 0; i < _recvBuffers.size(); ++i) {
        io_uring_buf_ring_add(_bufRing, _recvBuffers[i].WritePos(), _recvBuffers[i].FreeSize(), i,
                              io_uring_buf_ring_mask(_recvBuffers.size()), i);
    }

    io_uring_buf_ring_advance(_bufRing, _recvBuffers.size());
}

RecvBuffer* SendBuffer::GetRecvBuffer() {
    // 간단한 구현을 위해 첫 번째 버퍼를 반환합니다.
    // 실제 구현에서는 사용 가능한 버퍼를 찾아 반환해야 합니다.
    return &_recvBuffers[0];
}

SendBufferRef SendBuffer::Open(uint32_t size) {
    // 간단한 구현을 위해 첫 번째 청크를 사용합니다.
    // 실제 구현에서는 사용 가능한 청크를 찾아 반환해야 합니다.
    return _sendBufferChunks[0]->Open(size);
}