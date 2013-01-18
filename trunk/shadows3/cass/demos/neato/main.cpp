#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <stdlib.h>
#include <vector>

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

#define TEXTURE_SIZE 256

glut_callbacks cb;
glut_simple_mouse_interactor camera, object, spotlight, *current_interactor;

tex_object_2D spot, decal, depthmap, depthmap_alpha, depthmap_rgba,
 *current_depthmap, *current_projective;
tex_object_1D identity, identity_alpha, *current_identity;
display_list quad, wirecube, sphere;
texture_env_combine shadow0, shadow1, shadow0_fast, shadow1_fast;

vec4f light_intensity(1,1,1,1);
vec4f spot_intensity(1,1,1,1);
float spot_fovy = 35.f;
float spot_near = .4f;
float spot_far = 4.f;
float volume_density = .35;
int volume_num = 30;

int master_depthmap_version = 0;
int depthmap_version = -1;
int depthmap_rgba_version = -1;

bool b[256];

void draw_camera_view();
void draw_camera_view_depthmap();
void draw_spot_view();
void draw_spot_view_depthmap();
void draw_spot_view_depthmap_rgba();
void draw_spot_view_depthmap_rgba2();
void draw_camera_view_projective_depthmap();
void draw_camera_view_lit_region();
void draw_camera_view_shadowed();
void draw_camera_view_shadowed_faster();

int current_display_func;
vector<void (*)()> display_funcs;

// glut callbacks
void key(unsigned char k, int x, int y);
void reshape(int w, int h);
void display();
void menu(int entry);
void motion(int x, int y);

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
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB|GLUT_ALPHA|GLUT_STENCIL);
	glutCreateWindow("Projective Spotlight Texture");

	init_opengl();	
	
	int stencilbits;
	glGetIntegerv(GL_STENCIL_BITS, &stencilbits);
	cerr <<"We have " << stencilbits << " bits of stencil." << endl;

	// setup glut event multiplexor
	glut_helpers_initialize();


	// configure interactors
	cb.keyboard_function = key;
	cb.motion_function = motion;
	camera.configure_buttons(2);
	object.configure_buttons(2);
	spotlight.configure_buttons(2);

	object.dolly.dolly[2] = -3; // push plane forward
	camera.trackball.centroid = vec3f(0, 0, -3); // rotate camera trackball about the plane...
	camera.trackball.r = 
		rotationf(vec3f(0,1,0), to_radians(-45.f)) * 
		rotationf(vec3f(1.f, 0.f, 0.f), to_radians(-30.f));
	spotlight.pan.pan[0] = -.25;
	spotlight.pan.pan[1] = 1.5;
	spotlight.dolly.dolly[2] = -2.; // push spotlight forward
	spotlight.trackball.r =  rotationf(vec3f(1.f, 0.f, 0.f), to_radians(-60.f));
	spotlight.trackball.invert_increment = true;

	camera.set_camera_mode(true);

	current_interactor = & camera;
	// attach interactors to the event multiplexor
	glut_add_interactor(&cb);
	glut_add_interactor(& camera);

	current_display_func = 8;
	display_funcs.push_back(&draw_camera_view);
	display_funcs.push_back(&draw_spot_view);
	display_funcs.push_back(&draw_camera_view_depthmap);
	display_funcs.push_back(&draw_spot_view_depthmap);
	display_funcs.push_back(&draw_spot_view_depthmap_rgba2);
	display_funcs.push_back(&draw_camera_view_projective_depthmap);
	display_funcs.push_back(&draw_camera_view_lit_region);
	display_funcs.push_back(&draw_camera_view_shadowed);
	display_funcs.push_back(&draw_camera_view_shadowed_faster);
	
	int render_mode = glutCreateMenu(menu);
	glutAddMenuEntry("camera view unshadowed", 10);
	glutAddMenuEntry("spotlight view", 11);
	glutAddMenuEntry("camera view rangemap", 12);
	glutAddMenuEntry("spotlight view depthmap", 13);
	glutAddMenuEntry("spotlight view depthmap rgba", 14);
	glutAddMenuEntry("camera view of light's projected depth map", 15);
	glutAddMenuEntry("camera view lit region", 16);
	glutAddMenuEntry("camera view shadowed", 17);
	glutAddMenuEntry("camera view shadowed - faster", 18);


	int mouse_control_menu = glutCreateMenu(menu);
	glutAddMenuEntry("move camera", CAMERA);
	glutAddMenuEntry("move object", OBJECT);
	glutAddMenuEntry("move spotlight", SPOTLIGHT);
	glutAddMenuEntry("----------------", 1000);
	glutAddMenuEntry("left-mouse is trackball", 1000);
	glutAddMenuEntry("middle-mouse is pan", 1000);
	glutAddMenuEntry("ctrl-left-mouse is dolly ", 1000);

	int toggle_menu = glutCreateMenu(menu);
	b['a'] = true;
	glutAddMenuEntry("atmospheric effect", 'a');
	b['s'] = true;
	glutAddMenuEntry("spotlight frustum outline", 's');

	glutCreateMenu(menu);
	glutAddSubMenu("Render Modes", render_mode);
	glutAddSubMenu("Mouse Controls", mouse_control_menu);
	glutAddSubMenu("Toggle Parameters", toggle_menu);
	glutAddMenuEntry("quit <esc>", 27);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}

