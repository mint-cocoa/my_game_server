cmake_minimum_required(VERSION 3.10)
add_library(ServerCore STATIC
        Network/SocketAddress.cpp
        Network/UDPSocket.cpp
        Network/SocketUtil.cpp
        Network/TCPSocket.cpp
        Network/EPollServer.cpp
        Memory/Lock.cpp
        Network/RecvBuffer.cpp
        Network/include/RecvBuffer.h
        Network/protobuf.cpp
        Network/include/protobuf.h
        Network/Session.cpp
        Network/include/Session.h
        Network/include/Session.h
        Network/include/Session.h
        Network/include/Session.h
        Main/include/Types.h

)
target_include_directories(ServerCore PUBLIC Memory/include)