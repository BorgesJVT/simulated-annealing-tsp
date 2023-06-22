cmake_minimum_required(VERSION 2.6)
project(SolvingTSP)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

find_package( OpenCV REQUIRED )

add_executable(solving_tsp src/main.cpp src/tsp.cpp src/optimizer.cpp)

target_compile_definitions(solving_tsp PUBLIC CVPLOT_HEADER_ONLY)
target_link_libraries( solving_tsp
    ${OpenCV_LIBS}
    ${Python3_LIBRARIES}
)
