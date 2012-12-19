/**
 * main.cpp
 * Copyright 2000 by Mark B. Allan
 * 
 * Fooling around with cube map reflections and multitexture...
 */

#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#define GL_GLEXT_PROTOTYPES
#else 
#include "winstuff.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/glpng.h>
#ifndef _WIN32
#include <GL/glx.h>
#endif

#include "main.h"

#include "room.h"
#include "reflect.h"

int		timerFuncVal = 16;
int		lastTimeVal = 0;
int		timeVal		= 0;
double	fps			= 30.0;
double	fpsRunning	= 0.0;
double	fpsTotal	= 0.0;

MVertex	fr(1.0, 1.0, 0.0);
MVertex to(0.0, 0.0, 0.0);
MVertex vec;

float	floorReflect = 0.8; //-- not used any more
bool	use_stencil = false;

GLuint frontList	= 0;
GLuint wallsList	= 0;
GLuint objectList	= 0;
GLuint mirrorList	= 0;

void rebuildDisplayLists();
void lookInDirection(MVertex &dir);

#ifdef _WIN32
PFNGLACTIVETEXTUREARBPROC   glActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
#endif

/*----------------------------------------------------------*/
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL );
	glutInitWindowSize(640, 480);
//	glutInitWindowSize(1024, 768);
	glutCreateWindow("mba_nvdemo");
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPress);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseAction);
	
	for(int i = 1; i < argc; i++)
	{	
		if(!strcmp(argv[i], "-timer") && (i+1) < argc)
		{
			sscanf(argv[++i], "%d", &timerFuncVal);
		}
	}
	
	//--Make sure we support the correct extensions before we continue...
	checkRenderer();

#ifdef _WIN32
	glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC)  wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
#endif

	glPointSize(2.0);
	
	
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
	
	glEnable(GL_CULL_FACE);
	
	glDisable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor4fv(baseColor);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	loadRoomTextures();
	makeCubeMap();
	
	upVec.set(0.0, 1.0, 0.0);
	camPos.set(0.0, 2.5,  7.0);
	tgtPos.set(0.0, 2.5, -1.5);
	
	frontList	= glGenLists(1);
	wallsList	= glGenLists(1);
	objectList	= glGenLists(1);
	mirrorList	= glGenLists(1);
	rebuildDisplayLists();
	
	glMatrixMode(GL_MODELVIEW);
	
	doAnim = true;
	glutTimerFunc(100, updateTimer, doAnim);
	
	glutMainLoop();
	return 0;
}

void rebuildDisplayLists()
{
	glNewList(frontList, GL_COMPILE);
	drawFrontWall();
	glEndList();
	
	glNewList(wallsList, GL_COMPILE);
	drawRoomWalls();
	glEndList();
	
	glNewList(objectList, GL_COMPILE);
	drawRoomObjects();
	glEndList();
	
	glNewList(mirrorList, GL_COMPILE);
	drawMirror();
	glEndList();
	
}

