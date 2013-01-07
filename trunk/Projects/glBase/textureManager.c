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
	File:		textureManager.c

	Function:	Manage loading textures into OpenGL texture objects.
*/

#include "textureManager.h"
#include "tgafile.h"
#include "opengl.h"

typedef struct txm_str {
	char *name;
	int texNum;
	struct txm_str *next;
} *txm;

static txm globalTexList;

static txm txm_create(char *name)
{
	int size;
	txm t;

	size = strlen(name) + 1 + sizeof(struct txm_str);
	t = (txm)malloc(size);
	if (t) {
		t->name = (char *)(t + 1);
		strcpy(t->name, name);
		t->texNum = 0;
		t->next = globalTexList;
		globalTexList = t;
	}
	return t;
}

static void txm_destroy(txm t)
{
	txm *tp = &globalTexList;
	if (t) {
		/* Find the pointer to t in the list */
		while (*tp != t)
			tp = &((*tp)->next);
		*tp = t->next;
		free(t);
	}
}

static txm txm_find(char *name)
{
	txm t = globalTexList;
	while (t) {
		if (SCMP(name, t->name))
			return t;
		t = t->next;
	}
	return NULL;
}

static int txm_genTexObject(unsigned char *texData, int w, int h,
								int format, bool repeat, bool mipmap)
{
	unsigned int texNum;

	glGenTextures(1, &texNum);

	if (texData) {

		glBindTexture(GL_TEXTURE_2D, texNum);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		/* Set the tiling mode */
		if (repeat) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else if (globalGL.supportsEdgeClamp) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}

		/* Set the filtering */
		if (mipmap) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
													GL_LINEAR_MIPMAP_LINEAR);
			gluBuild2DMipmaps(GL_TEXTURE_2D, format, w, h,
						format, GL_UNSIGNED_BYTE, texData);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0,
						format, GL_UNSIGNED_BYTE, texData);
		}
	}
	return texNum;
}

int txm_addRawFile(char *filename, int w, int h, int format, bool repeat, bool mipmap)
{
	int texNum = 0;
	unsigned char *data = NULL;
	int depth;
	FILE *f;
	txm t;

	t = txm_find(filename);
	if (t)
		return t->texNum;
	t = txm_create(filename);
	if (!t)
		return 0;

	switch (format) {
	case GL_RGBA:
		depth = 4;
		break;
	case GL_RGB:
		depth = 3;
		break;
	default:
	case GL_ALPHA:
		depth = 1;
		break;
	}
	f = fopen(filename, "rb");
	if (f) {
		data = (unsigned char*)malloc(w * h * depth);
		if (data)
			fread(data, sizeof(unsigned char), w * h * depth, f);
		fclose(f);
	}

	if (data) {
		texNum = txm_genTexObject(data, w, h, format, repeat, mipmap);
		free(data);
		t->texNum = texNum;
	}
	else
		txm_destroy(t);

	return texNum;
}

int txm_addTgaFile(char *filename, bool alpha, bool repeat, bool mipmap)
{
	int texNum = 0;
	tga f;
	txm t;

	t = txm_find(filename);
	if (t)
		return t->texNum;
	t = txm_create(filename);
	if (!t)
		return 0;

	f = tga_create(filename);
    if (f) {
		if (f->depth == 32 && alpha) {
			texNum = txm_genTexObject(f->data,f->width,f->height,
											GL_RGBA,repeat,mipmap);
			t->texNum = texNum;
		}
		else if (f->depth == 32) {
			tga_stripAlpha(f);
			texNum = txm_genTexObject(f->data,f->width,f->height,
											GL_RGB,repeat,mipmap);
			t->texNum = texNum;
		}
		else if (f->depth == 24 && !alpha) {
			texNum = txm_genTexObject(f->data,f->width,f->height,
											GL_RGB,repeat,mipmap);
			t->texNum = texNum;
		}
		else if (f->depth == 8 && alpha) {
			texNum = txm_genTexObject(f->data,f->width,f->height,
											GL_ALPHA,repeat,mipmap);
			t->texNum = texNum;
		}
		else
			txm_destroy(t);
		tga_destroy(f);
	}
	else
		txm_destroy(t);

	return texNum;
}

int txm_addRawData(unsigned char* data, int w, int h, int format, bool repeat, bool mipmap)
{
	int texNum = 0;
	txm t;

	t = txm_create("");
	if (!t)
		return 0;

	if (data) {
		texNum = txm_genTexObject(data, w, h, format, repeat, mipmap);
		t->texNum = texNum;
	}
	else
		txm_destroy(t);

	return texNum;
}

void txm_deleteTextures()
{
	while(globalTexList) {
		glDeleteTextures(1, &globalTexList->texNum);
		txm_destroy(globalTexList);
	}
}
