cmake_minimum_required(VERSION 3.21)

project(MyServer)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ServerCore)


find_package(PkgConfig REQUIRED)
find_package(protobuf REQUIRED)

# Google Protocol Buffers 설정



# ServerCore 라이브러리 추가


# 메인 프로그램 빌드
add_executable(MyServer main.cpp)
target_link_libraries(MyServer PRIVATE ServerCore)

