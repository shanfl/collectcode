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
	File:		lscape.c

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

#include "lscape.h"
#include "opengl.h"

/*
 * Calculate the number of geoMipmap levels for a patch.
 */
int lsc_countPatchLevels(lsc l)
{
	int i = 0;
	int patchSize = l->patchSize;

	while (patchSize > 0) {
		i++;
		patchSize >>= 1;
	}
	return i;
}

/*
 * Calculate the number of quadtree nodes for a given patch size.
 */
int lsc_countNodes(lsc l, int size)
{
	int i = 1, count = 1;
	int patchSize = l->patchSize;

	while (size > patchSize) {
		i <<= 2;
		count += i;
		size >>= 1;
	}
	return count;
}

/*
 * Allocate memory for the landscape.
 */
static void lsc_allocMem(lsc l)
{
	int diff;

	/*
	 * We only allocate enough vertex array pointers and error arrays for
	 * for each patch in the heightmap, not the entire landscape, so that
	 * the same patch in different heightmap tiles will share the same 
	 * vertex and error arrays.
	 *
	 * This is not true of index arrays, because the same patch in different
	 * heightmap tiles will usually have a different tesselation.
	 */

	int numPatchesInHeightMap = SQR(l->patchTile);
	int numNodesInLandscape = lsc_countLandscapeNodes(l);
	int numNodesInHeightMap = lsc_countHeightMapNodes(l);
	int numPatchesInLandscape = SQR(l->hmTile * l->patchTile);
	int numPatchIdxs = lsc_numPatchVtxs(l->patchSize, 0);
	int numLevels = l->numLevels;
	size_t space = 0;
	unsigned char *d;

	/* patch error arrays */
	l->patchErrArrSz = (numLevels * sizeof(float) + 15) & ~15;

	space += (numPatchIdxs * sizeof(unsigned short) + 15) & ~15;
	space += (numPatchesInHeightMap * sizeof(cchobj) + 15) & ~15;
	space += numPatchesInHeightMap * l->patchErrArrSz;
	space += (numPatchesInLandscape * sizeof(cchobj) + 15) & ~15;
	space += (numNodesInLandscape * sizeof(int) + 15) & ~15;
	space += (numPatchesInLandscape * sizeof(struct lscpatch_str) + 15) & ~15;
	space += (2 * l->sectors * sizeof(float) + 15) & ~15;
	space += (numNodesInHeightMap * l->sectors * l->maxOccPts *
												2 * sizeof(int) + 15) & ~15;

	space += 16;

	printf("Landscape requires %dk\n", (space + 1023)/1024);
	fflush(stdout);

	l->data = (unsigned char*)malloc(space);
	if (l->data == NULL)
		return;
	memset(l->data, 0, space);

	d = (unsigned char*)((int)(l->data + 15) & ~15);

	l->patchIdx = (unsigned short*)d;
	d += (numPatchIdxs * sizeof(unsigned short) + 15) & ~15;

	l->patchVtxPtr = (cchobj*)d;
	d += (numPatchesInHeightMap * sizeof(cchobj) + 15) & ~15;

	l->patchErrArr = (float*)d;
	d += numPatchesInHeightMap * l->patchErrArrSz;

	l->patchIdxPtr = (cchobj*)d;
	d += (numPatchesInLandscape * sizeof(cchobj) + 15) & ~15;

	l->patches = (lscpatch)d;
	d += (numPatchesInLandscape * sizeof(struct lscpatch_str) + 15) & ~15;

	l->quadtree = (int*)d;
	d += (numNodesInLandscape * sizeof(int) + 15) & ~15;

	l->sectorTrig = (float*)d;
	d += (2 * l->sectors * sizeof(float) + 15) & ~15;

	l->occPts = (int*)d;
	d += (numNodesInHeightMap * l->sectors * l->maxOccPts *
										2 * sizeof(int) + 15) & ~15;

	/* Allow for indexing the arrays from the top of the quadtree */
	diff = lsc_countNodes(l, l->hmSize >> 1);
	l->patchVtxPtr = lsc_getPatchVtxPtr(l, -diff);
	l->patchErrArr = lsc_getPatchErrArr(l, -diff);
	diff = lsc_countNodes(l, (l->hmSize * l->hmTile) >> 1);
	l->patchIdxPtr = lsc_getPatchIdxPtr(l, -diff);

	/* Create the patch vertex array cache */
	memset(&l->patchVtxCch, 0, sizeof(struct cch_str));

#ifdef USE_NV_VAR_FENCE
	l->patchVtxCch.fast = true;
#else
	l->patchVtxCch.fast = false;
#endif

	cch_init(&l->patchVtxCch, l->patchVtxCchSz);
	printf("Created %dk patch vertex array cache in %s memory\n",
								(l->patchVtxCch.size + 1023)/1024,
								l->patchVtxCch.fast ? "fast" : "system");
	fflush(stdout);

	/* Create the patch index array cache */
	memset(&l->patchIdxCch, 0, sizeof(struct cch_str));
	cch_init(&l->patchIdxCch, l->patchIdxCchSz);
	printf("Created %dk patch index array cache in %s memory\n",
								(l->patchIdxCch.size + 1023)/1024,
								l->patchIdxCch.fast ? "fast" : "system");
	fflush(stdout);
}

