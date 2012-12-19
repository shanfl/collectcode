/**
 * reflect.h
 * Copyright 2000 by Mark B. Allan
 * 
 * load reflection map
 */
#ifndef reflect_h
#define reflect_h

#include "winstuff.h"
#include <GL/gl.h>

void loadFace(GLenum target, char *filename);
void updateTexgen(void);
void updateWrap(void);
void makeCubeMap(void);

#endif // reflect_h


