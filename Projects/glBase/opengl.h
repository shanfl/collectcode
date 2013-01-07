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
	File:		opengl.h

	Function:	opengl class determines available opengl extensions
				and prints out driver details.
*/

#ifndef OPENGL_H
#define OPENGL_H

#include <gl/glut.h>
#include "general.h"

/* EXT_texture_edge_clamp */
#define GL_CLAMP_TO_EDGE                    0x812F

/* ARB_multitexture */
#define GL_MAX_TEXTURES_UNITS_ARB           0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_MAX_ELEMENTS_VERTICES            0x80E8
#define GL_MAX_ELEMENTS_INDICES             0x80E9

/* ARB_texture_env_combine */
#define GL_COMBINE_ARB                      0x8570
#define GL_COMBINE_RGB_ARB                  0x8571
#define GL_COMBINE_ALPHA_ARB                0x8572
#define GL_SOURCE0_RGB_ARB                  0x8580
#define GL_SOURCE1_RGB_ARB                  0x8581
#define GL_SOURCE2_RGB_ARB                  0x8582
#define GL_SOURCE0_ALPHA_ARB                0x8588
#define GL_SOURCE1_ALPHA_ARB                0x8589
#define GL_SOURCE2_ALPHA_ARB                0x858A
#define GL_OPERAND0_RGB_ARB                 0x8590
#define GL_OPERAND1_RGB_ARB                 0x8591
#define GL_OPERAND2_RGB_ARB                 0x8592
#define GL_OPERAND0_ALPHA_ARB               0x8598
#define GL_OPERAND1_ALPHA_ARB               0x8599
#define GL_OPERAND2_ALPHA_ARB               0x859A
#define GL_RGB_SCALE_ARB                    0x8573
#define GL_ADD_SIGNED_ARB                   0x8574
#define GL_INTERPOLATE_ARB                  0x8575
#define GL_SUBTRACT_ARB                     0x84E7
#define GL_CONSTANT_ARB                     0x8576
#define GL_PRIMARY_COLOR_ARB                0x8577
#define GL_PREVIOUS_ARB                     0x8578

#ifndef WIN32

/* Assume LINUX */

/* EXT_compiled_vertex_array */
typedef void (*GLLOCKARRAYSEXTPROC)(GLint first, GLsizei count);
typedef void (*GLUNLOCKARRAYSEXTPROC)();

/* ARB_multitexture */
typedef void (*GLMULTITEXCOORD2FARBPROC)(GLenum target, GLfloat s, GLfloat t);
typedef void (*GLACTIVETEXTUREARBPROC)(GLenum target);
typedef void (*GLCLIENTACTIVETEXTUREARBPROC)(GLenum target);

/*
 * I'm not going to try supporting GLX pixel buffers or VAR/Fence until my Linux
 * X-Windows system is up and running again.
 */

#else

/* WINDOWS */

/* EXT_compiled_vertex_array */
typedef void (APIENTRY * GLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * GLUNLOCKARRAYSEXTPROC) ();

/* ARB_multitexture */
typedef void (APIENTRY * GLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * GLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * GLCLIENTACTIVETEXTUREARBPROC) (GLenum target);

/* WGL_ARB_extension_string */
typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hDC);

/* WGL_ARB_pixel_format */
#define WGL_NUMBER_PIXEL_FORMATS_ARB         0x2000
#define WGL_DRAW_TO_WINDOW_ARB               0x2001
#define WGL_DRAW_TO_BITMAP_ARB               0x2002
#define WGL_ACCELERATION_ARB                 0x2003
#define WGL_NEED_PALETTE_ARB                 0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB          0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB           0x2006
#define WGL_SWAP_METHOD_ARB                  0x2007
#define WGL_NUMBER_OVERLAYS_ARB              0x2008
#define WGL_NUMBER_UNDERLAYS_ARB             0x2009
#define WGL_TRANSPARENT_ARB                  0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB        0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB      0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB       0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB      0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB      0x203B
#define WGL_SHARE_DEPTH_ARB                  0x200C
#define WGL_SHARE_STENCIL_ARB                0x200D
#define WGL_SHARE_ACCUM_ARB                  0x200E
#define WGL_SUPPORT_GDI_ARB                  0x200F
#define WGL_SUPPORT_OPENGL_ARB               0x2010
#define WGL_DOUBLE_BUFFER_ARB                0x2011
#define WGL_STEREO_ARB                       0x2012
#define WGL_PIXEL_TYPE_ARB                   0x2013
#define WGL_COLOR_BITS_ARB                   0x2014
#define WGL_RED_BITS_ARB                     0x2015
#define WGL_RED_SHIFT_ARB                    0x2016
#define WGL_GREEN_BITS_ARB                   0x2017
#define WGL_GREEN_SHIFT_ARB                  0x2018
#define WGL_BLUE_BITS_ARB                    0x2019
#define WGL_BLUE_SHIFT_ARB                   0x201A
#define WGL_ALPHA_BITS_ARB                   0x201B
#define WGL_ALPHA_SHIFT_ARB                  0x201C
#define WGL_ACCUM_BITS_ARB                   0x201D
#define WGL_ACCUM_RED_BITS_ARB               0x201E
#define WGL_ACCUM_GREEN_BITS_ARB             0x201F
#define WGL_ACCUM_BLUE_BITS_ARB              0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB             0x2021
#define WGL_DEPTH_BITS_ARB                   0x2022
#define WGL_STENCIL_BITS_ARB                 0x2023
#define WGL_AUX_BUFFERS_ARB                  0x2024
#define WGL_NO_ACCELERATION_ARB              0x2025
#define WGL_GENERIC_ACCELERATION_ARB         0x2026
#define WGL_FULL_ACCELERATION_ARB            0x2027
#define WGL_SWAP_EXCHANGE_ARB                0x2028
#define WGL_SWAP_COPY_ARB                    0x2029
#define WGL_SWAP_UNDEFINED_ARB               0x202A
#define WGL_TYPE_RGBA_ARB                    0x202B
#define WGL_TYPE_COLORINDEX_ARB              0x202C

typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iLayerPlane,
                                        UINT nAttributes,
                                        const int *piAttributes,
                                        int *piValues);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iLayerPlane,
                                        UINT nAttributes,
                                        const int *piAttributes,
                                        FLOAT *pfValues);
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (
                                        HDC hDC,
                                        const int *piAttribIList,
                                        const FLOAT *pfAttribFList,
                                        UINT nMaxFormats,
                                        int *piFormats,
                                        UINT *nNumFormats);

/* WGL_ARB_make_current_read */
typedef BOOL (WINAPI * PFNWGLMAKECONTEXTCURRENTARBPROC) (
                                        HDC hDrawDC,
                                        HDC hReadDC,
                                        HGLRC hGLRC);
typedef HDC (WINAPI * PFNWGLGETCURRENTREADDCARBPROC) (VOID);

/* WGL_ARB_pbuffer */
#define WGL_DRAW_TO_PBUFFER_ARB              0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB           0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB            0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB           0x2030
#define WGL_PBUFFER_LARGEST_ARB              0x2033
#define WGL_PBUFFER_WIDTH_ARB                0x2034
#define WGL_PBUFFER_HEIGHT_ARB               0x2035
#define WGL_PBUFFER_LOST_ARB				 0x2036

DECLARE_HANDLE(HPBUFFERARB);

typedef HPBUFFERARB (WINAPI * PFNWGLCREATEPBUFFERARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iWidth,
                                        int iHeight,
                                        const int *piAttribList);
typedef HDC (WINAPI * PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef int (WINAPI * PFNWGLRELEASEPBUFFERDCARBPROC) (
                                        HPBUFFERARB hPbuffer,
                                        HDC hDC);
typedef BOOL (WINAPI * PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
typedef BOOL (WINAPI * PFNWGLQUERYPBUFFERARBPROC) (
                                        HPBUFFERARB hPbuffer,
                                        int iAttribute,
                                        int *piValue);

/* GL_NV_VERTEX_ARRAY_RANGE */
#define GL_VERTEX_ARRAY_RANGE_NV             0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV      0x851E
#define GL_VERTEX_ARRAY_RANGE_VALID_NV       0x851F
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_NV     0x8521
typedef void (WINAPI * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef void (WINAPI * PFNGLVERTEXARRAYRANGENVPROC) (
										GLsizei size,
										const GLvoid *pointer);
typedef void* (WINAPI * PFNWGLALLOCATEMEMORYNVPROC) (
										int size,
										float readfreq,
										float writefreq,
										float priority);
typedef void  (WINAPI * PFNWGLFREEMEMORYNVPROC) (void *pointer);

/* GL_NV_FENCE */
#define GL_ALL_COMPLETED_NV                  0x84F2
#define GL_FENCE_STATUS_NV                   0x84F3
#define GL_FENCE_CONDITION_NV                0x84F4
typedef void (WINAPI * PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef void (WINAPI * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef GLboolean (WINAPI * PFNGLISFENCENVPROC) (GLuint fence);
typedef GLboolean (WINAPI * PFNGLTESTFENCENVPROC) (GLuint fence);
typedef void (WINAPI * PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
typedef void (WINAPI * PFNGLFINISHFENCENVPROC) (GLuint fence);
typedef void (WINAPI * PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);

#endif

typedef struct ogl_str {
	/* Multitexture function pointers */
	GLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
	GLACTIVETEXTUREARBPROC glActiveTextureARB;
	GLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

	/* Compiled vertex array function pointers */
	GLLOCKARRAYSEXTPROC glLockArraysEXT;
	GLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

#ifndef WIN32

/* Assume LINUX */

	/*
	 * I'm not going to try supporting GLX pixel buffers or VAR/Fence until my Linux
	 * X-Windows system is up and running again.
	 */

#else

/* WINDOWS */

	/* WGL_ARB_pixel_format */
	PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
	PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

	/* WGL_ARB_make_current_read */
	PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
	PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB;

	/* WGL_ARB_pbuffer */
	PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
	PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
	PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
	PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
	PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;

	/* GL_NV_VERTEX_ARRAY_RANGE */
	PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
	PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
	PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;

	/* GL_NV_FENCE */
	PFNGLDELETEFENCESNVPROC glDeleteFencesNV;
	PFNGLGENFENCESNVPROC glGenFencesNV;
	PFNGLISFENCENVPROC glIsFenceNV;
	PFNGLTESTFENCENVPROC glTestFenceNV;
	PFNGLGETFENCEIVNVPROC glGetFenceivNV;
	PFNGLFINISHFENCENVPROC glFinishFenceNV;
	PFNGLSETFENCENVPROC glSetFenceNV;

#endif

	int numTextureUnits;
	bool supportsCVA;
	bool supportsEdgeClamp;
	bool supportsPBuffer;
	bool supportsMakeCurrentRead;
	bool supportsVARFence;
	bool supportsTextureCombine;
} *ogl;

extern struct ogl_str globalGL;

void ogl_init(void);

#endif /* OPENGL_H */
