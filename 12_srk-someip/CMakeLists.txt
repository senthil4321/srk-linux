cmake_minimum_required(VERSION 3.5)
project(someip_example)

set(CMAKE_CXX_STANDARD 11)

# Add the path to the vsomeip3 package
set(CMAKE_PREFIX_PATH "/path/to/vsomeip3")

find_package(vsomeip3 REQUIRED)

add_executable(someip_server someip_server.cpp)
target_link_libraries(someip_server vsomeip3)

add_executable(someip_client someip_client.cpp)
target_link_libraries(someip_client vsomeip3)
