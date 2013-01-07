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
	File:		lsvis.c

	Function:	Hierarchical terrain visibility - based on the paper by
				A. James Stewart.
				http://www.dgp.toronto.edu/people/JamesStewart/papers/egwr97.html

	The basic idea is to calculate the horizon for each point in the heightmap
	representing the terrain. When the viewpoint is below the horizon for the
	point, the point need not be rendered since it will be hidden from view by
	the intervening terrain.

	Because storing the horizon for each heightmap point would be prohibitively
	expensive, the heightmap is divided into patches (quads) and the horizons
	for each point in a patch are merged to produce an "occlusion region" for
	the patch. At runtime we only have to determine if the viewpoint is inside
	or outside the occlusion region for the patch to decide whether to render
	the patch or not.

	Patch occlusion regions for neighbouring patches can be merged to form a
	hierarchy of occlusion regions corresponding to a terrain quadtree.

	See the referenced paper (above) for full details.
*/

#include "lscape.h"
#include "error.h"
#include <limits.h>

/*
  Occlusion node, used to build a linked list of occlusion regions when
  merging the occlusion regions for each point in a patch.
*/
typedef struct occNode_str {
	int s[2];					/* startx, starty */
	int p[2];					/* current position */
	float m;					/* gradient */
	struct occNode_str *n;		/* link */
} *occNode;

/*
  Landscape visibility class
*/
typedef struct lscvis_str {
	int hmSize;					/* Height map size = 2^n (see above) */
	int patchSize;				/* Patch size = 2^p (see above) */
	int hmTile;					/* Number of times to tile the hm in xy */
	int patchTile;				/* Number of patches per heightmap side */
	int sectors;				/* Number of occlusion region sectors */
	int subSectors;				/* Number of sub-sectors per sector */
	int maxOccPts;				/* Number of points per occlusion region */

	float *sectorTrig;			/* Sin and cos lookup table by sector */
	int *occPts;				/* Occlusion points per sector per patch */

	/* This stuff is used when calculating occlusion regions */
	unsigned char *hm;			/* Height map data */
	float *subSectorTrig;		/* Sin and cos lookup table by sub-sector */
	int *horizonPoints;			/* Horizon point for each point in a patch */
	occNode occArr;				/* Pool of available occlusion nodes */
	int occFree;				/* Index of next free node */
	occNode occList;			/* Linked list of active occlusion nodes */
	int avgPtsRqd;				/* Average points required per region */
	int maxPtsRqd;				/* Max points required per region */
} *lscvis;


/*
  lscvis_buildTrigTables()

  Build sector and sub-sector sine & cosine tables.

  Note that the sector sine and cosine are also the sector piPlane
  coefficients, i.e. a piPlane is defined by

  ax + by + cz + d = 0

  where c = d = 0, a = sin(theta), b = -cos(theta) and theta is the
  angle between the piPlane and the x axis.
*/
static void lscvis_buildTrigTables(lscvis l)
{
	int i, count;
	float azimuth;

	/* Build sector table */
	count = l->sectors;
	if (l->sectorTrig) {
		for (i = 0; i < count; i++) {
			azimuth = 2 * M_PI * ((float)i + 0.5f) / (float)count;
			l->sectorTrig[i*2 + 0] = sin(azimuth);
			l->sectorTrig[i*2 + 1] = cos(azimuth);
		}
	}

	/* Build sub-sector table */
	count = l->sectors * l->subSectors;
	l->subSectorTrig = (float*)malloc(2 * count * sizeof(float));
	if (l->subSectorTrig) {
		for (i = 0; i < count; i++) {
			azimuth = 2 * M_PI * ((float)i + 0.5f) / (float)count;
			l->subSectorTrig[i*2 + 0] = sin(azimuth);
			l->subSectorTrig[i*2 + 1] = cos(azimuth);
		}
	}
}

/*
  lscvis_perpProject()

  Perpendicular project a 3D world space point p into a 2D point pp
  in the piPlane for sector i.
*/
static void lscvis_perpProject(lscvis l, int *p, int i, int *pp)
{
	float s = l->sectorTrig[2*i + 0];
	float c = l->sectorTrig[2*i + 1];

	pp[0] = (int)floor(p[0]*c + p[1]*s + 0.5f);
	pp[1] = p[2];
}

