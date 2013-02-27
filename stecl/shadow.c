/*
 *  Real-time soft shadows.  Press 'h' for a help menu.  Drag the
 *  middle mouse button to rotate the scene.
 *
 *  author: Nate Robins
 *  email: ndr@pobox.com
 *  www: http://www.pobox.com/~ndr
 */

/* includes */
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include "trackball.h"
#include "gltx.h"
#include "glm.h"


/* defines */
#define WALL    20.0


/* enums */
enum {
  X, Y, Z, W
};

enum {
  A, B, C, D
};


/* data */
GLfloat floor_quad[4 * 3] = { 
  -WALL, 0.0, -WALL,
  -WALL, 0.0,  WALL,
   WALL, 0.0,  WALL,
   WALL, 0.0, -WALL
};

GLfloat back_quad[4 * 3] = { 
  -WALL,  0.0, -WALL,
   WALL,  0.0, -WALL,
   WALL, WALL, -WALL,
  -WALL, WALL, -WALL
};

GLfloat left_quad[4 * 3] = { 
  -WALL,  0.0,  WALL,
  -WALL,  0.0, -WALL,
  -WALL, WALL, -WALL,
  -WALL, WALL,  WALL
};

/* globals */
GLfloat* light_samples;
GLuint   light_size = 4.0;
GLfloat  light_position[4] = { WALL, WALL, WALL, 1.0 };

GLuint     floor_list = 0;		/* display list for floor */
GLuint     back_list = 0;		/* display list for back wall */
GLuint     left_list = 0;		/* display list for left wall */
GLuint     couch_list = 0;		/* display list for couch */
GLuint     lamp_list = 0;		/* display list for lamp */
GLuint     table_list = 0;		/* display list for table */
GLuint     floor_shadow_list = 0;	/* display list for shadows on floor */
GLuint     back_shadow_list = 0;	/* display list for shadows on back */
GLuint     left_shadow_list = 0;	/* display list for shadows on left */

GLuint     rug_list = 0;		/* display list for rug texture */
GLuint     wood_list = 0;		/* display list for wood texture */
GLuint     picture_list = 0;		/* display list for picture texture */
GLuint     design_list = 0;		/* display list for design texture */

GLfloat floor_plane[4];			/* ground plane */
GLfloat left_plane[4];			/* left wall plane */
GLfloat back_plane[4];			/* back wall plane */

GLuint  num_samples = 4;		/* number samples from light */

GLMmodel* couch;
GLMmodel* lamp;
GLMmodel* table;

GLboolean texturing = GL_FALSE;		/* draw with texturing? */
GLboolean draw_sample_vectors = GL_FALSE;
GLboolean view_from_light = GL_FALSE;	/* view fromthe light? */
GLboolean jittered = GL_FALSE;		/* jittered sampling? */
GLboolean draw_shadows = GL_TRUE;	/* draw shadows? */
GLboolean frame_rate = GL_FALSE;	/* show frame rate? */


/* functions */

void
normalize(GLfloat* n)
{
  GLfloat l;

  /* normalize */
  l = (GLfloat)sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  n[0] /= l;
  n[1] /= l;
  n[2] /= l;
}

void
cross(GLfloat* u, GLfloat* v, GLfloat* n)
{
  /* compute the cross product (u x v for right-handed [ccw]) */
  n[0] = u[1] * v[2] - u[2] * v[1];
  n[1] = u[2] * v[0] - u[0] * v[2];
  n[2] = u[0] * v[1] - u[1] * v[0];
}

/* plane: find a plane equation given 3 vertices 
 *
 * plane - plane equation return
 * v0    - first vertex in the plane
 * v1    - second vertex in the plane
 * v2    - third vertex in the plane
 */
