cmake_minimum_required(VERSION 3.21)
project(MyServer)
set(CMAKE_CXX_STANDARD 23)

# ServerCore 라이브러리 생성
add_library(ServerCore STATIC
    ServerCore/Network/SocketAddress.cpp
    ServerCore/Network/UDPSocket.cpp
    ServerCore/Network/SocketUtil.cpp
    ServerCore/Network/TCPSocket.cpp
    ServerCore/Network/EPollServer.cpp
    ServerCore/Memory/Lock.cpp
    ServerCore/Network/RecvBuffer.cpp
    ServerCore/Network/protobuf.cpp
    ServerCore/Network/Session.cpp
    ServerCore/Network/IOuringServer.cpp
)
target_include_directories(ServerCore PUBLIC ServerCore/Network/include ServerCore/Memory/include)

# 메인 프로그램 빌드
add_executable(MyServer main.cpp)
target_link_libraries(MyServer PRIVATE ServerCore)

# GTest 설정
find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

# 테스트 추가
add_executable(IOuringServerTest ServerCore/test/IOuringServerTest.cpp)
target_link_libraries(IOuringServerTest ${GTEST_LIBRARIES} pthread ServerCore)