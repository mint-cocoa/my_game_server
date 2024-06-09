#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <thread>

class Lock
{
public:
    void WriteLock();
    void WriteUnlock(const char *name);
    void ReadLock(const char *name);
    void ReadUnlock(const char *name);

private:
    mutable std::shared_mutex mutex_;
    unsigned int count_{};
};

/*----------------
    LockGuards
-----------------*/

class ReadLockGuard
{
public:
    ReadLockGuard(Lock &lock, const char *name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
    ~ReadLockGuard() { _lock.ReadUnlock(_name); }

private:
    Lock &_lock;
    const char *_name;
};

class WriteLockGuard
{
public:
    WriteLockGuard(Lock &lock, const char *name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
    ~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
    Lock &_lock;
    const char *_name;
};