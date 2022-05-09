cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

project(GDMP_client)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp src/*.c src/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
	external/enet/include
	external/minhook/include
	external/gd.h/include
	external/gd.h
)

add_subdirectory(external/enet)
add_subdirectory(external/fmt)
add_subdirectory(external/minhook)
add_subdirectory(external/cocos-headers)

target_link_libraries(${PROJECT_NAME} enet fmt minhook cocos2d)