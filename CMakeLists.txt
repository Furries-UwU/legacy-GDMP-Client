cmake_minimum_required(VERSION 3.12)

project(GDMP_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

include_directories(external/gd.h/include)
include_directories(external/gd.h)

include_directories(external/Mega-Hack-Extensions/include)
include_directories(external/Mega-Hack-Extensions/extensions1)

add_subdirectory(external/cocos-headers)
add_subdirectory(external/enet)
add_subdirectory(external/fmt)

include_directories(external/enet/include)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp src/*.c src/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} enet fmt cocos2d)
target_link_libraries(${PROJECT_NAME} external/Mega-Hack-Extensions/bin/hackpro)