void motion(int x, int y)
{
	if( current_interactor == & spotlight ||
		current_interactor == & object )
		 master_depthmap_version++;
}

void menu(int entry)
{
	key(entry, 0, 0);
}

void texenv_modulate()
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void init_opengl()
{
	if(! glutExtensionSupported("GL_EXT_texture_env_combine"))
	{
		cerr << "This program requries GL_EXT_texture_env_combine." << endl;
		exit(0);
	}
	if(! glutExtensionSupported("GL_ARB_multitexture"))
	{
		cerr << "This program requires GL_ARB_multitexture." << endl;
		exit(0);
	}
	init_GL_ARB_multitexture();

	decal.bind();
	read_ppm("decal_image.ppm");

	spot.bind();
	read_ppm("spotlight_image.ppm");
	spot.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	spot.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
	spot.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);

	{
		GLubyte img[TEXTURE_SIZE*TEXTURE_SIZE*4];
		depthmap.bind();
		depthmap.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		depthmap.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		depthmap.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
		depthmap.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, TEXTURE_SIZE, TEXTURE_SIZE, 0, 
			GL_LUMINANCE, GL_UNSIGNED_BYTE, img);		
		depthmap_alpha.bind();
		depthmap_alpha.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		depthmap_alpha.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		depthmap_alpha.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
		depthmap_alpha.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TEXTURE_SIZE, TEXTURE_SIZE, 0, 
			GL_ALPHA, GL_UNSIGNED_BYTE, img);		
		depthmap_rgba.bind();
		depthmap_rgba.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		depthmap_rgba.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		depthmap_rgba.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
		depthmap_rgba.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, img);		

	}

	{
		// use a cubic texture instead of linear
		// to help space the "depth" values more evenly
		// and get more use (hopefully) out of our limited precision
		GLubyte bytes[65536];
		for(int i=0; i < 65536; i++)
		{
			float f = (i/65535.0);
			bytes[i] = GLubyte(255*f*f*f);
		}
		identity.bind();
		gluBuild1DMipmaps(GL_TEXTURE_1D, GL_LUMINANCE8, 65536, GL_LUMINANCE,
						  GL_UNSIGNED_BYTE, bytes);

		identity.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		identity.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		identity_alpha.bind();
		gluBuild1DMipmaps(GL_TEXTURE_1D, GL_ALPHA8, 65536, GL_ALPHA,
						  GL_UNSIGNED_BYTE, bytes);
		identity_alpha.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		identity_alpha.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	identity.unbind();

	// shadow0 combiner
	shadow0.rgb.combine = GL_REPLACE;
	shadow0.rgb.src[0] = GL_PREVIOUS_EXT;
	shadow0.a.combine = GL_REPLACE;
	shadow0.a.src[0] = GL_TEXTURE;

	// shadow1 combiner
	shadow1.rgb.combine = GL_REPLACE;
	shadow1.rgb.src[0] = GL_PREVIOUS_EXT;
	shadow1.a.combine = GL_ADD_SIGNED_EXT;
	shadow1.a.src[0] = GL_PREVIOUS_EXT;
	shadow1.a.src[1] = GL_TEXTURE;
	shadow1.a.op[1]  = GL_ONE_MINUS_SRC_ALPHA;


	// shadow0_fast combiner
	shadow0_fast.rgb.combine = GL_REPLACE;
	shadow0_fast.rgb.src[0] = GL_PREVIOUS_EXT;
	shadow0_fast.a.combine = GL_REPLACE;
	shadow0_fast.a.src[0] = GL_TEXTURE;


	// shadow1_fast combiner
	shadow1_fast.rgb.combine = GL_MODULATE;
	shadow1_fast.rgb.src[0] = GL_TEXTURE;
	shadow1_fast.rgb.src[1] = GL_PREVIOUS_EXT;
	shadow1_fast.a.combine = GL_ADD_SIGNED_EXT;
	shadow1_fast.a.src[0] = GL_PREVIOUS_EXT;
	shadow1_fast.a.src[1] = GL_TEXTURE;
	shadow1_fast.a.op[1]  = GL_ONE_MINUS_SRC_ALPHA;

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
	glutWireCube(2); // got to figure out why this is not "2"!
	wirecube.end_list();
	
	sphere.new_list(GL_COMPILE);
	glPushMatrix();
	glTranslatef(0, .4, 0);
	glutSolidTeapot(.5);
	glPopMatrix();
	sphere.end_list();

	glEnable(GL_LIGHT0);
	material(GL_FRONT, GL_SHININESS, 32);

	light(GL_LIGHT0, GL_AMBIENT, vec4f(0,0,0,0));
	light(GL_LIGHT0, GL_DIFFUSE, light_intensity);
	light(GL_LIGHT0, GL_SPECULAR, light_intensity);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

}
void key(unsigned char k, int x, int y)
{
	b[k] = !b[k];
	if(k==27) exit(0);
	if(k==0 || k == 1 || k == 2)
	{
		glut_remove_interactor(current_interactor);
		if     (k==CAMERA) current_interactor = & camera;
		else if(k==OBJECT) current_interactor = & object;
		else               current_interactor = & spotlight;
		glut_add_interactor(current_interactor);
	}
	if(k=='v')
		spot_fovy += 5.f;
	if(k=='V')
		spot_fovy -= 5.f;
	if(k=='+')
		volume_num ++;
	if(k=='-')
		volume_num --;
	if(k==' ')
	{
		current_display_func++;
		current_display_func %= display_funcs.size();
	}

	if(k=='i')
	{
		light_intensity += .05;
		light(GL_LIGHT0, GL_DIFFUSE, light_intensity);
		light(GL_LIGHT0, GL_SPECULAR, light_intensity);
	}
	if(k=='I')
	{
		light_intensity -= .05;
		light(GL_LIGHT0, GL_DIFFUSE, light_intensity);
		light(GL_LIGHT0, GL_SPECULAR, light_intensity);
	}

	if(k>= 10 && k < (display_funcs.size()+10)) current_display_func = k-10;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	float aspect = float(w)/float(h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(spot_fovy, aspect, spot_near, spot_far);
	glMatrixMode(GL_MODELVIEW);
}

void config_depthmap_pass()
{
	matrix4f m;
	current_identity->bind();

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
	glTranslatef(.5, 0, 0);
	glScalef(.5, 1, 1);
	glRotatef(90, 0, 1, 0);
	// how can this be done linearly?...
	gluPerspective(spot_fovy, 1, spot_near, spot_far);
	spotlight.apply_inverse_transform();
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_1D);
	
}

