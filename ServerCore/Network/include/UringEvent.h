#pragma once

#include <cstdint>

class Session;

enum class EventType : uint8_t
{
    Connect,
    Disconnect,
    Accept,
    //PreRecv,
    Recv,
    Send
};

class UringEvent
{
public:
    UringEvent(EventType type);
    void			Init();

public:
    EventType		eventType;
    UringObjectRef	owner;
};

/*----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public UringEvent
{
public:
    ConnectEvent() : UringEvent(EventType::Connect) { }
};

/*--------------------
	DisconnectEvent
----------------------*/

class DisconnectEvent : public UringEvent
{
public:
    DisconnectEvent() : UringEvent(EventType::Disconnect) { }
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public UringEvent
{
public:
    AcceptEvent() : UringEvent(EventType::Accept) { }

public:
    SessionRef	session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public UringEvent
{
public:
    RecvEvent() : UringEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public UringEvent
{
public:
    SendEvent() : UringEvent(EventType::Send) { }
    
    std::vector<SendBufferRef> sendBuffers;
};