
glBase is a framework for developing simple OpenGL applications. It uses
GLUT, so it should be fairly portable.

Homepage: http://www.futurenation.net/glbase/about.htm


===========
Features
===========

	3D maths operations
	View frustum culling
	Texture object management
	TGA file loading
	Texture mapped fonts
	Free (6DOF) camera movement
	Simple cache memory manager 
	OpenGL extension detection
	Support for pixel buffer extensions
	Support for nVidia VAR Fence extensions 

glBase is written in 'straight' C using the free compiler/IDE LCC-Win32. It
should also build ok with MS Visual C++ and Linux gcc. A lot of the
functionality in glBase is inspired by source code written by Brian Sharp
for his Gamasutra articles or by stuff in the Quake source releases from ID.

Note for Linux users: glBase support for pixel buffers and NV VAR / Fence is only
for Microsoft Windows at the moment.

The code comes with a simple demo application that uses render to texture
and projective texture mapping to create a rippling water reflection effect.

There is a detailed description of how the demo works, and some performance
stats, at http://www.future-nation.net/glbase/reflect.htm


===========
Download
===========

Three zip archives are available for download at 
http://www.futurenation.net/glbase/downloads.htm

glbase.zip (279 Kb) - glBase library and demo app source code.
glbexe.zip (237 Kb) - glBase demo app prebuilt exe (MSVC6 build).
raw2tga.zip (17 Kb) - raw3tga exe for converting raw files to TGA format.


===========
Pre-requisites
===========

To just run the pre-built Windows executable you will need the prebuilt
GLUT DLL from either of the distros described below.

To build with MS VC++:
You need the prebuilt GLUT libraries for Win32 which can be downloaded by
following the link here:
http://www.opengl.org/developers/documentation/glut/#windows
MS VC++ build has been verified with MS VC++ Version 6.0.

To build with LCC-Win32:
LCC-Win32 uses a different object library format to MS Visual C++. A prebuilt 
GLUT 3.6 for LCC-Win32 can be downloaded from
http://www.geocities.com/SiliconValley/Station/1177/toolchst/lccglut36.zip. 
Unpack the zip file and copy all .h files to drive:\lcc\include\gl, 
all .lib files to drive:\lcc\lib, all .exp files to drive:\lcc\buildlib 
and all DLL files to drive:\windows\system.
LCC-Win32 build has been verified with LCC-Win32 Version 3.3


===========
Running the demo
===========

The prebuilt demo has been tested on the following graphics cards
 - Sparkle geForce2 Ti
 - 3dfx Voodoo3 3000

Use the mouse to look up/down/left/right and change direction.
Mouse left button = move forward.
Mouse right button = move backward.
Keys:
  u : Moves the ball up.
  d : Moves the ball down.
  r : Toggles render to texture on/off.
  p : Toggles pixel buffer use on/off (if your OpenGL supports it).
  m : Toggles use of the make current read extension on/off if the pixel
      buffer is in use (this was an experiment to try and punt Radeon
      cards onto the optimised path for glCopyTexSubImage).
  f : Toggles between windowed and full screen mode.
  s : Dumps a screen shot into c:\temp in a 24bpp raw file (use raw2tga to
      convert to a TGA file).
esc : Exits.


===========
Change Log
===========

28 May 2001:
	- Added WGL_pixel_buffer_ARB support to glBase (pbuffer.c
	  & pbuffer.h).
	- Modified the demo to use a pbuffer, if available, for
	  updating the reflection texture.
	- Fixed a bug in the demo when using the back buffer for
	  updating the reflection texture. The size of the used area
	  of the reflection texture shouldn't exceed the size of the
	  buffer.
13 May 2001:
	- Texture Manager class was using GL_RGB8 & GL_RGBA8, now
	  uses GL_RGB & GL_RGBA so that internal texture format
	  should match frame buffer format by default.
	- Texture Mapped Font string functions were changing texture
	  env mode to replace.
11 June 2001:
	- There was a problem with pic.c in that we assume an
	  orthogonal projection with 0,0 in the top left of the
	  screen but were drawing clockwise quads for this coord
	  system - fixed now to draw CCW quads (tested draw char
	  but not draw pic).
04 February 2002:
	- Added simple cache memory manager.
	- Fixed pixel buffer support so no longer requires make
	  current read extension - works ok on geForce now.
	- Better control of texture internal formats in texture
	  manager class.
	- Added support for NV VAR/Fence.
11 February 2002:
	- Improved VAR/Fence support in cache.c
25 February 2002:
	- Added ARB_texture_env_combine to opengl.c & opengl.h.
17 Mar 2002:
	- Fixed constant string overwrite problem in app.c (caused demo
	  to crash when built with MSVC6).
	- Replaced MSVC5 project files with MSVC6 project files.
	- Thanks to Ben Discoe of www.vterrain.org for new project files and
	  debugging with MSVC6.


glBase readme.txt, 17 March 2002

This code is released under the terms of the GPL (gnu public license) -
http://www.gnu.org/copyleft/gpl.html.