void config_projective_pass()
{
	matrix4f m;
	current_projective->bind();

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
	gluPerspective(spot_fovy, 1, spot_near, spot_far);
	spotlight.apply_inverse_transform();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	
}

void config_regular_pass()
{
	matrix4f m;
	decal.bind();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
	material(GL_FRONT, GL_DIFFUSE, vec4f( .4, .4, .4, 1));
	material(GL_FRONT, GL_SPECULAR, vec4f(0, 0, 0, 1));
	quad.call_list();

	if(! projective)
	{
		glDisable(GL_TEXTURE_2D);
		material(GL_FRONT, GL_DIFFUSE, vec4f(.35, .5, .35, 1));
		material(GL_FRONT, GL_SPECULAR, vec4f(.1, .2, .1, 1));
	}
	else
	{
		material(GL_FRONT, GL_DIFFUSE, vec4f(.35, .5, .35, 1));
		material(GL_FRONT, GL_SPECULAR, vec4f(.0, .0, .0, 1));
	}
	sphere.call_list();
	glPopMatrix();
}

void render_planes()
{
	float delta = 2.0/(volume_num-1);
	glPushMatrix();
	glTranslatef(0, -1, 0);
	{
		for(int i=0; i < volume_num; i++)
		{
			quad.call_list();
			glTranslatef(0, delta, 0);
		}
		quad.call_list();
	}
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glTranslatef(0, -1, 0);
	{
		for(int i=0; i < volume_num; i++)
		{
			quad.call_list();
			glTranslatef(0, delta, 0);
		}
		quad.call_list();
	}
	glPopMatrix();
}

