cmake_minimum_required(VERSION 3.10)
project(TermProject)

set(CMAKE_C_STANDARD 99)

add_executable(TermProject main.c cJSON.c)

include_directories(.)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(TermProject ${CURL_LIBRARIES})