void
planeequation(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
  GLfloat vec0[3], vec1[3];

  /* need 2 vectors to find cross product */
  vec0[0] = v1[0] - v0[0];
  vec0[1] = v1[1] - v0[1];
  vec0[2] = v1[2] - v0[2];

  vec1[0] = v2[0] - v0[0];
  vec1[1] = v2[1] - v0[1];
  vec1[2] = v2[2] - v0[2];

  /* find cross product to get A, B, and C of plane equation */
  plane[0] = vec0[1] * vec1[2] - vec0[2] * vec1[1];
  plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
  plane[2] = vec0[0] * vec1[1] - vec0[1] * vec1[0];

  plane[3] = -(plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}

/* create a matrix that will project the desired shadow */
void
shadowmatrix(GLfloat matrix[4][4], GLfloat plane[4], GLfloat lightpos[4])
{
  GLfloat dot;

  /* find dot product between light position vector and plane normal */
  dot = plane[X] * lightpos[X] +
        plane[Y] * lightpos[Y] +
        plane[Z] * lightpos[Z] +
        plane[W] * lightpos[W];

  matrix[0][0] = dot - lightpos[X] * plane[X];
  matrix[1][0] = 0.f - lightpos[X] * plane[Y];
  matrix[2][0] = 0.f - lightpos[X] * plane[Z];
  matrix[3][0] = 0.f - lightpos[X] * plane[W];

  matrix[0][1] = 0.f - lightpos[Y] * plane[X];
  matrix[1][1] = dot - lightpos[Y] * plane[Y];
  matrix[2][1] = 0.f - lightpos[Y] * plane[Z];
  matrix[3][1] = 0.f - lightpos[Y] * plane[W];

  matrix[0][2] = 0.f - lightpos[Z] * plane[X];
  matrix[1][2] = 0.f - lightpos[Z] * plane[Y];
  matrix[2][2] = dot - lightpos[Z] * plane[Z];
  matrix[3][2] = 0.f - lightpos[Z] * plane[W];

  matrix[0][3] = 0.f - lightpos[W] * plane[X];
  matrix[1][3] = 0.f - lightpos[W] * plane[Y];
  matrix[2][3] = 0.f - lightpos[W] * plane[Z];
  matrix[3][3] = dot - lightpos[W] * plane[W];
}

/* samplelight: sample a point light as if it were an area light.
 *
 * samples - number of samples to take
 * size    - width & height of area light
 * light   - vector pointing TO the point light source
 *
 * returns a static array of light sample vectors (4 * samples)
 */
GLfloat*
samplelight(GLuint samples, GLuint size, GLfloat light[4])
{
  static GLfloat* lights = NULL;
  GLuint i, j, sqrt_samples;
  GLfloat x, y;
  GLfloat n[3], u[3], v[3];

  /* allocate new array for light samples */
  if (lights)
    free(lights);
  lights = (GLfloat*)malloc(sizeof(GLfloat) * 4 * samples);

  /* take care of a simple case */
  if (samples == 1) {
    lights[0] = light[0];
    lights[1] = light[1];
    lights[2] = light[2];
    lights[3] = 1.0;
    return lights;
  }

  /* calculate the vectors that define the area light (just a plane
     with sides of size): u is up, v is across */
  n[0] = light[0];
  n[1] = light[1] + 10;
  n[2] = light[2] + 30;
  normalize(n);
  u[0] = 0.0; u[1] = 1.0; u[2] = 0.0; 
  cross(u, n, v); normalize(v);
  cross(n, v, u); normalize(u);

  /* unitize the vectors */
  v[0] *= size/2.0; v[1] *= size/2.0; v[2] *= size/2.0;
  u[0] *= size/2.0; u[1] *= size/2.0; u[2] *= size/2.0;

  /* sample on a regular grid */
  sqrt_samples = (GLuint)sqrt(samples);
  y = -1.0 + 2.0 / sqrt_samples;
  for (j = 0; j < sqrt_samples; j++) {
    x = -1.0 + 2.0 / sqrt_samples;
    for (i = 0; i < sqrt_samples; i++) {
      lights[4 * (j * sqrt_samples + i) + 0] = light[0] + x * v[0] + y * u[0];
      lights[4 * (j * sqrt_samples + i) + 1] = light[1] + x * v[1] + y * u[1];
      lights[4 * (j * sqrt_samples + i) + 2] = light[2] + x * v[2] + y * u[2];
      lights[4 * (j * sqrt_samples + i) + 3] = 1.0;
      x += 2.0 / sqrt_samples;
    }
    y += 2.0 / sqrt_samples;
  }  

  return lights;
}


/* shadow: project a shadow that the model would cause on the plane
 * given the light
 *
 * list  - display list of model casting shadow
 * plane - plane that the shadow will be cast on
 * light - vector pointing TO the light source
 */
void
shadow(GLuint list, GLfloat plane[4], GLfloat light[3])
{
  GLfloat matrix[4][4];
  
  shadowmatrix(matrix, plane, light);

  glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
  glStencilFunc(GL_EQUAL, 0x1, 0xff);

  glPushMatrix();
  glMultMatrixf((GLfloat*)matrix);
  glColor4f(0.1, 0.1, 0.1, (GLfloat)pow(0.3, 1.0 / num_samples));
  glCallList(list);
  glPopMatrix();
}

GLvoid
displaywalls()
{  
  static GLuint i;

  /* draw the floor */
  if (texturing) {
    glEnable(GL_TEXTURE_2D);
    glCallList(rug_list);
    glCallList(floor_list);
    glDisable(GL_TEXTURE_2D);
  } else {
    glCallList(floor_list);
  }

  /* draw the back wall */
  glCallList(back_list);

  /* draw the left wall */
  glCallList(left_list);

  if (!draw_shadows) 
    return;

  /* state for shadows */
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  /* draw shadows on the floor */
  for (i = 0; i < num_samples; i++) {
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0x1, 0xff);
    glCallList(floor_list);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    shadow(floor_shadow_list, floor_plane, &light_samples[4 * i]);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  }
  glCallList(floor_list);

  /* draw shadows on the back wall */
  for (i = 0; i < num_samples; i++) {
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0x1, 0xff);
    glCallList(back_list);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    shadow(back_shadow_list, back_plane, &light_samples[4 * i]);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  }
  glCallList(back_list);

  /* draw shadows on the left wall */
  for (i = 0; i < num_samples; i++) {
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0x1, 0xff);
    glCallList(left_list);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    shadow(left_shadow_list, left_plane, &light_samples[4 * i]);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  }
  glCallList(left_list);

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