void set_parent_rotation(const rotationf & r)
{
	object.pan.parent_rotation = & r;
	object.dolly.parent_rotation = & r;
	object.trackball.parent_rotation = & r;
	spotlight.pan.parent_rotation = & r;
	spotlight.dolly.parent_rotation = & r;
	spotlight.trackball.parent_rotation = & r;
	camera.trackball.parent_rotation = & r;
}

void update_depth_map()
{
	if(depthmap_version == master_depthmap_version) return;
	depthmap_version = master_depthmap_version;
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	GLint sz = TEXTURE_SIZE;
	glViewport(0,0,sz, sz);
	draw_spot_view_depthmap();
	GLubyte img[TEXTURE_SIZE][TEXTURE_SIZE];
	glReadPixels(0, 0, sz, sz, GL_RED, GL_UNSIGNED_BYTE, img);
	// border
	{for(int i=0; i < sz; i++) img[   i][   0] = 255; }
	{for(int i=0; i < sz; i++) img[   i][sz-1] = 255; }
	{for(int i=0; i < sz; i++) img[   0][   i] = 255; }
	{for(int i=0; i < sz; i++) img[sz-1][   i] = 255; }
	depthmap.bind();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sz, sz, 
				 GL_LUMINANCE, GL_UNSIGNED_BYTE, img);
	depthmap_alpha.bind();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sz, sz, 
				 GL_ALPHA, GL_UNSIGNED_BYTE, img);
	depthmap_alpha.unbind();
	glViewport(vp[0], vp[1], vp[2], vp[3]);
}

