project(simpleBumpMapping)

set(src
	"Normalisation Cube Map.cpp"
	"Normalisation Cube Map.h"
	TORUS.cpp
	TORUS.h
	main.cpp
	main.h
)

set(maths
	Maths/COLOR.cpp
	Maths/COLOR.h
	Maths/MATRIX4X4.cpp
	Maths/MATRIX4X4.h
	Maths/Maths.h
	Maths/PLANE.cpp
	Maths/PLANE.h
	Maths/VECTOR2D.cpp
	Maths/VECTOR2D.h
	Maths/VECTOR3D.cpp
	Maths/VECTOR3D.h
	Maths/VECTOR4D.cpp
	Maths/VECTOR4D.h
)

set(image 
	Image/IMAGE.h
	Image/IMAGE_CalculateStride.cpp
	Image/IMAGE_ExpandPalette.cpp
	Image/IMAGE_Load.cpp
	Image/IMAGE_Load24BitBMP.cpp
	Image/IMAGE_Load8BitBMP.cpp
	Image/IMAGE_LoadAlpha.cpp
	Image/IMAGE_LoadBMP.cpp
	Image/IMAGE_LoadCompressedTrueColorTGA.cpp
	Image/IMAGE_LoadTGA.cpp
	Image/IMAGE_LoadUncompressed8BitTGA.cpp
	Image/IMAGE_LoadUncompressedTrueColorTGA.cpp
)

set(extern
	Extensions/ARB_multitexture_extension.cpp
	Extensions/ARB_multitexture_extension.h
	Extensions/ARB_texture_cube_map_extension.cpp
	Extensions/ARB_texture_cube_map_extension.h
	Extensions/ARB_texture_env_combine_extension.cpp
	Extensions/ARB_texture_env_combine_extension.h
	Extensions/ARB_texture_env_dot3_extension.cpp
	Extensions/ARB_texture_env_dot3_extension.h
)

add_executable(bumpdemo ${src} ${maths} ${image} ${extern})
