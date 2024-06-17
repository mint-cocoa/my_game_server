cmake_minimum_required(VERSION 3.21)

project(MyServer)
set(CMAKE_CXX_STANDARD 23)


# PkgConfig 및 liburing 설정
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBURING REQUIRED IMPORTED_TARGET liburing)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Google Test 설정
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# ServerCore 라이브러리 생성
add_library(ServerCore STATIC
        ServerCore/Network/SocketAddress.cpp
        ServerCore/Network/UDPSocket.cpp
        ServerCore/Network/SocketUtil.cpp
        ServerCore/Network/TCPSocket.cpp
        ServerCore/Network/EPollServer.cpp
        ServerCore/Memory/Lock.cpp
        ServerCore/Network/RecvBuffer.cpp
        ServerCore/Network/IOuringServer.cpp
        ServerCore/Network/protobuf.cpp
        ServerCore/Network/Session.cpp
        ServerCore/Memory/Lock.cpp
)

target_include_directories(ServerCore PUBLIC ServerCore/Network/include ${LIBURING_INCLUDE_DIRS})
target_include_directories(ServerCore PUBLIC ServerCore/Memory/include)

target_link_libraries(ServerCore PUBLIC PkgConfig::LIBURING)

# 메인 프로그램 빌드
add_executable(MyServer main.cpp
        ServerCore/Network/include/protobuf.h)
target_link_libraries(MyServer PRIVATE ServerCore)

# GTest 설정
include(GoogleTest)

# 테스트 추가
add_executable(IOuringServerTest
        ServerCore/test/IOuringServerTest.cpp
        GameServer/GameServer.cpp
)

target_include_directories(IOuringServerTest PUBLIC ServerCore/Network/include ${LIBURING_INCLUDE_DIRS})
target_link_libraries(IOuringServerTest PRIVATE ServerCore GTest::gtest_main PkgConfig::LIBURING)

# Google Test를 위한 테스트 디스커버리
gtest_discover_tests(IOuringServerTest)