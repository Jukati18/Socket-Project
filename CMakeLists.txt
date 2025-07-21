cmake_minimum_required(VERSION 3.10)
project(SocketProject)

set(CMAKE_CXX_STANDARD 17)

# Collect all .cpp files in the root directory, excluding Report folder
file(GLOB SRC_FILES
    "${CMAKE_SOURCE_DIR}/*.cpp"
)

add_executable(SocketProject ${SRC_FILES})

# If you have additional include directories, add them here
# target_include_directories(SocketProject PRIVATE ${CMAKE_SOURCE_DIR}/include)

# If you need to link libraries, add them here
# target_link_libraries(SocketProject <library>)
