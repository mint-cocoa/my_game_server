#pragma once
#include <memory>
#include <vector>
#include "../Main/include/Types.h"
class SendBufferChunk;
class SendBuffer;
class SendBufferManager;

using SendBufferChunkRef = std::shared_ptr<SendBufferChunk>;
using SendBufferRef = std::shared_ptr<SendBuffer>;

class SendBuffer
{
public:
    SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32_t allocSize);
    ~SendBuffer();

    BYTE*		Buffer() { return _buffer; }
    uint32_t 	AllocSize() { return _allocSize; }
    uint32_t	WriteSize() { return _writeSize; }
    void		Close(uint32_t writeSize);

private:
    BYTE*				_buffer;
    uint32_t			_allocSize = 0;
    uint32_t			_writeSize = 0;
    SendBufferChunkRef	_owner;
};

/*--------------------
	SendBufferChunk
--------------------*/

class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
    enum
    {
        SEND_BUFFER_CHUNK_SIZE = 6000
    };

public:
    SendBufferChunk();
    ~SendBufferChunk();

    void				Reset();
    SendBufferRef		Open(uint32_t allocSize);
    void				Close(uint32_t writeSize);

    bool				IsOpen() { return _open; }
    BYTE*				Buffer() { return &_buffer[_usedSize]; }
    uint32_t			FreeSize() { return static_cast<uint32_t>(_buffer.size()) - _usedSize; }

private:
    std::array<BYTE, SEND_BUFFER_CHUNK_SIZE>    _buffer = {};
    bool									    _open = false;
    uint32_t									_usedSize = 0;
};

/*---------------------
	SendBufferManager
----------------------*/

class SendBufferManager
{
public:
    SendBufferRef		Open(uint32_t size);

private:
    SendBufferChunkRef	Pop();
    void				Push(SendBufferChunkRef buffer);

    static void			PushGlobal(SendBufferChunk* buffer);

private:

    std::vector<SendBufferChunkRef> _sendBufferChunks;
};