void
models()
{
  GLfloat dimensions[3];

  couch = glmReadOBJ("data/couch.obj");
  glmUnitize(couch);
  /*  glmSpheremapTexture(couch); */
  glmScale(couch, 10.0);
  glmDimensions(couch, dimensions);
  couch->position[X] = 2.5;
  couch->position[Y] = dimensions[Y] / 2.0;
  couch->position[Z] = -WALL + dimensions[Z];

  lamp = glmReadOBJ("data/lamp.obj");
  glmUnitize(lamp);
  /*  glmLinearTexture(lamp); */
  glmDimensions(lamp, dimensions);
  glmScale(lamp, 8.0);
  glmDimensions(lamp, dimensions);
  lamp->position[X] = -WALL + dimensions[X];
  lamp->position[Y] = dimensions[Y] / 2.0;
  lamp->position[Z] = -WALL + dimensions[Z];

  table = glmReadOBJ("data/table.obj");
  glmUnitize(table);
  glmLinearTexture(table);
  glmScale(table, 7.0);
  glmDimensions(table, dimensions);
  glmFacetNormals(table);
  table->position[X] = 2.5;
  table->position[Y] = dimensions[Y] / 2.0;
  table->position[Z] = 2.0;
}

void
textures()
{
  GLTXimage *image;
  
  image = gltxReadRGB("data/floor.rgb");
  rug_list = glGenLists(1);
  glNewList(rug_list, GL_COMPILE);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, image->components, 
		    image->width, image->height, 
		    image->components == 4 ? GL_RGBA : GL_RGB,
		    GL_UNSIGNED_BYTE, image->data);
  glEndList();
  gltxDelete(image);

  image = gltxReadRGB("data/table.rgb");
  wood_list = glGenLists(1);
  glNewList(wood_list, GL_COMPILE);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, image->components, 
		    image->width, image->height, 
		    image->components == 4 ? GL_RGBA : GL_RGB,
		    GL_UNSIGNED_BYTE, image->data);
  glEndList();
  gltxDelete(image);

  image = gltxReadRGB("data/picture.rgb");
  picture_list = glGenLists(1);
  glNewList(picture_list, GL_COMPILE);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, image->components, 
		    image->width, image->height, 
		    image->components == 4 ? GL_RGBA : GL_RGB,
		    GL_UNSIGNED_BYTE, image->data);
  glEndList();
  gltxDelete(image);

  image = gltxReadRGB("data/design.rgb");
  design_list = glGenLists(1);
  glNewList(design_list, GL_COMPILE);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, image->components, 
		    image->width, image->height, 
		    image->components == 4 ? GL_RGBA : GL_RGB,
		    GL_UNSIGNED_BYTE, image->data);
  glEndList();
  gltxDelete(image);
}

