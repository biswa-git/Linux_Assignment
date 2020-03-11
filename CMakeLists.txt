cmake_minimum_required (VERSION 2.8)
project (ComputationalGeometryAlgorithm)

include_directories(${CMAKE_SOURCE_DIR}/source/hpp)
include_directories(${CMAKE_SOURCE_DIR}/source/third_party/boost)

add_executable(CGA
source/cpp/main.cpp
source/cpp/data.cpp
source/cpp/edge.cpp
source/cpp/face.cpp
source/cpp/vector.cpp
source/cpp/vertex.cpp
)

target_link_libraries(CGA
   ${CMAKE_SOURCE_DIR}/source/third_party/boost/stage/lib/libboost_filesystem.a
)

install(TARGETS CGA DESTINATION ${CMAKE_SOURCE_DIR}/exe)
