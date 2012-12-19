TARGET		= mba_nvdemo
TEMPLATE	= app
CONFIG		= warn_on release

INCLUDEPATH	= ../include ../glpng/include /usr/X11R6/include 

LIBDIRS		= -L../glpng/lib -L/usr/X11R6/lib
LIBS		= $$LIBDIRS -lGL -lGLU -lglut -lglpng -lXi -lXmu -lXext -lX11 -lm

CLEAN_FILES += *.bck

###############################
## WIN32
#win32-msvc:TEMPLATE    = vcapp
win32-msvc:TEMPLATE    = app
win32-msvc:CONFIG     += opengl console
win32-msvc:INCLUDEPATH = ../include ../glpng/include ../glut/include
win32-msvc:LIBS        = ../glut/lib/glut32.lib ../glpng/lib/glpng.lib 

###############################
HEADERS		= \
			RglPrims.h \
			room.h \
			reflect.h \
			mmath.h \
			main.h

SOURCES		= \
			RglPrims.cpp \
			room.cpp \
			reflect.cpp \
			mmath.cpp \
			main.cpp
