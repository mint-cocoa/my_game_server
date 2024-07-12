target_sources(ServerCore
        PRIVATE
        SocketAddress.cpp
        UDPSocket.cpp
        SocketUtil.cpp
        TCPSocket.cpp
        EPollServer.cpp
        protobuf.cpp
        Session.cpp
        Service.cpp
        SendBuffer.cpp
        Listener.cpp
)

target_include_directories(ServerCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)