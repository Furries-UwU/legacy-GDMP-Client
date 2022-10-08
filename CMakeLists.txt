cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GDMP-Client VERSION 1.0.0)

set(CMAKE_OSX_ARCHITECTURES x86_64)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake"
            TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_autodetect(settings)

conan_cmake_run(CONANFILE conanfile.txt
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing)

file(GLOB_RECURSE PROTO_SOURCE_FILES CONFIGURE_DEPENDS src/*.proto)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.c)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCE_FILES})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${PROTO_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include ${Protobuf_INCLUDE_DIRS} ${PROTO_HDRS})
target_link_libraries(${PROJECT_NAME} $${Protobuf_LIBRARIES} CONAN_PKG::zmqpp CONAN_PKG::fmt)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

set(GEODE_LINK_SOURCE On)
add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

target_link_libraries(${PROJECT_NAME} geode-sdk)
create_geode_file(${PROJECT_NAME})