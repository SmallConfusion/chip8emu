﻿cmake_minimum_required (VERSION 3.24)

option(USE_LTO "Uses link time optimization if release build" ON)

project ("chip8")

include(FetchContent)

add_executable (chip8
	src/main.cpp
	src/ui.cpp

	src/engine/engine.cpp
	src/engine/debug.cpp
	src/engine/rand.cpp
	src/engine/sound.cpp
	src/engine/memory.cpp
)

FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.91.6
	GIT_SHALLOW ON
)

FetchContent_Declare(SDL2
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-2.30.10
	GIT_SHALLOW ON
)

FetchContent_Declare(nfd
	GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
	GIT_TAG v1.2.1
	GIT_SHALLOW ON
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND ${USE_LTO})
	include(CheckIPOSupported)
	check_ipo_supported(RESULT supported OUTPUT error)

	if (supported)
		message("Link time optimization enabled")
		set_property(TARGET chip8 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

FetchContent_MakeAvailable(SDL2 imgui)

target_sources(chip8 PUBLIC
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
)



if (NOT EMSCRIPTEN)
	FetchContent_MakeAvailable(nfd)
	target_link_libraries(chip8 PRIVATE nfd)
else()
	set(CMAKE_EXECUTABLE_SUFFIX ".html") 
endif()

target_link_libraries(chip8 PRIVATE SDL2::SDL2)

if (WIN32)
	target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()


target_include_directories(chip8 PRIVATE
	src
	${SDL2_SOURCE_DIR}/include
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

set_property(TARGET chip8 PROPERTY CXX_STANDARD 23)
