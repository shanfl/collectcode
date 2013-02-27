#Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

Win32:
	gcc.exe -c noisevsnoise.c -o noisevsnoise.o -I. -IC:/Dev-Cpp/include -Wall -O3 -ffast-math -g3
	gcc.exe noisevsnoise.o -o noisevsnoise.exe -LC:/Dev-Cpp/lib -mwindows -lglfw -lopengl32 -lglu32 -mconsole -g3

Linux:
	gcc -I. -I/usr/X11/include noisevsnoise.c -lglfw -o noisevsnoise

MacOSX:
	cc -I. -I/usr/X11/include -c -o noisevsnoise.o noisevsnoise.c
	cc -framework Cocoa -framework OpenGL -lglfw noisevsnoise.o -o noisevsnoise
	./bundle.sh noisevsnoise.app noisevsnoise ; chmod a-x noisevsnoise

clean:
	rm -f noisevsnoise.o

distclean:
	rm -rf noisevsnoise.o noisevsnoise noisevsnoise.exe noisevsnoise.app noisevsnoise.log
