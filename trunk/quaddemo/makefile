# makefile	-thatcher 10/1/1999 Copyright Thatcher Ulrich

# Makefile for quadtree heightfield sample code.

#// This code may be freely modified and redistributed.  I make no
#// warrantees about it; use at your own risk.  If you do incorporate
#// this code into a project, I'd appreciate a mention in the credits.
#//
#// Thatcher Ulrich <tu@tulrich.com>


all: quadtest.exe


.SUFFIXES : .obj .cpp

cpp = cl /c
link = link


!if "$(debug)" == "1"

cflags = -Zi
lflags = -map -debug:full -debugtype:cv

!else

cflags = -Ox
lflags = 

!endif


.cpp.obj :
	$(cpp) $(cflags) $<


OBJS = quadtest.obj quadtree.obj geometry.obj

quadtest.exe: $(OBJS)
	$(link) $(lflags) /NODEFAULTLIB:libc -out:$@ $(OBJS) glut32.lib glu32.lib opengl32.lib libcmt.lib libcpmt.lib

clean:
	rm $(OBJS)
	rm quadtest.exe
