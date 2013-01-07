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
	File:		tgafile.c

	Function:	Targa image loading class.
*/

#include "tgafile.h"
#include "error.h"

static bool tga_load(tga t, char *filename);
static unsigned char* tga_getData(tga t, FILE *f);

static bool tga_load(tga t, char *filename)
{
	unsigned char type[4];
	unsigned char info[7];
	FILE *f;

	if (!(f = fopen(filename, "r+bt")))
		return false;

	/* Read in type and info blocks */
	fread(type, sizeof(unsigned char), 3, f);
	fseek(f, 12, SEEK_SET);
	fread(info, sizeof(unsigned char), 6, f);

	/* Skip to image data */
	fseek(f, type[0], SEEK_CUR);

	/* Wrong file type ? */
	if (type[1] != 0 || (type[2] != 2 && type[2] != 3))
		return false;

	t->width  = (unsigned int)info[0] + (unsigned int)info[1] * 256;
	t->height = (unsigned int)info[2] + (unsigned int)info[3] * 256;
	t->depth  = (unsigned int)info[4];

	/* Make sure we are loading a supported type */
	if (t->depth != 8 && t->depth != 24 && t->depth != 32)
		return false;

	t->data = tga_getData(t, f);

	fclose(f);

	/* No image data */
	if (!t->data)
		return false;
	return true;
}

static unsigned char* tga_getData(tga t, FILE *f)
{
	unsigned char *d, temp;
	int bpp = (t->depth == 8) ? 1 : ((t->depth == 24) ? 3 : 4);
	int size = t->width * t->height;
	int i, bread = 0;

	/* Alocate space for image bytes */
	d = (unsigned char *)malloc(size * bpp);
	if (!d)
		return d;

	/* Read image bytes */
	for (i = t->height-1; i >= 0; i--)
		bread += fread(&d[i*t->width*bpp], sizeof(unsigned char), t->width*bpp, f);

	/* All bytes properly read ? */
	if (bread != size * bpp) {
		free(d);
		return NULL;
	}

	if (bpp > 1) {
		/* Swap the R & B bytes */
		for (i = 0; i < size; i++) {
			temp = d[i * bpp + 0];
			d[i * bpp + 0] = d[i * bpp + 2];
			d[i * bpp + 2] = temp;
		}
	}

	return d;
}

tga tga_create(char *filename)
{
	tga t = (tga)malloc(sizeof(struct tga_str));
	if (t) {
		if (!tga_load(t, filename)) {
			free(t);
			t = NULL;
		}
	}
	return t;
}

void tga_destroy(tga t)
{
	if (t) {
		if (t->data)
			free(t->data);
		free(t);
	}
}

void tga_stripAlpha(tga t)
{
	int i, size;

	if (t->depth != 32)
		return;

	/* Remove alpha */
	size = t->width * t->height;
	for (i = 0; i < size; i++) {
		t->data[i * 3 + 0] = t->data[i * 4 + 0];
		t->data[i * 3 + 1] = t->data[i * 4 + 1];
		t->data[i * 3 + 2] = t->data[i * 4 + 2];
	}
}