/*----------------------------------------------------------*/
void	draw()
{
//	int errCnt = 0;
//	fprintf(stderr, "%2d GL_ERROR = %d\n", ++errCnt, glGetError());
 	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	glLoadIdentity();
	
	camNow = camPos+camAdd;
	camVec = camNow-tgtPos;
	lookInDirection(camVec);
	glTranslatef( -camNow.x(), -camNow.y(), -camNow.z() );
	
//	drawTarget();
	
	glPushMatrix();
	
	glColor4fv(baseColor);
	
	MVertex nscale( 1.0,  1.0,  1.0);
	MVertex iscale;
	
	if(use_stencil)
	{
		//--setup reflection stencil...
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
		glCallList(frontList);
		drawFloor(1.0);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	
	//-- Room reflection...
	glPushMatrix();
	iscale.set(1.0, 1.0, -1.0);
	glScalef  (1.0, 1.0, -1.0);
	glFrontFace(GL_CW);
	glTranslatef(0.0, 0.0, 14.0);
	glCallList(wallsList);
	drawFloor(1.0);
	drawReflect(camNow, iscale);
	glCallList(objectList);
	glFrontFace(GL_CCW);
	glPopMatrix();
	
	if(floorReflect)
	{
		//-- Floor reflection...
		glPushMatrix();
		glScalef  (1.0, -1.0,  1.0);
		iscale.set(1.0, -1.0,  1.0);
		glFrontFace(GL_CW);
		glCallList(frontList);
		glCallList(wallsList);
		drawMirror(false);
		drawReflect(camNow, iscale);
		glCallList(objectList);
		glFrontFace(GL_CCW);
		glPopMatrix();
	}
	
	if(use_stencil)
	{
		glDisable(GL_STENCIL_TEST);
	}
	
	//-- 'poke hole' in wall to reflection...
	glCallList(mirrorList);
	
	//-- draw the room...
	
	glCallList(frontList);
	glCallList(wallsList);
	glBlendFunc(GL_ONE, GL_SRC_COLOR);
	drawFloor(1.0-floorReflect);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	
	drawReflect(camNow, nscale);
	
	glCallList(objectList);

	glPopMatrix();

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
	glutSwapBuffers();
}

/*----------------------------------------------------------*/
void drawTarget()
{
	float sz = 0.5;
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(tgtPos.x()-sz, tgtPos.y(), tgtPos.z() );
	glVertex3f(tgtPos.x()+sz, tgtPos.y(), tgtPos.z() );
	glVertex3f(tgtPos.x(), tgtPos.y()-sz, tgtPos.z() );
	glVertex3f(tgtPos.x(), tgtPos.y()+sz, tgtPos.z() );
	glVertex3f(tgtPos.x(), tgtPos.y(), tgtPos.z()-sz );
	glVertex3f(tgtPos.x(), tgtPos.y(), tgtPos.z()+sz );
	glEnd();
}

/*----------------------------------------------------------*/
void reshape(int w, int h)
{
	scrW = (float)w;
	scrH = (float)h;
	
	aspect = scrW/scrH;
	fprintf(stderr, "new size = %d x %d\n", w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( fov, (double)w/(double)h, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,w,h);
	
	//-- reset frame count average
	fprintf(stderr, "reset fps average\n");
	fps = 30;
	fpsRunning = 0;
	fpsTotal = 0;
}

#define SRAND (2.0*(0.5-(rand()/(double)RAND_MAX)))

/*----------------------------------------------------------*/
void updateTimer(int) 
{
	static bool	moveCam, centerCam = false;
	static float decay = 0.0;
	static float xfact, yfact, zfact;
	static float dframe = 0.0;
	
	frame++;
	dframe += baseRate;
	if(doAnim)
	{
		//This func seems to be a very inaccurate timer...
//		glutTimerFunc(0, updateTimer, doAnim);
		glutTimerFunc(timerFuncVal, updateTimer, doAnim);
	}
	glutPostRedisplay();
	
//	if(frame < 5)
	{
		float x = dframe+150.0;
		float a, b, c;
		a =  4.0*sin(x*0.0110);
		b =  1.0*sin(x*0.0025)+3.0;
		c =  5.0*sin(x*0.0050)-5.5;
		tgtPos.set(a, b, c);

		a =  5.00*sin(x*0.0060);
		b =  3.50*sin(x*0.0035)+4.0;
		c =  9.00*sin(x*0.0050)+3.5;
		camPos.set(a, b, c);
	}
	
	if(use_stencil) // only alter the camera path if we can't see the stencil trick...
	{
		if(!(frame%6000))
			centerCam = true;
		if(!(frame%2400))
		{
			moveCam = true;
			decay = 1.0;
			zfact = 0.5*SRAND;
			xfact = 0.5*SRAND;
			yfact = 0.5*SRAND;
			if(yfact < 0.0)
				yfact *= 0.02;
			if(zfact < 0.0)
				zfact *= 0.02;
		}
		if(moveCam)
		{
			if(decay > 0.1)
			{
				if(centerCam)
				{
					camAdd.setX(camAdd.x()*0.9);
					camAdd.setY(camAdd.y()*0.9);
					camAdd.setZ(camAdd.z()*0.9);
				}
				else
				{
					camAdd.addX(decay*xfact);
					camAdd.addY(decay*yfact);
					camAdd.addZ(decay*zfact);
				}
				decay *= 0.97;
			}
			else
			{
				moveCam = false;
				centerCam = false;
			}
		}
	}
	
	int			timeDiff;
#ifndef _WIN32
	struct		timeval	tv;
	gettimeofday(&tv, 0);
	timeVal = (tv.tv_sec * 1000000) + tv.tv_usec;
#else
	timeVal = glutGet(GLUT_ELAPSED_TIME)*1000;
#endif
	
	if(lastTimeVal)
	{
		timeDiff = timeVal-lastTimeVal;
		fps      = 0.8*fps + 0.2*(1000000.0/timeDiff);
		if( !(frame%100) )
		{
			fprintf(stdout, "baseRate = %f   \t", baseRate);
			fprintf(stdout, "fps = %f\n", fps);
			fpsRunning += fps;
			fpsTotal += 1.0;
			 
			baseRate = 36.0/(fpsRunning/fpsTotal);
			
			//-- check for errors every once in a while...
			int glError = glGetError();
			if(	glError != GL_NO_ERROR )
			{
				fprintf(stderr, "ERROR!!! glGetError = \"%s\"\n", gluErrorString(glError) );
			}	
		}
	}
	lastTimeVal = timeVal;
}

/*----------------------------------------------------------*/
void update (void) 
{
	draw();
}

/*----------------------------------------------------------*/
void keyPress (unsigned char key, int, int) 
{
	static int oldWidth, oldHeight;
	static bool fullScrn = false;
	static double viewGamma = 1.0;
	
	switch(key)
	{
		case '\t':
			fullScrn = !fullScrn;
			if(fullScrn)
			{
				oldWidth = glutGet(GLenum(GLUT_WINDOW_WIDTH));
				oldHeight = glutGet(GLenum(GLUT_WINDOW_HEIGHT));
				glutSetCursor(GLUT_CURSOR_NONE);
				glutFullScreen(); 
			}
			else
			{
				glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
				glutReshapeWindow(oldWidth, oldHeight);
			}
			break;
		case 'c':	
		case 'C':	
			camAdd.set(0.0, 0.0, 0.0);
			break;
		case ' ':
			doAnim = !doAnim;
			glutTimerFunc(0, updateTimer, doAnim);
			break;
		case '0':
			viewGamma = 1.0;
			fprintf(stderr, "reload textures (gamma %f...)\n", viewGamma);
			pngSetViewingGamma(viewGamma);
			reloadRoomTextures();
			makeCubeMap();
			fprintf(stderr, "...done.\n");
			break;
		case '=':
		case '+':
			viewGamma += 0.1;
			fprintf(stderr, "set view gamma %f...\n", viewGamma);
			pngSetViewingGamma(viewGamma);
			reloadRoomTextures();
			makeCubeMap();
			fprintf(stderr, "...done.\n");
			break;
		case '-':
		case '_':
			viewGamma -= 0.1;
			fprintf(stderr, "set view gamma %f...\n", viewGamma);
			pngSetViewingGamma(viewGamma);
			reloadRoomTextures();
			makeCubeMap();
			fprintf(stderr, "...done.\n");
			break;
		case 'q':
		case 'Q':
		case '': /*ESC*/
			glDeleteLists(frontList , 1);
			glDeleteLists(wallsList , 1);
			glDeleteLists(objectList, 1);
			glDeleteLists(mirrorList, 1);
			delRoomLists(); // delete room display lists
			fprintf(stdout, "exit. frame = %d\n", frame);
//			if(fpsTotal)
//				fprintf(stdout, "average fps = %g\n", fpsRunning/fpsTotal);
			fflush(stdout);
			exit(0);
			break;
	}
	glutPostRedisplay();
}

/*----------------------------------------------------------*/
void mouseAction(int button, int state, int xPos, int yPos)
{
	lastX = xPos;
	lastY = yPos;
	
	switch(state)
	{
		case GLUT_UP:
			mouseUp(button, xPos, yPos);
			break;
		case GLUT_DOWN:
			mouseDown(button, xPos, yPos);
			break;
		default:
			break;
	}
}

/*----------------------------------------------------------*/
void mouseDown(int button, int /*xPos*/, int /*yPos*/)
{
	switch(button)
	{
		case GLUT_LEFT_BUTTON:
			mouseState = Move;
			break;
		case GLUT_RIGHT_BUTTON:
			mouseState = Track;
			break;
		case GLUT_MIDDLE_BUTTON:
			mouseState = Zoom;
			break;
	}
}

/*----------------------------------------------------------*/
void mouseUp(int /*button*/, int /*xPos*/, int /*yPos*/)
{
}

/*----------------------------------------------------------*/
void mouseMove(int xPos, int yPos)
{
	int dx = xPos - lastX;
	int dy = yPos - lastY;
	lastX = xPos;
	lastY = yPos;
	
	camNow = camPos+camAdd;
	camVec = tgtPos-camNow;
	camVec.setY(0.0);
	camVec.normalize();
	camMat.pointAt(camVec, upVec);
	MVertex xVec = camMat*MVertex(1.0, 0.0, 0.0);
	MVertex zVec = camMat*MVertex(0.0, 0.0, 1.0);
	
	float mov = 0.05;
	switch(mouseState)
	{
		case Move:
			xVec = xVec * ( dx*mov);
			camAdd = camAdd + xVec;
			camAdd.addY(dy*mov);
			break;
		case Track:
			zVec = zVec * (-dy*mov);
			camAdd = camAdd + zVec;
			break;
		case Zoom:
			fov += dy*0.25;
			reshape((int)scrW, (int)scrH);
			break;
	}
	if(!doAnim)
		glutPostRedisplay();
}

/*----------------------------------------------------------*/
void lookInDirection(MVertex &dir)
{
	double	xzlen, yzlen;
	double	yrot, xrot;
					
	if (dir.x() == 0.0 && dir.y() == 0.0 && dir.z()==0.0 )
		return;
		
	xzlen = sqrt(dir.x()*dir.x()+dir.z()*dir.z());
	if (xzlen == 0)
		yrot = (dir.y()<0.0) ? 180.0 : 0.0;
	else
		yrot = 180.0*acos(dir.z()/xzlen)/M_PI;
		
	yzlen = sqrt(dir.y()*dir.y()+xzlen*xzlen);
	if (yzlen == 0.0)
		yzlen = 0.0000001;
	xrot = 180.0*acos(xzlen/yzlen)/M_PI;
	
	if (dir.y() > 0.0)
		glRotatef( xrot, 1.0, 0.0, 0.0);
	else
		glRotatef(-xrot, 1.0, 0.0, 0.0);
		
	if (dir.x() > 0.0)
		glRotatef(-yrot, 0.0, 1.0, 0.0);
	else
		glRotatef( yrot, 0.0, 1.0, 0.0);
}

//----------------------------------------------------------
bool extensionSupported(const char *extension)
{
	const char *extensions	= 0;
	const char *start		= 0;
	char *where, *terminator;

	where = (char*)strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;
	extensions = (const char*)glGetString(GL_EXTENSIONS);
	start = extensions;
	while(true)
	{
		where = (char*)strstr(start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}
	return false;
}

//----------------------------------------------------------
void checkRenderer()
{
	#define NUM_EXTENSIONS 2
	bool bad = false;
	GLint bits;
	const char *vendor;
	const char *renderer;
	const char *version;
	const char *extensions;
	const char *ext[NUM_EXTENSIONS] = {	"GL_ARB_texture_cube_map", 
										"GL_ARB_multitexture"		};

	vendor		= (const char*) glGetString(GL_VENDOR);
	renderer	= (const char*) glGetString(GL_RENDERER);
	version		= (const char*) glGetString(GL_VERSION);
	extensions	= (const char*) glGetString(GL_EXTENSIONS);
	glGetIntegerv(GL_INDEX_BITS, &bits);
 
	fprintf(stderr, "------------------------------------------------------------\n");
	fprintf(stderr, "GL_VENDOR     = \"%s\"\n", vendor);
	fprintf(stderr, "GL_RENDERER   = \"%s\"\n", renderer);
	fprintf(stderr, "GL_VERSION    = \"%s\"\n", version);
	fprintf(stderr, "GL_INDEX_BITS = %d\n", bits);
//	fprintf(stderr, "GL_EXTENSIONS = \"%s\"\n", extensions);
	
	fprintf(stderr, "\nChecking extensions:\n");
	fprintf(stderr, "------------------------------------------------------------\n");
	
	for(int i = 0; i < NUM_EXTENSIONS; i++)
	{
		if(extensionSupported(ext[i]))
			fprintf(stderr, "GOOD: %s is supported.\n", ext[i]);
		else
		{
			fprintf(stderr, "BAD!! %s is NOT supported.\n", ext[i]);
			bad = true;
		}
	}
	
	fprintf(stderr, "------------------------------------------------------------\n");
	if(bad)
	{
		fprintf(stderr, "Sorry, it looks like your card doesn't support the\n");
		fprintf(stderr, "GL extensions required to run this demo.\n");
		fprintf(stderr, "------------------------------------------------------------\n");
		exit(1);
	}
	/*
	 * only use the stencil buffer if we have an NVIDIA card
	 * running at 32 bits. It would be nice if there were an 
	 * easy, generic way to test if the stencil buffer is 
	 * hardware accelerated.
	 */
	if(!strncmp("NVIDIA", vendor, 6) && bits == 32)
	{
		use_stencil = true;
	}
	else
	{
		fprintf(stderr, "NOT OPTIMAL!: STENCIL BUFFER DISABLED if depth != 32.\n");
		fprintf(stderr, "The demo will work fine at this color depth, but things\n");
		fprintf(stderr, "will look funky if viewed from outside the room walls.\n");
		fprintf(stderr, "------------------------------------------------------------\n");
	}
	#undef NUM_EXTENSIONS
	
	#ifndef _WIN32
	int major, minor;
	int screen;
	Display *dpy = glXGetCurrentDisplay();
	if(dpy)
	{
		glXQueryVersion(dpy, &major, &minor);
		fprintf(stderr, "GLX version %d.%d\n", major, minor);
		screen = DefaultScreen(dpy);
		fprintf(stderr, "GLX extensions : \"%s\"\n", glXQueryExtensionsString(dpy, screen));
	}
	else
	{
		fprintf(stderr, "glXGetCurrentDisplay() = %p!\n", dpy);
	}
	fprintf(stderr, "------------------------------------------------------------\n");
	#endif
}

