add_library(ServerCore STATIC
        Network/include/IOuringServer.h
        Network/BufferManager.cpp
        Network/include/BufferManager.h
        Network/UringEvent.cpp
        Network/include/UringEvent.h
        Network/include/UringEvent.h
        Network/include/UringEvent.h
        Network/UringCore.cpp
)

# Network 하위 디렉토리 추가
add_subdirectory(Network)

# Memory 하위 디렉토리 추가
add_subdirectory(Memory)

target_include_directories(ServerCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBURING_INCLUDE_DIRS}
)
