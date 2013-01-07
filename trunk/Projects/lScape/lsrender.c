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
	File:		lsrender.c

	Function:	Landscape rendering functions.
*/

#include "lscape.h"
#include "opengl.h"

/*
 * Render a landscape patch.
 */
static void lsc_renderPatch(lsc l, int n, int m,
							int x0, int x1, int y0, int y1, int mode)
{
	int px = x0 / l->patchSize;
	int py = y0 / l->patchSize;
	lscpatch patch = lsc_getPatch(l, px, py);
	int npx, npy, i, nL, icount;
	vec3 offset;
	float *vtxArr;
	unsigned short *idxArr;
	bool useTextureCombine = mode & 0x10;

	mode &= 0x0F;

	/* Check if rebuild required */
	nL = 0;
	for (i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			/* Left */
			npy = py;
			npx = px - 1;
			break;
		case 1:
			/* Right */
			npy = py;
			npx = px + 1;
			break;
		case 2:
			/* Top */
			npy = py + 1;
			npx = px;
			break;
		case 3:
			/* Bottom */
			npy = py - 1;
			npx = px;
			break;
		}

		if (npx >= 0 && npx < l->patchTile * l->hmTile
				&& npy >= 0 && npy < l->patchTile * l->hmTile)
			nL |= (lsc_getPatch(l, npx, npy)->newLevel & 0xFF) << i*8;
	}

	/* Integer math - get rid of remainders */
	offset[0] = ((x0 / l->patchSize) / l->patchTile)
						* l->hmSize * l->scale[0];
	offset[1] = ((y0 / l->patchSize) / l->patchTile)
						* l->hmSize * l->scale[1];
	offset[2] = 0;

	glTranslatef(offset[0], offset[1], offset[2]);

	vtxArr = lsc_getPatchVtxArr(l, m, x0, x1, y0, y1);
	idxArr = lsc_getPatchIdxArr(l, n, patch->newLevel, nL);
	icount = 3 * (int)(*idxArr);
	idxArr++;

	if (!mode) {
		/* Line mode */
		glVertexPointer(3, GL_FLOAT, 32, vtxArr);
		glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);
	}
	else if (mode == 1) {
		/* Single pass texture mode */
		if (globalGL.numTextureUnits > 1) {
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 6);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		}
		glBindTexture(GL_TEXTURE_2D, patch->colourMapTexObj);
		glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 4);
		glVertexPointer(3, GL_FLOAT, 32, vtxArr);
		glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);
	}
	else if (!useTextureCombine) {
		/* Texture splat mode, assume multitexture extensions */

		/* Get distance blend factor */
		cam c = cam_getGlobalCamera();
		float a;
		if (patch->d > c->farPlane/2.0f) {
			/* Too far, no splatting */
			a = 1.0f;
		}
		else if (patch->d > c->farPlane/4.0f) {
			/* Middle distance, fade out splatting */
			a = (2.0f*patch->d)/c->farPlane;
			if (a < 0.5f)
				a = 0.5f;
			else if (a > 1.0f)
				a = 1.0f;
		}
		else {
			/* Foreground */
			a = 0.5f;
		}

		if (a == 1.0f) {

			/* Too far, no splatting */
			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, patch->colourMapTexObj);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 4);
			glVertexPointer(3, GL_FLOAT, 32, vtxArr);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		}
		else {

			/* Set distance blend factor */
			glColor4f(0.0f, 0.0f, 0.0f, a);

			glEnable(GL_BLEND);

			/* glBlendFunc(source, dest); */
			glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, l->splatTextureTexObj[0]);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 6);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[1]);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 4);
			glVertexPointer(3, GL_FLOAT, 32, vtxArr);

			if (globalGL.supportsCVA && mode == 3)
				globalGL.glLockArraysEXT(0,
					lsc_numPatchVtxs(l->patchSize, patch->newLevel));

			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			/* glBlendFunc(source, dest); */
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, l->splatTextureTexObj[1]);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[2]);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, l->splatTextureTexObj[2]);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[3]);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			/* glBlendFunc(source, dest); */
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, patch->colourMapTexObj);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);

			glDisable(GL_BLEND);

			if (globalGL.supportsCVA && mode == 3)
				globalGL.glUnlockArraysEXT();
		}
	}
	else {
		/* Texture splat mode, assume multitexture extensions and
		   texture_env_combine extension */

		/* Get distance blend factor */
		cam c = cam_getGlobalCamera();
		float a;
		if (patch->d > c->farPlane/2.0f) {
			/* Too far, no splatting */
			a = 1.0f;
		}
		else if (patch->d > c->farPlane/4.0f) {
			/* Middle distance, fade out splatting */
			a = (2.0f*patch->d)/c->farPlane;
			if (a < 0.5f)
				a = 0.5f;
			else if (a > 1.0f)
				a = 1.0f;
		}
		else {
			/* Foreground */
			a = 0.5f;
		}

		if (a == 1.0f) {

			/* Too far, no splatting */
			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[0]);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 4);
			glVertexPointer(3, GL_FLOAT, 32, vtxArr);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		}
		else {

			/* Set distance blend factor */
			glColor4f(0.0f, 0.0f, 0.0f, a);

			glEnable(GL_BLEND);

			/* glBlendFunc(source, dest); */
			glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_ONE_MINUS_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[1]);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 4);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, l->splatTextureTexObj[0]);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, 32, vtxArr + 6);
			glVertexPointer(3, GL_FLOAT, 32, vtxArr);

			if (globalGL.supportsCVA && mode == 3)
				globalGL.glLockArraysEXT(0,
					lsc_numPatchVtxs(l->patchSize, patch->newLevel));

			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			/* glBlendFunc(source, dest); */
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[2]);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, l->splatTextureTexObj[1]);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, patch->blendMapTexObj[3]);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, l->splatTextureTexObj[2]);
			glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, idxArr);

			glDisable(GL_BLEND);

			if (globalGL.supportsCVA && mode == 3)
				globalGL.glUnlockArraysEXT();
		}
	}

	/*
	 * Set fence for a patch so the cache manager won't reallocate its vertex
	 * vertex array until it has been DMA'd.
	 */
	if (l->patchVtxCch.fast) {
		cchobj *obj = lsc_getPatchVtxPtr(l,m);
		globalGL.glSetFenceNV((*obj)->fence, GL_ALL_COMPLETED_NV);
	}

	glTranslatef(-offset[0], -offset[1], -offset[2]);
}

