cmake_minimum_required(VERSION 3.5)

project(xinlink LANGUAGES C)



include_directories(./include)
include_directories(./src)

set(SRC_LIST ./src/main.c ./src/core.c ./src/event.c ./src/instruction.c ./src/network.c )

add_executable(xinlink ${SRC_LIST})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(xinlink PRIVATE Threads::Threads)
