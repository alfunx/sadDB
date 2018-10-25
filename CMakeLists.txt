cmake_minimum_required(VERSION 3.0.0)
project(sadDB)
set(CMAKE_CXX_STANDARD 17)

# Debug
set(CMAKE_BUILD_TYPE Debug)
#set(Boost_DEBUG ON)

# Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost
    REQUIRED
    COMPONENTS serialization
)

file(GLOB sadDB_SRC
    "src/*.h"
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${sadDB_SRC})

file(GLOB sadDB_client_SRC
    "client_src/*.h"
    "client_src/*.cpp"
)

add_executable(${PROJECT_NAME}_client ${sadDB_client_SRC})
