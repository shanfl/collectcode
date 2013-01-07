# Wedit Makefile for project glBase
SRCDIR=c:\projects\glbase
CFLAGS=-Ic:\lcc\include  -ansic
CC=c:\lcc\bin\lcc.exe
LINKER=c:\lcc\bin\lcclnk.exe
OBJS=\
	app.obj \
	boundbox.obj \
	cache.obj \
	callback.obj \
	camera.obj \
	error.obj \
	freecamera.obj \
	frustum.obj \
	main.obj \
	opengl.obj \
	pbuffer.obj \
	pic.obj \
	plane.obj \
	texturefont.obj \
	texturemanager.obj \
	tgafile.obj \
	vector.obj \
	viewpoint.obj

LIBS=glu32.lib glut32.lib opengl32.lib

glbase.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\glbase.exe $(OBJS) $(LIBS)

# Build APP.C
APP_C=\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\main.h\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\viewpoint.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\frustum.h\
	$(SRCDIR)\plane.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\boundbox.h\
	$(SRCDIR)\frustum.h\
	$(SRCDIR)\callback.h\
	$(SRCDIR)\freeCamera.h\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\callback.h\
	$(SRCDIR)\textureManager.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\textureFont.h\
	$(SRCDIR)\pic.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\pbuffer.h\
	$(SRCDIR)\opengl.h\

app.obj: $(APP_C) $(SRCDIR)\app.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\app.c

# Build BOUNDBOX.C
BOUNDBOX_C=\
	$(SRCDIR)\boundbox.h\
	$(SRCDIR)\frustum.h\
	$(SRCDIR)\plane.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\viewpoint.h\
	$(SRCDIR)\vector.h\

boundbox.obj: $(BOUNDBOX_C) $(SRCDIR)\boundbox.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\boundbox.c

# Build CACHE.C
CACHE_C=\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\cache.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\error.h\
	$(SRCDIR)\general.h\

cache.obj: $(CACHE_C) $(SRCDIR)\cache.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\cache.c

# Build CALLBACK.C
CALLBACK_C=\
	$(SRCDIR)\callback.h\

callback.obj: $(CALLBACK_C) $(SRCDIR)\callback.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\callback.c

# Build CAMERA.C
CAMERA_C=\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\viewpoint.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\

camera.obj: $(CAMERA_C) $(SRCDIR)\camera.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\camera.c

# Build ERROR.C
ERROR_C=\
	$(SRCDIR)\error.h\
	$(SRCDIR)\general.h\

error.obj: $(ERROR_C) $(SRCDIR)\error.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\error.c

# Build FREECAMERA.C
FREECAMERA_C=\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\viewpoint.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\freecamera.h\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\callback.h\

freecamera.obj: $(FREECAMERA_C) $(SRCDIR)\freecamera.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\freecamera.c

# Build FRUSTUM.C
FRUSTUM_C=\
	$(SRCDIR)\frustum.h\
	$(SRCDIR)\plane.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\camera.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\viewpoint.h\
	$(SRCDIR)\vector.h\

frustum.obj: $(FRUSTUM_C) $(SRCDIR)\frustum.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\frustum.c

# Build MAIN.C
MAIN_C=\
	$(SRCDIR)\general.h\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\error.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\main.h\

main.obj: $(MAIN_C) $(SRCDIR)\main.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\main.c

# Build OPENGL.C
OPENGL_C=\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\

opengl.obj: $(OPENGL_C) $(SRCDIR)\opengl.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\opengl.c

# Build PBUFFER.C
PBUFFER_C=\
	$(SRCDIR)\pbuffer.h\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\

pbuffer.obj: $(PBUFFER_C) $(SRCDIR)\pbuffer.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\pbuffer.c

# Build PIC.C
PIC_C=\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\tgafile.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\textureManager.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\pic.h\
	$(SRCDIR)\general.h\

pic.obj: $(PIC_C) $(SRCDIR)\pic.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\pic.c

# Build PLANE.C
PLANE_C=\
	$(SRCDIR)\plane.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\

plane.obj: $(PLANE_C) $(SRCDIR)\plane.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\plane.c

# Build TEXTUREFONT.C
TEXTUREFONT_C=\
	$(SRCDIR)\textureFont.h\
	$(SRCDIR)\pic.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\

texturefont.obj: $(TEXTUREFONT_C) $(SRCDIR)\texturefont.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\texturefont.c

# Build TEXTUREMANAGER.C
TEXTUREMANAGER_C=\
	$(SRCDIR)\textureManager.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\opengl.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\tgafile.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\opengl.h\

texturemanager.obj: $(TEXTUREMANAGER_C) $(SRCDIR)\texturemanager.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\texturemanager.c

# Build TGAFILE.C
TGAFILE_C=\
	$(SRCDIR)\tgafile.h\
	$(SRCDIR)\general.h\
	$(SRCDIR)\error.h\
	$(SRCDIR)\general.h\

tgafile.obj: $(TGAFILE_C) $(SRCDIR)\tgafile.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tgafile.c

# Build VECTOR.C
VECTOR_C=\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\

vector.obj: $(VECTOR_C) $(SRCDIR)\vector.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\vector.c

# Build VIEWPOINT.C
VIEWPOINT_C=\
	$(SRCDIR)\viewpoint.h\
	$(SRCDIR)\vector.h\
	$(SRCDIR)\general.h\

viewpoint.obj: $(VIEWPOINT_C) $(SRCDIR)\viewpoint.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\viewpoint.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\glbase.exe $(OBJS) $(LIBS)