/*
  lscvis_findHorizon()

  Find the horizon point for a point p(x,y,z) in direction dx, dy. This
  uses a Bresenham type DDA walk down the center of the sub-sector, so
  there may be undersampling artifacts if the sub-sectors are too wide.

  Returns the distance to the horizon in d, the horizon elevation in e,
  and the horizon coordinates in hp.

  Note: we are assuming here that the heightmap is repeated in xy so
  that the view at the right edge of the heightmap wraps back to the left
  edge, etc. hmSize is assumed to be a power of two.
*/
static void lscvis_findHorizon(lscvis l, int *p, int dx, int dy,
							   float *d, float *e, int *hp)
{
	int c, M, D, xinc=1, yinc=1;
	int maxx, maxy, minx, miny;
	int x0, x, y0, y, z0, z;
	int X, Y;
	float dist, elevation;

	x = x0 = p[0];
	y = y0 = p[1];
	z = z0 = p[2];

	maxx = x0 + l->hmTile * l->hmSize;
	maxy = y0 + l->hmTile * l->hmSize;
	minx = x0 - l->hmTile * l->hmSize;
	miny = y0 - l->hmTile * l->hmSize;

	*e = INT_MIN;

	if (dx < 0) {
		xinc = -1;
		dx = -dx;
	}
	if (dy < 0) {
		yinc = -1;
		dy = -dy;
	}
	if (dy < dx) {
		c = 2 * dx;
		D = M = 2 * dy;
		x += xinc;
		if (D > dx) {
			y += yinc;
			D -= c;
		}
		while (x <= maxx && x >= minx) {
			X = x & (l->hmSize - 1);
			Y = y & (l->hmSize - 1);
			z = l->hm[X + Y*(l->hmSize + 1)];
			dist = sqrt(SQR(x - x0) + SQR(y - y0));
			elevation = (z - z0) / dist;
			if (elevation > *e) {
				*d = dist;
				*e = elevation;
				hp[0] = x;
				hp[1] = y;
				hp[2] = z;
			}
			x += xinc;
			D += M;
			if (D > dx) {
				y += yinc;
				D -= c;
			}
		}
	}
	else {
		c = 2 * dy;
		D = M = 2 * dx;
		y += yinc;
		if (D > dy) {
			x += xinc;
			D -= c;
		}
		while (y <= maxy && y >= miny) {
			X = x & (l->hmSize - 1);
			Y = y & (l->hmSize - 1);
			z = l->hm[X + Y*(l->hmSize + 1)];
			dist = sqrt(SQR(x - x0) + SQR(y - y0));
			elevation = (z - z0) / dist;
			if (elevation > *e) {
				*d = dist;
				*e = elevation;
				hp[0] = x;
				hp[1] = y;
				hp[2] = z;
			}
			y += yinc;
			D += M;
			if (D > dy) {
				x += xinc;
				D -= c;
			}
		}
	}
}

/*
  lscvis_findSectorHorizon()

  Find the minimum horizon point for a point p(x,y,z) for sector i,
  i.e. find hmin(i).

  Returns the distance to the horizon in d, the horizon elevation in e,
  and the horizon coordinates in hmin.
*/
static void lscvis_findSectorHorizon(lscvis l, int *p, int i, float *d,
									 float *e, int *hmin)
{
	int j, dx, dy;
	float s, c, dist, elevation;
	int hp[3];

	/* For each sub-sector */
	for (j = 0; j < l->subSectors; j++) {

		/* Find the horizon point */
		s = l->subSectorTrig[2*(i*l->subSectors + j) + 0];
		c = l->subSectorTrig[2*(i*l->subSectors + j) + 1];
		dx = (int)floor(c*l->hmSize + 0.5f);
		dy = (int)floor(s*l->hmSize + 0.5f);
		lscvis_findHorizon(l, p, dx, dy, &dist, &elevation, hp);

		/* Save the minimum elevation horizon point at
		   the greatest distance in the return variables */
		if (!j) {
			*e = elevation;
			*d = dist;
			hmin[0] = hp[0];
			hmin[1] = hp[1];
		}
		else {
			if (elevation < *e)
				*e = elevation;
			if (dist > *d) {
				*d = dist;
				hmin[0] = hp[0];
				hmin[1] = hp[1];
			}
		}
	}

	/* Calculate z for the returned distance and elevation */
	hmin[2] = (int)floor((*d)*(*e) + p[2]);
}

