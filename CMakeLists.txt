cmake_minimum_required(VERSION 3.11.0)
set(CMAKE_C_CSTANDARD C11)
project(LIBMONTMUL C)
set(CMAKE_C_FLAGS "-Wall -Werror -pedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
add_library(montmul SHARED src/montmul.c)
add_executable(test src/test.c)
target_link_libraries(test PRIVATE montmul)

target_include_directories(test
    PRIVATE
        $(PROJECT_SOURCE_DIR)/src
)
