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
	File:		main.h

	Function:	Application independent main() function.
*/

#ifndef MAIN_H
#define MAIN_H

extern int win_width;
extern int win_height;

void screenShot();
void key(unsigned char, int, int);
void draw();
int  main(int, char**);

/* We expect the application to provide these */
const char* appWindowName();
void appInit(int, char**);
void appKey(unsigned char, int, int);
void appDraw();
void appCleanup();

#endif /* MAIN_H */