/*
  lscvis_calcHorizonPoints()

  For each point in a patch for sector i we calculate and store the
  perpendicular projection of the point and minimum horizon point onto
  the sectors piPlane.

  The patch is defined by the x,y point p0 and the patch size. The
  output is saved in horizonPoints.
*/
static void lscvis_calcHorizonPoints(lscvis l, int *p0, int i)
{
	int X, Y, p[3], hmin[3], *hp;
	float distance, elevation;

	hp = l->horizonPoints;

	/* For each point in the patch */
	for (p[1] = p0[1]; p[1] <= (p0[1]+l->patchSize); p[1]++) {
		Y = p[1] & (l->hmSize - 1);

		for (p[0] = p0[0]; p[0] <= (p0[0]+l->patchSize); p[0]++) {
			X = p[0] & (l->hmSize - 1);

			p[2] = l->hm[X + Y*(l->hmSize + 1)];

			/* Find the minimum elevation horizon point for the sector */
			lscvis_findSectorHorizon(l, p, i, &distance, &elevation, hmin);

			/* Save the projected heightmap point */
			lscvis_perpProject(l, p, i, hp);
			hp += 2;

			/* Save the projected minimum horizon point */
			lscvis_perpProject(l, hmin, i, hp);
			hp += 2;
		}
	}
}

/*
  lscvis_buildOcclusionList()

  Build a linked list of occlusion regions for each point in a patch,
  sorted by gradient.
*/
static void lscvis_buildOcclusionList(lscvis l, int *p0)
{
	int p[2];
	occNode *o, new;
	int *occPts;

	/* Reset linked list */
	l->occList = NULL;
	l->occFree = SQR(l->patchSize + 1);

	/* For each point in the patch */
	for (p[1] = 0; p[1] <= l->patchSize; p[1]++) {

		for (p[0] = 0; p[0] <= l->patchSize; p[0]++) {

			occPts = &l->horizonPoints[4*(p[0] + p[1]*(l->patchSize + 1))];

			/* Add the occlusion region to the active list,
		       sorted by gradient */
			if (l->occFree) {
				new = &l->occArr[--(l->occFree)];
				new->p[0] = new->s[0] = occPts[2];
				new->p[1] = new->s[1] = occPts[3];
				new->m = (float)(occPts[3] - occPts[1]) /
									(float)(occPts[2] - occPts[0]);
				if (!l->occList) {
					/* Add the first list element */
					new->n = l->occList;
					l->occList = new;
				}
				else {
					/* Highest gradient at start of list */
					for (o = &l->occList; *o; o = &(*o)->n) {
						if (new->m > (*o)->m) {
							new->n = *o;
							*o = new;
							break;
						}
						else if (!(*o)->n) {
							new->n = (*o)->n;
							(*o)->n = new;
							break;
						}
					}
				}
			}
		}
	}
}