// The depthmap_rgba has the actual depthmap in the alpha
// channel and the rgb contains the slide projector image.
// This allows single pass rendering with only two texture
// units at the expense of respecifying a the slide projector
// image each frame and requiring the depthmap and slide image
// be the same resolution (which may not be optimal).
void update_depth_map_rgba()
{
	if(depthmap_rgba_version == master_depthmap_version) return;
	depthmap_rgba_version = master_depthmap_version;

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	GLint sz = TEXTURE_SIZE;
	glViewport(0,0,sz, sz);
	draw_spot_view_depthmap_rgba();
	GLubyte img[TEXTURE_SIZE][TEXTURE_SIZE][4];
	glReadPixels(0, 0, sz, sz, GL_RGBA, GL_UNSIGNED_BYTE, img);
	// border
	{for(int i=0; i < sz; i++) img[   i][   0][0] =   0; }
	{for(int i=0; i < sz; i++) img[   i][sz-1][0] =   0; }
	{for(int i=0; i < sz; i++) img[   0][   i][0] =   0; }
	{for(int i=0; i < sz; i++) img[sz-1][   i][0] =   0; }
	{for(int i=0; i < sz; i++) img[   i][   0][1] =   0; }
	{for(int i=0; i < sz; i++) img[   i][sz-1][1] =   0; }
	{for(int i=0; i < sz; i++) img[   0][   i][1] =   0; }
	{for(int i=0; i < sz; i++) img[sz-1][   i][1] =   0; }
	{for(int i=0; i < sz; i++) img[   i][   0][2] =   0; }
	{for(int i=0; i < sz; i++) img[   i][sz-1][2] =   0; }
	{for(int i=0; i < sz; i++) img[   0][   i][2] =   0; }
	{for(int i=0; i < sz; i++) img[sz-1][   i][2] =   0; }
	{for(int i=0; i < sz; i++) img[   i][   0][3] = 255; }
	{for(int i=0; i < sz; i++) img[   i][sz-1][3] = 255; }
	{for(int i=0; i < sz; i++) img[   0][   i][3] = 255; }
	{for(int i=0; i < sz; i++) img[sz-1][   i][3] = 255; }
	depthmap_rgba.bind();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sz, sz, 
				 GL_RGBA, GL_UNSIGNED_BYTE, img);
	depthmap_rgba.unbind();
	glViewport(vp[0], vp[1], vp[2], vp[3]);
}


void do_shadowed_atmospheric_effect()
{

	// test, but don't write the depth buffer
	glDepthMask(0);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);

	matrix4f view = get_matrix(GL_MODELVIEW_MATRIX);
	float delta = 2.0/(volume_num-1);
	glPushMatrix();
	spotlight.apply_transform();
	mult_matrix(perspective_inverse(spot_fovy, 1, spot_near, spot_far));

	glPushMatrix();
	glTranslatef(0, -1, 0);
	for(int i=0; i < volume_num; i++)
	{
		// mark unshadowed stencil=1
		current_projective = &depthmap_alpha;	
		current_identity = &identity_alpha;

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glPushMatrix();
		load_matrix(view);
		config_depthmap_pass();
		glPopMatrix();	
		shadow0.apply(GL_RGB);
		shadow0.apply(GL_ALPHA);
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glPushMatrix();
		load_matrix(view);
		config_projective_pass();
		glPopMatrix();	
		shadow1.apply(GL_RGB);
		shadow1.apply(GL_ALPHA);
		
		glAlphaFunc(GL_LESS, 0.5);
		glEnable(GL_ALPHA_TEST);
		
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		glColorMask(0,0,0,0);
		quad.call_list();		
		glColorMask(1,1,1,1);
		
		glDisable(GL_ALPHA_TEST);
		
		texenv_modulate();
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		texenv_modulate();
		glDisable(GL_TEXTURE_1D);
		
		// now unshadowed has stencil=1
		
		glStencilFunc(GL_NOTEQUAL, 0, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		current_projective = &spot;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		
		glPushMatrix();
		load_matrix(view);
		config_projective_pass();
		glPopMatrix();

		glEnable(GL_TEXTURE_2D);	
		glColor4f(1,1,1, volume_density/volume_num);
		quad.call_list();
		glDisable(GL_TEXTURE_2D);	

		glTranslatef(0, delta, 0);
	}
	glPopMatrix();

	{
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glTranslatef(0, -1, 0);
	for(int i=0; i < volume_num; i++)
	{
		// mark unshadowed stencil=1
		current_projective = &depthmap_alpha;	
		current_identity = &identity_alpha;

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glPushMatrix();
		load_matrix(view);
		config_depthmap_pass();
		glPopMatrix();	
		shadow0.apply(GL_RGB);
		shadow0.apply(GL_ALPHA);
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glPushMatrix();
		load_matrix(view);
		config_projective_pass();
		glPopMatrix();	
		shadow1.apply(GL_RGB);
		shadow1.apply(GL_ALPHA);
		
		glAlphaFunc(GL_LESS, 0.5);
		glEnable(GL_ALPHA_TEST);
		
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		glColorMask(0,0,0,0);
		quad.call_list();		
		glColorMask(1,1,1,1);
		
		glDisable(GL_ALPHA_TEST);
		
		texenv_modulate();
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		texenv_modulate();
		glDisable(GL_TEXTURE_1D);
		
		// now unshadowed has stencil=1
		
		glStencilFunc(GL_NOTEQUAL, 0, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		current_projective = &spot;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		
		glPushMatrix();
		load_matrix(view);
		config_projective_pass();
		glPopMatrix();

		glEnable(GL_TEXTURE_2D);	
		glColor4f(1,1,1,.02);
		quad.call_list();
		glDisable(GL_TEXTURE_2D);	

		glTranslatef(0, delta, 0);
	}
	glPopMatrix();

	}
	glDepthMask(1);
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);
	glPopMatrix();

}

