/**
 * reflect.cpp
 * Copyright 2000 by Mark B. Allan
 * 
 * load reflection map
 */
#include "reflect.h"

#include <stdlib.h>

//#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glpng.h>

#include "RglPrims.h"

static bool mipmap = true;

static GLenum faceTarget[6] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
};

/* Pre-generated cube map images. */
char *faceFile[6] = {
  "PNG/e_left.png",
  "PNG/e_right.png",
  "PNG/e_top.png",
  "PNG/e_bottom.png",
  "PNG/e_back.png",
  "PNG/e_front.png",
};

#ifdef _WIN32
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
#endif

/*------------------------------------*/
void loadFace(GLenum target, char *filename)
{
	pngRawInfo png;
	GLenum	format;
	
	png.Data = 0;
	png.Palette = 0;
	pngLoadRaw(filename, &png);

	if(png.Palette)
	{
		fprintf(stderr, "error - \"%s\" - paletted images not supported.\n", filename);
		exit(0);
	}
	if(png.Alpha)
		format = GL_RGBA;
	else
		format = GL_RGB;

	if(mipmap)
	{
		gluBuild2DMipmaps(target, png.Components,
			png.Width, png.Height,
			format, GL_UNSIGNED_BYTE, png.Data);
	}
	else
	{
		glTexImage2D(target, 0, png.Components,
			png.Width, png.Height, 0,
			format, GL_UNSIGNED_BYTE, png.Data);
	}
	
	free(png.Data);
}


/*------------------------------------*/
void makeCubeMap(void)
{
	int i;

	for (i=0; i<6; i++) 
	{
		loadFace(faceTarget[i], faceFile[i]);
	}
	
	//-- defaults for texure unit 1
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(mipmap)
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	//-- defaults for texure unit 0
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(mipmap)
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
}

