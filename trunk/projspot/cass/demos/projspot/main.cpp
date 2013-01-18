#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>

#include "ppm.h"
#define GLH_EXT_SINGLE_FILE
#include "glh_ext.h"
#include "glh_obs.h"
#include "glh_convenience.h"
#include "glh_glut.h"


using namespace std;
using namespace glh;

#define CAMERA 0
#define OBJECT 1
#define SPOTLIGHT 2

glut_callbacks cb;
glut_simple_mouse_interactor camera, object, spotlight, *current_interactor;
glut_perspective_reshaper reshaper;

tex_object_2D spot, decal;
display_list quad, wirecube, teapot;

float fovy = 45.f;
rotationf parent;

// glut callbacks
void key(unsigned char k, int x, int y);
void reshape(int w, int h);
void display();
void menu(int entry);

void init_opengl();

void print_matrix(matrix4f & m)
{
  for(int i=0; i < 4; i++)
  {
	for(int j=0; j < 4; j++)
	  cerr << "   " << m[i][j];
	cerr << endl;
  }
}

ostream & operator << (ostream & o, vec3f & v)
{
	o << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
	return o;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB);
	glutCreateWindow("Projective Spotlight Texture");

	init_opengl();	

	// setup glut event multiplexor
	glut_helpers_initialize();

	// configure interactors
	cb.keyboard_function = key;
	camera.configure_buttons(2);
	object.configure_buttons(2);
	spotlight.configure_buttons(2);


	object.dolly.dolly[2] = -2; // push plane forward
	camera.trackball.centroid = vec3f(0, 0, -2); // rotate camera trackball about the plane...
	camera.trackball.r = rotationf(vec3f(1, 0, 0), .3);
	spotlight.pan.pan[1] = 1;
	spotlight.dolly.dolly[2] = -1; // push spotlight forward
	spotlight.trackball.r = rotationf(vec3f(1, 0, 0), -.8);
	spotlight.trackball.invert_increment = true;

	reshaper.fovy = 60;
	reshaper.zNear = .1f;
	reshaper.zFar = 20;

	current_interactor = & camera;
	// attach interactors to the event multiplexor
	glut_add_interactor(& cb);
	glut_add_interactor(& camera);
	glut_add_interactor(& reshaper);

	glutCreateMenu(menu);
	glutAddMenuEntry("move camera", CAMERA);
	glutAddMenuEntry("move plane", OBJECT);
	glutAddMenuEntry("move spotlight", SPOTLIGHT);
	glutAddMenuEntry("----------------", 10);
	glutAddMenuEntry("left-mouse is trackball", 10);
	glutAddMenuEntry("middle-mouse is pan", 10);
	glutAddMenuEntry("ctrl-left-mouse is dolly ", 10);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}

void menu(int entry)
{
	key(entry, 0, 0);
}

void init_opengl()
{
	decal.bind();
	read_ppm("decal_image.ppm");

	spot.bind();
	read_ppm("spotlight_image.ppm");
	spot.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	spot.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
	spot.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
	spot.unbind();

	quad.new_list(GL_COMPILE);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex2f(-1, -1);
	glVertex2f(-1,  1);
	glVertex2f( 1,  1);
	glVertex2f( 1, -1);
	glEnd();
	glPopMatrix();
	quad.end_list();

	wirecube.new_list(GL_COMPILE);
	glutWireCube(2);
	wirecube.end_list();
	
	teapot.new_list(GL_COMPILE);
	glPushMatrix();
	glTranslatef(0, .4, 0);
	glutSolidTeapot(.5);
	glPopMatrix();
	teapot.end_list();

	glEnable(GL_LIGHT0);
	material(GL_FRONT, GL_SHININESS, 32);
	light(GL_LIGHT0, GL_SPECULAR, vec4f(1,1,1,1));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

}


void key(unsigned char k, int x, int y)
{
	static int interactor = CAMERA;
	if(k==27) exit(0);
	if(k==0 || k == 1 || k == 2)
	{
		glut_remove_interactor(current_interactor);
		if(k==0)      current_interactor = & camera;
		else if(k==1) current_interactor = & object;
		else          current_interactor = & spotlight;
		glut_add_interactor(current_interactor);
	}
	if(k=='v')
		fovy += 5.f;
	if(k=='V')
		fovy -= 5.f;
	glutPostRedisplay();
}

void config_projective_pass()
{
	matrix4f m;
	spot.bind();

	set_texgen_planes(GL_EYE_PLANE, m);
	texgen_mode(GL_S, GL_EYE_LINEAR);
	texgen_mode(GL_T, GL_EYE_LINEAR);
	texgen_mode(GL_R, GL_EYE_LINEAR);
	texgen_mode(GL_Q, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);


	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(.5, .5, 0);
	glScalef(.5, .5, 1);
	gluPerspective(fovy, 1, .1, 1);
	spotlight.apply_inverse_transform();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	
}

void config_regular_pass()
{
	matrix4f m;
	decal.bind();
	set_texgen_planes(GL_OBJECT_PLANE, m);
	texgen_mode(GL_S, GL_OBJECT_LINEAR);
	texgen_mode(GL_T, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
}

void render_scene(bool projective = false)
{
	glPushMatrix();
	object.apply_transform();
	material(GL_FRONT, GL_SPECULAR, vec4f(0, 0, 0, 1));
	material(GL_FRONT, GL_DIFFUSE, vec4f(.8, .8, .8, 1));
	quad.call_list();

	if(! projective)
		glDisable(GL_TEXTURE_2D);
	material(GL_FRONT, GL_SPECULAR, vec4f(0, .1, 0, 1));
	material(GL_FRONT, GL_DIFFUSE, vec4f(.2, .3, .2, 1));
	teapot.call_list();
	glPopMatrix();
}

void display()
{
	parent = camera.trackball.r.inverse();
	spotlight.trackball.parent_rotation = 
		spotlight.pan.parent_rotation = 
		spotlight.dolly.parent_rotation = 
		object.trackball.parent_rotation = 
		object.pan.parent_rotation = 
		object.dolly.parent_rotation = 
			& parent;
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	// apply interactor transforms
	camera.apply_transform();

	glPushMatrix();
	spotlight.apply_transform();
	light(GL_LIGHT0, GL_POSITION, vec4f(0, 0, 0, 1));
	glPopMatrix();

	glEnable(GL_LIGHTING);

	config_regular_pass();
	render_scene();


	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDepthMask(0);
	glDepthFunc(GL_LESS);
	glPolygonOffset(-1, -2);
	glEnable(GL_POLYGON_OFFSET_FILL);

	config_projective_pass();
	render_scene(true);

	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_BLEND);
	glDepthMask(1);
	glDepthFunc(GL_LESS);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// draw spotlight
	glColor3f(1,1,1);
	glPushMatrix();
	spotlight.apply_transform();
	mult_matrix(perspective_inverse(fovy, 1, .1, 1));
	wirecube.call_list();
	glPopMatrix();
	glPopMatrix();
 
	glutSwapBuffers();
}