/*
 * Create the patchIdx array that will be used to access patch vertex
 * arrays in row major order.
 */
static void lsc_setupPatchIdx(lsc l)
{
	int i, j, k = 0;
	int patchSize = l->patchSize;
	unsigned short *idx, *patchIdx = l->patchIdx;
	int step = patchSize;

	j = SQR(patchSize + 1);
	for (i = 0; i < j; i++)
		patchIdx[i] = 0xFFFF;
	while (step > 0) {
		for (j = 0; j <= patchSize; j += step) {
			for (i = 0; i <= patchSize; i += step) {
				idx = &patchIdx[j*(patchSize + 1) + i];
				if (*idx == 0xFFFF)
					*idx = k++;
			}
		}
		step >>= 1;
	}
}

/*
 * Create the vertex array with vertexes in mip level order.
 */
static void lsc_setupPatchVtxArr(lsc l, int m,
								 int x0, int x1, int y0, int y1,
								 float *vtxArr)
{
	int i, j, k;
	vec3 v;
	unsigned char *hm = l->hm;
	int hmSize = l->hmSize;
	int patchSize = l->patchSize;
	float texScale = (float)l->texTile / (float)l->hmSize;
	float baseTexScale = (float)l->baseTexTile / (float)l->hmSize;
	int S = x0 % (l->hmSize / l->texTile);
	int T = y0 % (l->hmSize / l->texTile);
	unsigned short *patchIdx = l->patchIdx;

	if (hm)
		hm = &hm[x0 + y0*(hmSize+1)];

	for (j = 0; j <= patchSize; j++) {
		for (i = 0; i <= patchSize; i++) {

			k = patchIdx[j*(patchSize + 1) + i];

			v[0] = i + x0;
			v[1] = j + y0;

			/* Texture 0 */
			vtxArr[8*k + 4] = (S + i) * texScale;
			vtxArr[8*k + 5] = (T + j) * texScale;

			/* Texture 1 */
			vtxArr[8*k + 6] = v[0] * baseTexScale;
			vtxArr[8*k + 7] = v[1] * baseTexScale;

			/* Vertex */
			if (hm)
				v[2] = hm[j*(hmSize + 1) + i];
			else
				v[2] = 0.0f;
			vec3_mul(v, l->scale, v);
			vec3_cpy(v, (&vtxArr[8*k]));
		}
	}
}

/*
 * Return the cached vertex array for a patch.
 */
float *lsc_getPatchVtxArr(lsc l, int m,
						  int x0, int x1, int y0, int y1)
{
	cchobj *obj = lsc_getPatchVtxPtr(l,m);
	float *vtxArr;
	size_t size;

	/*
	 * AW TODO: don't always use level 0, check error array first to get
	 * lowest mip level needed ? Don't forget dependence of build error
	 * array on get vertex array !
	 */

	/* Check if this cache object is valid */
	if (!(*obj) || (*obj)->owner != obj) {

		/* Not valid, allocate cache object and rebuild array */

		/* stride = 8 for vertex array (incl 2 sets of texture coords) */
		size = (lsc_numPatchVtxs(l->patchSize, 0)*8*sizeof(float) + 15) & ~15;
		vtxArr = (float *)cch_malloc(&l->patchVtxCch, size, obj);
		/* AW TODO - some vtxArr == NULL handling here please */
		
		/* AW TODO: should replace /size and %size with shift and mask
		   all through the code ! */
		x0 %= l->hmSize;
		x1 %= l->hmSize;
		y0 %= l->hmSize;
		y1 %= l->hmSize;
		lsc_setupPatchVtxArr(l, m, x0, x1, y0, y1, vtxArr);
	}
	else
		vtxArr = (float *)((*obj)->data);

	return vtxArr;
}

/*
 * AW TODO: We shouldn't be using the rendering vertex arrays for collision
 * detection, these should be in a separate cache !
 */
float *lsc_getPatchVtxArrCollisionDetect(lsc l, int m,
										 int x0, int x1, int y0, int y1)
{
	return lsc_getPatchVtxArr(l, m, x0, x1, y0, y1);
}

/*
 * Calculate the error for a patch at a given level.
 */
