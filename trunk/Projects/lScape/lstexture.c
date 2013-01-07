/*
Copyright (C) 2000-2001 Adrian Welbourn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/*
	File:		lstexture.c

	Function:	Landscape texturing and lighting functions.
	
	AW TODO:	This just evolved over time rather than being designed.
				Probably needs a full rewrite.
*/

#include "error.h"
#include "tgafile.h"
#include "textureManager.h"
#include "lscape.h"
#include <limits.h>

/*
  lightMapPut()

  Put a pixel value into the light map.
*/
static void lsc_lightMapPut(unsigned char *lightMap, int size,
							int x, int y, unsigned char val)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	lightMap[x + y*size] = val;
}

/*
  lightMapGet()

  Get a pixel value from the light map.
*/
static unsigned char lsc_lightMapGet(unsigned char *lightMap, int size,
									 int x, int y)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	return lightMap[x + y*size];
}

/*
  normalMapGet()

  Get a normal from the normal map.
*/
static unsigned char *lsc_normalMapGet(unsigned char *normalMap, int size,
									   int x, int y)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	return &normalMap[3*(x + y*size)];
}

/*
  shadowMapPut()

  This is a helper function for drawing into the shadow map. This function
  is used when casting a ray from a heightmap point. If the height of the 
  heightmap at point x, y is less than the height z of the ray at point x, y
  then a full darkness shadow pixel is drawn.
*/
static void lsc_shadowMapPut(unsigned char *shadowMap,
							 unsigned char *heightMap, int size,
							 int x, int y, float z)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	if (heightMap[x + y*(size + 1)] <= z)
		shadowMap[x + y*size] = 255;
}

/*
  calcBumps()

  This calculates the bump map that is used to apply some noise to the normal
  map. This isn't a bump map in the usual graphics sense, its just random 
  noise that is used to vary the texture applied to flat areas of the map as
  if the terrain were more bumpy.
*/
static unsigned char *lsc_calcBumps(int size)
{
	unsigned char *bumpMap = (unsigned char*)malloc(SQR(size));
	int i, j;
	float p;
	
	if (!bumpMap) {
		err_report("lsc_calcBumps: cannot allocate %d bytes", SQR(size));
		return bumpMap;
	}

	for (j = 0; j < size; j++) {
		for (i = 0; i < size; i++) {
			p = (float)rand() / (float)RAND_MAX;
			if (p > 1.0f)
				p = 1.0f;
			bumpMap[j*size + i] = (unsigned char)(255.0f * p);
		}
	}

	return bumpMap;
}

/*
  calcNormals()

  Calculate a normal for each heightmap vertex by calculating a normal for
  each of the eight triangles sharing the vertex and averaging. The three 
  components of the normal are scaled to the range +-127 and stored as signed
  char values.

  If randScale and frequency are both greater than zero, a bump map (above)
  is generated with one point for every frequency points of the normal map,
  e.g. if the normal map is n x n and frequency is 4, the bump map will be 
  n/4 x n/4 and will then be interpolated across the normal map, smoothing the
  noise out. The cutoff parameter is the lower limit to normal.z above which
  noise is applied. This allows noise to only be applied to the flattest
  parts of the map if required.

  The bump map is deleted on completion.
*/
static char *lsc_calcNormals(unsigned char *heightMap, int size, vec3 scale,
							 float randScale, float cutoff, int frequency)
{
	unsigned char *bumpMap = NULL;
	int bumpMapSize;
	float scaleBump = randScale / 255.0f;
	float invFreq = 1.0f;
	char *normalMap;
	int X, Y, i, j;
	vec3 normal, temp, temp0, temp1;
	
	/* 
	 * Eight vectors connecting a point in the 
	 * heightmap to each of its neighbours.
	 */
	float v[24] = { -1, -1,  0,
					 0, -1,  0,
					 1, -1,  0,
					 1,  0,  0,
					 1,  1,  0,
					 0,  1,  0,
					-1,  1,  0,
					-1,  0,  0 };

	printf("Calculating normal map");
	fflush(stdout);

	/* Calculate the bumpmap */
	if (randScale > 0.0f && frequency > 0) {
		if (frequency > size)
			frequency = size;
		bumpMapSize = size / frequency;
		bumpMap = lsc_calcBumps(bumpMapSize);
		invFreq = 1.0f / frequency;
	}
	
	/* Allocate space for the normal map */
	normalMap = (char*)malloc(3 * SQR(size));
	if (!normalMap) {
		err_report("lsc_calcNormals: cannot allocate %d bytes", 3 * SQR(size));
		if (bumpMap)
			free(bumpMap);
		return normalMap;
	}
	
	/* For each point in the heightmap */
	for (Y = 0; Y < size; Y++) {

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < size; X++) {
			
			/* Get the eight vectors connecting 
			   this point to its neighbours */
			for (i = 0; i < 8; i++) {
				int x, y, z;

				/* Assume size is a power of two */
				x = (X + (int)(v[3*i + 0])) & (size - 1);
				y = (Y + (int)(v[3*i + 1])) & (size - 1);
				z = (int)heightMap[x + y*(size + 1)]
					- (int)heightMap[X + Y*(size + 1)];
				v[3*i + 2] = (float)z;
			}
			
			/* Calculate the eight cross products and average */
			for (i = 0; i < 3; i++)
				normal[i] = 0;
			for (i = 0, j = 1; i < 8; i++, j++) {
				if (j >= 8)
					j = 0;
				vec3_mul((&v[i*3]), scale, temp0);
				vec3_mul((&v[j*3]), scale, temp1);
				vec3_cross(temp0, temp1, temp);
				/* Make sure the normal points up */
				if (temp[2] < 0.0f)
					vec3_mulS(temp, -1.0f, temp);
				vec3_add(normal, temp, normal);
			}
			vec3_norm(normal);
			
			/* If we have a bump map and normal.z is above
			   the threshold, apply some noise */
			if (normal[2] >= cutoff && bumpMap) {
				float fbX = (float)X * invFreq;
				float fbY = (float)Y * invFreq;
				int bX0 = (int)floor(fbX);
				int bX1 = (int)ceil(fbX);
				int bY0 = (int)floor(fbY);
				int bY1 = (int)ceil(fbY);
				float b;
				
				if (bX0 == bX1 && bY0 == bY1) {
					/* Heightmap point corresponds to an 
					   exact bump map point */
					b = lsc_lightMapGet(bumpMap, bumpMapSize, bX0, bY0);
				}
				else {
					/* Heightmap point falls between bump
					   map points, interpolate */
					float scale;
					float b00 = lsc_lightMapGet(bumpMap, bumpMapSize, bX0, bY0);
					float b01 = lsc_lightMapGet(bumpMap, bumpMapSize, bX1, bY0);
					float b10 = lsc_lightMapGet(bumpMap, bumpMapSize, bX0, bY1);
					float b11 = lsc_lightMapGet(bumpMap, bumpMapSize, bX1, bY1);
					float b0 = b00;
					float b1 = b10;
					
					if (bX0 != bX1) {
						/* Lerp x */
						scale = (float)bX1 - fbX;
						b0 *= scale;
						b1 *= scale;
						scale = fbX - (float)bX0;
						b0 += scale * b01;
						b1 += scale * b11;
					}
					
					b = b0;
					if (bY0 != bY1) {
						/* Lerp y */
						scale = (float)bY1 - fbY;
						b *= scale;
						scale = fbY - (float)bY0;
						b += scale * b1;
					}
				}
				
				/* Apply noise and re-normalize */
				b *= scaleBump;
				normal[2] -= b;

/*
  Assuming z is near 1 and x, y and noise b are small compared to z, 
  we have to add something to x & y otherwise when we renormalize, 
  z returns to near its previous value. The something added to x & y 
  should tend to keep the normal's length unchanged so that when we 
  renormalize, z retains its adjusted value.

  Since

    x^2 + y^2 + z'^2  approx=  x^2 + y^2 + z^2 - 2b

  if we add sqrt(b) to x and y we get

    x'^2 + y'^2 + z'^2  approx=  x^2 + b + y^2 + b + z^2 - 2b 
			      =  x^2 + y^2 + z^2

  which is what we want.
*/
				b = sqrt(b);
				normal[0] += b;
				normal[1] += b;
				if (normal[2] < 0.0f)
					normal[2] = 0.0f;
				vec3_norm(normal);
			}

			/* Save the normal in the normal map */
			normalMap[3*(X + Y*size) + 0] = (char)(127.0f * normal[0]);
			normalMap[3*(X + Y*size) + 1] = (char)(127.0f * normal[1]);
			normalMap[3*(X + Y*size) + 2] = (char)(127.0f * normal[2]);
		}
	}
	
	/* Delete the bump map */
	if (bumpMap)
		free(bumpMap);
	
	printf("\n");
	fflush(stdout);

	return normalMap;
}

