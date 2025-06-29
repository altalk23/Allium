cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Allium VERSION 1.0.6)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/data/Geometry.cpp
    src/hooks/EditorUI.cpp
    src/manager/BrushManager.cpp
    src/ui/AlliumButtonBar.cpp
    src/ui/SupportPopup.cpp
    src/ui/MenuItemTogglerExtra.cpp
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

CPMAddPackage(
    NAME Boost
    VERSION 1.88.0 # Versions less than 1.85.0 may need patches for installation targets.
    URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.7z
    OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON" # Set `OFF` for installation
            "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES geometry" # Note the escapes!
)

target_link_libraries(${PROJECT_NAME}
    poly2tri Boost::geometry 
)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

add_subdirectory(third_party/agg-2.6)
target_link_libraries(${PROJECT_NAME} Agg-2.6)