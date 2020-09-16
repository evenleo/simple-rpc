cmake_minimum_required(VERSION 2.8)
project(rpc)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)
link_directories(/usr/local/lib64/)

SET(CMAKE_BUILD_TYPE "Debug")

add_executable(serializer_test serializer_test.cpp)

add_executable(rpc_server rpc_server.cpp)
target_link_libraries(rpc_server -lzmq)

add_executable(rpc_client rpc_client.cpp)
target_link_libraries(rpc_client -lzmq)