static float lsc_calcError(lsc l, int level, float *vtxArr)
{
	int step, limiti, limitj, patchSize = l->patchSize;
	unsigned short *patchIdx = l->patchIdx;
	int i0, i1, i2, j0, j1, j2, count = 0;
	float z[4], zsl, zsr, zsa, zl, zr, za, zreal;
	float err = 0.0f;

	/* Error for level 0 must be 0 */
	if (level <= 0)
		return err;

	step = 1 << level;
	for (j0 = 0; j0 < patchSize; j0 = j1) {

		j1 = j0 + step;
		if (j1 == patchSize)
			limitj = step + 1;
		else
			limitj = step;

		for (i0 = 0; i0 < patchSize; i0 = i1) {

			i1 = i0 + step;
			if (i1 == patchSize)
				limiti = step + 1;
			else
				limiti = step;

			/* Heightmap z for patch points for mip level */
			z[0] = vtxArr[8*patchIdx[j0*(patchSize + 1) + i0] + 2];
			z[1] = vtxArr[8*patchIdx[j1*(patchSize + 1) + i0] + 2];
			z[2] = vtxArr[8*patchIdx[j0*(patchSize + 1) + i1] + 2];
			z[3] = vtxArr[8*patchIdx[j1*(patchSize + 1) + i1] + 2];

			/* Left & right edge interpolants */
			zsl = (z[1] - z[0]) / step;
			zsr = (z[3] - z[2]) / step;

			for (j2 = 0; j2 < limitj; j2++) {

				/* Lerped left & right edge z */
				zl = z[0] + j2*zsl;
				zr = z[2] + j2*zsr;

				zsa = (zr - zl) / step;
				for (i2 = 0; i2 < limiti; i2++) {

					/* Lerped z for patch point */
					za = zl + i2*zsa;
					/* Real heightmap z for patch point */
					zreal = vtxArr[8*patchIdx[
								(j0 + j2)*(patchSize + 1) + i0 + i2] + 2];
#ifdef AVGERR
					/* Average error */
					err += fabs(za - zreal);
					count++;
#else
					/* Max error */
					if (fabs(za - zreal) > err)
						err = fabs(za - zreal);
#endif
				}
			}
		}
	}

#ifdef AVGERR
	if (count)
		err /= count;
#endif

	return err;
}

/*
 * Recursively create the patch error arrays.
 */
static void lsc_recurSetupPatchErrArr(lsc l, int m,
									  int x0, int x1, int y0, int y1)
{
	/* If not a leaf recurse to child nodes */
	if (x1 - x0 > l->patchSize) {

		int xc, yc;
		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_recurSetupPatchErrArr(l, LSCQT_BL(m), x0, xc, y0, yc);
		lsc_recurSetupPatchErrArr(l, LSCQT_BR(m), xc, x1, y0, yc);
		lsc_recurSetupPatchErrArr(l, LSCQT_TL(m), x0, xc, yc, y1);
		lsc_recurSetupPatchErrArr(l, LSCQT_TR(m), xc, x1, yc, y1);
	}

	/* Leaf - set up the patch */
	else {

		int level = 0;
		int numLevels = l->numLevels;
		float *vtxArr = lsc_getPatchVtxArr(l, m, x0, x1, y0, y1);
		float *errArr = lsc_getPatchErrArr(l, m);
		float err;

		errArr[level++] = 0.0f;
		while (level < numLevels) {
			errArr[level] = errArr[level - 1];
			err = lsc_calcError(l, level, vtxArr);
			if (err > errArr[level])
				errArr[level] = err;
			level++;
		}
	}
}

/*
 * Create the patch error arrays.
 */
static void lsc_setupPatchErrArr(lsc l)
{
	lsc_recurSetupPatchErrArr(l, 0,
		0, l->hmSize,
		0, l->hmSize);
}

/*
 * Write a horizontal triangle strip into a patch index array.
 */
static int lsc_acrossStrip(unsigned short *patchIdx, int patchSize,
							unsigned short *triIdx, int *idx,
							int startx, int starty, int step, int end)
{
	int i, j0 = starty, j1 = starty + step, k = *idx, n = 0;

	for (i = startx; i < end; i += step) {
		triIdx[k++] = patchIdx[j1*(patchSize + 1) + i];
		triIdx[k++] = patchIdx[j0*(patchSize + 1) + i];
		triIdx[k++] = patchIdx[j1*(patchSize + 1) + i + step];
		triIdx[k++] = patchIdx[j0*(patchSize + 1) + i + step];
		triIdx[k] = triIdx[k - 2];
		k++;
		triIdx[k] = triIdx[k - 4];
		k++;
		n += 2;
	}
	*idx = k;
	return n;
}

/*
 * Write a vertical triangle strip into a patch index array.
 */
static int lsc_downStrip(unsigned short *patchIdx, int patchSize,
							unsigned short *triIdx, int *idx,
							int startx, int starty, int step, int end)
{
	int i0 = startx, i1 = startx + step, j, k = *idx, n = 0;

	for (j = starty; j < end; j += step) {
		triIdx[k++] = patchIdx[j*(patchSize + 1) + i0];
		triIdx[k++] = patchIdx[j*(patchSize + 1) + i1];
		triIdx[k++] = patchIdx[(j + step)*(patchSize + 1) + i0];
		triIdx[k++] = patchIdx[(j + step)*(patchSize + 1) + i1];
		triIdx[k] = triIdx[k - 2];
		k++;
		triIdx[k] = triIdx[k - 4];
		k++;
		n += 2;
	}
	*idx = k;
	return n;
}

/*
 * Write the bottom row of triangle fans into a patch index array, to glue
 * the patch to a neighbour at a higher mip level.
 */
