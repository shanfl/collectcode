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
	File:		lscollide.c

	Function:	Landscape collision detection functions.
*/

#include "lscape.h"
#include "opengl.h"
#include <limits.h>

/*
	Trace stuff to file - designed to work with Quake
	so we can see how Quake is using collision detection.
*/

/* #define TRACE */

#ifdef TRACE
#include <fcntl.h>
#include "../quakedef.h"

static bool lsc_trace = false;			/* Is trace on or off */

static void trace(char *fmt, ...)
{
	va_list argptr; 
	static char data[1024];
	static int fd = -1;
	static char *file = "C:/lsctrace.log";

	if (!lsc_trace)
		return;

	va_start(argptr, fmt);
	vsprintf(data, fmt, argptr);
	va_end(argptr);
	if (fd < 0)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	write(fd, data, strlen(data));
	/* close(fd);
	   fd = -1; */
}

static void toggleTrace(void)
{
	lsc_trace = !lsc_trace;
	Con_Printf("lsctrace %s\n", lsc_trace ? "on" : "off");
	trace("lsctrace %s\n", lsc_trace ? "on" : "off");
}

void lsc_initTrace(void)
{
	Cmd_AddCommand("lsctrace", toggleTrace);
	lsc_trace = false;
}

#else

void lsc_initTrace(void)
{
	return;
}

#endif

/******************************************************************************/
/*                        COLLISION DETECTION STUFF                           */
/******************************************************************************/

#define LSC_EPSILON		0.03125f/* Epsilon for landscape collision detection */
#define USE_Z_AXIS					/* Use z axis only for closest approach */

/*
 * Check for intersection of a line segment with a quadtree node bounding
 * box.
 * 
 * Returns:
 *
 *   0 if completely outside
 *   1 if completely inside
 *   2 if overlap
 *
 */
static int lsc_lineIntersectNode(lsc l, int n,
								 int x0, int x1, int y0, int y1,
								 vec3 *p, bbox entBox, bool nosize,
								 bbox moveBox)
{
	int clip, node = l->quadtree[n];
	float s[2];

	/* Construct the node bounding box */
	struct bbox_str nodeBox;
	nodeBox.minp[0] = x0;
	nodeBox.minp[1] = y0;
	nodeBox.minp[2] = LSCQT_Z0(node);
	nodeBox.maxp[0] = x1;
	nodeBox.maxp[1] = y1;
	nodeBox.maxp[2] = LSCQT_Z1(node);
	vec3_mul(nodeBox.minp, l->scale, nodeBox.minp);
	vec3_mul(nodeBox.maxp, l->scale, nodeBox.maxp);

	/* Check move box against node box */
	clip = bbox_isInsideBox(moveBox, &nodeBox);
	if (clip == 2) {
		/* Boxes overlap - check line against node box */
		/* Note that the node must be enlarged by the moving entity's size
		   to allow for later adjustment to the line start and end points */
		if (!nosize) {
			vec3_add(nodeBox.minp, entBox->minp, nodeBox.minp);
			vec3_add(nodeBox.maxp, entBox->maxp, nodeBox.maxp);
		}
		if (!bbox_intersectLine(&nodeBox, p[0], p[1], &s[0], &s[1]))
			clip = 0;					/* Line misses enlarged node box */
	}

	return clip;
}

/*
 * Check for collision with a landscape patch.
 */
