cmake_minimum_required(VERSION 3.0.0)
project(sadDB)
set(CMAKE_CXX_STANDARD 17)

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
