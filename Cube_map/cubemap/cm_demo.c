
/* cm_demo.c - EXT_texture_cube_map example */

/* Copyright NVIDIA Corporation, 1999. */

/* This program uses a pre-generated set of cube map images for an
   outdoor patio environment.  The EXT_texture_cube_map extension is used.

   The cube map environment is be applied to a teapot, torus, or sphere.
   You can spin the object with the mouse.  In theory, cube maps are
   view independent so the eye position could change (this would not be
   true of a sphere map), but this demo does not change the eye position.

   Also the EXT_texture_lod_bias can be used (when supported) to bias
   the texture LOD to force blurrier mipmap levels.  This can simulate
   a less shiny surface with duller reflections.  */

/* Windows command line compile instructions:

     cl cm_demo.c trackball.c tga.c glut32.lib

   Unix command line compile instructions:

     cc -o cm_demo cm_demo.c trackball.c tga.c -lglut -lGLU -lGL -lXext -lX11 -lm

   Note that this program requires the OpenGL Utility Toolkit (GLUT).
   You can download GLUT from http://reality.sgi.com/opengl/glut3 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glut.h>

/* In case your <GL/gl.h> does not advertise EXT_texture_lod_bias... */
#ifndef GL_EXT_texture_lod_bias
# define GL_MAX_TEXTURE_LOD_BIAS_EXT         0x84fd
# define GL_TEXTURE_FILTER_CONTROL_EXT       0x8500
# define GL_TEXTURE_LOD_BIAS_EXT             0x8501
#endif

/* In case your <GL/gl.h> does not advertise EXT_texture_cube_map... */
#ifndef GL_EXT_texture_cube_map
# define GL_NORMAL_MAP_EXT                   0x8511
# define GL_REFLECTION_MAP_EXT               0x8512
# define GL_TEXTURE_CUBE_MAP_EXT             0x8513
# define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514
# define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A
# define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B
# define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C
#endif

#include "tga.h"        /* simple TGA image file loader */
#include "trackball.h"  /* trackball quaternion routines */

static GLenum faceTarget[6] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
};

/* Pre-generated cube map images. */
char *faceFile[6] = {
  "cm_left.tga", 
  "cm_right.tga", 
  "cm_top.tga", 
  "cm_bottom.tga", 
  "cm_back.tga", 
  "cm_front.tga", 
};

/* Menu items. */
enum {
  M_TEAPOT, M_TORUS, M_SPHERE,
  M_SHINY, M_DULL,
  M_REFLECTION_MAP, M_NORMAL_MAP,
};

int hasTextureLodBias = 0;

int mode = GL_REFLECTION_MAP_EXT;
int wrap = GL_CLAMP;
int shape = M_TEAPOT;
int mipmaps = 1;

float lodBias = 0.0;

int spinning = 0, moving = 0;
int beginx, beginy;
int W = 300, H = 300;
float curquat[4];
float lastquat[4];

/* Rim, body, lid, and bottom data must be reflected in x and
   y; handle and spout data across the y axis only.  */

static int patchdata[][16] =
{
    /* rim */
  {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15},
    /* body */
  {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27},
  {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36,
    37, 38, 39, 40},
    /* lid */
  {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101,
    101, 0, 1, 2, 3,},
  {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117},
    /* bottom */
  {118, 118, 118, 118, 124, 122, 119, 121, 123, 126,
    125, 120, 40, 39, 38, 37},
    /* handle */
  {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
    53, 54, 55, 56},
  {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    28, 65, 66, 67},
    /* spout */
  {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83},
  {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
    92, 93, 94, 95}
};
/* *INDENT-OFF* */