/*
  lscvis_mergeOcclusionRegions()

  Merge the occlusion regions for each point in a patch. Returns the merged
  occlusion region as a set of maxOccPts points in the occPts array. Returns
  the total number of points required to fully define the occlusion region
  (may be greater than maxOccPts).
*/
static int lscvis_mergeOcclusionRegions(lscvis l, int *p0, int *occPts)
{
	occNode o, next;
	int x, y, xmax;
	int count = 0, required = 0;

	/* Put occlusion regions for each point in the patch into
	   a linked list sorted by gradient, highest gradient first */
	lscvis_buildOcclusionList(l, p0);

	/* Find the occlusion region with the rightmost start point */
	next = l->occList;
	for (o = l->occList->n; o; o = o->n) {
		if (o->s[0] > next->s[0])
			next = o;
	}

	/* Set all occlusion regions to the rightmost start x and
	   find the corresponding minimum height */
	x = next->p[0];
	y = next->p[1];
	for (o = l->occList; o; o = o->n) {
		o->p[0] = x;
		o->p[1] = o->s[1] + (int)floor(o->m*(o->p[0] - o->s[0]));
		if (o->p[1] < y) {
			next = o;
			y = next->p[1];
		}
	}

	/* We now have the first point in our merged occlusion region */
	occPts[2*count + 0] = x;
	occPts[2*count + 1] = y;
	count++;
	required++;

	/* Remove any regions from the active list with gradient greater than
	   or equal to the current region */
	l->occList = next;
	for (o = l->occList->n; o; o = o->n) {
		if (o->m >= l->occList->m)
			l->occList->n = o->n;
		else
			break;
	}

	/* Now step x to the limits of the terrain finding the lowest height
       at each point */
	xmax = x + l->hmTile * l->hmSize;
	for (x++; x <= xmax; x++) {

		o = next = l->occList;
		o->p[0] = x;
		o->p[1] = o->s[1] + (int)floor(o->m*(o->p[0] - o->s[0]));
		y = o->p[1];

		/* Going in order of decreasing gradient */
		for (o = o->n; o; o = o->n) {
			o->p[0] = x;
			o->p[1] = o->s[1] + (int)floor(o->m*(o->p[0] - o->s[0]));
			if (o->p[1] <= y) {
				y = o->p[1];
				next = o;
			}
		}
		/* If the lowest point is in a different occlusion region
		   than the current */
		if (next != l->occList) {

			/* We now have the next point in our merged occlusion region */
			if (count < l->maxOccPts) {
				occPts[2*count + 0] = x;
				occPts[2*count + 1] = y;
				count++;
				required++;
			}
			else {
				/* occPts array is full, replace the last point */
				occPts[2*(count - 1) + 0] = x;
				occPts[2*(count - 1) + 1] = y;
				required++;
			}

			/* Remove any regions from the active list with gradient greater
			   than or equal to current */
			l->occList = next;
			for (o = l->occList->n; o; o = o->n) {
				if (o->m >= l->occList->m)
					l->occList->n = o->n;
				else
					break;
			}
		}
	}

	/* We now have the last point in our merged occlusion region */
	if (count < l->maxOccPts) {
		occPts[2*count + 0] = x;
		occPts[2*count + 1] = y;
		count++;
		required++;
		/* Make sure we fill up the occPts array */
		while (count < l->maxOccPts) {
			occPts[2*count + 0] = x;
			occPts[2*count + 1] = y;
			count++;
		}
	}
	else {
		/* occPts array is full, replace the last point */
		occPts[2*(count - 1) + 0] = x;
		occPts[2*(count - 1) + 1] = y;
		required++;
	}

	return required;
}

/*
  lscvis_mergePatchOcclusionRegions()

  Merge the occlusion regions for four patches into one. Returns the merged
  occlusion region as a set of maxOccPts points in the occPts array.
*/
static void lscvis_mergePatchOcclusionRegions(lscvis l, int **patchOccPts,
											  int *occPts)
{
	int x, xmax, y[4], dx, dy;
	int i, j, cur, next, count = 0;

	/* Find the occlusion region with the rightmost start point */
	x = INT_MIN;
	for (i = 0; i < 4; i++) {
		if (patchOccPts[i][0] > x)
			x = patchOccPts[i][0];
	}

	/* Set all occlusion regions to the rightmost start x and
	   find the corresponding minimum height */
	cur = 0;
	for (i = 0; i < 4; i++) {
		for (j = 1; j < l->maxOccPts; j++) {
			if (x < patchOccPts[i][2*j + 0])
				break;
		}
		if (j < l->maxOccPts) {
			dx = patchOccPts[i][2*j + 0] - patchOccPts[i][2*(j - 1) + 0];
			dy = patchOccPts[i][2*j + 1] - patchOccPts[i][2*(j - 1) + 1];
			y[i] = (int)floor((float)(x - patchOccPts[i][2*(j - 1) + 0])
											* (float)dy / (float)dx);
			y[i] += patchOccPts[i][2*(j - 1) + 1];
		}
		else {
			y[i] = INT_MAX;
		}
		if (y[i] < y[cur])
			cur = i;
	}

	/* We now have the first point in our merged occlusion region */
	occPts[2*count + 0] = x;
	occPts[2*count + 1] = y[cur];
	count++;

	/* Now step x to the limits of the terrain finding the lowest height
       at each point */
	xmax = x + l->hmTile * l->hmSize;
	for (x++; x <= xmax; x++) {

		next = 0;
		for (i = 0; i < 4; i++) {
			for (j = 1; j < l->maxOccPts; j++) {
				if (x < patchOccPts[i][2*j + 0])
					break;
			}
			if (j < l->maxOccPts) {
				dx = patchOccPts[i][2*j + 0] - patchOccPts[i][2*(j - 1) + 0];
				dy = patchOccPts[i][2*j + 1] - patchOccPts[i][2*(j - 1) + 1];
				y[i] = (int)floor((float)(x - patchOccPts[i][2*(j - 1) + 0])
												* (float)dy / (float)dx);
				y[i] += patchOccPts[i][2*(j - 1) + 1];
			}
			else {
				y[i] = INT_MAX;
			}
			if (y[i] < y[next])
				next = i;
		}

		/* If the lowest point is in a different occlusion region
		   than the current */
		if (next != cur) {

			/* We now have the next point in our merged occlusion region */
			cur = next;
			if (count < l->maxOccPts) {
				occPts[2*count + 0] = x;
				occPts[2*count + 1] = y[cur];
				count++;
			}
			else {
				/* occPts array is full, replace the last point */
				occPts[2*(count - 1) + 0] = x;
				occPts[2*(count - 1) + 1] = y[cur];
			}
		}
	}

	/* We now have the last point in our merged occlusion region */
	if (count < l->maxOccPts) {
		occPts[2*count + 0] = x;
		occPts[2*count + 1] = y[cur];
		count++;
		/* Make sure we fill up the occPts array */
		while (count < l->maxOccPts) {
			occPts[2*count + 0] = x;
			occPts[2*count + 1] = y[cur];
			count++;
		}
	}
	else {
		/* occPts array is full, replace the last point */
		occPts[2*(count - 1) + 0] = x;
		occPts[2*(count - 1) + 1] = y[cur];
	}
}

