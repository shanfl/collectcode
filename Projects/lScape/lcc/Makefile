# Wedit Makefile for project lScape
SRCDIR=c:\projects\lscape
CFLAGS=-Ic:\lcc\include -Ic:\projects\glBase  -ansic
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
	viewpoint.obj \
	lscape.obj \
	lscollide.obj \
	lsrender.obj \
	lstexture.obj \
	lsvis.obj

LIBS=glu32.lib glut32.lib opengl32.lib

lscape.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\lscape.exe $(OBJS) $(LIBS)

# Build APP.C
APP_C=\
	c:\projects\glBase\opengl.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\main.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\viewpoint.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\plane.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\callback.h\
	c:\projects\glBase\freeCamera.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\callback.h\
	c:\projects\glBase\textureManager.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\opengl.h\
	c:\projects\glBase\textureFont.h\
	c:\projects\glBase\pic.h\
	c:\projects\glBase\general.h\
	$(SRCDIR)\lscape.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\cache.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\tgafile.h\
	c:\projects\glBase\general.h\

app.obj: $(APP_C) $(SRCDIR)\app.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\app.c

# Build BOUNDBOX.C
BOUNDBOX_C=\
	c:\projects\glbase\boundbox.h\
	c:\projects\glbase\frustum.h\
	c:\projects\glbase\plane.h\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\camera.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\viewpoint.h\
	c:\projects\glbase\vector.h\

boundbox.obj: $(BOUNDBOX_C) c:\projects\glbase\boundbox.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\boundbox.c

# Build CACHE.C
CACHE_C=\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\cache.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\error.h\
	c:\projects\glbase\general.h\

cache.obj: $(CACHE_C) c:\projects\glbase\cache.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\cache.c

# Build CALLBACK.C
CALLBACK_C=\
	c:\projects\glbase\callback.h\

callback.obj: $(CALLBACK_C) c:\projects\glbase\callback.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\callback.c

# Build CAMERA.C
CAMERA_C=\
	c:\projects\glbase\camera.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\viewpoint.h\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\

camera.obj: $(CAMERA_C) c:\projects\glbase\camera.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\camera.c

# Build ERROR.C
ERROR_C=\
	c:\projects\glbase\error.h\
	c:\projects\glbase\general.h\

error.obj: $(ERROR_C) c:\projects\glbase\error.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\error.c

# Build FREECAMERA.C
FREECAMERA_C=\
	c:\projects\glbase\camera.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\viewpoint.h\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\freecamera.h\
	c:\projects\glbase\camera.h\
	c:\projects\glbase\callback.h\

freecamera.obj: $(FREECAMERA_C) c:\projects\glbase\freecamera.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\freecamera.c

# Build FRUSTUM.C
FRUSTUM_C=\
	c:\projects\glbase\frustum.h\
	c:\projects\glbase\plane.h\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\camera.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\viewpoint.h\
	c:\projects\glbase\vector.h\

frustum.obj: $(FRUSTUM_C) c:\projects\glbase\frustum.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\frustum.c

# Build MAIN.C
MAIN_C=\
	c:\projects\glbase\general.h\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\error.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\main.h\

main.obj: $(MAIN_C) c:\projects\glbase\main.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\main.c

# Build OPENGL.C
OPENGL_C=\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\

opengl.obj: $(OPENGL_C) c:\projects\glbase\opengl.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\opengl.c

# Build PBUFFER.C
PBUFFER_C=\
	c:\projects\glbase\pbuffer.h\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\

pbuffer.obj: $(PBUFFER_C) c:\projects\glbase\pbuffer.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\pbuffer.c

# Build PIC.C
PIC_C=\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\tgafile.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\textureManager.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\pic.h\
	c:\projects\glbase\general.h\

pic.obj: $(PIC_C) c:\projects\glbase\pic.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\pic.c

# Build PLANE.C
PLANE_C=\
	c:\projects\glbase\plane.h\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\

plane.obj: $(PLANE_C) c:\projects\glbase\plane.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\plane.c

# Build TEXTUREFONT.C
TEXTUREFONT_C=\
	c:\projects\glbase\textureFont.h\
	c:\projects\glbase\pic.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\

texturefont.obj: $(TEXTUREFONT_C) c:\projects\glbase\texturefont.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\texturefont.c

# Build TEXTUREMANAGER.C
TEXTUREMANAGER_C=\
	c:\projects\glbase\textureManager.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\opengl.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\tgafile.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\opengl.h\

texturemanager.obj: $(TEXTUREMANAGER_C) c:\projects\glbase\texturemanager.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\texturemanager.c

# Build TGAFILE.C
TGAFILE_C=\
	c:\projects\glbase\tgafile.h\
	c:\projects\glbase\general.h\
	c:\projects\glbase\error.h\
	c:\projects\glbase\general.h\

tgafile.obj: $(TGAFILE_C) c:\projects\glbase\tgafile.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\tgafile.c

# Build VECTOR.C
VECTOR_C=\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\

vector.obj: $(VECTOR_C) c:\projects\glbase\vector.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\vector.c

# Build VIEWPOINT.C
VIEWPOINT_C=\
	c:\projects\glbase\viewpoint.h\
	c:\projects\glbase\vector.h\
	c:\projects\glbase\general.h\

viewpoint.obj: $(VIEWPOINT_C) c:\projects\glbase\viewpoint.c
	$(CC) -c $(CFLAGS) c:\projects\glbase\viewpoint.c

# Build LSCAPE.C
LSCAPE_C=\
	$(SRCDIR)\lscape.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\plane.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\viewpoint.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\cache.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\opengl.h\
	c:\projects\glBase\general.h\

lscape.obj: $(LSCAPE_C) $(SRCDIR)\lscape.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lscape.c

# Build LSCOLLIDE.C
LSCOLLIDE_C=\
	$(SRCDIR)\lscape.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\plane.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\viewpoint.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\cache.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\opengl.h\
	c:\projects\glBase\general.h\

lscollide.obj: $(LSCOLLIDE_C) $(SRCDIR)\lscollide.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lscollide.c

# Build LSRENDER.C
LSRENDER_C=\
	$(SRCDIR)\lscape.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\plane.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\viewpoint.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\cache.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\opengl.h\
	c:\projects\glBase\general.h\

lsrender.obj: $(LSRENDER_C) $(SRCDIR)\lsrender.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lsrender.c

# Build LSTEXTURE.C
LSTEXTURE_C=\
	c:\projects\glBase\error.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\tgafile.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\textureManager.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\opengl.h\
	c:\projects\glBase\general.h\
	$(SRCDIR)\lscape.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\plane.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\viewpoint.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\cache.h\
	c:\projects\glBase\general.h\

lstexture.obj: $(LSTEXTURE_C) $(SRCDIR)\lstexture.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lstexture.c

# Build LSVIS.C
LSVIS_C=\
	$(SRCDIR)\lscape.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\boundbox.h\
	c:\projects\glBase\frustum.h\
	c:\projects\glBase\plane.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\camera.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\viewpoint.h\
	c:\projects\glBase\vector.h\
	c:\projects\glBase\cache.h\
	c:\projects\glBase\general.h\
	c:\projects\glBase\error.h\
	c:\projects\glBase\general.h\

lsvis.obj: $(LSVIS_C) $(SRCDIR)\lsvis.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lsvis.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\lscape.exe $(OBJS) $(LIBS)