static float cpdata[][3] =
{
    {0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
    -0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
    {0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
    0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
    2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
    2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
    {1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
    1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
    {0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
    0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
    0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
    {1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
    {1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
    -1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
    -0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
    2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
    2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
    2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
    {-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
    -0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
    1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
    -0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
    1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
    0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
    0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
    {2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
    -0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
    2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
    {3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
    {3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
    {2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
    3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
    3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
    -0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
    2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
    2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
    2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
    {0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
    -0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
    0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
    {0.84, -1.5, 0.075}
};

/* *INDENT-ON* */

/* This "fastTeapot" routine is faster than glutSolidTeapot. */
static void
fastSolidTeapot(GLint grid, GLdouble scale)
{
  float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
  long i, j, k, l;

  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_MAP2_VERTEX_3);
  glRotatef(270.0, 1.0, 0.0, 0.0);
  glScalef(0.5 * scale, 0.5 * scale, 0.5 * scale);
  glTranslatef(0.0, 0.0, -1.5);
  for (i = 0; i < 10; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 3; l++) {
          p[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
          q[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
          if (l == 1)
            q[j][k][l] *= -1.0;
          if (i < 6) {
            r[j][k][l] =
              cpdata[patchdata[i][j * 4 + (3 - k)]][l];
            if (l == 0)
              r[j][k][l] *= -1.0;
            s[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
            if (l == 0)
              s[j][k][l] *= -1.0;
            if (l == 1)
              s[j][k][l] *= -1.0;
          }
        }
      }
    }
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
      &p[0][0][0]);
    glMapGrid2f(grid, 0.0, 1.0, grid, 0.0, 1.0);
    glEvalMesh2(GL_FILL, 0, grid, 0, grid);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
      &q[0][0][0]);
    glEvalMesh2(GL_FILL, 0, grid, 0, grid);
    if (i < 6) {
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
        &r[0][0][0]);
      glEvalMesh2(GL_FILL, 0, grid, 0, grid);
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
        &s[0][0][0]);
      glEvalMesh2(GL_FILL, 0, grid, 0, grid);
    }
  }
}

void
loadFace(GLenum target, char *filename)
{
  FILE *file;
  gliGenericImage *image;

  file = fopen(filename, "rb");
  if (file == NULL) {
    printf("cm_demo: could not open \"%s\"\n", filename);
    exit(1);
  }
  image = gliReadTGA(file, filename);
  fclose(file);

  if (mipmaps) {
    gluBuild2DMipmaps(target, image->components,
      image->width, image->height,
      image->format, GL_UNSIGNED_BYTE, image->pixels);
  } else {
    glTexImage2D(target, 0, image->components,
      image->width, image->height, 0,
      image->format, GL_UNSIGNED_BYTE, image->pixels);
  }
}

void
updateTexgen(void)
{
  assert(mode == GL_NORMAL_MAP_EXT || mode == GL_REFLECTION_MAP_EXT);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
}

void
updateWrap(void)
{
  glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, wrap);
}

void
makeCubeMap(void)
{
  int i;

  for (i=0; i<6; i++) {
    loadFace(faceTarget[i], faceFile[i]);
  }
  if (mipmaps) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER,
      GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  glEnable(GL_TEXTURE_CUBE_MAP_EXT);

  updateTexgen();
  updateWrap();

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
}

void
display(void)
{
  GLfloat m[4][4];

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  build_rotmatrix(m, curquat);
  glMultMatrixf(&m[0][0]);

  switch (shape) {
  case M_TEAPOT:
    fastSolidTeapot(5, 1.1);
    break;
  case M_TORUS:
    glutSolidTorus(0.4, 0.9, 35, 35);
    break;
  case M_SPHERE:
    glutSolidSphere(1.0, 35, 35);
    break;
  default:
    assert(0);
  }
  glPopMatrix();

  glutSwapBuffers();
}

void
reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  W = w;
  H = h;
}

void
keyboard(unsigned char c, int x, int y)
{
  switch (c) {
  case 27:
    exit(0);
    break;
  case ' ':
    if (mode == GL_REFLECTION_MAP_EXT) {
      mode = GL_NORMAL_MAP_EXT;
    } else {
      mode = GL_REFLECTION_MAP_EXT;
    }
    updateTexgen();
    glutPostRedisplay();
    break;
  case 'c':
    if (wrap == GL_REPEAT) {
      wrap = GL_CLAMP;
    } else {
      wrap = GL_REPEAT;
    }
    updateWrap();
    glutPostRedisplay();
    break;
  case 's':
    shape++;
    if (shape > M_SPHERE) {
      shape = M_TEAPOT;
    }
    glutPostRedisplay();
    break;
  case 'm':
    glMatrixMode(GL_TEXTURE);
    glScalef(-1,-1,-1);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
    break;
  case 'a':
    if (hasTextureLodBias) {
      lodBias += 0.05;
      if (lodBias > 5.0) {
        lodBias = 5.0;
      }
      glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,
        GL_TEXTURE_LOD_BIAS_EXT, lodBias);
      glutPostRedisplay();    
    }
    break;
  case 'z':
    if (hasTextureLodBias) {
      lodBias -= 0.05;
      if (lodBias < 0.0) {
        lodBias = 0.0;
      }
      glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,
        GL_TEXTURE_LOD_BIAS_EXT, lodBias);
      glutPostRedisplay();    
    }
    break;
  }
}

void
mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    spinning = 0;
    glutIdleFunc(NULL);
    moving = 1;
    beginx = x;
    beginy = y;
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    moving = 0;
  }
}

void
animate(void)
{
  add_quats(lastquat, curquat, curquat);
  glutPostRedisplay();
}

void
motion(int x, int y)
{
  if (moving) {
    trackball(lastquat,
      (2.0 * beginx - W) / W,
      (H - 2.0 * beginy) / H,
      (2.0 * x - W) / W,
      (H - 2.0 * y) / H
      );
    beginx = x;
    beginy = y;
    spinning = 1;
    glutIdleFunc(animate);
  }
}

void
menu(int item)
{
  switch (item) {
  case M_TEAPOT:
  case M_TORUS:
  case M_SPHERE:
    shape = item;
    break;
  case M_SHINY:
    lodBias = 0.0;
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,
      GL_TEXTURE_LOD_BIAS_EXT, lodBias);
    break;
  case M_DULL:
    lodBias = 1.4;
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT,
      GL_TEXTURE_LOD_BIAS_EXT, lodBias);
    break;
  case M_NORMAL_MAP:
    mode = GL_NORMAL_MAP_EXT;
    updateTexgen();
    break;
  case M_REFLECTION_MAP:
    mode = GL_REFLECTION_MAP_EXT;
    updateTexgen();
    break;
  default:
    assert(0);
  }
  glutPostRedisplay();
}

