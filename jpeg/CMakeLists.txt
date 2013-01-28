project(jpeg)
set(src
	JPEG_compression.cpp
	jtypes.h
	jtables.h
	jglobals.h
)
add_executable(jpeg_enc ${src})