/*
  calcShadows()

  For each point in the heightmap we cast a ray in the light direction. Any 
  points intersected in x-y which are below the ray in z are deemed to be in
  shadow.
*/
static unsigned char *lsc_calcShadows(unsigned char *heightMap, int size,
											vec3 scale, vec3 dir)
{
	const float scaleNormal = 1.0f / 127.0f;
	unsigned char *shadowMap;
	vec3 L;
	int X, Y;
	
	printf("Calculating shadow map");
	fflush(stdout);

	/* Allocate space for the shadow map */
	shadowMap = (unsigned char*)malloc(SQR(size));
	if (!shadowMap) {
		err_report("lsc_calcShadows: cannot allocate %d bytes", SQR(size));
		return shadowMap;
	}
	memset(shadowMap, 0, SQR(size));
	
	/* Make sure the light source is normalised */
	vec3_cpy(dir, L);
	vec3_div(L, scale, L);
	vec3_norm(L);
	if (L[2] == 0.0f) {
		/* Pathological case */
		err_report("lsc_calcShadows: light vector horizontal");
		return shadowMap;
	}
	
	/* For each heightmap vertex */
	for (Y = 0; Y < size; Y++) {

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < size; X++) {
			float z;
			
			/* If vertex already in shadow ignore it */
			if (shadowMap[X + Y*size])
				continue;
			
			/* Step along a line through the vertex in the direction of L */

			z = (float)heightMap[X + Y*(size + 1)];
			if (fabs(L[0]) < fabs(L[1])) {
				float incx = L[0] / L[1];
				float incz = L[2] / L[1];
				int y, incy = 1;
				float x;
				if (L[1] < 0) {
					incx = -incx; incy = -incy, incz = -incz;
				}
				x = X + incx;
				z += incz;
				for (y = Y + incy; 1; x += incx, y += incy, z += incz) {
					if (z < 0.0f)
						break;
					lsc_shadowMapPut(shadowMap, heightMap, size, (int)x, y, z);
				}
			}
			else {
				float incy = L[1] / L[0];
				float incz = L[2] / L[0];
				int x, incx = 1;
				float y;
				if (L[0] < 0) {
					incx = -incx; incy = -incy, incz = -incz;
				}
				y = Y + incy;
				z += incz;
				for (x = X + incx; 1; x += incx, y += incy, z += incz) {
					if (z < 0.0f)
						break;
					lsc_shadowMapPut(shadowMap, heightMap, size, x, (int)y, z);
				}
			}
		}
	}
	
	printf("\n");
	fflush(stdout);

	return shadowMap;
}

/*
  blurLightMap()

  Applies a simple blurring filter to the light map. The blurring filter is
  first weighted according to the light direction.
*/
static void lsc_blurLightMap(unsigned char *lightMap, int size, vec3 dir)
{
	char *lightMap2;
	int blurMap[9] = { 64,  64,  64,
					   64, 255,  64, 
					   64,  64,  64 };
	int X, Y, i, divisor = 0;
	
	lightMap2 = (unsigned char*)malloc(SQR(size));
	if (!lightMap2) {
		err_report("lsc_blurLightMap: cannot allocate %d bytes", SQR(size));
		return;
	}
	memset(lightMap2, 0, SQR(size));
	
	if (dir[0] > 0.6) {
		blurMap[2] = blurMap[5] = blurMap[8] = 128;
	}
	else if (dir[0] < 0.6) {
		blurMap[0] = blurMap[3] = blurMap[6] = 128;
	}
	if (dir[1] > 0.6) {
		blurMap[0] = blurMap[1] = blurMap[2] = 128;
	}
	else if (dir[1] < 0.6) {
		blurMap[6] = blurMap[7] = blurMap[8] = 128;
	}
	
	for (i = 0; i < 9; i++)
		divisor += blurMap[i];
	
	/* For each heightmap vertex */
	for (Y = 0; Y < size; Y++) {
		for (X = 0; X < size; X++) {
			int x, y, accum = 0;
			for (y = 0; y < 3; y++) {
				for (x = 0; x < 3; x++) {
					accum += blurMap[x + 3*y]
						* lsc_lightMapGet(lightMap, size, X+x-1, Y+y-1);
				}
			}
			accum /= divisor;
			if (accum > 255)
				accum = 255;
			lsc_lightMapPut(lightMap2, size, X, Y, (unsigned char)accum);
		}
	}
	memcpy(lightMap, lightMap2, SQR(size));
	free(lightMap2);
}

