cmake_minimum_required(VERSION 4.2.1)

project(trace-cpu)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    refs/tags/1.0.3
)
FetchContent_MakeAvailable(glm)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

file(GLOB SOURCES src/*.cpp src/*.h vendor/*.h)
add_executable(trace)
target_sources(trace PRIVATE ${SOURCES})
target_link_libraries(trace PRIVATE glm::glm)