static int lsc_bottomFans(unsigned short *patchIdx, int patchSize,
							unsigned short *triIdx, int *idx,
							int minLevel, int maxLevel)
{
	int maxPoints = patchSize >> minLevel;
	int minPoints = patchSize >> maxLevel;
	int maxStep = 1 << maxLevel;
	int minStep = 1 << minLevel;
	int i, j0 = minStep, j1 = 2*minStep, k = *idx, n = 0;

	for (i = 0; i < patchSize; ) {
		for ( ; j0 < (i + maxStep) && j0 < (patchSize - minStep); j1 += minStep) {
			triIdx[k++] = patchIdx[i];
			triIdx[k++] = patchIdx[minStep*(patchSize + 1) + j1];
			triIdx[k++] = patchIdx[minStep*(patchSize + 1) + j0];
			j0 = j1;
			n++;
		}
		triIdx[k++] = patchIdx[i];
		i += maxStep;
		triIdx[k++] = patchIdx[i];
		triIdx[k++] = patchIdx[minStep*(patchSize + 1) + j0];
		n++;
	}
	*idx = k;
	return n;
}

/*
 * Write the top row of triangle fans into a patch index array, to glue
 * the patch to a neighbour at a higher mip level.
 */
static int lsc_topFans(unsigned short *patchIdx, int patchSize,
							unsigned short *triIdx, int *idx,
							int minLevel, int maxLevel)
{
	int maxPoints = patchSize >> minLevel;
	int minPoints = patchSize >> maxLevel;
	int maxStep = 1 << maxLevel;
	int minStep = 1 << minLevel;
	int patchSizeSq = SQR(patchSize);
	int i, j0 = minStep, j1 = 2*minStep, k = *idx, n = 0;

	for (i = 0; i < patchSize; ) {
		for ( ; j0 < (i + maxStep) && j0 < (patchSize - minStep); j1 += minStep) {
			triIdx[k++] = patchIdx[patchSizeSq + patchSize + i];
			triIdx[k++] = patchIdx[(patchSize - minStep)*(patchSize + 1) + j0];
			triIdx[k++] = patchIdx[(patchSize - minStep)*(patchSize + 1) + j1];
			j0 = j1;
			n++;
		}
		triIdx[k++] = patchIdx[patchSizeSq + patchSize + i];
		triIdx[k++] = patchIdx[(patchSize - minStep)*(patchSize + 1) + j0];
		i += maxStep;
		triIdx[k++] = patchIdx[patchSizeSq + patchSize + i];
		n++;
	}
	*idx = k;
	return n;
}

/*
 * Write the right column of triangle fans into a patch index array, to glue
 * the patch to a neighbour at a higher mip level.
 */
static int lsc_rightFans(unsigned short *patchIdx, int patchSize,
							unsigned short *triIdx, int *idx,
							int minLevel, int maxLevel)
{
	int maxPoints = patchSize >> minLevel;
	int minPoints = patchSize >> maxLevel;
	int maxStep = 1 << maxLevel;
	int minStep = 1 << minLevel;
	int i, j0 = minStep, j1 = 2*minStep, k = *idx, n = 0;

	for (i = 0; i < patchSize; ) {
		for ( ; j0 < (i + maxStep) && j0 < (patchSize - minStep); j1 += minStep) {
			triIdx[k++] = patchIdx[patchSize + i*(patchSize + 1)];
			triIdx[k++] = patchIdx[patchSize - minStep + j1*(patchSize + 1)];
			triIdx[k++] = patchIdx[patchSize - minStep + j0*(patchSize + 1)];
			j0 = j1;
			n++;
		}
		triIdx[k++] = patchIdx[patchSize + i*(patchSize + 1)];
		i += maxStep;
		triIdx[k++] = patchIdx[patchSize + i*(patchSize + 1)];
		triIdx[k++] = patchIdx[patchSize - minStep + j0*(patchSize + 1)];
		n++;
	}
	*idx = k;
	return n;
}

/*
 * Write the left column of triangle fans into a patch index array, to glue
 * the patch to a neighbour at a higher mip level.
 */
static int lsc_leftFans(unsigned short *patchIdx, int patchSize,
							unsigned short *triIdx, int *idx,
							int minLevel, int maxLevel)
{
	int maxPoints = patchSize >> minLevel;
	int minPoints = patchSize >> maxLevel;
	int maxStep = 1 << maxLevel;
	int minStep = 1 << minLevel;
	int i, j0 = minStep, j1 = 2*minStep, k = *idx, n = 0;

	for (i = 0; i < patchSize; ) {
		for ( ; j0 < (i + maxStep) && j0 < (patchSize - minStep); j1 += minStep) {
			triIdx[k++] = patchIdx[i*(patchSize + 1)];
			triIdx[k++] = patchIdx[minStep + j0*(patchSize + 1)];
			triIdx[k++] = patchIdx[minStep + j1*(patchSize + 1)];
			j0 = j1;
			n++;
		}
		triIdx[k++] = patchIdx[i*(patchSize + 1)];
		triIdx[k++] = patchIdx[minStep + j0*(patchSize + 1)];
		i += maxStep;
		triIdx[k++] = patchIdx[i*(patchSize + 1)];
		n++;
	}
	*idx = k;
	return n;
}

/*
 * Calculate the index array for patch at geomip level L, taking into account
 * the mip levels of its neighbour patches.
 */
