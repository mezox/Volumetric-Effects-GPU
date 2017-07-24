project(vfxDemo)
cmake_minimum_required(VERSION 3.5.2)

include_directories(include)
include_directories(../vfxFluid/include)
include_directories(${3RD_PARTY_LIBS_dir}/assimp/include/assimp)

file(GLOB DEMO_HEADERS include/*.h)
file(GLOB DEMO_SOURCES src/*.cpp)

add_executable(	vfxDemo
				${DEMO_SOURCES} ${DEMO_HEADERS}
				${VFX_FLUID_HEADERS} ${VFX_FLUID_SOURCES}
				${VFX_FLUID_SHADERS}
)
target_link_libraries(vfxDemo assimp vfxEngine vfxFluid)