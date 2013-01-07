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
	File:		opengl.c

	Function:	opengl class determines available opengl extensions
				and prints out driver details.
*/

#include "opengl.h"

struct ogl_str globalGL;

#ifndef WIN32

/* Assume LINUX */

#include <dlfcn.h>

void ogl_init(void)
{
	char *extensions = (char*)glGetString(GL_EXTENSIONS);
	void *libHandle = dlopen(NULL, RTLD_LAZY);
	char *c;
	int done = 0;
	int r, g, b, a, i, d;

	glGetIntegerv(GL_RED_BITS, &r);
	glGetIntegerv(GL_GREEN_BITS, &g);
	glGetIntegerv(GL_BLUE_BITS, &b);
	glGetIntegerv(GL_ALPHA_BITS, &a);
	glGetIntegerv(GL_INDEX_BITS, &i);
	glGetIntegerv(GL_DEPTH_BITS, &d);

	printf("RGBAID = %d, %d, %d, %d, %d, %d\n", r, g, b, a, i, d);

	extensions = (char*)malloc(strlen(extensions) + 1);

	globalGL.numTextureUnits = 0;
	globalGL.supportsCVA = false;
	globalGL.supportsEdgeClamp = false;
	globalGL.supportsPBuffer = false;
	globalGL.supportsMakeCurrentRead = false;
	globalGL.supportsVARFence = false;
	globalGL.supportsTextureCombine = false;

	strcpy(extensions, glGetString(GL_EXTENSIONS));
	c = strtok(extensions, " ");
	while (c) {
		if (SICMP(c, "GL_ARB_multitexture") && !globalGL.numTextureUnits) {
			printf("Supports ARB_multitexture - ");
			globalGL.glMultiTexCoord2fARB =
				(GLMULTITEXCOORD2FARBPROC)dlsym(libHandle, "glMultiTexCoord2fARB");
			globalGL.glActiveTextureARB =
				(GLACTIVETEXTUREARBPROC)dlsym(libHandle, "glActiveTextureARB");
			globalGL.glClientActiveTextureARB =
				(GLCLIENTACTIVETEXTUREARBPROC)dlsym(libHandle, "glClientActiveTextureARB");
			glGetIntegerv(GL_MAX_TEXTURES_UNITS_ARB, &globalGL.numTextureUnits);
			printf("%d units\n", globalGL.numTextureUnits);
			done++;
		}
		else if (SICMP(c, "GL_EXT_compiled_vertex_array") && !globalGL.supportsCVA) {
			printf("Supports EXT_compiled_vertex_array\n");
			globalGL.glLockArraysEXT =
				(GLLOCKARRAYSEXTPROC)dlsym(libHandle, "glLockArraysEXT");
			globalGL.glUnlockArraysEXT =
				(GLUNLOCKARRAYSEXTPROC)dlsym(libHandle, "glUnlockArraysEXT");
			globalGL.supportsCVA = true;
			done++;
		}
		else if (SICMP(c, "GL_EXT_texture_edge_clamp") && !globalGL.supportsEdgeClamp) {
			printf("Supports EXT_texture_edge_clamp\n");
			globalGL.supportsEdgeClamp = true;
			done++;
		}
		else if (SICMP(c, "GL_ARB_texture_env_combine") && !globalGL.supportsTextureCombine) {
			printf("Supports ARB_texture_env_combine\n");
			globalGL.supportsTextureCombine = true;
			done++;
		}
		if (done == 4)
			break;
		c = strtok(NULL, " ");
	}

	free(extensions);
	fflush(stdout);

	dlclose(libHandle);
}

#else

/* WINDOWS */