int
main(int argc, char **argv)
{
  int i;

  glutInitWindowSize(500, 500);
  glutInit(&argc, argv);
  for (i=1; i<argc; i++) {
    if (!strcmp(argv[i], "-nomipmap")) {
      mipmaps = 0;
    }
    if (!strcmp(argv[i], "-v")) {
      gliVerbose(1);
    }
  }
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("EXT_texture_cube_map demo");

  if (!glutExtensionSupported("GL_EXT_texture_cube_map")) {
    printf("cm_demo: Your OpenGL implementation does not support EXT_texture_cube_map.\n");
    printf("cm_demo: This program requires it to run.\n");
    exit(1);
  }
 
  hasTextureLodBias = glutExtensionSupported("GL_EXT_texture_lod_bias");

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 1.0, /* Z far */ 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */

  glEnable(GL_DEPTH_TEST);

  makeCubeMap();

  trackball(curquat, 0.0, 0.0, 0.0, 0.0);

  glutCreateMenu(menu);
  glutAddMenuEntry("Teapot", M_TEAPOT);
  glutAddMenuEntry("Torus", M_TORUS);
  glutAddMenuEntry("Sphere", M_SPHERE);
  glutAddMenuEntry("Reflection map", M_REFLECTION_MAP);
  glutAddMenuEntry("Normal map", M_NORMAL_MAP);
  if (hasTextureLodBias) {
    glutAddMenuEntry("Shiny reflection", M_SHINY);
    glutAddMenuEntry("Dull reflection", M_DULL);
  } else {
    printf("cm_demo: Your OpenGL does not support EXT_texture_lod_bias.\n");
    printf("cm_demo: Therefore dull reflections are not supported.\n");
  }
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  return 0;
}