static bool lsc_patchCheckCollision(lsc l, int n, int m,
									int x0, int x1, int y0, int y1,
									vec3 *p, vec3 move, bbox moveBox,
									bbox entBox, bbox entAbsBox, bool nosize,
									float *frac, plane collisionPlane)
{
	int px = x0 / l->patchSize;
	int py = y0 / l->patchSize;
	lscpatch patch = lsc_getPatch(l, px, py);
	vec3 offset;
	float *vtxArr;
	unsigned short *idxArr;
	int i, j, triCount;

	float *tv[3];
	vec3 v0, v1, v2;
	struct plane_str plane;
	float d1, d2, t;
	vec3 p_adj[2];
	bool intersect, ret = false;

	/* Barycentric coordinate test */
	float alpha, beta, gamma;

	/* Integer math - get rid of remainders */
	offset[0] = ((x0 / l->patchSize) / l->patchTile)
						* l->hmSize * l->scale[0];
	offset[1] = ((y0 / l->patchSize) / l->patchTile)
						* l->hmSize * l->scale[1];
	offset[2] = 0;

	vec3_sub(p[0], offset, p[0]);
	vec3_sub(p[1], offset, p[1]);

	vtxArr = lsc_getPatchVtxArrCollisionDetect(l, m, x0, x1, y0, y1);
	idxArr = lsc_getPatchIdxArrCollisionDetect(l, n);
	triCount = (int)(*idxArr);
	idxArr++;

#ifdef TRACE
	trace("Patch %d %d\n", m, n);
	trace("p[0] p[1]: %f, %f, %f : %f, %f, %f\n", 
				p[0][0], p[0][1], p[0][2],
				p[1][0], p[1][1], p[1][2]);
	{
	entity_t *ent = &cl_entities[cl.viewentity];
	trace("player: %f, %f, %f\n",
				ent->origin[0], ent->origin[1], ent->origin[2]);
	}
#endif

	/* For each triangle */
	for (i = 0; i < triCount; i++) {

		/*
		 * AW TODO: Should probably have a deeper quadtree for collision
		 * detection than for rendering to prevent testing against so many
		 * triangles
		 */
		
		/* Vertex stride is 8 */
		for (j = 0; j < 3; j++)
			tv[j] = &vtxArr[8 * (int)(*idxArr++)];

		/* Get the plane of the triangle */
		vec3_sub(tv[1], tv[0], v1);
		vec3_sub(tv[2], tv[0], v2);
		vec3_cross(v1, v2, plane.v);

		/* Make sure the normal points up */
		if (plane.v[2] < 0.0f)
			vec3_mulS(plane.v, -1.0f, plane.v);
		plane.d = -vec3_dot(plane.v, tv[0]);
		plane.signbits = 0;

		/* Get an offset to adjust the line start and end by the distance
		   of closest approach of the face plane with the entity bound box */
#ifdef USE_Z_AXIS
		vec3_cpy(p[0], p_adj[0]);
		vec3_cpy(p[1], p_adj[1]);
		if (!nosize) {
			p_adj[0][2] += entBox->minp[2];
			p_adj[1][2] += entBox->minp[2];
		}
#else
		for (j = 0; j < 3; j++) {
			p_adj[0][j] = p[0][j];
			p_adj[1][j] = p[1][j];
			if (!nosize) {
				if (plane.v[j] < -LSC_EPSILON) {
					p_adj[0][j] += entBox->maxp[j];
					p_adj[1][j] += entBox->maxp[j];
				}
				else if (plane.v[j] > LSC_EPSILON) {
					p_adj[0][j] += entBox->minp[j];
					p_adj[1][j] += entBox->minp[j];
				}
			}
		}
#endif

		/* Test for intersection of line with plane */
		d1 = vec3_dot(plane.v, p_adj[0]) + plane.d;
		d2 = vec3_dot(plane.v, p_adj[1]) + plane.d;
		if (d1 < -LSC_EPSILON || d2 >= LSC_EPSILON)
			continue;						/* No intersection */
		if ((d1 - d2) == 0.0f)
			continue;						/* No intersection */

		/* Get intersection point */
		t = d1 / (d1 - d2);
		if (t > 1.0f)
			t = 1.0f;
		else if (t < 0.0f)
			t = 0.0f;
		p_adj[1][0] = p_adj[0][0] + t * (p_adj[1][0] - p_adj[0][0]);
		p_adj[1][1] = p_adj[0][1] + t * (p_adj[1][1] - p_adj[0][1]);
		p_adj[1][2] = p_adj[0][2] + t * (p_adj[1][2] - p_adj[0][2]);

		/* Check if the collision point is inside the triangle */
		/* Barycentric coord test 0 <= alpha, beta, gamma <= 1 */
		intersect = true;
		v0[0] = p_adj[1][0] - tv[0][0];			/* xp - x1 */
		v0[1] = p_adj[1][1] - tv[0][1];			/* yp - y1 */

		/* We know we don't get zero area triangles so don't worry
		   about div by zero
		alpha = (v0[0]*v1[1] - v1[0]*v0[1]) / (v2[0]*v1[1] - v1[0]*v2[1])
		beta  = (v0[0]*v2[1] - v2[0]*v0[1]) / (v1[0]*v2[1] - v2[0]*v1[1]) */
		if (v1[1] == 0.0f)
			alpha = (v1[0]*v0[1]) / (v1[0]*v2[1]);
		else if (v1[0] == 0.0f)
			alpha = (v0[0]*v1[1]) / (v2[0]*v1[1]);
		else
			alpha = (v0[0]*v1[1] - v1[0]*v0[1]) / (v2[0]*v1[1] - v1[0]*v2[1]);
		if (alpha < 0.0f || alpha > 1.0f) {
			intersect = false;				/* No intersection*/
		}
		else {
			if (v2[1] == 0.0f)
				beta = (v2[0]*v0[1]) / (v2[0]*v1[1]);
			else if (v2[0] == 0.0f)
				beta = (v0[0]*v2[1]) / (v1[0]*v2[1]);
			else
				beta = (v0[0]*v2[1] - v2[0]*v0[1]) / (v1[0]*v2[1] - v2[0]*v1[1]);
			if (beta < 0.0f || beta > 1.0f) {
				intersect = false;			/* No intersection*/
			}
			else {
				gamma = 1.0f - (alpha + beta);
				if (gamma < 0.0f || gamma > 1.0f) {
					intersect = false;		/* No intersection*/
				}
			}
		}
			
		/* If no intersection, check if the triangle will clip
		   the entity bound box */

		if (!intersect && !nosize) {

			/* Find the point on the triangle perimeter
			   closest to the intersection point */

			int k;
			vec3 t0, t1, closest;
			float proj, lensq;

#ifdef TRACE
			trace("Tri ");
#endif

			d2 = (float)INT_MAX * (float)INT_MAX;

			/* For each triangle edge */
			k = 2;
			for (j = 0; j < 3; j++) {

#ifdef TRACE
				trace(": %f %f %f", tv[j][0], tv[j][1], tv[j][2]);
#endif

				/* Get projection of tv0->p_adj[1] on edge tv0->tv1 */
				vec3_sub(tv[j], tv[k], t0);
				vec3_sub(p_adj[1], tv[k], t1);
				proj = vec3_dot(t0, t1);

				if (proj <= 0.0f) {
					/* tv0 is closest on this edge */
					d1 = vec3_dot(t1, t1);
					if (d1 < d2) {
						d2 = d1;
						vec3_cpy(tv[k], closest);
					}
				}
				else {
					lensq = vec3_dot(t0, t0);
					if (proj >= lensq) {
						/* tv1 is closest on this edge */
						vec3_sub(p_adj[1], tv[j], t1);
						d1 = vec3_dot(t1, t1);
						if (d1 < d2) {
							d2 = d1;
							vec3_cpy(tv[j], closest);
						}
					}
					else {
						/* closest point on this edge is
						   between and perp to tv0 & tv1 */
						proj /= lensq;
						vec3_mulS(t0, proj, t0);
						vec3_add(tv[k], t0, t0);
						vec3_sub(p_adj[1], t0, t1);
						d1 = vec3_dot(t1, t1);
						if (d1 < d2) {
							d2 = d1;
							vec3_cpy(t0, closest);
						}
					}
				}

				k = j;
			}

#ifdef TRACE
			trace("\n");
#endif

			/* See if the closest point on the triangle
			   will hit the entity's bounding box */
			/* AW DEBUG */
			vec3_sub(closest, move, t0);
			vec3_add(closest, move, t1);
			intersect = bbox_intersectLine(
					entAbsBox, t0, /*closest*/ t1, &proj, &lensq);
		}

		if (intersect) {
			
			/* We hit this triangle */

			ret = true;

			t -= LSC_EPSILON;
			if (t < 0)
				t = 0;
			p[1][0] = p[0][0] + t * (p[1][0] - p[0][0]);
			p[1][1] = p[0][1] + t * (p[1][1] - p[0][1]);
			p[1][2] = p[0][2] + t * (p[1][2] - p[0][2]);
			*frac *= t;

			/* Return the plane */
			t = 1.0f / vec3_getLen(plane.v);
			vec3_mulS(plane.v, t, collisionPlane->v);
			collisionPlane->d = t * plane.d;
		}
	}

	vec3_add(p[0], offset, p[0]);
	vec3_add(p[1], offset, p[1]);
	
	if (ret) {
	
		/* Recalc bounding box of move - only p[1] has changed */
		for (i = 0; i < 3; i++) {
			if (p[0][i] < p[1][i])
				moveBox->maxp[i] = p[1][i] + entBox->maxp[i];
			else
				moveBox->minp[i] = p[1][i] + entBox->minp[i];
		}
	}

	return ret;
}

