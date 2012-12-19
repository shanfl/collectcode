projective grid concept demo
______________________________________________________________________________

Compiling
______________________________________________________________________________

GPU generated normals are only supported on hardware that is capable of using 
16-bit rendertargets under DirectX9. Currently this is limited to ATi's DX9-
generation of cards as far as I'm aware.. 

To compile the demo with CPU generated normals insted (like demo_NV20.exe)
use the CPU_NORMALS pre-processor directive.

You probably want to increase the grid-resolution when using CPU generated 
normals. This can be done with home/end and pgup/pgdown controls (see below). 


Controls
______________________________________________________________________________

The following controls are available.

movement:
LMB - rotate camera
MMB - zoom camera
RMB - pan camera
a/d - move left/right
s/w - move backwards/forwards
z/q - move down/up

hold left control for slower movement (x0.1)
hold left shift for faster movement (x10)

parameters:

1-6		- load preset
up/down 	- select parameter
right/left 	- increase/decrease parameter (relative: by 5%)
+/-		- increase/decrease parameter (absolute: by 0.0001)
0		- set parameter to zero

camera selection:
Although the projected grid only uses a single camera (and derives a projector 
from it) the demo has two different cameras that can be controlled independently.
The rendering camera is the one which the geometry is tessellated to match. The 
only purpose of the observing camera is to observe how the tessellation works 
from the outside.

tab - switch which camera that is controlled by the mouse (rendering/observing)
c   - switch camera used for rendering (rendering/observing)

grid detail level (does not affect GPU generated normalmap):
home/end - increase/decrase vertical resolution of CPU generated grid 
pgup/pgdwn - increase/decrase horisontal resolution of CPU generated grid

other:
e - calculate efficiency (amount of the processed vertices that end up within 
the frustum)
t - toggle text display

______________________________________________________________________________

Claes Johanson
claes@vember.net