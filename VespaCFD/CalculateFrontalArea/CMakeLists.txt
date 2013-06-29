cmake_minimum_required(VERSION 2.8)

PROJECT(CalculateFrontalArea)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(CalculateFrontalArea MACOSX_BUNDLE CalculateFrontalArea)

if(VTK_LIBRARIES)
  target_link_libraries(CalculateFrontalArea ${VTK_LIBRARIES})
else()
  target_link_libraries(CalculateFrontalArea vtkHybrid)
endif()

