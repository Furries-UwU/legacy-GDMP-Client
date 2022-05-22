cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GDMP-Client VERSION 1.0.0)

# add_definitions(-DMAC_EXPERIMENTAL)

find_path(GEODE_SDK_PATH
        NAMES Geode.cmake
        PATHS $ENV{GEODE_SUITE}/sdk /Users/Shared/Geode/suite/sdk /usr/local/geode/sdk
        DOC "Geode SDK path."
        REQUIRED
        )
include(${GEODE_SDK_PATH}/Geode.cmake)

add_subdirectory(external/GDMP-Shared)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp src/*.c src/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PROJECT_NAME} GDMP-Shared)

setup_geode_mod()

create_geode_file(${PROJECT_NAME})