/*
 * Recursively merge the merged occlusion regions for adjacent patches to
 * form an occlusion region quadtree.
 */
static void lscvis_recurSetupQuadtree(lscvis l, int n,
									  int x0, int x1, int y0, int y1)
{
	int xc, yc, p0[2], i, ptsRqd;
	int *occPts[5];

	xc = (x0 + x1) >> 1;
	yc = (y0 + y1) >> 1;

	occPts[0] = l->occPts + n*l->sectors*l->maxOccPts*2;

	if (x1 - x0 > l->patchSize) {

		/* Recur to child nodes */
		lscvis_recurSetupQuadtree(l, LSCQT_BL(n), x0, xc, y0, yc);
		lscvis_recurSetupQuadtree(l, LSCQT_BR(n), xc, x1, y0, yc);
		lscvis_recurSetupQuadtree(l, LSCQT_TL(n), x0, xc, yc, y1);
		lscvis_recurSetupQuadtree(l, LSCQT_TR(n), xc, x1, yc, y1);

		/* Now merge the occlusion regions for the child nodes */
		occPts[1] = l->occPts + LSCQT_BL(n)*l->sectors*l->maxOccPts*2;
		occPts[2] = l->occPts + LSCQT_BR(n)*l->sectors*l->maxOccPts*2;
		occPts[3] = l->occPts + LSCQT_TL(n)*l->sectors*l->maxOccPts*2;
		occPts[4] = l->occPts + LSCQT_TR(n)*l->sectors*l->maxOccPts*2;

		/* For each sector */
		for (i = 0; i < l->sectors; i++) {
			lscvis_mergePatchOcclusionRegions(l, &occPts[1], occPts[0]);
			occPts[0] += 2 * l->maxOccPts;
			occPts[1] += 2 * l->maxOccPts;
			occPts[2] += 2 * l->maxOccPts;
			occPts[3] += 2 * l->maxOccPts;
			occPts[4] += 2 * l->maxOccPts;
		}
	}
	else {

		/* This is a leaf node, i.e. a patch */
		p0[0] = x0;
		p0[1] = y0;

		/* For each sector */
		for (i = 0; i < l->sectors; i++) {

			printf("Patch %d of %d, sector %d of %d    \r",
					(x0/l->patchSize) + (y0/l->patchSize)*l->patchTile,
					SQR(l->patchTile), i, l->sectors);
			fflush(stdout);

			lscvis_calcHorizonPoints(l, p0, i);
			ptsRqd = lscvis_mergeOcclusionRegions(l, p0, occPts[0]);

			occPts[0] += 2 * l->maxOccPts;

			l->avgPtsRqd += ptsRqd;
			if (ptsRqd > l->maxPtsRqd)
				l->maxPtsRqd = ptsRqd;
		}
	}
}


