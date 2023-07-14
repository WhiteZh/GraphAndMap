project(Train_Problem)
cmake_minimum_required(VERSION 3.25)

add_executable(solution src/main.cpp)
set_target_properties(solution PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/)