void draw_spotlight_frustum_outline()
{
	if(!b['s']) return;
	// draw spotlight
	glPushMatrix();
	spotlight.apply_transform();
	mult_matrix(perspective_inverse(spot_fovy, 1, spot_near, spot_far));
	wirecube.call_list();
	glPopMatrix();


}

void draw_camera_view_shadowed_faster()
{

	update_depth_map_rgba();
	current_projective = & depthmap_rgba;	
	current_identity = & identity_alpha;
	set_parent_rotation(camera.trackball.r);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	camera.apply_inverse_transform();

	glPushMatrix();
	spotlight.apply_transform();
	light(GL_LIGHT0, GL_POSITION, vec4f(0, 0, 0, 1));
	glPopMatrix();

	glEnable(GL_LIGHTING);
	
	if(b['%']) glColorMask(0,0,0,0);
	config_regular_pass();
	render_scene(false);
	if(b['%']) glColorMask(1,1,1,1);



	glActiveTextureARB(GL_TEXTURE0_ARB);
	config_depthmap_pass();	
	shadow0_fast.apply(GL_RGB);
	shadow0_fast.apply(GL_ALPHA);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	config_projective_pass();	
	shadow1_fast.apply(GL_RGB);
	shadow1_fast.apply(GL_ALPHA);

	glAlphaFunc(GL_LEQUAL, 0.5);
	glEnable(GL_ALPHA_TEST);

	light(GL_LIGHT0, GL_DIFFUSE, spot_intensity);
	light(GL_LIGHT0, GL_SPECULAR, spot_intensity);

	glEnable(GL_LIGHTING);

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	glDepthMask(0);
	glDepthFunc(GL_LESS);
	glPolygonOffset(-1, -2);          // this is not required on GeForce
	glEnable(GL_POLYGON_OFFSET_FILL); // but is on TNT

	render_scene(true);

	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_LIGHTING);
	light(GL_LIGHT0, GL_DIFFUSE, light_intensity);
	light(GL_LIGHT0, GL_SPECULAR, light_intensity);
	glDepthFunc(GL_LESS);

	if(b['a'])
	{
		float blend = volume_density / volume_num;
		glColor4f(blend, blend, blend, 1);
		glBlendFunc(GL_ONE, GL_ONE);

		glPushMatrix();
		spotlight.apply_transform();
		mult_matrix(perspective_inverse(spot_fovy, 1, spot_near, spot_far));
		
		render_planes();
		
		glPopMatrix();
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glColor4f(1,1,1,1);
	}

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);


	texenv_modulate();
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	texenv_modulate();
	glDisable(GL_TEXTURE_1D);

	glDepthMask(1);


	draw_spotlight_frustum_outline();

	glPopMatrix();
	
}

