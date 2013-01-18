CXX=c++
#OpenGL = -lMesaGL
OpenGL = -lGL
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,%.o, $(SRC))

all: projspot

projspot: ${OBJ}
	c++ -D__INTEL__ -o projspot *.o -L/usr/X11R6/lib -lglut -lGLU ${OpenGL} -lXmu -lXi -lX11 -lm -g

%.o: %.cpp
	${CXX} -D__INTEL__ -I../../glh -I. -g -c $<

clean:
	-rm -f *.o
	-rm -f projspot
	-rm -f *~
	-rm -f *.ncb
	-rm -f *.opt
	-rm -f *.plg
	-rm -rf Debug
