cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Allium VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/hooks/EditorUI.cpp
    src/manager/BrushManager.cpp
    src/ui/AlliumButtonBar.cpp
    src/ui/SupportPopup.cpp
    src/util/BrushDrawer.cpp
    src/util/CurveBrushDrawer.cpp
    src/util/EditorUtilities.cpp
    src/util/FreeBrushDrawer.cpp
    src/util/LineBrushDrawer.cpp
    src/util/PolylineConverter.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/Allium
)

if (WIN32)
    # This allows you to compile in debug mode
    add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0)
    add_definitions(-D_HAS_ITERATOR_DEBUGGING=0)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

add_subdirectory(third_party/agg-2.6)
target_link_libraries(${PROJECT_NAME} Agg-2.6)