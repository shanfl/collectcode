/**
 * main.h
 * Copyright 2000 by Mark B. Allan
 * 
 * Fooling around with cube map reflections and multitexture...
 */
#ifndef main_h
#define main_h

#include "mmath.h"

void reshape(int w, int h);
void update();
void updateTimer(int);
void draw();
void keyPress(unsigned char, int, int);
void mouseAction(int button, int state, int x, int y);
void mouseDown(int button, int x, int y);
void mouseUp(int button, int x, int y);
void mouseMove(int x, int y);
void updateTimer(int);
void drawTarget();

void checkRenderer();

enum	{ Move, Track, Target, Zoom };
int		mouseState = Move;
int		lastX = 0;
int		lastY = 0;
int		frame		=  0;
int		activePoint = -1;

float	fov			=  50.0;
float	aspect		=  1.0;
float	nearPlane	=  0.1;
float	farPlane	=  75.0;
float	scrW		=  0.0;
float	scrH		=  0.0;
float	xRot		=  0.0;
float	yRot		=  0.0;

float	track		=  12.0;
float	tgt[3]		=  { 0.0, -2.5, 1.5 };

bool	doAnim		= false;

float	baseColor[4] = { 1.0, 1.0, 1.0, 1.0 };
float	baseRate = 1.0;

MVertex	camPos;
MVertex	camAdd;
MVertex	camNow;
MVertex tgtPos;
MVertex	upVec;
MVertex camVec;
MMatrix camMat;
	
float	cam_mat[16];

#define F_RAND ((float)rand()/(float)RAND_MAX)

#endif
