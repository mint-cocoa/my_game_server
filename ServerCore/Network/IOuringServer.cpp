#include "UringCore.h"
#include "UringBuffer.h"
#include "SocketUtil.h"
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <error.h>
#include <iostream>

constexpr int EXIT_ERROR = 1;
constexpr int ERROR = -1;

UringCore::UringCore()
        : buffer_manager(1024, 4096)  // 예시 값, 실제 요구사항에 맞게 조정 필요
{
    listening_socket_ = SocketUtil::CreateTCPSocket(INET);
    init_io_uring(&ring_, NUM_SUBMISSION_QUEUE_ENTRIES);
    ring_initialized_ = true;
}

UringCore::~UringCore() {
    if (ring_initialized_) {
        io_uring_queue_exit(&ring_);
    }
}

void UringCore::Dispatch() {
    std::vector<io_uring_cqe*> cqe_array(CQE_BATCH_SIZE);

    const unsigned num_cqes = io_uring_peek_batch_cqe(&uring_, cqes.data(), CQE_BATCH_SIZE);
    for (unsigned cqe_idx = 0; cqe_idx < num_cqes; ++cqe_idx) {
        io_uring_cqe* cqe = cqe_array[cqe_idx];

        const auto result = cqe->res;
        const auto ctx = get_context(cqe);

        switch (ctx.type) {
            case ContextType::Accept:
                HandleAccept(cqe);
                break;
            case ContextType::Close:
                // No-op
                LOG_INFO("Closed: %d\n", ctx.client_fd);
                break;
            case ContextType::Read:
                HandleRead(cqe, ctx.client_fd);
                break;
            case ContextType::Write:
                HandleWrite(ctx);
                break;
            default:
                error(EXIT_ERROR, 0, "context type not handled: %d", static_cast<int>(ctx.type));
                break;
        }
    }
    // Mark the SQEs as handled
    io_uring_cq_advance(&ring_, num_cqes);
}

io_uring_sqe* UringCore::get_sqe() {
    io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
    if (sqe == nullptr) {
        io_uring_submit(&ring_);
        sqe = io_uring_get_sqe(&ring_);
    }

    if (sqe == nullptr) {
        error(EXIT_ERROR, 0, "io_uring_get_sqe");
    }

    return sqe;
}

static UringObject get_context(io_uring_cqe* cqe) {
    UringObject ctx{};
    auto* buffer = reinterpret_cast<uint8_t*>(&cqe->user_data);

    ctx.client_fd = *(reinterpret_cast<int32_t*>(buffer));
    buffer += 4;
    ctx.type = static_cast<ContextType>(*buffer);
    buffer += 1;
    ctx.buffer_idx = *(reinterpret_cast<uint16_t*>(buffer));

    return ctx;
}

void UringCore::set_context(io_uring_sqe* sqe, ContextType type, int32_t client_fd, uint16_t buffer_idx) {
    static_assert(8 == sizeof(__u64));

    auto* buffer = reinterpret_cast<uint8_t*>(&sqe->user_data);

    *(reinterpret_cast<int32_t*>(buffer)) = client_fd;
    buffer += 4;
    *buffer = static_cast<uint8_t>(type);
    buffer += 1;
    *(reinterpret_cast<uint16_t*>(buffer)) = buffer_idx;
}

void UringCore::HandleAccept(io_uring_cqe* cqe) {
    const auto client_fd = cqe->res;

    if (client_fd >= 0) {
        // Valid fd, start reading
        add_recv(client_fd);
        LOG_INFO("New connection: %d\n", client_fd);
    } else {
        LOG_ERROR("Accept error: %d\n", client_fd);
    }

    if (!flag_is_set(cqe, IORING_CQE_F_MORE)) {
        auto socket = SocketUtil::CreateTCPSocket(INET);
        add_accept(socket);
    }
}

void UringCore::handle_read(io_uring_cqe* cqe, int client_fd) {
    const auto bytes_read = cqe->res;

    if (bytes_read <= 0) {
        if (bytes_read < 0) {
            LOG_ERROR("Read error: %d\n", bytes_read);
        }
        add_close(client_fd);
        return;
    }

    // 여기서 실제 데이터 처리 로직을 구현해야 합니다.
    // 예를 들어, echo 서버라면 받은 데이터를 그대로 클라이언트에게 보내면 됩니다.
    uint16_t buffer_idx = buffer_manager.get_free_buffer();
    char* buffer = buffer_manager.get_buffer(buffer_idx);
    memcpy(buffer, cqe->buf, bytes_read);

    add_write(client_fd, buffer, bytes_read, buffer_idx);

    // 다음 읽기 작업을 준비합니다.
    if (!flag_is_set(cqe, IORING_CQE_F_MORE)) {
        add_recv(client_fd);
    }
}

void UringCore::HandleWrite(UringObject ctx) {
    const auto result = ctx.res;

    if (result == -EPIPE || result == -EBADF || result == -ECONNRESET) {
        add_close(ctx.client_fd);
    } else if (result < 0) {
        LOG_ERROR("Write error: %d\n", result);
    }

    buffer_manager.recycle_buffer(ctx.buffer_idx);
}

void UringCore::add_accept(TCPSocketPtr& socket) {
    socket->Listen();
    io_uring_sqe* sqe = get_sqe();
    set_context(sqe, ContextType::Accept, socket->getFD() , 0u);
    const int flags = 0;
    io_uring_prep_multishot_accept(sqe,
                                   listening_socket_->getFD(),
                                   (sockaddr*)&client_addr_,
                                   &client_addr_len_,
                                   flags);
}

void UringCore::add_close(int client_fd) {
    io_uring_sqe* sqe = get_sqe();
    set_context(sqe, ContextType::Close, client_fd, 0u);
    io_uring_prep_close(sqe, client_fd);
}

void UringCore::add_recv(int client_fd) {
    io_uring_sqe* sqe = get_sqe();
    set_context(sqe, ContextType::Read, client_fd, 0u);
    io_uring_prep_recv_multishot(sqe, client_fd, nullptr, 0, 0);
}

void UringCore::add_write(int client_fd, const void* data, unsigned length, uint16_t buffer_idx) {
    io_uring_sqe* sqe = get_sqe();
    set_context(sqe, ContextType::Write, client_fd, buffer_idx);
    io_uring_prep_write(sqe, client_fd, data, length, 0);
}