#ifndef MYSERVER_SERVICE_H
#define MYSERVER_SERVICE_H

#include <memory>
#include <functional>
#include <unordered_set>
#include "NetAddress.h"
#include "IocpCore.h"

class Session;
class Listener;

using SessionRef = std::shared_ptr<Session>;
using ListenerRef = std::shared_ptr<Listener>;
using SessionFactory = std::function<SessionRef(void)>;

enum class ServiceType : uint8_t
{
    Server,
    Client
};

class Service : public std::enable_shared_from_this<Service>
{
public:
    Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32_t maxSessionCount);
    virtual ~Service();

    virtual bool Start() = 0;
    bool CanStart() { return _sessionFactory != nullptr; }

    virtual void CloseService();
    void Broadcast(SendBufferRef sendBuffer);

    SessionRef CreateSession();
    void AddSession(SessionRef session);
    void ReleaseSession(SessionRef session);

    int32_t GetCurrentSessionCount() { return _sessionCount; }
    int32_t GetMaxSessionCount() { return _maxSessionCount; }

    ServiceType GetServiceType() { return _type; }
    NetAddress GetNetAddress() { return _netAddress; }
    UringCoreRef& GetUringCore() { return _uringCore; }

protected:
    ServiceType _type;
    NetAddress _netAddress;
    UringCoreRef _uringCore;

    SessionFactory _sessionFactory;
    int32_t _maxSessionCount;
    int32_t _sessionCount;
    std::unordered_set<SessionRef> _sessions;
};

using ServiceRef = std::shared_ptr<Service>;

class ClientService : public Service
{
public:
    ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32_t maxSessionCount);
    virtual bool Start() override;
};

class ServerService : public Service
{
public:
    ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32_t maxSessionCount);
    virtual bool Start() override;
    virtual void CloseService() override;

private:
    ListenerRef _listener;
};

#endif //MYSERVER_SERVICE_H