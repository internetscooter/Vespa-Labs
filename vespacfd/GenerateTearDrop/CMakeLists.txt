cmake_minimum_required(VERSION 2.8)

PROJECT(GenerateTearDrop)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(GenerateTearDrop MACOSX_BUNDLE GenerateTearDrop)

if(VTK_LIBRARIES)
  target_link_libraries(GenerateTearDrop ${VTK_LIBRARIES})
else()
  target_link_libraries(GenerateTearDrop vtkHybrid)
endif()

