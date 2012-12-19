/**
 * room.h
 * Copyright 2000 by Mark B. Allan
 * 
 * draw the room.
 */
#ifndef room_h
#define room_h

#include "mmath.h"

void loadRoomTextures();
void reloadRoomTextures();
void deleteRoomTextures();

void genRoomLists();
void delRoomLists();
void createLists();

void drawFrontWall();
void drawFloor(float alpha);
void drawRoomWalls();
void drawMirror(bool alpha = true);
void drawChair();
void drawRoomObjects();
void drawReflect(MVertex &eye, MVertex &scale);

#endif
