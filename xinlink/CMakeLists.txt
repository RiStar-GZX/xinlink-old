cmake_minimum_required(VERSION 3.5)

project(xinlink LANGUAGES C)



include_directories(./include)

set(SRC_LIST main.c core.c device.c event.c instruction.c network.c)

add_executable(xinlink ${SRC_LIST})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(xinlink PRIVATE Threads::Threads)