void
lists()
{
  /* lamp */
  lamp_list = glmList(lamp, GLM_SMOOTH | GLM_COLOR);

  /* table */
  table_list = glmList(table, GLM_SMOOTH | GLM_COLOR | GLM_TEXTURE);

  /* couch */
  couch_list = glmList(couch, GLM_SMOOTH | GLM_COLOR);

  /* floor shadow */
  floor_shadow_list = glGenLists(1);
  glNewList(floor_shadow_list, GL_COMPILE);
  glmDraw(lamp, GLM_NONE);
  glmDraw(couch, GLM_NONE);
  glmDraw(table, GLM_NONE);
  glEndList();

  /* back wall shadow */
  back_shadow_list = glGenLists(1);
  glNewList(back_shadow_list, GL_COMPILE);
  glmDraw(lamp, GLM_NONE);
  glmDraw(couch, GLM_NONE);
  glEndList();

  /* left wall shadow */
  left_shadow_list = glGenLists(1);
  glNewList(left_shadow_list, GL_COMPILE);
  glmDraw(lamp, GLM_NONE);
  glEndList();

  /* floor */
  floor_list = glGenLists(1);
  glNewList(floor_list, GL_COMPILE);
  glColor3f(1.0, 1.0, 0.7);
  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glTexCoord2f(0.0, 0.0);
  glVertex3fv(&floor_quad[3 * 0]);
  glTexCoord2f(1.0, 0.0);
  glVertex3fv(&floor_quad[3 * 1]);
  glTexCoord2f(1.0, 1.0);
  glVertex3fv(&floor_quad[3 * 2]);
  glTexCoord2f(0.0, 1.0);
  glVertex3fv(&floor_quad[3 * 3]);
  glEnd();
  glEndList();

  /* back */
  back_list = glGenLists(1);
  glNewList(back_list, GL_COMPILE);
  glColor3f(1.0, 1.0, 0.7);
  glBegin(GL_QUADS);
  glNormal3f(0.0, 0.0, 1.0);
  glVertex3fv(&back_quad[3 * 0]);
  glVertex3fv(&back_quad[3 * 1]);
  glVertex3fv(&back_quad[3 * 2]);
  glVertex3fv(&back_quad[3 * 3]);
  glEnd();
  glEndList();

  /* left */
  left_list = glGenLists(1);
  glNewList(left_list, GL_COMPILE);
  glColor3f(1.0, 1.0, 0.7);
  glBegin(GL_QUADS);
  glNormal3f(1.0, 0.0, 0.0);
  glVertex3fv(&left_quad[3 * 0]);
  glVertex3fv(&left_quad[3 * 1]);
  glVertex3fv(&left_quad[3 * 2]);
  glVertex3fv(&left_quad[3 * 3]);
  glEnd();
  glEndList();
}

void
init(void)
{
  tbInit(GLUT_MIDDLE_BUTTON);

  models();
  textures();
  lists();

  planeequation(floor_plane, 
		&floor_quad[3*0], &floor_quad[3*1], &floor_quad[3*2]);
  planeequation(back_plane, 
		&back_quad[3*0], &back_quad[3*1], &back_quad[3*2]);
  planeequation(left_plane, 
		&left_quad[3*0], &left_quad[3*1], &left_quad[3*2]);

  light_samples = samplelight(num_samples, light_size, light_position);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);

  glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);

  glClearStencil(0);
}

void
reshape(int width, int height)
{
  tbReshape(width, height);

  glViewport(0, 0, width, height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)height / (GLfloat)width, 1.0, 128.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, -10.0, -30.0);
  glRotatef(-20.0, 0.0, 1.0, 0.0);
}

void
display(void)
{
  static GLuint i;
  static int start, end;

  if(frame_rate)
    start = glutGet(GLUT_ELAPSED_TIME);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  /* do the trackball rotation. */
  glPushMatrix();

  if (view_from_light) {
    gluLookAt(light_position[0], light_position[1], light_position[2],
	      0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  } else
    tbMatrix();

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  /* draw a yellow sphere for the light */
  glDisable(GL_LIGHTING);
  glPushMatrix();
  glTranslatef(light_position[0], light_position[1], light_position[2]);
  glColor3f(1.0, 1.0, 0.0);
  glutSolidSphere(light_size / 2.0, 8, 8);
  glPopMatrix();

  /* draw green sample vectors */
  if (draw_sample_vectors) {
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    for (i = 0; i < num_samples; i++) {
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(light_samples[4 * i + 0], 
		 light_samples[4 * i + 1], 
		 light_samples[4 * i + 2]);
    }
    glEnd();
  }

  glEnable(GL_LIGHTING);

  /* draw the scene */
  displaywalls();

  if (texturing) {
    glEnable(GL_TEXTURE_2D);
    glCallList(wood_list);
    glCallList(table_list);

    glCallList(picture_list);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-2.0, 15.0, -WALL+0.5);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-2.0, 9.0, -WALL+0.5);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(8.0, 9.0, -WALL+0.5);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(8.0, 15.0, -WALL+0.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  } else {
    glCallList(table_list);
  }

  glCallList(couch_list);
  glCallList(lamp_list);

  glPopMatrix();

  glutSwapBuffers();
  
  if (frame_rate) {
    glFinish();				/* make sure rendering is done */
    end = glutGet(GLUT_ELAPSED_TIME);
    fprintf(stderr, " %.1f fps       \r", 
	    1.0 / ((double)(end - start) / 1000.0));
  }
}

