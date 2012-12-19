/**
 * RglPrims.h
 * Copyright 2000 by Mark B. Allan
 * 
 * GL counterparts to RenderMan quadric primitives.
 * Be aware of handedness switch.
 *
 * Put these calls in display lists! There was no attempt made at 
 * efficiency - each call has many calls to sin() and new/delete.
 *
 * The RenderMan(R) Interface Procedures and RIB Protocol are
 * Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan(R) is a registered trademark of Pixar.
 */
#ifndef RglPrims_h
#define RglPrims_h

#include "mmath.h"

#include <math.h>
#include <GL/gl.h>

#define D2RAD(d) (2.0*M_PI*d/360.0)

void RglDisk (float height, float radius, float thetamax, int detail, bool dcomp = true);
void RglCone(float height, float radius, float thetamax, int detail, bool dcomp = true);
void RglCylinder(float radius, float zmin, float zmax, float thetamax, int detail, bool dcomp = true);
void RglSphere (float radius, float zmin, float zmax, float thetamax, int detail);
void RglSphereN (float radius, float zmin, float zmax, float thetamax, int detail, MVertex &nscale);

void RglBilinearPatch (float p[2][2][3], int detail);
void RglCube(MVertex &c, MVertex &v1, MVertex &v2, float depth, int detail = 2, GLuint *texArray = 0);

float	*vecReplace (float *vec, int index, float value); 
float	*copyVec(float *newVec, float *oldVec, int dim);
float	*calcNormal (float *norm, float *vert1, float *vert2, float *vert3 );
float	*normalizeVec (float *vec);

#endif