void ogl_init(void)
{
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;
	char *extensions = (char*)glGetString(GL_EXTENSIONS);
	char *c;
	int done = 0;
	int r, g, b, a, i, d;

	glGetIntegerv(GL_RED_BITS, &r);
	glGetIntegerv(GL_GREEN_BITS, &g);
	glGetIntegerv(GL_BLUE_BITS, &b);
	glGetIntegerv(GL_ALPHA_BITS, &a);
	glGetIntegerv(GL_INDEX_BITS, &i);
	glGetIntegerv(GL_DEPTH_BITS, &d);

	printf("RGBAID = %d, %d, %d, %d, %d, %d\n", r, g, b, a, i, d);

	extensions = (char*)malloc(strlen(extensions) + 1);

	globalGL.numTextureUnits = 0;
	globalGL.supportsCVA = false;
	globalGL.supportsEdgeClamp = false;
	globalGL.supportsPBuffer = false;
	globalGL.supportsMakeCurrentRead = false;
	globalGL.supportsVARFence = false;
	globalGL.supportsTextureCombine = false;

	strcpy(extensions, glGetString(GL_EXTENSIONS));
	c = strtok(extensions, " ");
	while (c) {
		if (SICMP(c, "GL_ARB_multitexture") && !globalGL.numTextureUnits) {
			printf("Supports ARB_multitexture - ");
			globalGL.glMultiTexCoord2fARB =
				(GLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
			globalGL.glActiveTextureARB =
				(GLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
			globalGL.glClientActiveTextureARB =
				(GLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
			glGetIntegerv(GL_MAX_TEXTURES_UNITS_ARB, &globalGL.numTextureUnits);
			printf("%d units\n", globalGL.numTextureUnits);
			done++;
		}
		else if (SICMP(c, "GL_EXT_compiled_vertex_array") && !globalGL.supportsCVA) {
			printf("Supports EXT_compiled_vertex_array\n");
			globalGL.glLockArraysEXT =
				(GLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArraysEXT");
			globalGL.glUnlockArraysEXT =
				(GLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArraysEXT");
			globalGL.supportsCVA = true;
			done++;
		}
		else if (SICMP(c, "GL_EXT_texture_edge_clamp") && !globalGL.supportsEdgeClamp) {
			printf("Supports EXT_texture_edge_clamp\n");
			globalGL.supportsEdgeClamp = true;
			done++;
		}
		else if (SICMP(c, "GL_NV_vertex_array_range") &&
								!(globalGL.supportsVARFence & 0x01)) {
			printf("Supports NV_vertex_array_range\n");
			globalGL.supportsVARFence |= 0x01;
			globalGL.glFlushVertexArrayRangeNV =
				(PFNGLFLUSHVERTEXARRAYRANGENVPROC)wglGetProcAddress("glFlushVertexArrayRangeNV");
			globalGL.glVertexArrayRangeNV =
				(PFNGLVERTEXARRAYRANGENVPROC)wglGetProcAddress("glVertexArrayRangeNV");
			globalGL.wglAllocateMemoryNV =
				(PFNWGLALLOCATEMEMORYNVPROC)wglGetProcAddress("wglAllocateMemoryNV");
			globalGL.wglFreeMemoryNV =
				(PFNWGLFREEMEMORYNVPROC)wglGetProcAddress("wglFreeMemoryNV");
			done++;
		}
		else if (SICMP(c, "GL_NV_Fence") &&
								!(globalGL.supportsVARFence & 0x02)) {
			printf("Supports NV_Fence\n");
			globalGL.supportsVARFence |= 0x02;
			globalGL.glDeleteFencesNV =
				(PFNGLDELETEFENCESNVPROC)wglGetProcAddress("glDeleteFencesNV");
			globalGL.glGenFencesNV =
				(PFNGLGENFENCESNVPROC)wglGetProcAddress("glGenFencesNV");
			globalGL.glIsFenceNV =
				(PFNGLISFENCENVPROC)wglGetProcAddress("glIsFenceNV");
			globalGL.glTestFenceNV =
				(PFNGLTESTFENCENVPROC)wglGetProcAddress("glTestFenceNV");
			globalGL.glGetFenceivNV =
				(PFNGLGETFENCEIVNVPROC)wglGetProcAddress("glGetFenceivNV");
			globalGL.glFinishFenceNV =
				(PFNGLFINISHFENCENVPROC)wglGetProcAddress("glFinishFenceNV");
			globalGL.glSetFenceNV =
				(PFNGLSETFENCENVPROC)wglGetProcAddress("glSetFenceNV");
			done++;
		}
		else if (SICMP(c, "GL_ARB_texture_env_combine") && !globalGL.supportsTextureCombine) {
			printf("Supports ARB_texture_env_combine\n");
			globalGL.supportsTextureCombine = true;
			done++;
		}
		if (done == 6)
			break;
		c = strtok(NULL, " ");
	}

	free(extensions);
	fflush(stdout);

	/*
	 * The WGL extension specs say we have to use wglGetExtensionsStringARB
	 * to check for support.
	 */
	wglGetExtensionsStringARB =
		(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if (!wglGetExtensionsStringARB)
		return;
	extensions = (char*)wglGetExtensionsStringARB(wglGetCurrentDC());
	extensions = (char*)malloc(strlen(extensions) + 1);
	strcpy(extensions, wglGetExtensionsStringARB(wglGetCurrentDC()));
	c = strtok(extensions, " ");
	done = 0;
	while (c) {
		if (SICMP(c, "WGL_ARB_pixel_format") &&
								!(globalGL.supportsPBuffer & 0x01)) {
			printf("Supports WGL_ARB_pixel_format\n");
			globalGL.wglGetPixelFormatAttribivARB =
				(PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
			globalGL.wglGetPixelFormatAttribfvARB =
				(PFNWGLGETPIXELFORMATATTRIBFVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
			globalGL.wglChoosePixelFormatARB =
				(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			globalGL.supportsPBuffer |= 0x01;
			done++;
		}
		else if (SICMP(c, "WGL_ARB_pbuffer") &&
								!(globalGL.supportsPBuffer & 0x02)) {
			printf("Supports WGL_ARB_pbuffer\n");
			globalGL.wglCreatePbufferARB =
				(PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
			globalGL.wglGetPbufferDCARB =
				(PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
			globalGL.wglReleasePbufferDCARB =
				(PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
			globalGL.wglDestroyPbufferARB =
				(PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
			globalGL.wglQueryPbufferARB =
				(PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
			globalGL.supportsPBuffer |= 0x02;
			done++;
		}
		else if (SICMP(c, "WGL_ARB_make_current_read") &&
								!globalGL.supportsMakeCurrentRead) {
			printf("Supports WGL_ARB_make_current_read\n");
			globalGL.wglMakeContextCurrentARB =
				(PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
			globalGL.wglGetCurrentReadDCARB =
				(PFNWGLGETCURRENTREADDCARBPROC)wglGetProcAddress("wglGetCurrentReadDCARB");
			globalGL.supportsMakeCurrentRead = true;
			done++;
		}
		if (done == 3)
			break;
		c = strtok(NULL, " ");
	}

	/* Need pixel format & pbuffer together */
	if (globalGL.supportsPBuffer == 0x03)
		globalGL.supportsPBuffer = true;
	else
		globalGL.supportsPBuffer = false;

	/* Need VAR & Fence togther */
	if (globalGL.supportsVARFence == 0x03)
		globalGL.supportsVARFence = true;
	else
		globalGL.supportsVARFence = false;

	free(extensions);
	fflush(stdout);
}

#endif