/*
  calcLighting()

  This uses the normal map and shadow map to calculate a light map for the
  landscape. A light intensity is calculated for each point in the heightmap,
  scaled to the range 0 to 255 and stored as an unsigned char. We then apply
  a blurring filter to soften the shadow edges.
*/
static unsigned char *lsc_calcLighting(unsigned char *normalMap, 
									   unsigned char *shadowMap, int size,
									   vec3 scale, float amb, float diff,
									   vec3 dir)
{
	const float scaleNormal = 1.0f / 127.0f;
	const float scaleShadow = 1.0f / 255.0f;
	unsigned char *lightMap;
	int X, Y;
	vec3 L, normal;
	
	printf("Calculating light map");
	fflush(stdout);

	/* Allocate space for the light map */
	lightMap = (unsigned char*)malloc(SQR(size));
	if (!lightMap) {
		err_report("lsc_calcLighting: cannot allocate %d bytes", SQR(size));
		return lightMap;
	}
	
	/* Make sure the light source is normalised */
	vec3_cpy(dir, L);
	vec3_div(L, scale, L);
	vec3_norm(L);
	vec3_mulS(L, -1.0f, L);
	
	/* For each point */
	for (Y = 0; Y < size; Y++) {

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < size; X++) {
			float i, s, d;

			/* Get the dot product with the light source direction */
			normal[0] = normalMap[3*(X + Y*size) + 0];
			normal[1] = normalMap[3*(X + Y*size) + 1];
			normal[2] = normalMap[3*(X + Y*size) + 2];
			vec3_mulS(normal, scaleNormal, normal);
			d = vec3_dot(L, normal);
			if (d < 0.0f)
				d = 0.0f;
			
			/* Calculate illumination model (ambient plus diffuse) */
			s = 1.0f;
			if (shadowMap)
				s = 1.0f - scaleShadow * shadowMap[X + Y*size];
			i = amb + s * d * diff;
			if (i < 0.0f)
				i = 0.0f;
			else if (i > 1.0f)
				i = 1.0f;
			
			/* Save in light map */
			lightMap[X + Y*size] = (unsigned char)(i * 255.0f);
		}
	}

	/* Blur the light map */
	printf("blurring");
	fflush(stdout);
	lsc_blurLightMap(lightMap, size, L);
	lsc_blurLightMap(lightMap, size, L);

	printf("\n");
	fflush(stdout);

	return lightMap;
}

