cmake_minimum_required( VERSION 3.7 )
project( MandelbotRenderer )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" )

find_package( GLFW3 REQUIRED )
find_package( GLEW REQUIRED )
find_package( OpenGL REQUIRED )

set( SRC mandelbrot.cpp)
set(HEADER_FILES stb_image.h)
set( LIBRARIES
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARIES}
    ${GLFW3_LIBRARY}
)

include_directories(include)

#Include library files
include_directories(glm/include)


#Add deps
add_subdirectory(dependencies)
add_executable( Mandelbrot ${SRC} ${HEADER_FILES} )
target_link_libraries( Mandelbrot ${LIBRARIES} )
