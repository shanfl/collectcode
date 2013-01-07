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
	File:		lscape.h

	Function:	geoMipMap landscape class.

	This implementation is based on the paper "Fast Terrain Rendering Using
	Geometrical MipMapping" by Willem H. de Boer.

	We read in a heightmap with size 2^n and wrap the last row and column to
	get a heightmap size 2^n + 1. This is broken into patches size 2^p + 1
	where the last row and column of a patch overlaps with the first row and
	column of its neighbours. The reasons for breaking the heightmap into
	patches are

	- 	We can decide per frame the level of detail to use for each patch
		based on the roughness of the terrain and the distance from the
		camera.
	-	Patches provide a high level unit for view frustum clipping.
	-	Patches can easily be textured on hardware with tight limits on
		maximum texture size.

	We can tile the heightmap in xy as many times as we want simply by
	creating more than 2^n / 2^p patches in each direction and wrapping
	so that patch X,Y refers to the area of the heightmap starting at

	(X * 2^p) % 2^n , (Y * 2^p) % 2^n
*/

#ifndef LSCAPE_H
#define LSCAPE_H

#include "vector.h"
#include "boundbox.h"
#include "cache.h"

#define LSC_MAX_PATCHSIZE	128	/* Because patch indexes are unsigned short */
#define USE_NV_VAR_FENCE

typedef struct lscpatch_str {
	short newLevel;				/* New patch mip level */
	int colourMapTexObj;		/* Patch colourmap texture object for */
	                            /* single pass multitexturing */
	int blendMapTexObj[4];		/* Patch blend map texture objects for */
	                            /* multi pass texture splatting */
	float d;                    /* Perpendicular distance to viewpoint */
} *lscpatch;

typedef struct lsc_str {
	int hmSize;					/* height map size = 2^n (see above) */
	int patchSize;				/* patch size = 2^p (see above) */
	int hmTile;					/* Number of times to tile the hm in xy */
	int patchTile;				/* Number of patches per heightmap side */
	int numLevels;				/* Number of levels in a patch */
	vec3 scale;					/* Scale vector */
	float errorMetric;			/* Constant used to calc patch screen error */
	int sectors;				/* Number of occlusion region sectors */
	int occPatchSize;			/* Patch size for occlusion regions */
	int maxOccPts;				/* Number of points per occlusion region */
	int baseTextureTexObj;		/* Base texture object */
	int splatTextureTexObj[3];	/* Splat texture objects */
	int texSize;				/* Landscape texture tile size in pixels */
	int texTile;				/* Number of texture tiles per heightmap side */
	int baseTexTile;			/* Number of base texture tiles per hm side */
	unsigned char *hm;			/* Heightmap data */

	unsigned char *data;		/* Big chunk of mem for all landscape data */
	unsigned short *patchIdx;	/* Patch indexes (max patchSize = 128) */
	struct cch_str patchVtxCch;	/* Cache for patch vertex arrays */
	int patchVtxCchSz;			/* Requested cache size */
	cchobj *patchVtxPtr;		/* Pointers to the patch vertex arrays */
	float *patchErrArr;			/* Patch error arrays */
	size_t patchErrArrSz;		/* Patch error array size */
	struct cch_str patchIdxCch;	/* Cache for patch index arrays */
	int patchIdxCchSz;			/* Requested cache size */
	cchobj *patchIdxPtr;		/* Pointers to the patch index arrays */
	lscpatch patches;			/* All landscape patches */

	int *quadtree;				/* Implicit quadtree */
	float *sectorTrig;			/* Sin and cos lookup table by sector */
	int *occPts;				/* Hierarchical occlusion regions */
} *lsc;

