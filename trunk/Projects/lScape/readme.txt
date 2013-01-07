
lScape - Landcsape rendering using geomipmapping.

Homepage: http://www.futurenation.net/glbase/projects.htm#lScape


===========
Features
===========

Continuous level of detail using GeoMipMapping 
Texture splatting 
Hierarichical occlusion culling 
Collision detection 
Quadtree based frustum culling 
Accelerated rendering with NV Var Fence where supported by ICD 
Accelerated rendering with ARB_texture_env_combine where supported by ICD

The geomipmapping technique is based on a paper by Willem H. de Boer:
http://www.flipcode.com/tutorials/tut_geomipmaps.shtml

The hierarchical visibility technique is based on a paper by 
A. James Stewart:
http://www.cs.queensu.ca/home/jstewart/papers/egwr97.html

The texture splatting technique is based on a paper by Charles Bloom:
http://cbloom.com/3d/techdocs/splatting.txt


===========
Download
===========

Three zip archives are available for download at 
http://www.futurenation.net/glbase/downloads/lscape/

lscexe.zip (41 Kb) - Prebuilt Win32 executable (MSVC6 build).
lscdat.zip (2.6Mb) - Data files needed at runtime.
lscsrc.zip (55 Kb) - Source code.


===========
Pre-requisites
===========

To just run the pre-built Windows executable you will need the prebuilt
GLUT DLL from either of the distros described below.

To build the source you need the glBase library source code available at:
http://www.futurenation.net/glbase/downloads.htm#glBaseSrc
(you need last update 25 February 2002 or later)

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
 - Sparkle geForce2 Ti (no issues)
 - 3dfx Voodoo3 3000 (works but texture splatting mode just produces a
   horrible beige colour!).

NOTE: For best results set screen mode to 32 bit colours. With 16 bit colours
texture artefacts appear in splatting mode which become more extreme using the
texture_env_combine splatting mode. These artifacts disappear in 32 bit screen
mode.

Use the mouse to look up/down/left/right and change direction.
Mouse left button = move forward.
Mouse right button = move backward.
Keys:
  i : Toggles info display on/off.
  m : Change rendering mode (line mode, texture mode, texture splat mode).
  v : Toggles 'god' view on/off.
  o : Toggles hierarichical occlusion culling on/off.
  c : Toggles collision detection on/off.
  t : Toggles use of ARB_texture_env_combine extension for texture splatting.
  f : Toggles between windowed and full screen mode.
  s : Dumps a screen shot into c:\temp in a 24bpp raw file (use raw2tga to
      convert to a TGA file).
esc : Exits.

Note that when the program runs for the first time it will create two new
files in the data subdirectory: Colour512.raw and UnlitColour512.raw. This
will only take a few seconds.

It should be possible to run the demo for a completely different landscape
by doing the following.
 - Remove Colour512.raw and UnlitColour512.raw from the data directory.
 - Remove the bundled visibility data file Height512.vis.
 - Replace the bundled heightmap file Height512.tga with your own heightmap
   (uncompressed monochrome 8bpp TGA format).
 - Optionally replace any of the bundled texture.tga or colour.tga files 
   with your own textures (uncompressed RGB 24bpp TGA format).
 - Run the program.
 - Wait a very long time while the visibility data (Height512.vis) is
   recalculated, this is done only once (leave running overnight).

NOTE that this procedure is untested with the current version of the code!


===========
Change Log
===========

25 Feb 2002:
	- Improved texture splatting, now requires:
		1 pass per splat texture plus 1 pass for the colourmap if
		ARB_texture_env_combine not available,
	  or
		1 pass per splat texture if ARB_texture_env_combine available.
02 Mar 2002:
	- Fixed problems running on Voodoo3.
	- Fixed build problems with LCC-Win32.
17 Mar 2002:
	- Fixed constant string overwrite problems in app.c and lstexture.c
	  (these caused lScape to crash when built with MSVC6).
	- Replaced MSVC5 project files with MSVC6 project files.
	- Thanks to Ben Discoe of www.vterrain.org for new project files and
	  debugging with MSVC6.


lScape readme.txt, 17 March 2002

This code is released under the terms of the GPL (gnu public license) -
http://www.gnu.org/copyleft/gpl.html.
