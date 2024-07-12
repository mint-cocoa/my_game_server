target_sources(ServerCore
        PRIVATE
        Lock.cpp
)

target_include_directories(ServerCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)