/* Macros for implicit quadtree */
#define LSCQT_BL(n)			(((n)<<2)+1)		/* Bottom left quad */
#define LSCQT_BR(n)			(((n)<<2)+2)		/* Bottom right quad */
#define LSCQT_TL(n)			(((n)<<2)+3)		/* Top left quad */
#define LSCQT_TR(n)			(((n)<<2)+4)		/* Top right quad */
#define LSCQT_CF(n)			((n)&0xFF)			/* Get clip flags */
#define LSCQT_Z0(n)			(((n)>>8)&0xFF)		/* Get min Z */
#define LSCQT_Z1(n)			(((n)>>16)&0xFF)	/* Get max Z */
#define LSCQT_SETCF(n,c)	(((n)&0xFFFFFF00)|(c))		/* Set clip flags */
#define LSCQT_SETZ0(n,z)	(((n)&0xFFFF00FF)|(z)<<8)	/* Set min Z */
#define LSCQT_SETZ1(n,z)	(((n)&0xFF00FFFF)|(z)<<16)	/* Set max Z */

/* Number of vertexes in a patch size p at mip level l */
#define lsc_numPatchVtxs(p,l)	SQR((p >> l) + 1)

/* Number of triangles in a patch of size p at mip level l */
#define lsc_numPatchTris(p,l)	(2*SQR(p>>l))

/* Calculate the number of quadtree nodes for the height map */
#define lsc_countHeightMapNodes(l)	lsc_countNodes(l, l->hmSize)

/* Calculate the number of quadtree nodes for the landscape */
#define lsc_countLandscapeNodes(l)	lsc_countNodes(l, l->hmSize * l->hmTile)

/* Return a pointer to the patch */
#define lsc_getPatch(l,px,py)	\
	(&l->patches[px + py * l->patchTile * l->hmTile])

/* Return a pointer to the index array for a patch */
#define lsc_getPatchIdxPtr(l,n)	(l->patchIdxPtr + n)
unsigned short *lsc_getPatchIdxArr(lsc l, int n, int L, int nL);
unsigned short *lsc_getPatchIdxArrCollisionDetect(lsc l, int n);

/* Return a pointer to the vertex array for a patch */
#define lsc_getPatchVtxPtr(l,m)	(l->patchVtxPtr + m)
float *lsc_getPatchVtxArr(lsc l, int m,
						  int x0, int x1, int y0, int y1);
float *lsc_getPatchVtxArrCollisionDetect(lsc l, int m,
										 int x0, int x1, int y0, int y1);

/* Return a pointer to the error array for a patch */
#define lsc_getPatchErrArr(l,m)	\
	((float*)(((unsigned char*)l->patchErrArr) + m * l->patchErrArrSz))


lsc lsc_create(int hmSize, int patchSize, int hmTile,
							unsigned char *hm, vec3 scale,
							int texSize, int texTile, int baseTexTile,
							int sectors, int occPatchSz, int maxOccPts
							, int patchVtxCchSz
							, int patchIdxCchSz
							);
void lsc_default(lsc l, int hmSize, int patchSize, int hmTile,
							unsigned char *hm, vec3 scale,
							int texSize, int texTile, int baseTexTile,
							int sectors, int occPatchSz, int maxOccPts
							, int patchVtxCchSz
							, int patchIdxCchSz
							);
void lsc_destroy(lsc l);

int  lsc_countPatchLevels(lsc l);
int  lsc_countNodes(lsc l, int size);
void lsc_setErrorMetric(lsc l, float fovY, int scrHeight);
void lsc_clip(lsc l, fru frustum);
void lsc_setMipLevel(lsc l);
void lsc_render(lsc l, int mode, bool occDisable, bool useTextureCombine);
void lsc_forceMipLevel(lsc l, int level);

bool lsc_createColourMapTexObjs(lsc l, unsigned char *hm, float texScale);
bool lsc_createTextureSplatTexObjs(lsc l, unsigned char *hm, float texScale);

void lsc_getOcclusion(lsc l, unsigned char *hm);
bool lsc_isPatchVisible(lsc l, int *v, int n, int x0, int x1, int y0, int y1);

bool lsc_checkCollision(lsc l, vec3 *p, bbox entBox,
						float *frac, plane collisionPlane);

void lsc_initTrace(void);

void lscCleanup();
void lscInit();
void lscShare(lsc l);
lsc  lscGet(void);
bool lscCollisionCheck(vec3 *p, bbox entBox, float *frac, plane collisionPlane);
void lscDraw(cam camera, int win_height);

void lsc_reallocMem(lsc l);

#endif /* LSCAPE_H */
