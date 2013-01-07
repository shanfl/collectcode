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
	File:		main.c

	Function:	Application independent main() function.
*/

#include "general.h"
#include "opengl.h"
#include "error.h"
#include "main.h"

/* #define _DEBUG */

/*
 * Window globals
 */
static int window;					/* The number of our GLUT window */
static int fullscreen = 0;			/* toggle fullscreen */
static int win_x_position = 50;		/* window position on screen */
static int win_y_position = 50;
int win_width  = 640;		/* window size */
int win_height = 480;

/*
 * Frame timer globals
 */
static int timer_frames;
static int timer_start;
static int timer_end;

/*
 * Take a screen shot and dump to a raw pixel file on the root drive
 */
void screenShot()
{
	unsigned char *data;
	int i = 0;
	bool foundname = false;
	char filename[128];
	FILE *f;

	/* Read the pixels back */
	data = (unsigned char*)malloc(win_width * win_height * 3);
	if (!data)
		return;
	glReadPixels(0, 0, win_width, win_height, GL_RGB, GL_UNSIGNED_BYTE, data);

	/* Get the screen shot number */
	while (!foundname) {
		sprintf(filename, "%ssshot%d.raw", TEMPDIR, i);
		f = fopen(filename, "rb");
		if (!f)
			foundname = true;
		else
			fclose(f);
		i++;
	}

	f = fopen(filename, "wb");
	if (!f)
		err_report("screenShot: cannot open %s", filename);
	else {
		fwrite(data, 1, win_width * win_height * 3, f);
		fclose(f);
	}
	free(data);
}

/*
 * Resize the GLUT window
 */
GLvoid resizeViewport(GLsizei width, GLsizei height)
{
	win_width = width;
	win_height = height;
	if (!win_height)
		win_height = 1;
	glViewport(0, 0, win_width, win_height);
}

/*
 * Draw frame
 */
GLvoid drawScene(void)
{
	appDraw();
	glutSwapBuffers();
	timer_frames++;
}

/*
 * Stop graphics rendering when in iconised state (win and unix)
 * or when fully covered (unix only)
 */
GLvoid drawIconised(int state)
{
	switch (state) {
	case GLUT_VISIBLE:
		glutIdleFunc(drawScene);
		break;

	case GLUT_NOT_VISIBLE:
		glutIdleFunc(NULL);
		break;

	default:
		break;
	}
}

/*
 * Basic key handler.
 */
GLvoid keyPressed(unsigned char key, int x, int y)
{
	float fps;

	switch (key) {
	case 'f':
	case 'F':
		fullscreen = !fullscreen;
		if (fullscreen) {
			win_x_position = glutGet((GLenum)GLUT_WINDOW_X);
			win_y_position = glutGet((GLenum)GLUT_WINDOW_Y);
			win_width = glutGet((GLenum)GLUT_WINDOW_WIDTH);
			win_height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);
			glutFullScreen();
		}
		else {
			glutReshapeWindow(win_width, win_height);
			glutPositionWindow(win_x_position, win_y_position);
		}
		break;

	/* Hitting s takes a screenshot */
	case 's':
	case 'S':
		screenShot();
		break;

	/* Hitting escape exits */
	case 27:
		timer_end = glutGet(GLUT_ELAPSED_TIME);
		fps = 1000.0f * (float)timer_frames / (float)(timer_end-timer_start);
		printf("FPS: %f\n", fps);
		appCleanup();
		exit(0);
		break;

	default:
		break;
	}

	/* Check to see if the application wants the key */
	appKey(key, x, y);
}

/*
 * main()
 */
int main(int argc, char** argv) {
	/*
	 * Initialize GLUT state - see documentation at
	 * http://reality.sgi.com/mjk/spec3/spec3.html
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(win_width,win_height);
	glutInitWindowPosition(0, 0);
	window = glutCreateWindow(appWindowName());

	/* Tell us about the implementation */
	printf("Vendor:     %s\n", glGetString(GL_VENDOR));
	printf("Renderer:   %s\n", glGetString(GL_RENDERER));
	printf("Version:    %s\n", glGetString(GL_VERSION));
	printf("Extensions: %s\n", glGetString(GL_EXTENSIONS));

	/* Setup callbacks */
	glutKeyboardFunc(keyPressed);
	glutDisplayFunc(drawScene);
	glutIdleFunc(drawScene);
	glutReshapeFunc(resizeViewport);
	glutVisibilityFunc(drawIconised);

#ifdef _DEBUG
	printf("main: entering appInit\n");
	fflush(stdout);
#endif

	/* Do anything the application needs */
	appInit(argc, argv);

#ifdef _DEBUG
	printf("main: entering glutMainLoop\n");
	fflush(stdout);
#endif

	/* Go */
	timer_start = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

	return 0;
}