static void lsc_setupPatchIdxArr(lsc l, int n, int L, int nL,
								 unsigned short *triIdx)
{
	int j, k = 1, N = 0;
	int patchSize = l->patchSize;
	unsigned short *patchIdx = l->patchIdx;
	int step = 1 << L;
	int numTriangles = 0;

	for (j = 0; j < 4; j++) {
		if (((nL >> (j*8)) & 0xFF) > L)
			N |= 0x01 << j;
	}

	if (N == 0) {
		/*
		 * All neighbours at same or lower level, just output all
		 * triangle strips.
		 */
		for (j = 0; j < patchSize; j += step) {
			numTriangles += lsc_acrossStrip(patchIdx, patchSize,
												triIdx, &k,
												0, j,
												step, patchSize);
		}
	}
	else {
		/* Output triangle strips for patch middle */
		for (j = step; j < patchSize - step; j += step) {
			numTriangles += lsc_acrossStrip(patchIdx, patchSize,
												triIdx, &k,
												step, j,
												step, patchSize - step);
		}

		/* Patch left edge */
		if (N & 0x01) {
			/* Output triangle fans */
			numTriangles += lsc_leftFans(patchIdx, patchSize,
											triIdx, &k,
											L, nL & 0xFF);
		}
		else {
			/* Output triangle strip */

			triIdx[k++] = patchIdx[0];
			triIdx[k++] = patchIdx[step*(patchSize + 2)];
			triIdx[k++] = patchIdx[step*(patchSize + 1)];
			numTriangles++;

			numTriangles += lsc_downStrip(patchIdx, patchSize,
											triIdx, &k,
											0, step,
											step, patchSize - step);

			triIdx[k++] = patchIdx[(patchSize-step)*(patchSize + 1)];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 1 - step)];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 1)];
			numTriangles++;
		}

		/* Patch right edge */
		if (N & 0x02) {
			/* Output triangle fans */
			numTriangles += lsc_rightFans(patchIdx, patchSize,
											triIdx, &k,
											L, (nL >> 8) & 0xFF);
		}
		else {
			/* Output triangle strip */

			triIdx[k++] = patchIdx[patchSize];
			triIdx[k++] = patchIdx[patchSize*(step + 1) + step];
			triIdx[k++] = patchIdx[(step + 1)*patchSize];
			numTriangles++;

			numTriangles += lsc_downStrip(patchIdx, patchSize,
											triIdx, &k,
											patchSize - step, step,
											step, patchSize - step);

			triIdx[k++] = patchIdx[patchSize*(patchSize + 2)];
			triIdx[k++] = patchIdx[(patchSize - step)*(patchSize + 2)];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 2 - step) - step];
			numTriangles++;
		}

		/* Patch top edge */
		if (N & 0x04) {
			/* Output triangle fans */
			numTriangles += lsc_topFans(patchIdx, patchSize,
											triIdx, &k,
											L, (nL >> 16) & 0xFF);
		}
		else {
			/* Output triangle strip */

			triIdx[k++] = patchIdx[patchSize*(patchSize + 1) + step];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 1)];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 1 - step)];
			numTriangles++;

			numTriangles += lsc_acrossStrip(patchIdx, patchSize,
												triIdx, &k,
												step, patchSize - step,
												step, patchSize - step);

			triIdx[k++] = patchIdx[(patchSize - step)*(patchSize + 2)];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 2)];
			triIdx[k++] = patchIdx[patchSize*(patchSize + 2) - step];
			numTriangles++;
		}

		/* Patch bottom edge */
		if (N & 0x08) {
			/* Output triangle fans */
			numTriangles += lsc_bottomFans(patchIdx, patchSize,
											triIdx, &k,
											L, (nL >> 24) & 0xFF);
		}
		else {
			/* Output triangle strip */

			triIdx[k++] = patchIdx[step*(patchSize + 2)];
			triIdx[k++] = patchIdx[0];
			triIdx[k++] = patchIdx[step];
			numTriangles++;

			numTriangles += lsc_acrossStrip(patchIdx, patchSize,
												triIdx, &k,
												step, 0,
												step, patchSize - step);

			triIdx[k++] = patchIdx[patchSize - step];
			triIdx[k++] = patchIdx[patchSize];
			triIdx[k++] = patchIdx[(step + 1)*patchSize];
			numTriangles++;
		}
	}

	triIdx[0] = (unsigned short)numTriangles;
}

/*
 * Return the cached index array for a patch.
 */
unsigned short *lsc_getPatchIdxArr(lsc l, int n, int L, int nL)
{
	cchobj *obj = lsc_getPatchIdxPtr(l,n);
	unsigned short *idxArr;
	size_t size;
	bool rebuild = false;
	bool realloc = false;

	/* Check if this cache object is valid */
	if (!(*obj) || (*obj)->owner != obj) {
		/* Not valid, allocate cache object and rebuild index array */
		rebuild = true;
		realloc = true;
	}
	else {
		/* Valid - check level and neighbour levels */
		idxArr = (unsigned short *)(*obj)->data;
		if (*(int *)idxArr != L) {
			/* Level is wrong */
			realloc = true;
			rebuild = true;
		}
		idxArr += 2;
		if (*(int *)idxArr != nL) {
			/* Neighbour levels wrong */
			rebuild = true;
			*(int *)idxArr = nL;
		}
		idxArr += 2;
	}

	if (realloc) {
		/* Get a new cache object */
		size = ((3*lsc_numPatchTris(l->patchSize, L) + 5) *
									sizeof(unsigned short) + 15) & ~15;
		idxArr = (unsigned short *)cch_malloc(&l->patchIdxCch, size, obj);
		/* AW TODO - some idxArr == NULL handling here please */

		*(int *)idxArr = L;
		idxArr += 2;
		*(int *)idxArr = nL;
		idxArr += 2;
	}

	if (rebuild)
		lsc_setupPatchIdxArr(l, n, L, nL, idxArr);

	return idxArr;
}

