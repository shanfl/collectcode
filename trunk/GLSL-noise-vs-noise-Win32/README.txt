This is a benchmark to compare the old GLSL Perlin noise
implementations using texture lookup, as presented by
Stefan Gustavson in 2004, with the new, purely computational
versions presented by Ian McEwan in 2011.

The bottom line is that the old version is about twice as
fast on typical current desktop GPU hardware, but the new
version can come almost for free in an otherwise texture
bandwidth limited shader, and it is expected to scale better
with massive parallelism in future GPU architectures.
It is also better suited to a pure hardware implementation.

The code is cross-platform. A Makefile is provided to
build the binary on Windows, Linux and MacOSX. For Windows
systemsm you need to isntall either MinGW32 or MSYS to
get the GCC compiler. For Windows users, the GLFW library
is supplied in binary form, and a replacement glext.h is
provided. For other systems, you need to install the
development libraries for OpenGL and the GLFW package
before compilation. Depending on your system setup, you
may also have to tweak the include paths in the Makefile.

The C code in "noisevsnoise.c" is public domain.
The shader code is distributed under the conditions of
the MIT license. See LICENSE.txt for details.

  Stefan Gustavson 2011-04-11
