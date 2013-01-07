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
	File:		pic.c

	Function:	Pic class for drwing 2D tga pictures and texture mapped
				fonts.
*/

#include "opengl.h"
#include "tgafile.h"
#include "textureManager.h"
#include "pic.h"

pic pic_create(char *tgaFilename)
{
	pic p = (pic)malloc(sizeof(struct pic_str));
	if (p)
		pic_default(p, tgaFilename);
	return p;
}

void pic_default(pic p, char *tgaFilename)
{
	tga t = tga_create(tgaFilename);
	if (t && (t->depth >= 24)) {
		if (p) {
			p->width = t->width;
			p->height = t->height;
			p->texnum = txm_addRawData(t->data, t->width, t->height,
				(t->depth == 32) ? GL_RGBA : GL_RGB, false, true);
		}
	}
	if (t)
		tga_destroy(t);
}

pic pic_createAlpha(char *tgaFilename, unsigned char *alphaColour)
{
	pic p = (pic)malloc(sizeof(struct pic_str));
	if (p)
		pic_defaultAlpha(p, tgaFilename, alphaColour);
	return p;
}

void pic_defaultAlpha(pic p, char *tgaFilename, unsigned char *alphaColour)
{
	int size, bpp, i;
	unsigned char *data, *temp;
	tga t = tga_create(tgaFilename);
	if (t && (t->depth >= 24)) {
		if (p) {
			p->width = t->width;
			p->height = t->height;
			size = t->width * t->height;
			bpp = (t->depth == 32) ? 4 : 3;
			data = t->data;
			temp = (unsigned char*)malloc(size * 4);
			for (i = 0; i < size; i++) {
				temp[i*4 + 0] = data[i*bpp + 0];
				temp[i*4 + 1] = data[i*bpp + 1];
				temp[i*4 + 2] = data[i*bpp + 2];
				if (data[i*bpp + 0] == alphaColour[0] &&
					data[i*bpp + 1] == alphaColour[1] &&
					data[i*bpp + 2] == alphaColour[2] )
					temp[i*4 + 3] = 0;
				else
					temp[i*4 + 3] = 255;
			}
			p->texnum = txm_addRawData(temp, t->width, t->height,
										GL_RGBA, false, true);
		}
	}
	if (t)
		tga_destroy(t);
}

void pic_destroy(pic p)
{
	if (p)
		free(p);
}

void pic_draw(pic p, int x, int y)
{
	/* Assume top left is 0, 1 */
	glBindTexture(GL_TEXTURE_2D, p->texnum);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x, y);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x, y + p->height);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x + p->width, y + p->height);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(x + p->width, y);
	glEnd();
}

void pic_drawCharacter(pic p, int x, int y, int num, int w, int h, int tw, int th)
{
	int row, col;
	float t, s;
	float ssize, tsize;

	if (num == 32)
		return;			/* space */
	if (x <= -w)
		return;			/* off screen */
	if (y <= -h)
		return;			/* off screen */

	num -= ' ';
	num &= 255;
	row = num >> 4;
	col = num & 15;

	ssize = ((float)tw - 0.5f) / 256.0f;
	tsize = ((float)th - 0.5f) / 256.0f;
	s = ((float)col*tw + 0.5f) / 256.0f;
	t = 1.0f - ((float)row*th + 0.5f) / 256.0f;

	/* Assume top left is 0, 0 */
	glBindTexture(GL_TEXTURE_2D, p->texnum);
	glBegin(GL_QUADS);
	glTexCoord2f(s, t);
	glVertex2f(x, y);
	glTexCoord2f(s, t - tsize);
	glVertex2f(x, y + h);
	glTexCoord2f(s + ssize, t - tsize);
	glVertex2f(x + w, y + h);
	glTexCoord2f(s + ssize, t);
	glVertex2f(x + w, y);
	glEnd();
}

void pic_drawString(pic p, int x, int y, char *str, int w, int h, int tw, int th)
{
	while (*str) {
		pic_drawCharacter(p, x, y, (int)(*str++), w, h, tw, th);
		x += w;
	}
}