/*
  calcTextures()

  mode == 0:
  ---------

  This calculates one big texture map for the entire landscape, referred to as
  the colourmap.

  Up to ten texture tga files are loaded and scaled in size by the factor
  texScale. The files must be named colourn.tga, where n = 0 through 9. The
  file with the lowest n is taken to represent the flattest terrain. The file
  with the highest n is taken to represent the steepest terrain. E.g.

    colour0    grass
    colour1	   mud
    colour2	   sand
    colour3	   rock

  One big texture map, size (texPerSide x texSize)^2 is allocated in memory.
  For each point in the texture map a corresponding point in the heightmap 
  is found and the values of normal.z and light are read from the normal and 
  light maps (or more accurately, because the texture map point may lie
  between heightmap points, the values of normal.z & light are linearly 
  interpolated from the surrounding 4 heightmap points). The normal.z is 
  converted to a gradient which is then scaled into the range 0 to N-1, where 
  N is the number of texture maps read. The scaled gradient is then used as an
  index to the required texture file from which a texel is copied to the 
  heightmap texture (or if the scaled gradient is non-integer, two texels are
  read from different texture files and the final texel is interpolated). If 
  the scaled up texture files are smaller than the colourmap they will be
  tiled. Finally the texel value may be scaled by the light value (can be
  enabled / disabled with the prelit flag).

  The colourmap is created in GL_RGB format (24bpp).


  mode == 1:
  ---------

  This calculates the blend maps used for alpha blended texture splatting.

  Several blend maps are allocated sequentially in memory, each size
  hmSize x hmSize. The number of blend maps created depends on the textures to
  be used in texture splatting. The texture files must be named texturen.tga,
  where n = 0 through 9. The file with the lowest n is taken to represent the
  flattest terrain. The file with the highest n is taken to represent the
  steepest terrain. E.g.

    texture0    grass
    texture1    mud
    texture2    sand
    texture3    rock

  One blend map is created for each texture map, plus one blend map for the
  light map (e.g. 4 textures produces one light map plus 4 blend maps).

  The blend maps can be created in one of two formats, GL_ALPHA (8bpp) or
  GL_RGBA (32bpp), depending on the value of the colourmap argument. If 
  colourmap is null, GL_ALPHA format is used. If colourmap is non-null, it
  is interpreted as a pointer to the colourmap created by mode 0 (above). In
  this case the blend maps are created with GL_RGBA format and the colourmap
  is copied into the RGB channels of each blend map.

*/
static unsigned char *lsc_calcTextures(int mode, bool prelit,
									   unsigned char *heightMap,
									   char *normalMap,
									   unsigned char *lightMap,
									   unsigned char *colourMap,
									   int hmSize,
									   int texPerSide, int texSize, 
									   float texScale, int *numTextures)
{
	const float scaleNormal = 1.0f / 127.0f;
	const float scaleLights = 1.0f / 255.0f;
	
	char filename[64];
	int countLoaded = 0;
	int size[10];
	unsigned char *data[10];
	unsigned char *textureMap;
	int textureMapSize, i, X, Y;
	float invTexScale = 1.0f / texScale;

	/* Number of texture maps created */
	*numTextures = 0;

	/* Load up to ten texture files, must be square */
	printf("Read textures");
	fflush(stdout);

	for (i = 0; i < 10; i++) {
		tga f;
		
		/* Show progress */
		printf(".");
		fflush(stdout);

		if (mode == 0)
			/* If colour%d.tga found */
			sprintf(filename, "data/colour%d.tga", i);
		else
			/* If texture%d.tga found and is square */
			sprintf(filename, "data/texture%d.tga", i);
		f = tga_create(filename);
		if (f && f->data && f->width == f->height
			&& (f->depth == 24 || f->depth == 32)) {

			if (mode == 0) {

				if (f->depth == 32)
					tga_stripAlpha(f);
			
				/* Copy into memory, scaling if necessary */
				size[countLoaded] = (int)(f->width * texScale);
				data[countLoaded] = (unsigned char*)malloc(
									3*SQR(size[countLoaded]));
				if (!data[countLoaded]) {
					err_report("lsc_calcTextures: cannot allocate %d bytes",
									3*SQR(size[countLoaded]));
					continue;
				}
				if (size[countLoaded] == (int)f->width) {
					/* Copy texture to memory 1:1 */
					memcpy(data[countLoaded], f->data, 3*SQR(size[countLoaded]));
				}
				else {
					/* Copy texture into memory, scaling by texScale */
					int j, k;
					for (k = 0; k < size[countLoaded]; k++) {
						float fsk = k * invTexScale;
						int sk0 = (int)floor(fsk);
						int sk1 = (int)ceil(fsk);
						if (sk0 >= (int)f->width)
							sk0 = (int)f->width - 1;
						if (sk1 >= (int)f->width)
							sk1 = (int)f->width - 1;
						for (j = 0; j < size[countLoaded]; j++) {
							float fsj = j * invTexScale;
							int sj0 = (int)floor(fsj);
							int sj1 = (int)ceil(fsj);
							if (sj0 >= (int)f->width)
								sj0 = (int)f->width - 1;
							if (sj1 >= (int)f->width)
								sj1 = (int)f->width - 1;
							if (sk0 == sk1 && sj0 == sj1) {
								memcpy(&data[countLoaded][3*(k*size[countLoaded]+j)],
									&(f->data[3*(sk0*f->width+sj0)]), 3);
							}
							else {
								/* Lerp texels */
								unsigned char ctex[3];
								float scale;
								int l;
								float tex00[3], tex01[3], tex10[3], tex11[3];
								float tex0[3], tex1[3];
								float tex[3];
								memcpy(tex00, &(f->data[3*(sk0*f->width+sj0)]), 3);
								memcpy(tex01, &(f->data[3*(sk0*f->width+sj1)]), 3);
								memcpy(tex10, &(f->data[3*(sk1*f->width+sj0)]), 3);
								memcpy(tex11, &(f->data[3*(sk1*f->width+sj1)]), 3);
								memcpy(tex0, tex00, 3);
								memcpy(tex1, tex10, 3);
								if (sj0 != sj1) {
									/* Lerp j */
									scale = (float)sj1 - fsj;
									for (l = 0; l < 3; l++) {
										tex0[l] *= scale;
										tex1[l] *= scale;
									}
									scale = fsj - (float)sj0;
									for (l = 0; l < 3; l++) {
										tex0[l] += scale * tex01[l];
										tex1[l] += scale * tex11[l];
									}
								}
								memcpy(tex, tex0, 3);
								if (sk0 != sk1) {
									/* Lerp k */
									scale = (float)sk1 - fsk;
									for (l = 0; l < 3; l++)
										tex[l] *= scale;
									scale = fsk - (float)sk0;
									for (l = 0; l < 3; l++)
										tex[l] += scale * tex1[l];
								}
								for (l = 0; l < 3; l++) {
									if (tex[l] < 0.0f)
										tex[l] = 0.0f;
									else if (tex[l] > 255.0f)
										tex[l] = 255.0f;
									ctex[l] = (unsigned char)(tex[l]);
								}
								memcpy(&data[countLoaded][3*(k*size[countLoaded]+j)],
												ctex, 3);
							}
						}
					}
				}
			}

			countLoaded++;
		}
		
		if (f)
			tga_destroy(f);
	}

	if (mode == 0) {
	
		printf("\nMerge textures");
		fflush(stdout);

		/* Now allocate memory for the colourmap */
		textureMapSize = texPerSide * texSize;
		textureMap = (unsigned char*)malloc(3*SQR(textureMapSize));
		if (!textureMap) {
			err_report("lsc_calcTextures: cannot allocate %d bytes",
				3*SQR(textureMapSize));
			for (i = 0; i < countLoaded; i++)
				free(data[i]);
			return textureMap;
		}

		/* Number of texture maps created */
		*numTextures = 1;
	}
	else {

		/* mode == 1 */

		printf("\nGenerate blend maps");
		fflush(stdout);

		/* AW TODO: currently the engine can only handle three splat textures */
		if (countLoaded > 3)
			countLoaded = 3;

		/* One to one for blend maps */
		textureMapSize = hmSize;
		if (colourMap) {
			textureMap = (unsigned char*)malloc(4*(countLoaded+1)*SQR(textureMapSize));
			if (!textureMap) {
				err_report("lsc_calcTextures: cannot allocate %d bytes",
					4*countLoaded*SQR(textureMapSize));
				return textureMap;
			}
		}
		else {
			textureMap = (unsigned char*)malloc((countLoaded+1)*SQR(textureMapSize));
			if (!textureMap) {
				err_report("lsc_calcTextures: cannot allocate %d bytes",
					countLoaded*SQR(textureMapSize));
				return textureMap;
			}
		}

		/* Number of texture maps created */
		*numTextures = countLoaded + 1;
	}
	
	/* Tile the source textures into the colourmap based on height map
	   gradient, i.e. the size of the height map normal's z component. */
	for (Y = 0; Y < textureMapSize; Y++) {
		
		/* Scale y into the height map */
		float fhY = (float)Y * (float)hmSize / (float)textureMapSize;
		int hY0 = (int)floor(fhY);
		int hY1 = (int)ceil(fhY);
		
		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < textureMapSize; X++) {
			float r, g, b;

			/* Scale x into the height map */
			float fhX = (float)X * (float)hmSize / (float)textureMapSize;
			int hX0 = (int)floor(fhX);
			int hX1 = (int)ceil(fhX);
			
			/* Get gradient & light, lerping if necessary */
			float grad;
			float light;
			if (hX0 == hX1 && hY0 == hY1) {
				grad = lsc_normalMapGet(normalMap, hmSize, hX0, hY0)[2];
				light = lsc_lightMapGet(lightMap, hmSize, hX0, hY0);
			}
			else {
				float scale;
				float grad00 = lsc_normalMapGet(normalMap, hmSize, hX0, hY0)[2];
				float grad01 = lsc_normalMapGet(normalMap, hmSize, hX1, hY0)[2];
				float grad10 = lsc_normalMapGet(normalMap, hmSize, hX0, hY1)[2];
				float grad11 = lsc_normalMapGet(normalMap, hmSize, hX1, hY1)[2];
				float light00 = lsc_lightMapGet(lightMap, hmSize, hX0, hY0);
				float light01 = lsc_lightMapGet(lightMap, hmSize, hX1, hY0);
				float light10 = lsc_lightMapGet(lightMap, hmSize, hX0, hY1);
				float light11 = lsc_lightMapGet(lightMap, hmSize, hX1, hY1);
				float grad0 = grad00;
				float grad1 = grad10;
				float light0 = light00;
				float light1 = light10;
				if (hX0 != hX1) {
					/* Lerp x */
					scale = (float)hX1 - fhX;
					grad0 *= scale;
					grad1 *= scale;
					light0 *= scale;
					light1 *= scale;
					scale = fhX - (float)hX0;
					grad0 += scale * grad01;
					grad1 += scale * grad11;
					light0 += scale * light01;
					light1 += scale * light11;
				}
				grad = grad0;
				light = light0;
				if (hY0 != hY1) {
					/* Lerp y */
					scale = (float)hY1 - fhY;
					grad *= scale;
					light *= scale;
					scale = fhY - (float)hY0;
					grad += scale * grad1;
					light += scale * light1;
				}
			}
			/* Scale grad to range 0 to 1 */
			grad *= scaleNormal;
			
			/*
			 * Convert grad from normal.z to true gradient in range 0 to 1
			 */
			grad = 2.0f * acos(grad) / M_PI;
			if (grad < 0.0f)
				grad = 0.0f;
			else if (grad > 1.0f)
				grad = 1.0f;
			
			/* Scale light to range 0 to 1 */
			light *= scaleLights;
			if (light < 0.0f)
				light = 0.0f;
			else if (light > 1.0f)
				light = 1.0f;

			if (mode == 0) {
			
				/* Pixel colour if no textures loaded */
				r = 200.0;
				g =  25.0;
				b = 200.0;
			
				/* Use the gradient to get indexes to the source textures */
				if (countLoaded) {
					float fIndex = grad * (float)(countLoaded - 1);
					int Index0 = (int)floor(fIndex);
					int Index1 = (int)ceil(fIndex);
					int sourceSize = size[Index0];
					int sourceX = X%sourceSize;
					int sourceY = Y%sourceSize;
					
					r = data[Index0][3*(sourceY*sourceSize + sourceX) + 0];
					g = data[Index0][3*(sourceY*sourceSize + sourceX) + 1];
					b = data[Index0][3*(sourceY*sourceSize + sourceX) + 2];
					
					if (Index0 != Index1) {
					
						/* Lerp the texture between Index0 and Index1 */
						float r2, g2, b2;
						float scale = (float)Index1 - fIndex;
						r *= scale;
						g *= scale;
						b *= scale;
					
						sourceSize = size[Index1];
						sourceX = X%sourceSize;
						sourceY = Y%sourceSize;
					
						r2 = data[Index1][3*(sourceY*sourceSize + sourceX) + 0];
						g2 = data[Index1][3*(sourceY*sourceSize + sourceX) + 1];
						b2 = data[Index1][3*(sourceY*sourceSize + sourceX) + 2];
					
						scale = fIndex - (float)Index0;
					
						r += r2 * scale;
						g += g2 * scale;
						b += b2 * scale;
					}
				}
				
				if (prelit) {
					/* Apply lighting model */
					r *= light;
					g *= light;
					b *= light;
				}

				/* Range check */
				if (r < 0.0f)
					r = 0.0f;
				else if (r > 255.0f)
					r = 255.0f;
				if (g < 0.0f)
					g = 0.0f;
				else if (g > 255.0f)
					g = 255.0f;
				if (b < 0.0f)
					b = 0.0f;
				else if (b > 255.0f)
					b = 255.0f;
			
				/* Write to colourmap */
				textureMap[3*(X + Y*textureMapSize) + 0] = (unsigned char)r;
				textureMap[3*(X + Y*textureMapSize) + 1] = (unsigned char)g;
				textureMap[3*(X + Y*textureMapSize) + 2] = (unsigned char)b;
			}
			else {

				/* mode == 1 */

				/* Use the gradient to get indexes to the source textures */
				if (countLoaded) {
					float fIndex = grad * (float)(countLoaded - 1);
					unsigned char* map = textureMap;
					int Index0, Index1;
					float scale0, scale1;

					Index0 = (int)floor(fIndex);
					Index1 = (int)ceil(fIndex);
					if (Index1 == Index0)
						Index1++;
					scale0 = (float)Index1 - fIndex;
					scale1 = fIndex - (float)Index0;
					
					/* Write to light map */
					if (colourMap) {
						map[4 * (X + Y*textureMapSize) + 0] =
							colourMap[3 * (X + Y*textureMapSize) + 0];
						map[4 * (X + Y*textureMapSize) + 1] =
							colourMap[3 * (X + Y*textureMapSize) + 1];
						map[4 * (X + Y*textureMapSize) + 2] =
							colourMap[3 * (X + Y*textureMapSize) + 2];
						map[4 * (X + Y*textureMapSize) + 3] =
									(unsigned char)(light * 255.0f);
					}
					else {
						map[X + Y*textureMapSize] =
									(unsigned char)(light * 255.0f);
					}

					/* Write to blend maps */
					if (colourMap)
						map += 4*SQR(textureMapSize);
					else
						map += SQR(textureMapSize);
					for (i = 0; i < countLoaded; i++) {
						if (colourMap) {
							map[4 * (X + Y*textureMapSize) + 0] =
								colourMap[3 * (X + Y*textureMapSize) + 0];
							map[4 * (X + Y*textureMapSize) + 1] =
								colourMap[3 * (X + Y*textureMapSize) + 1];
							map[4 * (X + Y*textureMapSize) + 2] =
								colourMap[3 * (X + Y*textureMapSize) + 2];
							if (i == Index0)
								map[4 * (X + Y*textureMapSize) + 3] =
											(unsigned char)(scale0 * light * 255.0f);
							else if (i == Index1)
								map[4 * (X + Y*textureMapSize) + 3] =
											(unsigned char)(scale1 * light * 255.0f);
							else
								map[4 * (X + Y*textureMapSize) + 3] = 0;
						}
						else {
							if (i == Index0)
								map[X + Y*textureMapSize] =
											(unsigned char)(scale0 * light * 255.0f);
							else if (i == Index1)
								map[X + Y*textureMapSize] =
											(unsigned char)(scale1 * light * 255.0f);
							else
								map[X + Y*textureMapSize] = 0;
						}

						if (colourMap)
							map += 4*SQR(textureMapSize);
						else
							map += SQR(textureMapSize);
					}
				}
			}
		}
	}
	
	if (mode == 0) {

		/* Free up temporary storage */
		for (i = 0; i < countLoaded; i++)
			free(data[i]);
	}

	printf("\n");
	fflush(stdout);

	return textureMap;
}

