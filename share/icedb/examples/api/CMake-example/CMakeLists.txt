cmake_minimum_required (VERSION 3.1)
project(icedbLinkTest C CXX)

find_package(icedb REQUIRED)
add_executable(link-test test.cpp)
target_link_libraries(link-test icedb)