/*
 * Recursivley check for collision with the landscape quadtree.
 */
static bool lsc_recurCheckCollision(lsc l, int n, int m,
									int x0, int x1, int y0, int y1,
									vec3 *p, vec3 move, bbox moveBox,
									bbox entBox, bbox entAbsBox, bool nosize,
									float *frac, plane collisionPlane)
{
	int clip, i, xc, yc;
	int order[4];
	bool ret = false;
		
	/* If leaf */
	if (x1 - x0 <= l->patchSize)
		return lsc_patchCheckCollision(l, n, m, x0, x1, y0, y1,
					p, move, moveBox, entBox, entAbsBox, nosize,
					frac, collisionPlane);

	xc = (x0 + x1) >> 1;
	yc = (y0 + y1) >> 1;

	/* Order the quarter spaces */
	if (p[0][1] < (yc * l->scale[1])) {
		/* Bottom then top */
		if (p[0][0] < (xc * l->scale[0])) {
			/* Left then right */
			order[0] = 0;		/* Bottom left */
			order[1] = 1;		/* Bottom right */
			order[2] = 2;		/* Top left */
			order[3] = 3;		/* Top right */
		}
		else {
			/* Right then left */
			order[0] = 1;		/* Bottom right */
			order[1] = 0;		/* Bottom left */
			order[2] = 3;		/* Top right */
			order[3] = 2;		/* Top left */
		}
	}
	else {
		/* Top then bottom */
		if (p[0][0] < (xc * l->scale[0])) {
			/* Left then right */
			order[0] = 2;		/* Top left */
			order[1] = 3;		/* Top right */
			order[2] = 0;		/* Bottom left */
			order[3] = 1;		/* Bottom right */
		}
		else {
			/* Right then left */
			order[0] = 3;		/* Top right */
			order[1] = 2;		/* Top left */
			order[2] = 1;		/* Bottom right */
			order[3] = 0;		/* Bottom left */
		}
	}

	/* Ordered check for collision with child nodes */
	if (x1 - x0 > l->hmSize) {
		for (i = 0; i < 4; i++) {
			switch (order[i]) {
			case 0:
				clip = lsc_lineIntersectNode(l, LSCQT_BL(n), x0, xc, y0, yc,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_BL(n), 0,
							x0, xc, y0, yc,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			case 1:
				clip = lsc_lineIntersectNode(l, LSCQT_BR(n), xc, x1, y0, yc,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_BR(n), 0,
							xc, x1, y0, yc,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			case 2:
				clip = lsc_lineIntersectNode(l, LSCQT_TL(n), x0, xc, yc, y1,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_TL(n), 0,
							x0, xc, yc, y1,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			case 3:
				clip = lsc_lineIntersectNode(l, LSCQT_TR(n), xc, x1, yc, y1,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_TR(n), 0,
							xc, x1, yc, y1,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			}
			if (clip == 1)
				break;		/* No need to test other nodes */
		}
	}
	else {
		for (i = 0; i < 4; i++) {
			switch (order[i]) {
			case 0:
				clip = lsc_lineIntersectNode(l, LSCQT_BL(n), x0, xc, y0, yc,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_BL(n), LSCQT_BL(m),
							x0, xc, y0, yc,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			case 1:
				clip = lsc_lineIntersectNode(l, LSCQT_BR(n), xc, x1, y0, yc,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_BR(n), LSCQT_BR(m),
							xc, x1, y0, yc,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			case 2:
				clip = lsc_lineIntersectNode(l, LSCQT_TL(n), x0, xc, yc, y1,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_TL(n), LSCQT_TL(m),
							x0, xc, yc, y1,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			case 3:
				clip = lsc_lineIntersectNode(l, LSCQT_TR(n), xc, x1, yc, y1,
										p, entBox, nosize, moveBox);
				if (clip)
					ret |= lsc_recurCheckCollision(l, LSCQT_TR(n), LSCQT_TR(m),
							xc, x1, yc, y1,
							p, move, moveBox, entBox, entAbsBox, nosize,
							frac, collisionPlane);
				break;
			}
			if (clip == 1)
				break;		/* No need to test other nodes */
		}
	}
	return ret;
}

/*
 * Check for collision with the landscape quadtree.
 */
bool lsc_checkCollision(lsc l, vec3 *p, bbox entBox,
						float *frac, plane collisionPlane)
{
	struct bbox_str entAbsBox, moveBox;
	vec3 move;
	int i;
	bool nosize = false;

	/* Get the movement direction vector */
	vec3_sub(p[1], p[0], move);
	vec3_norm(move);
	vec3_mulS(move, 9999.0f, move);

	/* Set up bounding boxes */
	for (i = 0; i < 3; i++) {

		if ((entBox->maxp[i] - entBox->minp[i]) == 0.0f)
			nosize = true;

		entAbsBox.minp[i] = p[0][i] + entBox->minp[i];
		entAbsBox.maxp[i] = p[0][i] + entBox->maxp[i];

		if (p[0][i] < p[1][i]) {
			moveBox.minp[i] = p[0][i] + entBox->minp[i];
			moveBox.maxp[i] = p[1][i] + entBox->maxp[i];
		}
		else {
			moveBox.minp[i] = p[1][i] + entBox->minp[i];
			moveBox.maxp[i] = p[0][i] + entBox->maxp[i];
		}
	}

	/* Recursively check for collision with the landscape quadtree */
	return lsc_recurCheckCollision(l, 0, 0,
							0, l->hmSize * l->hmTile,
							0, l->hmSize * l->hmTile,
							p, move, &moveBox, entBox, &entAbsBox, nosize,
							frac, collisionPlane);
}