/*
 * AW TODO: We shouldn't be using the rendering index arrays for collision
 * detection, these should be in a separate cache !
 */
unsigned short *lsc_getPatchIdxArrCollisionDetect(lsc l, int n)
{
	cchobj *obj = lsc_getPatchIdxPtr(l,n);
	unsigned short *idxArr;
	size_t size;
	bool rebuild = false;
	bool realloc = false;
	int L = 0;
	int nL = 0;

	/* Check if this cache object is valid */
	if (!(*obj) || (*obj)->owner != obj) {
		/* Not valid, allocate cache object and rebuild index array */
		rebuild = true;
		realloc = true;
	}
	else {
		/* Valid - check level and neighbour levels */
		idxArr = (unsigned short *)(*obj)->data;
		if (*(int *)idxArr != L) {
			/* Level is wrong */
			realloc = true;
			rebuild = true;
		}
		idxArr += 2;
		if (*(int *)idxArr != nL) {
			/* Neighbour levels wrong */
			rebuild = true;
			*(int *)idxArr = nL;
		}
		idxArr += 2;
	}

	if (realloc) {
		/* Get a new cache object */
		size = ((3*lsc_numPatchTris(l->patchSize, L) + 5) *
									sizeof(unsigned short) + 15) & ~15;
		idxArr = (unsigned short *)cch_malloc(&l->patchIdxCch, size, obj);
		/* AW TODO - some idxArr == NULL handling here please */

		*(int *)idxArr = L;
		idxArr += 2;
		*(int *)idxArr = nL;
		idxArr += 2;
	}

	if (rebuild)
		lsc_setupPatchIdxArr(l, n, L, nL, idxArr);

	return idxArr;
}

/*
 * Recursively create the landscape quadtree.
 */
static int lsc_recurSetupQuadtree(lsc l, int n, int m,
								  int x0, int x1, int y0, int y1)
{
	int xc, yc;
	unsigned char z[2] = {255, 0};

	xc = (x0 + x1) >> 1;
	yc = (y0 + y1) >> 1;

	if (x1 - x0 > l->hmSize) {

		/* Recur to child nodes */
		int i, q[4];

		q[0] = lsc_recurSetupQuadtree(l, LSCQT_BL(n), 0, x0, xc, y0, yc);
		q[1] = lsc_recurSetupQuadtree(l, LSCQT_BR(n), 0, xc, x1, y0, yc);
		q[2] = lsc_recurSetupQuadtree(l, LSCQT_TL(n), 0, x0, xc, yc, y1);
		q[3] = lsc_recurSetupQuadtree(l, LSCQT_TR(n), 0, xc, x1, yc, y1);

		/* Get max and min z across all child nodes */
		for (i = 0; i < 4; i++) {
			if (LSCQT_Z0(q[i]) < z[0])
				z[0] = LSCQT_Z0(q[i]);
			if (LSCQT_Z1(q[i]) > z[1])
				z[1] = LSCQT_Z1(q[i]);
		}
	}
	else if (x1 - x0 > l->patchSize) {

		/* Recur to child nodes */
		int i, q[4];

		q[0] = lsc_recurSetupQuadtree(l, LSCQT_BL(n), LSCQT_BL(m),
														x0, xc, y0, yc);
		q[1] = lsc_recurSetupQuadtree(l, LSCQT_BR(n), LSCQT_BR(m),
														xc, x1, y0, yc);
		q[2] = lsc_recurSetupQuadtree(l, LSCQT_TL(n), LSCQT_TL(m),
														x0, xc, yc, y1);
		q[3] = lsc_recurSetupQuadtree(l, LSCQT_TR(n), LSCQT_TR(m),
														xc, x1, yc, y1);

		/* Get max and min z across all child nodes */
		for (i = 0; i < 4; i++) {
			if (LSCQT_Z0(q[i]) < z[0])
				z[0] = LSCQT_Z0(q[i]);
			if (LSCQT_Z1(q[i]) > z[1])
				z[1] = LSCQT_Z1(q[i]);
		}
	}
	else {

		/* This is a leaf node, i.e. a patch */
		float *vtxArr = lsc_getPatchVtxArr(l, m, x0, x1, y0, y1);
		unsigned char Z;
		int i, j, k;

		/* Get max and min z for patch */
		for (j = 0; j <= l->patchSize; j++) {
			for (i = 0; i <= l->patchSize; i++) {
				k = l->patchIdx[j*(l->patchSize + 1) + i];
				Z = (unsigned char)(vtxArr[8*k + 2] / l->scale[2]);
				if (Z < z[0])
					z[0] = Z;
				if (Z > z[1])
					z[1] = Z;
			}
		}
	}

	/* Save max and min z */
	l->quadtree[n] = 0;
	l->quadtree[n] = LSCQT_SETZ0(l->quadtree[n], z[0]);
	l->quadtree[n] = LSCQT_SETZ1(l->quadtree[n], z[1]);

	/* Set all clip flags on initially */
	l->quadtree[n] = LSCQT_SETCF(l->quadtree[n], 0x7E);

	return l->quadtree[n];
}