void draw_camera_view_shadowed()
{
	update_depth_map();
	current_projective = &depthmap_alpha;	
	current_identity = &identity_alpha;
	set_parent_rotation(camera.trackball.r);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	camera.apply_inverse_transform();

	glPushMatrix();
	spotlight.apply_transform();
	light(GL_LIGHT0, GL_POSITION, vec4f(0, 0, 0, 1));
	glPopMatrix();

	glEnable(GL_LIGHTING);
	
	if(b['%']) glColorMask(0,0,0,0);
	config_regular_pass();
	render_scene(false);
	if(b['%']) glColorMask(1,1,1,1);


	glDepthMask(0);
	glDepthFunc(GL_EQUAL);

	// mark unshadowed stencil=1
	glActiveTextureARB(GL_TEXTURE0_ARB);
	config_depthmap_pass();
	shadow0.apply(GL_RGB);
	shadow0.apply(GL_ALPHA);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	config_projective_pass();	
	shadow1.apply(GL_RGB);
	shadow1.apply(GL_ALPHA);

	glAlphaFunc(GL_LEQUAL, 0.5);
	glEnable(GL_ALPHA_TEST);

	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_STENCIL_TEST);

	glColorMask(0,0,0,0);
	render_scene(true);
	glColorMask(1,1,1,1);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);

	texenv_modulate();
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	texenv_modulate();
	glDisable(GL_TEXTURE_1D);

	// now unshadowed has stencil=1
	
	glStencilFunc(GL_NOTEQUAL, 0, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_LIGHTING);
	current_projective = &spot;

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	config_projective_pass();
	render_scene(true);

	glDepthMask(1);
	glDepthFunc(GL_LESS);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);

	if(b['a'])
		do_shadowed_atmospheric_effect();

	draw_spotlight_frustum_outline();

	glPopMatrix();
	
}


