cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

project(GDMP-Client)

add_compile_definitions(SHOW_DEBUG_CONSOLE)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp src/*.c src/*.h)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
	external/minhook/include
	external/gd.h/include
	external/gd.h
)

add_subdirectory(external/GDMP-Shared)
add_subdirectory(external/fmt)
add_subdirectory(external/minhook)
add_subdirectory(external/cocos-headers)

target_link_libraries(${PROJECT_NAME} GDMP-Shared fmt minhook cocos2d)