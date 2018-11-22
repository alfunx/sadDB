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
set(sadDB_generator "sadDB_gen")

# Server
file(GLOB sadDB_SRC
    "src/main.cpp"
    "src/record.cpp"
    "src/relation.cpp"
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
    "client_src/main.cpp"
)
add_executable(${sadDB_client}
    ${sadDB_client_SRC}
)
target_link_libraries(${sadDB_client}
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Generator
file(GLOB sadDB_generator_SRC
    "generator_src/main.cpp"
    "src/record.cpp"
    "src/relation.cpp"
)
add_executable(${sadDB_generator}
    ${sadDB_generator_SRC}
)
target_link_libraries(${sadDB_generator}
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
