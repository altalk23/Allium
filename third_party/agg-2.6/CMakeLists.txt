cmake_minimum_required(VERSION 3.21)

project(Agg-2.6 VERSION 2.6.0)

add_library(${PROJECT_NAME} STATIC
    src/agg_curves.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/agg-2.6
)