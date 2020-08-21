cmake_minimum_required(VERSION 3.12)
project(NativeAddIn)

set(CMAKE_CXX_STANDARD 17)
set(TARGET NativeAddIn)

add_library(${TARGET} SHARED
		src/AddInNative.cpp
		src/AddInNative.def
		src/AddInNative.h
		src/TestComponent.cpp
		src/TestComponent.h
		src/stdafx.h)

target_compile_definitions(${TARGET} PRIVATE UNICODE _UNICODE)

target_include_directories(${TARGET} PRIVATE include)

if (WIN32 AND NOT MSVC)
	message(FATAL_ERROR "Must be compiled with MSVC on Windows")
endif ()

if (WIN32)
	add_definitions(/MT)
	set(CMAKE_SUPPRESS_REGENERATION 1)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
	target_compile_definitions(${TARGET} PRIVATE _WINDOWS
			_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
	target_compile_options(${TARGET} PRIVATE /utf-8)
endif ()

if (UNIX)
	if (TARGET_ARCH STREQUAL "x86")
		set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
	endif ()
endif ()
