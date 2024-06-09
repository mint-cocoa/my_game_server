cmake_minimum_required(VERSION 3.28)
project(MyServer)
set(CMAKE_CXX_STANDARD 23)
# ServerCore 라이브러리 생성



# 라이브러리의 헤더 파일 경로 설정
add_subdirectory(ServerCore
)

# 메인 프로그램 빌드
add_executable(MyServer
        main.cpp

)

# MyProgram과 ServerCore 라이브러리 연결
target_link_libraries(MyServer PRIVATE ServerCore)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