/*
  createColourMapTexObjs()

  Create OpenGL texture object(s) from the colourmap.

  Once the colourmap has been calculated, it is cut up into texPerSide 
  x texPerSide tiles, each of size texSize x texSize, and each tile is used
  to create an OpenGL texture object. Once this has been done, the colourmap
  is deleted.

  Lastly, another texture object is created based on the texture file
  terrain.tga, to be used as the detail texture for the landscape in 
  multitexture rendering. The texture coordinates for the base texture are set
  up so that the baseTexture will occupy baseTexScale of a heightmap patch,
  e.g. baseTexScale = 0.25 means that the base texture tiles into a patch 4
  times in x and 4 times in y.

  Nuff said.
*/
bool lsc_createColourMapTexObjs(lsc l, unsigned char *hm, float texScale)
{
	int texPerSide = l->texTile;
	int texSize = l->texSize;
	int hmSize = l->hmSize;
	float *scale = l->scale;
	int px, py, tx, ty, pcount = l->patchTile * l->hmTile;
	int patchesPerTextureObj = l->patchTile / texPerSide;
	vec3 lightDir = {1, 0, -1};
	int textureMapSize = texPerSide * texSize;
	int numTextureMaps;
	int numTextures = SQR(texPerSide);
	int *textures = NULL;
	unsigned char *normalMap  = NULL;
	unsigned char *shadowMap  = NULL;
	unsigned char *lightMap   = NULL;
	unsigned char *textureMap = NULL;
	tga tgaFile;
	char fileName[30];
	FILE *f;
	bool readFromFile = false;

	/* Malloc space for texture object ids */
	textures = (int*)malloc(numTextures * sizeof(int));
	if (!textures) {
		err_report("lsc_createColourMapTexObjs: cannot allocate %d bytes",
			numTextures * sizeof(int));
		return false;
	}

	/* Try reading colourmap from file */
	sprintf(fileName, "data/Colour%d.raw", hmSize);
	f = fopen(fileName, "rb");
	if (f) {
		textureMap = (unsigned char*)malloc(3*SQR(textureMapSize));
		if (!textureMap) {
			err_report("lsc_createColourMapTexObjs: cannot allocate %d bytes",
				3*SQR(textureMapSize));
			free(textures);
			return false;
		}
		fread(textureMap, 1, 3*SQR(textureMapSize), f);
		fclose(f);
		readFromFile = true;
	}
	else {
		err_report("lsc_createColourMapTexObjs: cannot open %s", fileName);

		/* Create colourmap */

		/* AW TODO: calculate best values automatically */
		/* This was tuned to a 2048 map single texture map */
		/* normalMap = lsc_calcNormals(hm, hmSize, scale, 0.1f, 0.75f, 32); */
		normalMap = lsc_calcNormals(hm, hmSize, scale, 0.1f, 0.75f, 8);
		shadowMap = lsc_calcShadows(hm, hmSize, scale, lightDir);
		if (normalMap && shadowMap)
			lightMap  = lsc_calcLighting(normalMap, shadowMap, hmSize, scale,
											0.5f, 0.5f, lightDir);
		if (lightMap)
			textureMap = lsc_calcTextures(0, true, hm, normalMap, lightMap,
				NULL, hmSize, texPerSide, texSize, texScale, &numTextureMaps);
		if (normalMap)
			free(normalMap);
		if (shadowMap)
			free(shadowMap);
		if (lightMap)
			free(lightMap);

		if (!textureMap) {
			free(textures);
			return false;
		}
	}

	if (numTextures == 1) {

		/* Make sure the texture object tiles seamlessly */
		int X, Y;
		for (Y = 0; Y < (texSize-1); Y++) {
			textureMap[3*((Y+1)*texSize - 1) + 0] =
					textureMap[3*(Y*textureMapSize) + 0];
			textureMap[3*((Y+1)*texSize - 1) + 1] =
					textureMap[3*(Y*textureMapSize) + 1];
			textureMap[3*((Y+1)*texSize - 1) + 2] =
					textureMap[3*(Y*textureMapSize) + 2];
		}
		for (X = 0; X < texSize; X++) {
			textureMap[3*(Y*texSize + X) + 0] = textureMap[3*X + 0];
			textureMap[3*(Y*texSize + X) + 1] = textureMap[3*X + 1];
			textureMap[3*(Y*texSize + X) + 2] = textureMap[3*X + 2];
		}
				
		/* Make an OpenGL texture object (edge clamped) */
		textures[0] = txm_addRawData(textureMap, texSize, texSize,
												GL_RGB, false, true);
	}
	else {
		/* Chop up the colourmap into texSize x texSize chunks */
		int X, Y, i, j;
		unsigned char *source;
		unsigned char *littleTexture = (unsigned char*)malloc(
											3*texSize*texSize);
		if (!littleTexture) {
			err_report("lsc_createColourMapTexObjs: cannot allocate %d bytes",
												3*texSize*texSize);
			free(textureMap);
			free(textures);
			return false;
		}
		for (j = 0; j < texPerSide; j++) {
			for (i = 0; i < texPerSide; i++) {
				for (Y = 0; Y < (texSize-1); Y++) {
					source = &textureMap[3*texSize*(j*textureMapSize + i)];
					for (X = 0; X < (texSize-1); X++) {
						littleTexture[3*(Y*texSize + X) + 0] =
							source[3*(Y*textureMapSize + X) + 0];
						littleTexture[3*(Y*texSize + X) + 1] =
							source[3*(Y*textureMapSize + X) + 1];
						littleTexture[3*(Y*texSize + X) + 2] =
							source[3*(Y*textureMapSize + X) + 2];
					}
					/* Make sure each texture object tiles seamlessly
					   with its neighbours */
					if (i == (texPerSide-1))
						source = &textureMap[3*texSize*(j*textureMapSize - 1)];
					littleTexture[3*((Y+1)*texSize - 1) + 0] =
							source[3*(Y*textureMapSize + texSize) + 0];
					littleTexture[3*((Y+1)*texSize - 1) + 1] =
							source[3*(Y*textureMapSize + texSize) + 1];
					littleTexture[3*((Y+1)*texSize - 1) + 2] =
							source[3*(Y*textureMapSize + texSize) + 2];
				}
				/* Make sure each texture object tiles seamlessly
				   with its neighbours */
				if (j == (texPerSide-1))
					source = &textureMap[3*texSize*(i - textureMapSize)];
				else
					source = &textureMap[3*texSize*(j*textureMapSize + i)];
				for (X = 0; X < texSize; X++) {
					littleTexture[3*(Y*texSize + X) + 0] =
						source[3*((Y+1)*textureMapSize + X) + 0];
					littleTexture[3*(Y*texSize + X) + 1] =
						source[3*((Y+1)*textureMapSize + X) + 1];
					littleTexture[3*(Y*texSize + X) + 2] =
						source[3*((Y+1)*textureMapSize + X) + 2];
				}
				
				/* Make an OpenGL texture object (edge clamped) */
				textures[j*texPerSide + i] =
					txm_addRawData(littleTexture, texSize, texSize,
												GL_RGB, false, true);
			}
		}
		
		/* Free up temporary storage */
		free(littleTexture);
	}

	if (!readFromFile) {
		/* Save the colourmap for next time before freeing */
		sprintf(fileName, "data/Colour%d.raw", hmSize);
		f = fopen(fileName, "wb");
		if (!f) {
			err_report("lsc_createColourMapTexObjs: cannot open %s", fileName);
		}
		else {
			fwrite(textureMap, 1, 3*SQR(textureMapSize), f);
			fclose(f);
		}
	}

	free(textureMap);
	
	/* Base texture */
	tgaFile = tga_create("data/terrain.tga");
	if (tgaFile && tgaFile->data && tgaFile->width == tgaFile->height
			&& (tgaFile->depth == 24 || tgaFile->depth == 32)) {

		if (tgaFile->depth == 32)
			tga_stripAlpha(tgaFile);

		/* Make an OpenGL texture object (wrapped) */
		l->baseTextureTexObj = txm_addRawData(tgaFile->data, tgaFile->width,
									tgaFile->height, GL_RGB, true, true);
	}
	if (tgaFile)
		tga_destroy(tgaFile);

	/* Assign texture ids to landscape patches */
	for (py = 0; py < pcount; py++) {
		ty = (py % l->patchTile) / patchesPerTextureObj;
		for (px = 0; px < pcount; px++) {
			tx = (px % l->patchTile) / patchesPerTextureObj;
			lsc_getPatch(l, px, py)->colourMapTexObj = textures[ty*texPerSide + tx];
		}
	}

	free(textures);
	return true;
}

