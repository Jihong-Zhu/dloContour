cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)
cmake_policy(SET CMP0012 NEW)

project(detectContour)
find_package(OpenCV 3.0.0 REQUIRED) #OpenCV
find_package (Eigen3 3.3 REQUIRED NO_MODULE)  # Eigen3

message("OpenCV version: ${OpenCV_VERSION}")
include_directories(include) # include header file
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

file(GLOB SOURCES "src/*.cpp")
add_executable(detectContour ${SOURCES})
target_link_libraries(detectContour ${OpenCV_LIBS})
target_link_libraries (detectContour Eigen3::Eigen)