/******************************************************************************
  LANDSCAPE METHODS
******************************************************************************/

/*
  lsc_writeOccToFile

  Dump the hierarchical terrain visibility data from memory to file.
*/
static void lsc_writeOccToFile(lsc l)
{
	char fileName[30];
	FILE *f;
	size_t space;
	int numNodesInHeightMap = lsc_countHeightMapNodes(l);

	sprintf(fileName, "data/Height%d.vis", l->hmSize);
	f = fopen(fileName, "wb");

	if (!f) {
		err_report("lsc_writeOccToFile: cannot open %s", fileName);
		return;
	}

	space = (char*)(&(l->baseTextureTexObj)) - (char*)(l);
	space = (space + 15) & ~15;
	fwrite(l, 1, space, f);

	space = numNodesInHeightMap * l->sectors * l->maxOccPts *
												2 * sizeof(int);
	space = (space + 15) & ~15;
	fwrite(l->occPts, 1, space, f);

	fclose(f);
}

/*
  lsc_readOccFromFile

  Read the hierarchical terrain visibility data from file to memory.
*/
static bool lsc_readOccFromFile(lsc l)
{
	char fileName[30];
	FILE *f;
	size_t space;
	struct lsc_str ltemp;
	bool err = false;
	int numNodesInHeightMap = lsc_countHeightMapNodes(l);

	sprintf(fileName, "data/Height%d.vis", l->hmSize);
	f = fopen(fileName, "rb");

	if (!f) {
		err_report("lsc_readOccFromFile: cannot open %s", fileName);
		return false;
	}

	space = (char*)(&(l->baseTextureTexObj)) - (char*)(l);
	fread(&ltemp, 1, space, f);
	space = (space + 15) & ~15;
	fseek(f, space, SEEK_SET);

	if (l->hmSize != ltemp.hmSize) {
		err_report("lsc_readOccFromFile: hmSize mismatch");
		err = true;
	}
	if (l->occPatchSize != ltemp.occPatchSize) {
		err_report("lsc_readOccFromFile: occPatchSize mismatch");
		err = true;
	}
	if (l->sectors != ltemp.sectors) {
		err_report("lsc_readOccFromFile: sectors mismatch");
		err = true;
	}
	if (l->maxOccPts != ltemp.maxOccPts) {
		err_report("lsc_readOccFromFile: maxOccPts mismatch");
		err = true;
	}
	if (err) {
		fclose(f);
		return false;
	}

	space = numNodesInHeightMap * l->sectors * l->maxOccPts *
												2 * sizeof(int);
	fread(l->occPts, 1, space, f);

	fclose(f);
	return true;
}

/*
  lsc_calcOcclusion()

  Build merged occlusion regions for the heightmap quadtree.

  Creates an array containing maxOccPts 2D points for each sector for each
  element in the heightmap quadtree down to and including patch level. Each
  set of maxOccPts 2D points defines a convex occlusion region in the 2D
  space of the corresponding sectors piPlane.

  Note that the hmTile factor can be different than used by the source
  landcsape at rendering time. This is because there is no point in
  calculating the occlusion regions beyond the the far viewing distance.
*/
static void lsc_calcOcclusion(lsc l, unsigned char *hm, int hmTile,
							  int subSectors)
{
	struct lscvis_str vis;

	vis.hmSize = l->hmSize;
	vis.patchSize = l->occPatchSize;
	vis.hmTile = hmTile;
	vis.patchTile = l->hmSize / l->occPatchSize;
	vis.sectors = l->sectors;
	vis.subSectors = subSectors;
	vis.maxOccPts = l->maxOccPts;
	vis.sectorTrig = l->sectorTrig;
	vis.occPts = l->occPts;
	vis.hm = hm;
	vis.subSectorTrig = NULL;
	vis.horizonPoints = NULL;
	vis.occArr = NULL;
	vis.occFree = 0;
	vis.occList = NULL;
	vis.avgPtsRqd = 0;
	vis.maxPtsRqd = 0;

	/* Build sin and cos tables */
	printf("Calculating sin and cos tables");
	fflush(stdout);
	lscvis_buildTrigTables(&vis);
	if (!vis.sectorTrig || !vis.subSectorTrig) {
		err_report("Failed to build trig tables");
		return;
	}
	printf("\n");
	fflush(stdout);

	/* Allocate space for horizon points for each point in a single patch */
	vis.horizonPoints = (int*)malloc(4*SQR(vis.patchSize + 1)*sizeof(int));
	if (!vis.horizonPoints) {
		err_report("Failed to build occlusion regions");
		free(vis.subSectorTrig);
		return;
	}

	/* Allocate space for the current patch occlusion list */
	vis.occArr = (occNode)malloc(SQR(vis.patchSize + 1)
					* sizeof(struct occNode_str));
	if (!vis.occArr) {
		err_report("Failed to build patch occlusion list");
		free(vis.subSectorTrig);
		free(vis.horizonPoints);
		return;
	}

	printf("Calculating occlusion regions\n");
	fflush(stdout);
	lscvis_recurSetupQuadtree(&vis, 0, 0, vis.hmSize, 0, vis.hmSize);

	/* AW TODO: wrong! */
	vis.avgPtsRqd /= vis.sectors * SQR(vis.patchTile);

	printf("\n");
	printf("Avg Points Per Sector: %d\n", vis.avgPtsRqd);
	printf("Max Points Per Sector: %d\n", vis.maxPtsRqd);
	fflush(stdout);

	free(vis.subSectorTrig);
	free(vis.horizonPoints);
	free(vis.occArr);
}

