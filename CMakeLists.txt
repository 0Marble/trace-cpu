cmake_minimum_required(VERSION 4.2.1)

project(trace-cpu)
add_executable(trace)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -fsanitize=address -fsanitize=undefined")

message("-- ${CMAKE_BUILD_TYPE}")

include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	    https://github.com/g-truc/glm.git
	GIT_TAG 	    refs/tags/1.0.3
)
FetchContent_MakeAvailable(glm)

set(CMAKE_CXX_FLAGS_REL_PERF "-O2 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_BUILD_TYPE_REL_PERF "${CMAKE_CXX_FLAGS_REL_PERF}" CACHE STRING "Optimized, for running perf")

option(PARALLEL "parallel" OFF)
option(BVH "use BVH" ON)
option(NO_PROGRESS_REPORT "Disable progress reporter" OFF)

if (NO_PROGRESS_REPORT)
    message("-- Disabled progress report")
    target_compile_definitions(trace PRIVATE NO_PROGRESS_REPORT)
endif()

if (PROGRESS_CLEAR_SCREEN)
    message("-- Enabled Progress clearing screen")
    target_compile_definitions(trace PRIVATE PROGRESS_CLEAR_SCREEN)
endif()

if (BVH) 
    message("-- Enabled BVH")
    target_compile_definitions(trace PRIVATE USE_BVH)
endif()

if (PARALLEL)
    message("-- Enabled parallel support")
    find_package(OpenMP)
    if (OPENMP_CXX_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	target_compile_definitions(trace PRIVATE PARALLEL)
    else()
	message(WARNING "could not find OpenMP, disabling parallel support")
	set(PARALLEL OFF)
    endif()
endif()

file(GLOB SOURCES src/*.cpp src/*.h vendor/*.h)
target_sources(trace PRIVATE ${SOURCES})
target_link_libraries(trace PRIVATE glm::glm)
