cmake_minimum_required(VERSION 3.8)
project(os_ex3)

set(CMAKE_CXX_STANDARD 11)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


set(SOURCE_FILES
        MapReduceClient.h
        ErrorCodes.h
        FrameWork.h
        FrameWork.cpp
        Context.h
        Context.cpp
        MapReduceFramework.h
        MapReduceFramework.cpp
        Barrier.cpp
        Barrier.h
        TestClient.cpp
        SampleClient.cpp Semaphore.h Semaphore.cpp)

add_executable(os_ex3 ${SOURCE_FILES})