/*
  lsc_buildTrigTables()

  Build sin & cos lookup tables.
*/
static void lsc_buildTrigTables(lsc l)
{
	struct lscvis_str vis;

	vis.hmSize = l->hmSize;
	vis.patchSize = l->occPatchSize;
	vis.hmTile = 1;
	vis.patchTile = l->hmSize / l->occPatchSize;
	vis.sectors = l->sectors;
	vis.subSectors = 1;
	vis.maxOccPts = l->maxOccPts;
	vis.sectorTrig = l->sectorTrig;
	vis.occPts = l->occPts;
	vis.hm = NULL;
	vis.subSectorTrig = NULL;
	vis.horizonPoints = NULL;
	vis.occArr = NULL;
	vis.occFree = 0;
	vis.occList = NULL;
	vis.avgPtsRqd = 0;
	vis.maxPtsRqd = 0;

	/* Build sin and cos tables */
	printf("Calculating sin and cos tables");
	fflush(stdout);
	lscvis_buildTrigTables(&vis);
	if (!vis.sectorTrig || !vis.subSectorTrig) {
		err_report("Failed to build trig tables");
		return;
	}
	printf("\n");
	fflush(stdout);

	free(vis.subSectorTrig);
}

/*
  lsc_getOcclusion()

  Read merged occlusion regions or calculate and dump to file for next time.
*/
void lsc_getOcclusion(lsc l, unsigned char *hm)
{
	if (!lsc_readOccFromFile(l)) {

		int hmTile = 2;
		int subSectors = 256 / l->sectors;
		if (subSectors < 1)
			subSectors = 1;

		lsc_calcOcclusion(l, hm, hmTile, subSectors);
		lsc_writeOccToFile(l);
	}
	else {
		lsc_buildTrigTables(l);
	}
}


/******************************************************************************
  RENDER TIME VISIBILITY TESTING
******************************************************************************/

/*
  lsc_perpProject()

  Perpendicular project a 3D world space point p into a 2D point pp
  in the piPlane for sector i.
*/
static void lsc_perpProject(lsc l, int *p, int i, int *pp)
{
	float s = l->sectorTrig[2*i + 0];
	float c = l->sectorTrig[2*i + 1];

	pp[0] = (int)floor(p[0]*c + p[1]*s + 0.5f);
	pp[1] = p[2];
}