/*
  createTextureSplatTexObjs()

  Create OpenGL texture object(s) for texture splatting.

  Once the blend maps have been calculated, they are cut up into texPerSide 
  x texPerSide tiles, each of size texSize x texSize, and each tile is used
  to create an OpenGL texture object. Once this has been done, the blend maps
  are deleted.
*/
bool lsc_createTextureSplatTexObjs(lsc l, unsigned char *hm, float texScale)
{
	int alphaSize;
	int alphaMapSize;
	int alphaPerSide;
	int numAlphaMaps;
	int numAlphas;
	int hmSize = l->hmSize;
	float *scale = l->scale;
	int px, py, tx, ty, pcount = l->patchTile * l->hmTile;
	int patchesPerAlphaObj;
	vec3 lightDir = {1, 0, -1};
	int *alphas = NULL;
	unsigned char *normalMap  = NULL;
	unsigned char *shadowMap  = NULL;
	unsigned char *lightMap   = NULL;
	unsigned char *alphaMaps  = NULL;
	unsigned char *alphaMap   = NULL;
	int textureMapSize = l->texTile * l->texSize;
	unsigned char *textureMap = NULL;
	int k, countLoaded;
	char fileName[30];
	FILE *f;
	bool readFromFile = false;

	/* Create blend maps */
	alphaSize = l->texSize;
	alphaMapSize = l->hmSize;
	if (alphaSize > alphaMapSize)
		alphaSize = alphaMapSize;
	alphaPerSide = alphaMapSize / alphaSize;
	numAlphas = SQR(alphaPerSide);
	patchesPerAlphaObj = l->patchTile / alphaPerSide;

	if (textureMapSize != alphaMapSize) {
		err_report("lsc_createTextureSplatTexObjs: bad colour map size %d",
											textureMapSize);
		return false;
	}

	/* Try reading unlit colourmap from file */
	sprintf(fileName, "data/UnlitColour%d.raw", hmSize);
	f = fopen(fileName, "rb");
	if (f) {
		textureMap = (unsigned char*)malloc(3*SQR(textureMapSize));
		if (!textureMap) {
			err_report("lsc_createTextureSplatTexObjs: cannot allocate %d bytes",
				3*SQR(textureMapSize));
			return false;
		}
		fread(textureMap, 1, 3*SQR(textureMapSize), f);
		fclose(f);
		readFromFile = true;
	}
	else
		err_report("lsc_createTextureSplatTexObjs: cannot open %s", fileName);

	/* AW TODO: calculate best values automatically */
	/* This was tuned to a 2048 map single texture map */
	/* normalMap = lsc_calcNormals(hm, hmSize, scale, 0.1f, 0.75f, 32); */
	normalMap = lsc_calcNormals(hm, hmSize, scale, 0.1f, 0.75f, 8);
	shadowMap = lsc_calcShadows(hm, hmSize, scale, lightDir);
	if (normalMap && shadowMap)
		lightMap  = lsc_calcLighting(normalMap, shadowMap, hmSize, scale,
										0.5f, 0.5f, lightDir);
	if (lightMap && !readFromFile)
		textureMap = lsc_calcTextures(0, false, hm, normalMap, lightMap,
				NULL, hmSize, alphaPerSide, alphaSize, texScale, &numAlphaMaps);
	if (lightMap && textureMap)
		alphaMaps = lsc_calcTextures(1, false, hm, normalMap, lightMap, textureMap,
				hmSize, alphaPerSide, alphaSize, 1.0f, &numAlphaMaps);
	if (normalMap)
		free(normalMap);
	if (shadowMap)
		free(shadowMap);
	if (lightMap)
		free(lightMap);

	if (textureMap && !readFromFile) {
		/* Save the colourmap for next time before freeing */
		sprintf(fileName, "data/UnlitColour%d.raw", hmSize);
		f = fopen(fileName, "wb");
		if (!f) {
			err_report("lsc_createTextureSplatTexObjs: cannot open %s", fileName);
		}
		else {
			fwrite(textureMap, 1, 3*SQR(textureMapSize), f);
			fclose(f);
		}
	}

	if (textureMap)
		free(textureMap);

	if (!alphaMaps)
		return false;

	/* Malloc space for texture object ids */
	/* AW TODO: currently the engine can only handle three splat textures */
	if (numAlphaMaps > 4)
		numAlphaMaps = 4;
	alphas = (int*)malloc(numAlphas * numAlphaMaps * sizeof(int));
	if (!alphas) {
		err_report("lsc_createTextureSplatTexObjs: cannot allocate %d bytes",
			numAlphas * numAlphaMaps * sizeof(int));
		free(alphaMaps);
		return false;
	}

	alphaMap = alphaMaps;
	for (k = 0; k < numAlphaMaps; k++) {

		if (numAlphas == 1) {

			int X, Y;

			/* Make sure the texture object tiles seamlessly */
			for (Y = 0; Y < (alphaSize-1); Y++) {
				alphaMap[4 * ((Y+1)*alphaSize - 1) + 0] = 
							alphaMap[4 * (Y*alphaSize) + 0];
				alphaMap[4 * ((Y+1)*alphaSize - 1) + 1] = 
							alphaMap[4 * (Y*alphaSize) + 1];
				alphaMap[4 * ((Y+1)*alphaSize - 1) + 2] = 
							alphaMap[4 * (Y*alphaSize) + 2];
				alphaMap[4 * ((Y+1)*alphaSize - 1) + 3] = 
							alphaMap[4 * (Y*alphaSize) + 3];
			}
			for (X = 0; X < alphaSize; X++) {
				alphaMap[4 * (Y*alphaSize + X) + 0] =
										alphaMap[4 * X + 0];
				alphaMap[4 * (Y*alphaSize + X) + 1] =
										alphaMap[4 * X + 1];
				alphaMap[4 * (Y*alphaSize + X) + 2] =
										alphaMap[4 * X + 2];
				alphaMap[4 * (Y*alphaSize + X) + 3] =
										alphaMap[4 * X + 3];
			}

			/* Make an OpenGL texture object (edge clamped) */
			alphas[k] = txm_addRawData(alphaMap, alphaSize, alphaSize,
										GL_RGBA, false, true);
		}
		else {
			/* Chop up into alphaSize x alphaSize chunks */
			int X, Y, i, j;
			unsigned char *source;
			unsigned char *littleAlpha = (unsigned char*)malloc(
												4*alphaSize*alphaSize);
			if (!littleAlpha) {
				err_report(
				"lsc_createTextureSplatTexObjs: cannot allocate %d bytes",
													4*alphaSize*alphaSize);
				free(alphaMaps);
				free(alphas);
				return false;
			}
			for (j = 0; j < alphaPerSide; j++) {
				for (i = 0; i < alphaPerSide; i++) {
					for (Y = 0; Y < (alphaSize-1); Y++) {
						source = &alphaMap[4 * (alphaSize*(j*alphaMapSize + i))];
						for (X = 0; X < (alphaSize-1); X++) {
							littleAlpha[4 * (Y*alphaSize + X) + 0] =
								source[4 * (Y*alphaMapSize + X) + 0];
							littleAlpha[4 * (Y*alphaSize + X) + 1] =
								source[4 * (Y*alphaMapSize + X) + 1];
							littleAlpha[4 * (Y*alphaSize + X) + 2] =
								source[4 * (Y*alphaMapSize + X) + 2];
							littleAlpha[4 * (Y*alphaSize + X) + 3] =
								source[4 * (Y*alphaMapSize + X) + 3];
						}
						/* Make sure each texture object tiles seamlessly
						   with its neighbours */
						if (i == (alphaPerSide-1))
							source = &alphaMap[4*(alphaSize*(j*alphaMapSize - 1))];
						littleAlpha[4 * ((Y+1)*alphaSize - 1) + 0] =
								source[4 * (Y*alphaMapSize + alphaSize) + 0];
						littleAlpha[4 * ((Y+1)*alphaSize - 1) + 1] =
								source[4 * (Y*alphaMapSize + alphaSize) + 1];
						littleAlpha[4 * ((Y+1)*alphaSize - 1) + 2] =
								source[4 * (Y*alphaMapSize + alphaSize) + 2];
						littleAlpha[4 * ((Y+1)*alphaSize - 1) + 3] =
								source[4 * (Y*alphaMapSize + alphaSize) + 3];
					}
					/* Make sure each texture object tiles seamlessly
					   with its neighbours */
					if (j == (alphaPerSide-1))
						source = &alphaMap[4 * (alphaSize*(i - alphaMapSize))];
					else
						source = &alphaMap[4 * (alphaSize*(j*alphaMapSize + i))];
					for (X = 0; X < alphaSize; X++) {
						littleAlpha[4 * (Y*alphaSize + X) + 0] =
							source[4 * ((Y+1)*alphaMapSize + X) + 0];
						littleAlpha[4 * (Y*alphaSize + X) + 1] =
							source[4 * ((Y+1)*alphaMapSize + X) + 1];
						littleAlpha[4 * (Y*alphaSize + X) + 2] =
							source[4 * ((Y+1)*alphaMapSize + X) + 2];
						littleAlpha[4 * (Y*alphaSize + X) + 3] =
							source[4 * ((Y+1)*alphaMapSize + X) + 3];
					}

					/* Make an OpenGL texture object (edge clamped) */
					alphas[k*numAlphas + j*alphaPerSide + i] =
						txm_addRawData(littleAlpha, alphaSize, alphaSize,
												GL_RGBA, false, true);
				}
			}
			
			/* Free up temporary storage */
			free(littleAlpha);
		}

		alphaMap += 4 * SQR(alphaMapSize);
	}

	free(alphaMaps);

	/* Assign texture ids to landscape patches */
	for (k = 0; k <= numAlphaMaps; k++) {
		for (py = 0; py < pcount; py++) {
			ty = (py % l->patchTile) / patchesPerAlphaObj;
			for (px = 0; px < pcount; px++) {
				tx = (px % l->patchTile) / patchesPerAlphaObj;
				lsc_getPatch(l, px, py)->blendMapTexObj[k] =
					alphas[k*numAlphas + ty*alphaPerSide + tx];
			}
		}
	}

	free(alphas);

	/* Load splat textures */
	countLoaded = 0;
	for (k = 0; k < 10; k++) {
		tga f;
		char filename[64];
		
		/* If texture%d.tga found and is square */
		sprintf(filename, "data/texture%d.tga", k);
		f = tga_create(filename);
		if (f && f->data && f->width == f->height
			&& (f->depth == 24 || f->depth == 32)) {
			tga_destroy(f);
			l->splatTextureTexObj[countLoaded] = 
				txm_addTgaFile(filename, false, true, true);
			countLoaded++;
		}
		else if (f) {
			tga_destroy(f);
		}

		/* AW TODO: currently the engine can only handle three splat textures */
		if (countLoaded >= 3)
			break;
	}

	return true;
}
