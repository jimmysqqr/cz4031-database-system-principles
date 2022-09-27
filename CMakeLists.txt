# set minimum cmake version
cmake_minimum_required(VERSION 3.24)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)

# set linker flags
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++")

# set the project name
project(CZ4031)

# add the bp_tree and storage libraries
add_subdirectory(bp_tree)
add_subdirectory(storage)

# add the executable
add_executable(main main.cpp)

target_link_libraries(main PUBLIC bp_tree storage)

# add the binary tree to the search path for include files
# so that it will find the header files
target_include_directories(main 
    PUBLIC "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/bp_tree"
    "${PROJECT_SOURCE_DIR}/storage"
    )