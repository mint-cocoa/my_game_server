#include "pch.h"
#include "UringCore.h"
#include "UringEvent.h"

UringCore::UringCore()
{
    int ret = io_uring_queue_init(NUM_SUBMISSION_QUEUE_ENTRIES, &uring_, 0);
    ASSERT_CRASH(ret == 0);
}

UringCore::~UringCore()
{
    io_uring_queue_exit(&uring_);
}

bool UringCore::Register(UringObjectPtr uringObject)
{
    return true;
}

bool UringCore::Dispatch(uint32_t timeoutMs)
{
    io_uring_cqe* cqes[CQE_BATCH_SIZE];
    struct __kernel_timespec ts = {
        .tv_sec = timeoutMs / 1000,
        .tv_nsec = (timeoutMs % 1000) * 1000000
    };

    unsigned count = io_uring_peek_batch_cqe(&uring_, cqes, CQE_BATCH_SIZE);

    if (count == 0) {
        int ret = io_uring_wait_cqe_timeout(&uring_, &cqes[0], &ts);
        if (ret == -ETIME) {
            return false;
        }
        if (ret < 0) {
            return false;
        }
        count = 1;
    }

    for (unsigned i = 0; i < count; i++) {
        auto cqe = cqes[i];
        uint32_t buffer_id = cqe->flags >> IORING_CQE_BUFFER_SHIFT;
        UringEvent* uringEvent = static_cast<UringEvent*>(io_uring_cqe_get_data(cqe));
        UringObjectPtr uringObject = uringEvent->owner;
        uringObject->Dispatch(uringEvent, buffer_id);


    }

    return true;
}