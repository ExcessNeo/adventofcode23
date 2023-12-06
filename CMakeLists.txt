cmake_minimum_required(VERSION 3.27)
project(adventofcode23)

set(CMAKE_CXX_STANDARD 17)

add_executable(dayone common/common.cpp common/tokenizer.cpp dayone.cpp)
add_executable(daytwo common/common.cpp common/tokenizer.cpp daytwo.cpp)
add_executable(daythree common/common.cpp common/tokenizer.cpp daythree.cpp)
add_executable(dayfour common/common.cpp common/tokenizer.cpp dayfour.cpp)
add_executable(dayfive common/common.cpp common/tokenizer.cpp dayfive.cpp)
add_executable(daysix common/common.cpp common/tokenizer.cpp daysix.cpp)
