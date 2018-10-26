cmake_minimum_required(VERSION 3.0.0)
project(sadDB)
set(CMAKE_CXX_STANDARD 17)

# Debug
set(CMAKE_BUILD_TYPE Debug)
#set(Boost_DEBUG ON)

# Packages
find_package(
    Threads
)

# Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost
    REQUIRED
    COMPONENTS
        serialization
        system
        thread
)

# Executables
set(sadDB_server "sadDB")
set(sadDB_client "sadDB_client")

# Server
file(GLOB sadDB_SRC
    "src/*.cpp"
)
add_executable(${sadDB_server}
    ${sadDB_SRC}
)
target_link_libraries(${sadDB_server}
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Client
file(GLOB sadDB_client_SRC
    "client_src/*.cpp"
)
add_executable(${sadDB_client}
    ${sadDB_client_SRC}
)
target_link_libraries(${sadDB_client}
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Test TCP Server
file(GLOB sadDB_SRC_tcp-server
    "test/tcp_server.cpp"
    "src/*.cpp"
)
list(FILTER sadDB_SRC_tcp-server EXCLUDE REGEX
    "src/main.cpp$"
)
add_executable(tcp-server
    ${sadDB_SRC_tcp-server}
)
target_link_libraries(tcp-server
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Test TCP Client
file(GLOB sadDB_SRC_tcp-client
    "test/tcp_client.cpp"
    "src/*.cpp"
)
list(FILTER sadDB_SRC_tcp-client EXCLUDE REGEX
    "src/main.cpp$"
)
add_executable(tcp-client
    ${sadDB_SRC_tcp-client}
)
target_link_libraries(tcp-client
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Headers
include_directories(
    "src"
    "client_src"
    ${Boost_INCLUDE_DIR}
)
