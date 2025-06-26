cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Allium VERSION 0.4.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/data/Geometry.cpp
    src/hooks/EditorUI.cpp
    src/manager/BrushManager.cpp
    src/ui/AlliumButtonBar.cpp
    src/ui/SupportPopup.cpp
    src/util/BrushDrawer.cpp
    src/util/CurveBrushDrawer.cpp
    src/util/DrawNodeExtension.cpp
    src/util/FreeBrushDrawer.cpp
    src/util/LineBrushDrawer.cpp
    src/util/PolygonBrushDrawer.cpp
    src/util/TextBrushDrawer.cpp
    src/util/converter/BaseConverter.cpp
    src/util/converter/PolylineConverter.cpp
    src/util/converter/TriangulatorConverter.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/Allium
    third_party
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage(
    GITHUB_REPOSITORY "jhasse/poly2tri"
    GIT_TAG "13d64e7"
)

set(CLIPPER2_TESTS OFF CACHE BOOL "" FORCE)
set(CLIPPER2_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CLIPPER2_UTILS OFF CACHE BOOL "" FORCE)
set(CLIPPER2_USINGZ OFF CACHE BOOL "" FORCE)
CPMAddPackage(
    GITHUB_REPOSITORY "altalk23/Clipper2"
    GIT_TAG "main"
    SOURCE_SUBDIR "CPP"
)

target_link_libraries(${PROJECT_NAME}
    poly2tri Clipper2 
)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

add_subdirectory(third_party/agg-2.6)
target_link_libraries(${PROJECT_NAME} Agg-2.6)