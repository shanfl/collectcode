
This directory contains source code for two programs that use the
EXT_texture_cube_map extension:

  cubemap.c       - very simple test program for cube maps
  cm_demo.c       - more interesting example with a pre-generated environment

The cm_demo example also uses the EXT_texture_lod_bias when available
to simulate dynamic control of surface dullness or shininess.

Additionally, the cm_demo.c example requires the following other
source files:

  tga.c           - simple TGA image file loader
  tga.h           - header file for the TGA image file loader
  trackball.c     - trackball quaternion code
  trackball.h     - header file for the trackball quaternion code

Here are the six cube map face images for an outdoor patio environment
that are required by cm_demo at run-time:

  cm_front.tga
  cm_back.tga
  cm_left.tga
  cm_right.tga
  cm_top.tga
  cm_bottom.tga

The EXT_texture_cube_map OpenGL specification can be found at:

  http://www.berkelium.com/OpenGL/EXT/texture_cube_map.txt

See the source code for more details and compilation instructions.
These programs will compile and run correctly on a Windows PC with an
NVIDIA GeForce 256 GPU.

- NVIDIA Software Engineering
  September 23, 1999
  
