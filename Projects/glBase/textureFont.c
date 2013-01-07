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
	File:		textureFont.c

	Function:	Class for drawing texture mapped fonts.
*/

#include "textureFont.h"
#include "opengl.h"

tmf tmf_create(char *tgaFilename, int fWidth, int fHeight, float fScale)
{
	tmf t = (tmf)malloc(sizeof(struct tmf_str));
	if (t)
		tmf_default(t, tgaFilename, fWidth, fHeight, fScale);
	return t;
}

void tmf_default(tmf t, char *tgaFilename, int fWidth, int fHeight, float fScale)
{
	unsigned char alphaColour[3] = {0, 0, 255};
	if (t) {
		pic_defaultAlpha(&t->p, tgaFilename, alphaColour);
		t->fontWidth  = fWidth;
		t->fontHeight = fHeight;
		t->texWidth   = (int)floor(fWidth * fScale);
		t->texHeight  = (int)floor(fHeight * fScale);
	}
}

void tmf_destroy(tmf t)
{
	if (t)
		free(t);
}

void tmf_drawString(tmf t, int x, int y, char *str, float alpha)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	pic_drawString(&t->p, x, y, str, t->texWidth, t->texHeight,
					t->fontWidth, t->fontHeight);

	glDisable(GL_BLEND);
}

void tmf_drawStrings(tmf t, int x, int y, char **str, float alpha)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	while (*str) {
		pic_drawString(&t->p, x, y, *(str++), t->texWidth, t->texHeight,
						t->fontWidth, t->fontHeight);
		y += t->texHeight;
	}

	glDisable(GL_BLEND);
}
