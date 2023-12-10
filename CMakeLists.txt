cmake_minimum_required(VERSION 3.27)
project(adventofcode23)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)

add_executable(dayone common/common.cpp common/tokenizer.cpp dayone.cpp)
add_executable(daytwo common/common.cpp common/tokenizer.cpp daytwo.cpp)
add_executable(daythree common/common.cpp common/tokenizer.cpp daythree.cpp)
add_executable(dayfour common/common.cpp common/tokenizer.cpp dayfour.cpp)
add_executable(dayfive common/common.cpp common/tokenizer.cpp dayfive.cpp)
add_executable(daysix common/common.cpp common/tokenizer.cpp daysix.cpp)
add_executable(dayseven common/common.cpp common/tokenizer.cpp dayseven.cpp)
add_executable(dayeight common/common.cpp common/tokenizer.cpp dayeight.cpp)
add_executable(daynine common/common.cpp common/tokenizer.cpp daynine.cpp)
add_executable(dayten common/common.cpp dayten.cpp)