/*
 * Recursivley render the visible parts of the landscape quadtree.
 */
static void lsc_recurRender(lsc l, int n, int m,
							int x0, int x1, int y0, int y1, int mode)
{
	int node = l->quadtree[n];
	int clipFlags = LSCQT_CF(node);

	if (clipFlags == 1)
		return;			/* Outside view frustum */

	/* If not a leaf recurse to child nodes */
	if (x1 - x0 > l->hmSize) {

		int xc, yc;
		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_recurRender(l, LSCQT_BL(n), 0, x0, xc, y0, yc, mode);
		lsc_recurRender(l, LSCQT_BR(n), 0, xc, x1, y0, yc, mode);
		lsc_recurRender(l, LSCQT_TL(n), 0, x0, xc, yc, y1, mode);
		lsc_recurRender(l, LSCQT_TR(n), 0, xc, x1, yc, y1, mode);
	}
	else if (x1 - x0 > l->patchSize) {

		int xc, yc;
		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_recurRender(l, LSCQT_BL(n), LSCQT_BL(m), x0, xc, y0, yc, mode);
		lsc_recurRender(l, LSCQT_BR(n), LSCQT_BR(m), xc, x1, y0, yc, mode);
		lsc_recurRender(l, LSCQT_TL(n), LSCQT_TL(m), x0, xc, yc, y1, mode);
		lsc_recurRender(l, LSCQT_TR(n), LSCQT_TR(m), xc, x1, yc, y1, mode);
	}

	/* Leaf - render the patch */
	else {

		lsc_renderPatch(l, n, m, x0, x1, y0, y1, mode);
	}
}

/*
 * Recursivley render the visible parts of the landscape quadtree.
 */
