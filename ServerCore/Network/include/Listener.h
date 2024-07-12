#ifndef MYSERVER_LISTENER_H
#define MYSERVER_LISTENER_H

#include <memory>
#include <vector>
#include <liburing.h>
#include "NetAddress.h"

class Service;
class IocpEvent;
class AcceptEvent;

using ServerServiceRef = std::shared_ptr<Service>;

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener() = default;
    ~Listener();

    bool StartAccept(ServerServiceRef service);
    void CloseSocket();
    int GetSocket() const { return _socket; }
    void Dispatch(struct io_uring_cqe* cqe);

private:
    void RegisterAccept(AcceptEvent* acceptEvent);
    void ProcessAccept(AcceptEvent* acceptEvent);

    ServerServiceRef _service;
    TCPSocketPtr _socket ;
    std::vector<AcceptEvent*> _acceptEvents;
    struct io_uring* _ring = nullptr;
};

#endif //MYSERVER_LISTENER_H