/*
 * Create the landscape quadtree.
 */
static void lsc_setupQuadtree(lsc l)
{
	lsc_recurSetupQuadtree(l, 0, 0,
		0, l->hmSize * l->hmTile,
		0, l->hmSize * l->hmTile);
}

/*
 * Create a landscape object.
 */
lsc lsc_create(int hmSize, int patchSize, int hmTile,
			   unsigned char *hm, vec3 scale,
			   int texSize, int texTile, int baseTexTile,
			   int sectors, int occPatchSz, int maxOccPts
			   , int patchVtxCchSz
			   , int patchIdxCchSz
			   )
{
	lsc l = (lsc)malloc(sizeof(struct lsc_str));
	if (l) {
		l->data = NULL;
		lsc_default(l, hmSize, patchSize, hmTile, hm, scale,
						texSize, texTile, baseTexTile,
							sectors, occPatchSz, maxOccPts
							, patchVtxCchSz
							, patchIdxCchSz
							);
		if (!l->data) {
			free(l);
			l = NULL;
		}
	}
	return l;
}

/*
 * Set up a landsacpe object.
 */
void lsc_default(lsc l, int hmSize, int patchSize, int hmTile,
				 unsigned char *hm, vec3 scale,
				 int texSize, int texTile, int baseTexTile,
				 int sectors, int occPatchSz, int maxOccPts
				 , int patchVtxCchSz
				 , int patchIdxCchSz
				 )
{
	if (l) {

		l->hmSize = hmSize;
		l->patchSize = patchSize;
		l->hmTile = (hmTile <= 0) ? 1 : hmTile;
		l->patchTile = hmSize / patchSize;
		l->numLevels = lsc_countPatchLevels(l);
		vec3_cpy(scale, l->scale);
		l->errorMetric = 0.0;
		l->sectors = sectors;
		l->occPatchSize = occPatchSz;
		l->maxOccPts = maxOccPts;
		l->baseTextureTexObj = 0;
		l->texSize = texSize;
		l->texTile = texTile;
		l->baseTexTile = baseTexTile;
		/* AW TODO: copy hm into the data block ? */
		l->hm = hm;
		l->patchVtxCchSz = patchVtxCchSz * 1024;
		l->patchIdxCchSz = patchIdxCchSz * 1024;

		printf("Set up patches");
		fflush(stdout);

		if (l->data)
			free(l->data);
		lsc_allocMem(l);
		if (l->data) {
			lsc_setupPatchIdx(l);
			lsc_setupPatchErrArr(l);
			lsc_setupQuadtree(l);
		}

		printf("\n");
		fflush(stdout);
	}
}

/*
 * Delete a landscape object.
 */
void lsc_destroy(lsc l)
{
	if (l) {
		if (l->data)
			free(l->data);
		if (l->patchVtxCch.base)
			free(l->patchVtxCch.base);
		if (l->patchIdxCch.base)
			free(l->patchIdxCch.base);
		free(l);
	}
}

/*
 * Recursively clip the landscape quadtree to a view frustum.
 */
static void lsc_recurClip(lsc l, int n, int x0, int x1, int y0, int y1,
							fru frustum, int parentClipFlags)
{
	int node = l->quadtree[n];
	int clipFlags = LSCQT_CF(node);
	int prevClipFlags = clipFlags;		/* Remember current clip flags */

	/* If parent node was all in, so are its children */
	if (!parentClipFlags)
		clipFlags = 0;

	/* Else If parent node was all out, so are its children */
	else if (parentClipFlags == 1)
		clipFlags = 1;

	/*
	 * Else parent intersected frustum so children must be
	 * tested against frustum.
	 */
	else {

		/* Construct the bounding box */
		struct bbox_str box;
		box.minp[0] = x0;
		box.minp[1] = y0;
		box.minp[2] = LSCQT_Z0(node);
		box.maxp[0] = x1;
		box.maxp[1] = y1;
		box.maxp[2] = LSCQT_Z1(node);
		vec3_mul(box.minp, l->scale, box.minp);
		vec3_mul(box.maxp, l->scale, box.maxp);

		/* Clip to frustum */
		clipFlags = bbox_isInsideFrustum(&box, frustum, parentClipFlags);
	}

	/* Save the clip flags */
	if (clipFlags != prevClipFlags) {
		node = LSCQT_SETCF(node, clipFlags);
		l->quadtree[n] = node;
	}

	/* If the node is all out don't go any further */
	if (clipFlags == 1)
		return;

	/*
	 * Else if the clip code hasn't changed and the node is all in then we
	 * can infer that none of its children's clip codes will have changed
	 * either.
	 */
	else if ((clipFlags == prevClipFlags) && !clipFlags)
		return;

	/* Else if not a leaf recurse to child nodes */
	else if (x1 - x0 > l->patchSize) {

		int xc, yc;
		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_recurClip(l, LSCQT_BL(n), x0, xc, y0, yc, frustum, clipFlags);
		lsc_recurClip(l, LSCQT_BR(n), xc, x1, y0, yc, frustum, clipFlags);
		lsc_recurClip(l, LSCQT_TL(n), x0, xc, yc, y1, frustum, clipFlags);
		lsc_recurClip(l, LSCQT_TR(n), xc, x1, yc, y1, frustum, clipFlags);
	}
}