void
special(int key, int x, int y)
{
  switch (key) { 
    case GLUT_KEY_UP:
      break;

    case GLUT_KEY_DOWN:
      break;

    case GLUT_KEY_RIGHT:
      break;

    case GLUT_KEY_LEFT:
      break;
 }

  glutPostRedisplay();
}

void
keyboard(char key, int x, int y)
{
  switch (key) {
  case 'h':
    printf("shadow help\n\n");
    printf("f            -  Filled\n");
    printf("w            -  Wireframe\n");
    printf("o            -  Toggle shadows\n"); 
    printf("j            -  Toggle jittered sampling\n");
    printf("c            -  Toggle backface culling\n");
    printf("t            -  Toggle texturing\n");
    printf("r            -  Reset the view\n");
    printf("v            -  Change view\n");
    printf("V            -  View from light\n");
    printf("s            -  Toggle sampling vectors\n");
    printf("S            -  Print stats\n");
    printf("F            -  Show frame rate\n");
    printf("+            -  Increase number of samples\n");
    printf("-            -  Decrease number of samples\n");
    printf("l            -  Decrease light width\n");
    printf("L            -  Increase light width\n");
    printf("escape or q  -  Quit\n\n");
    break;

  case 'F':
    frame_rate = !frame_rate;
    break;

  case 'S':
    printf("Number of polygons/frame: %d (%dx%d)\n", (couch->numtriangles + table->numtriangles + lamp->numtriangles + 3) * (num_samples + 1), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    break;

  case 'j':
    jittered = !jittered;
    printf("jittered = %d\n", jittered);
    break;

  case 'o':
    draw_shadows = !draw_shadows;
    break;

  case 's':
    draw_sample_vectors = !draw_sample_vectors;
    break;

  case 'f':
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;

  case 'w':
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;

  case 'c':
    if (glIsEnabled(GL_CULL_FACE))
      glDisable(GL_CULL_FACE);
    else
      glEnable(GL_CULL_FACE);      
    break;

  case 't':
    texturing = !texturing;
    lists();
    break;

  case 'v':
    light_position[X] = -light_position[X];
    light_samples = samplelight(num_samples, light_size, light_position);
    break;

  case 'V':
    view_from_light = !view_from_light;
    break;

  case 'r':
    tbInit(GLUT_MIDDLE_BUTTON);
    break;

  case '+':
    num_samples = (sqrt(num_samples)+1) * (sqrt(num_samples)+1);
    light_samples = samplelight(num_samples, light_size, light_position);
    printf("Number of samples: %d\n", num_samples);
    break;

  case '-':
    num_samples = (sqrt(num_samples)-1) * (sqrt(num_samples)-1);
    if (num_samples < 1)
      num_samples = 1;
    light_samples = samplelight(num_samples, light_size, light_position);
    printf("Number of Samples: %d\n", num_samples);
    break;

  case 'L':
    light_size += 1;
    light_samples = samplelight(num_samples, light_size, light_position);
    printf("Size of the light: %d\n", light_size);
    break;

  case 'l':
    light_size -= 1;
    if (light_size < 1)
      light_size = 1;
    light_samples = samplelight(num_samples, light_size, light_position);
    printf("Size of the light: %d\n", light_size);
    break;

  case 'q':
  case 27:
    exit(0);
    break;
  }

  glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
  tbMouse(button, state, x, y);
}

void
motion(int x, int y)
{
  tbMotion(x, y);
}

int
main(int argc, char** argv)
{
  int buffer = GLUT_DOUBLE;

  glutInit(&argc, argv);

  while (--argc) {
    if (strcmp("-sb", argv[argc]) == 0) {
      buffer = GLUT_SINGLE;
    } else {
      printf("%s [-sb]\n", argv[0]);
      printf("  -sb   single buffered\n");
      exit(0);
    }
  }

  glutInitWindowSize(256, 256);
  glutInitDisplayMode(GLUT_RGB | buffer | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
  glutCreateWindow("Shadow");
  
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  
  init();
  
  glutMainLoop();
  return 0;
}