/*
  lsc_isPatchVisibleSector()

  Check visibility of patch from viewpoint in sector i.
*/
static bool lsc_isPatchVisibleSector(lsc l, int *v, int n, int i)
{
	int sectors = l->sectors;
	int maxOccPts = l->maxOccPts;
	int *occPts = l->occPts;
	int pv[2];
	int j, y, dy, dx;

	/* Project viewpoint onto sector's pi plane */
	lsc_perpProject(l, v, i, pv);

	/* Get occlusion points for this patch and sector */
	occPts += n * sectors * maxOccPts * 2;
	occPts += i * maxOccPts * 2;

	/* Check projected viewpoint against occlusion region */
	if (pv[0] < occPts[0])
		return true;			/* Before occlusion region */

	for (j = 1; j < maxOccPts; j++) {
		if (pv[0] < occPts[2*j + 0])
			break;
	}
	if (j < maxOccPts) {
		dx = occPts[2*j + 0] - occPts[2*(j - 1) + 0];
		dy = occPts[2*j + 1] - occPts[2*(j - 1) + 1];
		y = (int)floor((float)(pv[0] - occPts[2*(j - 1) + 0])
							* (float)dy / (float)dx);
		y += occPts[2*(j - 1) + 1];
		if (pv[1] >= y)
			return true;	/* Above occlusion region */
	}

	return false;
}

/*
  lsc_getPatchSectors()

  Get the range of sector numbers in which the viewpoint lies
  relative to the specified patch.
*/
static void lsc_getPatchSectors(lsc l, int vx, int vy,
								int x0, int x1, int y0, int y1, int *i)
{
	int sectors = l->sectors;
	int j;
	float dx, dy, mx = 0.0f, my = 0.0f;
	float phi[4], mphi, minPhi = 0.0f, maxPhi = 0.0f;
	int corner[8];

	/* Patch corners */
	corner[0] = x0;
	corner[1] = y0;
	corner[2] = x0;
	corner[3] = y1;
	corner[4] = x1;
	corner[5] = y0;
	corner[6] = x1;
	corner[7] = y1;

	for (j = 0; j < 4; j++) {
		dx = vx - corner[2*j + 0];
		dy = vy - corner[2*j + 1];
		mx += dx;
		my += dy;
		if (dy == 0.0f) {
			if (dx >= 0.0f)
				phi[j] = 0.0f;
			else
				phi[j] = M_PI;
		}
		else if (dx == 0.0f) {
			if (dy >= 0.0f)
				phi[j] = 0.5f * M_PI;
			else
				phi[j] = 1.5f * M_PI;
		}
		else {
			phi[j] = atan2(dy, dx);
			if (phi[j] < 0.0f)
				phi[j] += 2*M_PI;
		}
	}
	if (my == 0.0f) {
		if (mx >= 0.0f)
			mphi = 0.0f;
		else
			mphi = M_PI;
	}
	else if (mx == 0.0f) {
		if (my >= 0.0f)
			mphi = 0.5f*M_PI;
		else
			mphi = 1.5f*M_PI;
	}
	else {
		mphi = atan2(my, mx);
		if (mphi < 0.0f)
			mphi += 2*M_PI;
	}

	for (j = 0; j < 4; j++) {
		phi[j] -= mphi;
		if (phi[j] > M_PI)
			phi[j] -= 2*M_PI;
		if (phi[j] < minPhi)
			minPhi = phi[j];
		else if (phi[j] > maxPhi)
			maxPhi = phi[j];
	}
	minPhi += mphi;
	maxPhi += mphi;

	i[0] = (int)floor(0.5f + (minPhi*sectors) / (2*M_PI));
	i[1] = (int)floor(0.5f + (maxPhi*sectors) / (2*M_PI));
	i[0] %= sectors;
	i[1] %= sectors;
	if (i[0] < 0)
		i[0] += sectors;
	if (i[1] < 0)
		i[1] += sectors;
}

/*
  lsc_isPatchVisible()

  Check visibility of patch from viewpoint.
*/
bool lsc_isPatchVisible(lsc l, int *v, int n, int x0, int x1, int y0, int y1)
{
	int i[2], vpt[3], j, sectors = l->sectors;

	if (v[0] >= x0 && v[0] <= x1 &&
		v[1] >= y0 && v[1] <= y1)
		return true;

	lsc_getPatchSectors(l, v[0], v[1], x0, x1, y0, y1, i);
	vpt[0] = v[0] - ((x0 / l->patchSize) / l->patchTile) * l->hmSize;
	vpt[1] = v[1] - ((y0 / l->patchSize) / l->patchTile) * l->hmSize;
	vpt[2] = v[2];
	j = i[0];
	while (true) {
		if (lsc_isPatchVisibleSector(l, vpt, n, j))
			return true;
		if (++j >= sectors)
			j = 0;
		if (j == i[1])
			break;
	}
	return false;
}
