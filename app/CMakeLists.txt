cmake_minimum_required (VERSION 2.6)

project (CMP143)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )
# LINK_DIRECTORIES( src/ )
# add_subdirectory( src/ )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set (CMAKE_DEBUG_POSTFIX "_d")

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

if(WIN32)
set(COMMON_LIBS ${OPENGL_LIBRARIES} optimized glfw debug glfw)
elseif (UNIX)
set(COMMON_LIBS ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
else()
set(COMMON_LIBS)
endif()
set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)


set(SOURCES 
    lib/gl3w.c
    src/util.h src/util.cpp 
    src/IndexBuffer.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
    src/Object.cpp
    src/PlyObject.cpp
    src/PlyPointCloud.cpp
    src/close2gl.cpp
    src/opengl_reference.cpp
    src/close2gl_math.cpp
    src/FullImageBuffer.cpp
)

include_directories(lib/happly/)
include_directories(imgui/)
add_library(imgui
    imgui/imgui.cpp 
    imgui/imgui_demo.cpp 
    imgui/imgui_draw.cpp 
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_stdlib.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_impl_opengl3.cpp
    ${HEADER_FILES}
)
include_directories(stb_image/)
add_library(stb_image 
    stb_image/stb_image.cpp
    stb_image/stb_image.h
)

add_library(mylib src/util.cpp ${HEADER_FILES})
# add_compile_options(-fsanitize=address)
# add_compile_options(-fsanitize=undefined)
# add_link_options(-fsanitize=address)
# add_link_options(-fsanitize=undefined)

add_executable(CMP143 src/main.cpp ${SOURCES})


set_property(TARGET CMP143 PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(CMP143 ${COMMON_LIBS})
target_link_libraries(CMP143 glm::glm)
target_link_libraries(CMP143 mylib)
target_link_libraries(CMP143 imgui)
target_link_libraries(CMP143 stb_image)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories( include/ )
include_directories( src/ )
include_directories(lib/glfw/include)

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw)
