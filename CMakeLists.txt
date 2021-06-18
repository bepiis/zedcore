cmake_minimum_required(VERSION 3.16.3)
project(ZedCore C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(lib)

add_executable(ZedCore main.c server.c client.c header/server.h header/client.h)

target_link_libraries(ZedCore PUBLIC lib)

target_include_directories(ZedCore PUBLIC  "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/lib")