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
	File:		callback.h

	Function:	callback class, provides a set of functions that
				will be used as callback functions by GLUT.
*/

#ifndef CALLBACK_H
#define CALLBACK_H

typedef struct cbk_str {
	void (*mouse)(struct cbk_str *c, int button, int state, int x, int y);
	void (*motion)(struct cbk_str *c, int x, int y);
	void (*special)(struct cbk_str *c, int key, int x, int y);
	void (*update)(struct cbk_str *c);
	void (*keyboard)(struct cbk_str *c, unsigned char key, int x, int y);
} *cbk;

void cbk_update(void);
void cbk_keyboard(unsigned char key, int x, int y);
void cbk_init(void);
void cbk_register(cbk c);

#endif /* CALLBACK_H */
