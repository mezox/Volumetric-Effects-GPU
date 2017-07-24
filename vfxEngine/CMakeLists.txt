project(vfxEngine)
cmake_minimum_required(VERSION 3.5.2)

include_directories(include)
include_directories(include/graphics)
include_directories(include/core)
include_directories(include/utils)
include_directories(include/logger)
include_directories(include/scene)
include_directories(include/systems)

# external
include_directories(${3RD_PARTY_LIBS_dir}/rapidjson)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# graphics sources & headers
file(GLOB GFX_GL_SOURCES src/graphics/*.cpp)
file(GLOB GFX_GL_HEADERS include/graphics/*.h)

# core sources & headers
file(GLOB CORE_SOURCES src/core/*.cpp)
file(GLOB CORE_HEADERS include/core/*.h)

# core sources & headers
file(GLOB UTILS_SOURCES src/utils/*.cpp)
file(GLOB UTILS_HEADERS include/utils/*.h)

# systems sources & headers
file(GLOB SYSTEMS_SOURCES src/systems/*.cpp)
file(GLOB SYSTEMS_HEADERS include/systems/*.h)

# systems sources & headers
file(GLOB LOGGER_SOURCES src/logger/*.cpp)
file(GLOB LOGGER_HEADERS include/logger/*.h)

# systems sources & headers
file(GLOB SCENE_SOURCES src/scene/*.cpp)
file(GLOB SCENE_HEADERS include/scene/*.h)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)
file(GLOB INTERFACE include/vfxEngine.h)

source_group("graphics" FILES ${GFX_GL_SOURCES} ${GFX_GL_HEADERS})
source_group("utils" FILES ${UTILS_SOURCES} ${UTILS_HEADERS})
source_group("systems" FILES ${SYSTEMS_SOURCES} ${SYSTEMS_HEADERS})
source_group("core" FILES ${CORE_SOURCES} ${CORE_HEADERS})
source_group("logging" FILES ${LOGGER_SOURCES} ${LOGGER_HEADERS})
source_group("scene" FILES ${SCENE_SOURCES} ${SCENE_HEADERS})

add_library(vfxEngine
			STATIC
			${HEADERS} ${SOURCES}
			${GFX_GL_SOURCES} ${GFX_GL_HEADERS}
			${CORE_SOURCES} ${CORE_HEADERS}
			${UTILS_SOURCES} ${UTILS_HEADERS}
			${SYSTEMS_SOURCES} ${SYSTEMS_HEADERS}
			${LOGGER_SOURCES} ${LOGGER_HEADERS}
			${SCENE_SOURCES} ${SCENE_HEADERS}
)
target_include_directories(vfxEngine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(vfxEngine glew glm glfw)

install(TARGETS vfxEngine
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})