/*
 * Clip the landscape to a view frustum.
 */
void lsc_clip(lsc l, fru frustum)
{
	lsc_recurClip(l, 0,
		0, l->hmSize * l->hmTile,
		0, l->hmSize * l->hmTile,
		frustum, 0x7E);		/* All clip flags on initially */
}

/*
 * Set the error metric for the current field of view and window height.
 */
void lsc_setErrorMetric(lsc l, float fovY, int scrHeight)
{
	float halfFovY;

	if (fovY >= 180.0f)
		fovY = 179.99f;
	else if (fovY < 0.0f)
		fovY = 0.0f;

	halfFovY = DEG2RAD(fovY / 2.0f);
	l->errorMetric = (float)tan(halfFovY) / (float)scrHeight;
}

/*
 * Recursivley set the geo-mip level for the visible parts of the
 * landscape quadtree.
 */
static void lsc_recurSetMipLevel(lsc l, int n, int m,
									int x0, int x1, int y0, int y1)
{
	int node = l->quadtree[n];
	int clipFlags = LSCQT_CF(node);
	int xc, yc;

	if (clipFlags == 1)
		return;			/* Outside view frustum */

	xc = (x0 + x1) >> 1;
	yc = (y0 + y1) >> 1;

	/* If not a leaf recurse to child nodes */
	if (x1 - x0 > l->hmSize) {

		lsc_recurSetMipLevel(l, LSCQT_BL(n), 0, x0, xc, y0, yc);
		lsc_recurSetMipLevel(l, LSCQT_BR(n), 0, xc, x1, y0, yc);
		lsc_recurSetMipLevel(l, LSCQT_TL(n), 0, x0, xc, yc, y1);
		lsc_recurSetMipLevel(l, LSCQT_TR(n), 0, xc, x1, yc, y1);
	}
	else if (x1 - x0 > l->patchSize) {

		lsc_recurSetMipLevel(l, LSCQT_BL(n), LSCQT_BL(m), x0, xc, y0, yc);
		lsc_recurSetMipLevel(l, LSCQT_BR(n), LSCQT_BR(m), xc, x1, y0, yc);
		lsc_recurSetMipLevel(l, LSCQT_TL(n), LSCQT_TL(m), x0, xc, yc, y1);
		lsc_recurSetMipLevel(l, LSCQT_TR(n), LSCQT_TR(m), xc, x1, yc, y1);
	}

	/* Leaf - set the patch mip level */
	else {

		int px = x0 / l->patchSize;
		int py = y0 / l->patchSize;
		lscpatch patch = lsc_getPatch(l, px, py);
		float *errArr = lsc_getPatchErrArr(l, m);
		cam c = cam_getGlobalCamera();
		float *vpt = c->viewPt.origin;
		float *vdir = c->viewPt.orientation;
		float f = c->farPlane;
		vec3 mid;
		float d;
		int i, level = l->numLevels - 1;

		/* Get middle of patch bound box */
		mid[0] = xc * l->scale[0];
		mid[1] = yc * l->scale[1];
		mid[2] = ((LSCQT_Z0(node) + LSCQT_Z1(node)) >> 1) * l->scale[2];

		/* Get perpendicular distance between viewpoint & patch mid */
		d = fabs(vec3_dot(vpt, vdir) - vec3_dot(mid, vdir));
		patch->d = d;

		/* Get mip level based on log of distance */
		i = (l->patchSize * (int)(f - d)) / (int)f;
		if (i < 0)
			i = 0;
		while (i) {
			level--;
			i >>= 1;
		}

		/* Get mip level based on screen distortion metric */
		d *= l->errorMetric;
		for (i = l->numLevels - 1; i >= 0; i--) {
			if (errArr[i] <= d)
				break;
		}

		/* If screen distortion error allows a higher
		   mip level then use it, e.g. flat patch */
		if (i > level)
			level = i;

		/* Set patch level */
		lsc_getPatch(l, x0/l->patchSize, y0/l->patchSize)->newLevel = level;
	}
}

/*
 * Set the geo-mip level for the visible parts of the landscape.
 */
void lsc_setMipLevel(lsc l)
{
	lsc_recurSetMipLevel(l, 0, 0,
		0, l->hmSize * l->hmTile,
		0, l->hmSize * l->hmTile);
}

/*
 * Force all patches to a given geo-mip level.
 */
void lsc_forceMipLevel(lsc l, int level)
{
	int nL, px, py, pcount = l->patchTile * l->hmTile;
	lscpatch patch;

	nL = 0;
	for (px = 0; px < 4; px++)
		nL |= (level & 0xFF) << px*8;

	for (py = 0; py < pcount; py++) {
		for (px = 0; px < pcount; px++) {
			patch = lsc_getPatch(l, px, py);
			patch->newLevel = level;
		}
	}
}
