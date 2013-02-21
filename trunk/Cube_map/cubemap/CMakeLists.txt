cmake_minimum_required(VERSION 2.8)
project(cube_DEMO)
include_directories(E:/opengl/freeglut-2.8.0/include)
#include_directories(E:/opengl/freeglut-2.8.0/include)
link_directories(E:/opengl/debug)
ADD_DEFINITIONS(-DFREEGLUT_STATIC)
set(CM_CUBE
	cm_demo.c
	#cubemap.c
	tga.c
	tga.h
	trackball.c
	trackball.h
)

ADD_EXECUTABLE(cm_cube ${CM_CUBE})

set(CUBE 
	#cm_demo.c
	cubemap.c
	tga.c
	tga.h
	trackball.c
	trackball.h

)

ADD_EXECUTABLE(cube ${CUBE})