project(vfxFluid)
cmake_minimum_required(VERSION 3.5.2)

include_directories(include)
include_directories(../vfxEngine/include/)
include_directories(${3RD_PARTY_LIBS_dir}/rapidjson)

file(GLOB VFX_FLUID_HEADERS include/*.h)
file(GLOB VFX_FLUID_SOURCES src/*.cpp)

# shaders
file(GLOB VFX_FLUID_SHADERS
		shaders/*.comp
		shaders/*.vert
		shaders/*.frag
)

# Advection algorithms
set(VFX_FLUID_ADVECTION
	include/SemiLagrangian.h src/SemiLagrangian.cpp
	include/MacCormack.h src/MacCormack.cpp
)

# Injection algorithms
set(VFX_FLUID_INJECTION
	include/TempInjection.h src/TempInjection.cpp
	include/VelocityInjection.h src/VelocityInjection.cpp
	include/DensityInjection.h src/DensityInjection.cpp
)

# obstacles
set(VFX_FLUID_OBSTACLES
	include/Cube.h src/Cube.cpp
	include/Sphere.h src/Sphere.cpp
	include/NoObstacle.h src/NoObstacle.cpp
	include/Obstacle.h src/Obstacle.cpp
)

source_group("obstacles" FILES ${VFX_FLUID_OBSTACLES})
source_group("interface" FILES ${VFX_FLUID_PUBLIC_INTERFACE})
source_group("advection" FILES ${VFX_FLUID_ADVECTION})
source_group("injection" FILES ${VFX_FLUID_INJECTION})
source_group("shaders" FILES ${VFX_FLUID_SHADERS})

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

add_library(vfxFluid STATIC
			${VFX_FLUID_HEADERS} ${VFX_FLUID_SOURCES}
			${VFX_FLUID_SHADERS}
			${VFX_FLUID_OBSTACLES}
			${VFX_FLUID_INJECTION}
			${VFX_FLUID_ADVECTION}
)

target_link_libraries(vfxFluid vfxEngine glew)

install(FILES resources/config.json DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
install(DIRECTORY resources/shaders DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
install(FILES resources/config.json DESTINATION ${DEPLOYMENT_DIR}/${CMAKE_BUILD_TYPE})
install(DIRECTORY resources/shaders DESTINATION ${DEPLOYMENT_DIR}/${CMAKE_BUILD_TYPE})