void draw_camera_view_lit_region()
{
	update_depth_map();
	current_projective = &depthmap_alpha;	
	current_identity = &identity_alpha;
	set_parent_rotation(camera.trackball.r);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	camera.apply_inverse_transform();

	glPushMatrix();
	spotlight.apply_transform();
	light(GL_LIGHT0, GL_POSITION, vec4f(0, 0, 0, 1));
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	config_depthmap_pass();
	shadow0.apply(GL_RGB);
	shadow0.apply(GL_ALPHA);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	config_projective_pass();	
	shadow1.apply(GL_RGB);
	shadow1.apply(GL_ALPHA);

	glAlphaFunc(GL_LEQUAL, 0.5);
	glEnable(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	render_scene(true);
	glDisable(GL_BLEND);

	glDisable(GL_ALPHA_TEST);

	glDisable(GL_LIGHTING);
	texenv_modulate();
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	texenv_modulate();
	glDisable(GL_TEXTURE_1D);

	draw_spotlight_frustum_outline();

	glPopMatrix();
}


void draw_camera_view_projective_depthmap()
{
	update_depth_map();
	current_projective = &depthmap;	
	set_parent_rotation(camera.trackball.r);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	camera.apply_inverse_transform();


	glColor4f(1,1,1, 1);

	config_projective_pass();
	render_scene(true);

	glDisable(GL_TEXTURE_2D);
	draw_spotlight_frustum_outline();
	glPopMatrix();
 
}

void draw_spot_view_depthmap_rgba()
{
	set_parent_rotation(rotationf());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(spot_fovy, 1, spot_near, spot_far);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	spotlight.apply_inverse_transform();

	glDisable(GL_LIGHTING);
	glColor4f(1,1,1,1);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	current_projective = & spot;
	config_projective_pass();
	texenv_modulate();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	current_identity = &identity_alpha;
	config_depthmap_pass();
	texenv_modulate();

	// got to draw a plane at the far clip so
	// the spot image atmospheric effects aren't
	// clipped where there's no geometry
	glPushMatrix();
	spotlight.apply_transform();
	mult_matrix(perspective_inverse(spot_fovy, 1, spot_near, spot_far));
	glTranslatef( 0, 0, .995);
	glDepthFunc(GL_ALWAYS);
	glBegin(GL_QUADS);
	glVertex2f(-1, -1);
	glVertex2f(-1,  1);
	glVertex2f( 1,  1);
	glVertex2f( 1, -1);
	glEnd();
	glDepthFunc(GL_LESS);
	glPopMatrix();

	render_scene(true);

	glDisable(GL_TEXTURE_1D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix(); 
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
}

// this function is sort of a lame way
// to see both the alpha and color channels
void draw_spot_view_depthmap_rgba2()
{
	set_parent_rotation(rotationf());
	draw_spot_view_depthmap_rgba();

	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(90, 1, 0, 0);

	glDisable(GL_LIGHTING);
	glColor4f(.5,.5,.5,.5);

	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
	glEnable(GL_BLEND);

	quad.call_list();

	glDisable(GL_BLEND);


	glPopMatrix(); 
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
}

void draw_spot_view_depthmap()
{
	set_parent_rotation(rotationf());
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(spot_fovy, 1, spot_near, spot_far);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	spotlight.apply_inverse_transform();

	glDisable(GL_LIGHTING);

	current_identity = &identity;
	config_depthmap_pass();
	render_scene(true);

	glDisable(GL_TEXTURE_1D);

	glPopMatrix(); 
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
}


void draw_camera_view_depthmap()
{	
	set_parent_rotation(camera.trackball.r);
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	camera.apply_inverse_transform();

	glDisable(GL_LIGHTING);

	glColor3f(1,1,1);
	current_identity = &identity;
	config_depthmap_pass();
	texenv_modulate();
	render_scene(true);

	glDisable(GL_TEXTURE_1D);

	glColor3f(0, 0, 0);
	draw_spotlight_frustum_outline();

	glPopMatrix(); 
}

void draw_spot_view()
{
	current_projective = &spot;	
	set_parent_rotation(rotationf());
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	spotlight.apply_inverse_transform();

	glPushMatrix();
	glLoadIdentity();
	light(GL_LIGHT0, GL_POSITION, vec4f(0, 0, 0, 1));
	glPopMatrix();

	glEnable(GL_LIGHTING);

	config_regular_pass();
	render_scene();


	glDepthMask(0);
	glDepthFunc(GL_EQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	config_projective_pass();
	render_scene(true);

	glDepthFunc(GL_LESS);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	spotlight.apply_transform();
	mult_matrix(perspective_inverse(spot_fovy, 1, spot_near, spot_far));
	glColor4f(1, 1, 1, volume_density/volume_num);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	render_planes();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
	glPopMatrix();

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glColor3f(1,1,1);
	draw_spotlight_frustum_outline();

	glPopMatrix();
 
}

void draw_camera_view()
{
	set_parent_rotation(camera.trackball.r);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	camera.apply_inverse_transform();

	glPushMatrix();
	spotlight.apply_transform();
	light(GL_LIGHT0, GL_POSITION, vec4f(0, 0, 0, 1));
	glPopMatrix();

	glEnable(GL_LIGHTING);

	config_regular_pass();
	render_scene();


	glDepthMask(0);
	glDepthFunc(GL_EQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	current_projective = &spot;		
	config_projective_pass();
	render_scene(true);

	glDepthFunc(GL_LESS);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	spotlight.apply_transform();
	mult_matrix(perspective_inverse(spot_fovy, 1, spot_near, spot_far));
	glColor4f(1,1,1, volume_density/volume_num);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	if(b['a'])
		render_planes();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
	glPopMatrix();

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glColor3f(1,1,1);
	draw_spotlight_frustum_outline();

	glPopMatrix();
}

void display()
{

	display_funcs[current_display_func]();

	glutSwapBuffers();
}