static void lsc_occRecurRender(lsc l, int n, int m, int *vpt,
							   int x0, int x1, int y0, int y1, int mode)
{
	int node = l->quadtree[n];
	int clipFlags = LSCQT_CF(node);

	if (clipFlags == 1)
		return;			/* Outside view frustum */

	/* If not a leaf recurse to child nodes */
	if (x1 - x0 > l->hmSize) {

		int xc, yc;
		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_occRecurRender(l, LSCQT_BL(n), 0, vpt, x0, xc, y0, yc, mode);
		lsc_occRecurRender(l, LSCQT_BR(n), 0, vpt, xc, x1, y0, yc, mode);
		lsc_occRecurRender(l, LSCQT_TL(n), 0, vpt, x0, xc, yc, y1, mode);
		lsc_occRecurRender(l, LSCQT_TR(n), 0, vpt, xc, x1, yc, y1, mode);
	}
	else if (x1 - x0 > l->occPatchSize) {

		int xc, yc;

		if (!lsc_isPatchVisible(l, vpt, m, x0, x1, y0, y1))
			return;			/* Occluded */

		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_occRecurRender(l, LSCQT_BL(n), LSCQT_BL(m), vpt,
													x0, xc, y0, yc, mode);
		lsc_occRecurRender(l, LSCQT_BR(n), LSCQT_BR(m), vpt,
													xc, x1, y0, yc, mode);
		lsc_occRecurRender(l, LSCQT_TL(n), LSCQT_TL(m), vpt,
													x0, xc, yc, y1, mode);
		lsc_occRecurRender(l, LSCQT_TR(n), LSCQT_TR(m), vpt,
													xc, x1, yc, y1, mode);
	}
	else if (x1 - x0 > l->patchSize) {

		int xc, yc;

		if (x1 - x0 == l->occPatchSize)
			if (!lsc_isPatchVisible(l, vpt, m, x0, x1, y0, y1))
				return;			/* Occluded */

		xc = (x0 + x1) >> 1;
		yc = (y0 + y1) >> 1;
		lsc_occRecurRender(l, LSCQT_BL(n), LSCQT_BL(m), vpt,
													x0, xc, y0, yc, mode);
		lsc_occRecurRender(l, LSCQT_BR(n), LSCQT_BR(m), vpt,
													xc, x1, y0, yc, mode);
		lsc_occRecurRender(l, LSCQT_TL(n), LSCQT_TL(m), vpt,
													x0, xc, yc, y1, mode);
		lsc_occRecurRender(l, LSCQT_TR(n), LSCQT_TR(m), vpt,
													xc, x1, yc, y1, mode);
	}

	/* Leaf - render the patch */
	else {

		if (x1 - x0 == l->occPatchSize)
			if (!lsc_isPatchVisible(l, vpt, m, x0, x1, y0, y1))
				return;			/* Occluded */

		lsc_renderPatch(l, n, m, x0, x1, y0, y1, mode);
	}
}

/*
 * Render the visible parts of the landscape.
 */
void lsc_render(lsc l, int mode, bool occDisable, bool useTextureCombine)
{
	cch_initFrame(&l->patchVtxCch);
	cch_initFrame(&l->patchIdxCch);

	if (mode == 0) {
		/* Line mode */
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (globalGL.numTextureUnits > 1) {
			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		}
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else if (mode == 1) {
		/* Single pass texture mode */
		if (globalGL.numTextureUnits > 1) {
			globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, l->baseTextureTexObj);
			globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
			globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		}
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else {
		/* Multi-pass texture splat mode, we can 
		   assume multi-texture extensions exist */
		globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	mode |= (useTextureCombine << 4);

	if (occDisable) {
		lsc_recurRender(l, 0, 0,
			0, l->hmSize * l->hmTile,
			0, l->hmSize * l->hmTile,
			mode);
	}
	else {
		float *origin = cam_getGlobalCamera()->viewPt.origin;
		int vpt[3];
		vpt[0] = (int)floor(0.5f + origin[0] / l->scale[0]);
		vpt[1] = (int)floor(0.5f + origin[1] / l->scale[1]);
		vpt[2] = (int)floor(0.5f + origin[2] / l->scale[2]);
		lsc_occRecurRender(l, 0, 0, vpt,
			0, l->hmSize * l->hmTile,
			0, l->hmSize * l->hmTile,
			mode);
	}
	
	/* Set back to default mode */
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (globalGL.numTextureUnits > 1) {
		globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
		globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
	}
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	/* Check for cache thrashing */
	if (cch_thrashed(&l->patchVtxCch))
		printf("Vertex array cache thrashed\n");
	/* Check for cache thrashing */
	if (cch_thrashed(&l->patchIdxCch))
		printf("Index array cache thrashed\n");
}
