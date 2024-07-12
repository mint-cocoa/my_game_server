#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32_t maxSessionCount)
    : _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount), _sessionCount(0)
{
}

Service::~Service()
{
    CloseService();
}

void Service::CloseService()
{
    // TODO: Implement proper cleanup
}

void Service::Broadcast(SendBufferRef sendBuffer)
{
    // TODO: Implement thread-safe mechanism (e.g., std::shared_mutex)
    for (const auto& session : _sessions)
    {
        session->Send(sendBuffer);
    }
}

SessionRef Service::CreateSession()
{
    SessionRef session = _sessionFactory();
    session->SetService(shared_from_this());

    if (_UringCore->Register(session) == false)
        return nullptr;

    return session;
}

void Service::AddSession(SessionRef session)
{
    // TODO: Implement thread-safe mechanism
    _sessionCount++;
    _sessions.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
    // TODO: Implement thread-safe mechanism
    auto it = _sessions.find(session);
    if (it != _sessions.end())
    {
        _sessions.erase(it);
        _sessionCount--;
    }
}

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32_t maxSessionCount)
    : Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

bool ClientService::Start()
{
    if (CanStart() == false)
        return false;

    const int32_t sessionCount = GetMaxSessionCount();
    for (int32_t i = 0; i < sessionCount; i++)
    {
        SessionRef session = CreateSession();
        if (session->Connect(GetNetAddress()) == false)
            return false;

    return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32_t maxSessionCount)
    : Service(ServiceType::Server, address, core, factory, maxSessionCount)
{
}

bool ServerService::Start()
{
    if (CanStart() == false)
        return false;

    _listener = std::make_shared<Listener>();
    if (_listener == nullptr)
        return false;

    ServerServiceRef service = std::static_pointer_cast<ServerService>(shared_from_this());
    if (_listener->StartAccept(service) == false)
        return false;

    return true;
}

void ServerService::CloseService()
{
    // TODO: Implement proper cleanup for server service
    Service::CloseService();
}