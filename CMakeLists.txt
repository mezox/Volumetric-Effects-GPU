project(vfx)
cmake_minimum_required(VERSION 3.5.2)

set(3RD_PARTY_LIBS_dir ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deployment)
set(DEPLOYMENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deployment)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(3rdParty)
add_subdirectory(vfxEngine)
add_subdirectory(vfxFluid)
add_subdirectory(vfxDemo)

install(DIRECTORY data DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})