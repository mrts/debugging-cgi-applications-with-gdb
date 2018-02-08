cmake_minimum_required (VERSION 2.6)

project (cgi-debugging-example)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_BUILD_TYPE Debug)

include_directories (${CMAKE_SOURCE_DIR}/restclient-cpp/include)
link_directories (${CMAKE_SOURCE_DIR}/restclient-cpp/.libs)

add_executable (cgi-debugging-example cgi-debugging-example.cpp)

find_package (CURL REQUIRED)
target_link_libraries (cgi-debugging-example restclient-cpp ${CURL_LIBRARIES})
