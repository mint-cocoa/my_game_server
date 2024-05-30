cmake_minimum_required(VERSION 3.28)
project(my_game_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(my_game_server main.cpp
        ServerCore/SocketAddress.cpp
        ServerCore/SocketAddress.h
        ServerCore/UDPSocket.cpp
        ServerCore/UDPSocket.h
        ServerCore/SocketUtil.cpp
        ServerCore/SocketUtil.h
        ServerCore/TCPSocket.cpp
        ServerCore/TCPSocket.h
        ServerCore/SocketAddressFactory.cpp
        ServerCore/SocketAddressFactory.h
        ServerCore/SocketPoll.cpp
        ServerCore/SocketPoll.h
)
