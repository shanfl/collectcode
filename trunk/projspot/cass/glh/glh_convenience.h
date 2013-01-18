#ifndef GLH_CONVENIENCE_H
#define GLH_CONVENIENCE_H

// Convenience methods for using glh_linear objects
// with opengl...

// debugging hack...
#include <iostream>
using namespace std;

#include <glh_linear.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glh_ext.h>

namespace glh
{

  // per-vertex helpers

  inline void color(const vec3f & c) { glColor3fv(&c[0]); }
  inline void color(const vec4f & c) { glColor4fv(&c[0]); }

  inline void normal(const vec3f & n) { glNormal3fv(&n[0]); }

  inline void texcoord(const vec2f & t) { glTexCoord2fv(&t[0]); }
  inline void texcoord(const vec3f & t) { glTexCoord3fv(&t[0]); }
  inline void texcoord(const vec4f & t) { glTexCoord4fv(&t[0]); }

#ifdef GL_ARB_multitexture
  inline void mtexcoord(GLenum unit, const vec2f & t) { glMultiTexCoord2fvARB(unit, &t[0]); }
  inline void mtexcoord(GLenum unit, const vec3f & t) { glMultiTexCoord3fvARB(unit, &t[0]); }
  inline void mtexcoord(GLenum unit, const vec4f & t) { glMultiTexCoord4fvARB(unit, &t[0]); }

#endif


  inline void vertex(const vec2f & v) { glVertex2fv(&v[0]); }
  inline void vertex(const vec3f & v) { glVertex3fv(&v[0]); }
  inline void vertex(const vec4f & v) { glVertex4fv(&v[0]); }

  // lighting helpers

  inline void material(GLenum face, GLenum pname, GLint i)
  { glMateriali(face, pname, i); }

  inline void material(GLenum face, GLenum pname, GLfloat f)
  { glMaterialf(face, pname, f); }

  inline void material(GLenum face, GLenum pname, const vec4f & v)
  { glMaterialfv(face, pname, &v[0]); }

  inline void light(GLenum light, GLenum pname, GLint i)
  { glLighti(light, pname, i); } 

  inline void light(GLenum light, GLenum pname, GLfloat f)
  { glLightf(light, pname, f); } 

  inline void light(GLenum light, GLenum pname, const vec4f & v)
  { glLightfv(light, pname, &v[0]); } 

  inline void light(GLenum light, GLenum pname, const vec3f & v, float wa = 1)
  {   vec4f v4(v[0], v[1], v[2], wa); glLightfv(light, pname, &v4[0]); } 


  // matrix helpers

  inline matrix4f get_matrix(GLenum matrix) 
  {
	GLfloat m[16];
	glGetFloatv(matrix, m);
	
	return matrix4f(m);
  }

  inline void load_matrix(const matrix4f & m)
  {
	matrix4f m2(m.transpose());
	// matrix4f is row-major, so we must transpose
	// before we get the pointer...
	glLoadMatrixf(m2[0]);	
  }

  inline void mult_matrix(const matrix4f & m)
  {
	matrix4f m2(m.transpose());
	// matrix4f is row-major, so we must transpose
	// before we get the pointer...
	glMultMatrixf(m2[0]);	
  }

  // transform helpers

  inline void rotate(const rotationf & r)
  {
	float angle;
	vec3f axis;
	r.get_value(axis, angle);
	glRotatef(to_degrees(angle), axis[0], axis[1], axis[2]);
  }

  inline void translate(const vec3f & t)
  {
	glTranslatef(t[0], t[1], t[2]);
  }

  inline void scale(const vec3f & s)
  {
 	glScalef(s[0], s[1], s[2]);
  }

  // inverse of camera_lookat
  inline matrix4f object_lookat(const vec3f & eye, const vec3f & lookpoint, const vec3f & up)
  {
	  // calculate look vector rotation
	  vec3f initial_look(0,0,-1);
	  vec3f look = lookpoint - eye;
	  look.normalize();
	  rotationf look_rot(initial_look, look);
	  matrix4f look_mat;
	  look_mat.set_rotate(look_rot);

	  // calculate up vector rotation
	  vec3f initial_up(0, 1, 0), up2;
	  vec3f orth_up = up - (look * look.dot(up)); // not normalized
	  look_mat.mult_matrix_vec(initial_up, up2);
	  rotationf up_rot(up2, orth_up);
	  matrix4f up_mat;
	  up_mat.set_rotate(up_rot);

	  // translate to eye position
	  matrix4f eye_mat;
	  eye_mat.set_translate(eye);

	  return eye_mat * up_mat * look_mat;
  }

  // inverse of object_lookat
  inline matrix4f camera_lookat(const vec3f & eye, const vec3f & lookpoint, const vec3f & up)
  {
	  // calculate look vector rotation
	  vec3f initial_look(0,0,-1);
	  vec3f look = lookpoint - eye;
	  look.normalize();
	  rotationf look_rot(initial_look, look);
	  matrix4f look_mat;
	  look_mat.set_rotate(look_rot);

	  
	  // calculate up vector rotation
	  vec3f initial_up(0, 1, 0), up2;
	  vec3f orth_up = up - (look * look.dot(up)); // not normalized
	  look_mat.mult_matrix_vec(initial_up, up2);
	  rotationf up_rot(up2, orth_up);
	  matrix4f up_mat;
	  up_mat.set_rotate(up_rot.inverse());

	  // used the real look_rot to calculate the up_rot
	  // need to invert it before we're done
	  look_mat.set_rotate(look_rot.inverse());
	  
	  // translate to eye position
	  matrix4f eye_mat;
	  eye_mat.set_translate(-eye);
	
	  matrix4f m =  look_mat * up_mat * eye_mat;	 
	  return m;
  }


  inline matrix4f frustum(float left, float right,
				   float bottom, float top,
				   float zNear, float zFar)
  {
	matrix4f m;
	m.make_identity();

	m[0][0] = (2*zNear) / (right - left);
	m[0][2] = (right + left) / (right - left);
	
	m[1][1] = (2*zNear) / (top - bottom);
	m[1][2] = (top + bottom) / (top - bottom);
	
	m[2][2] = -(zFar + zNear) / (zFar - zNear);
	m[2][3] = -2*zFar*zNear / (zFar - zNear);
   
	m[3][2] = -1;
	m[3][3] = 0;

	return m;
  }

  inline matrix4f frustum_inverse(float left, float right,
						   float bottom, float top,
						   float zNear, float zFar)
  {
	matrix4f m;
	m.make_identity();

	m[0][0] = (right - left) / (2 * zNear);
	m[0][3] = (right + left) / (2 * zNear);
	
	m[1][1] = (top - bottom) / (2 * zNear);
	m[1][3] = (top + bottom) / (2 * zNear);

	m[2][2] = 0;
	m[2][3] = -1;
	
	m[3][2] = -(zFar - zNear) / (2 * zFar * zNear);
	m[3][3] = (zFar + zNear) / (2 * zFar * zNear);

	return m;
  }

  inline matrix4f perspective(float fovy, float aspect, float zNear, float zFar)
  {
	double tangent = tan(to_radians(fovy/2.0));
	float y = tangent * zNear;
	float x = aspect * y;
	return frustum(-x, x, -y, y, zNear, zFar);
  }

  inline matrix4f perspective_inverse(float fovy, float aspect, float zNear, float zFar)
  {
	double tangent = tan(to_radians(fovy/2.0));
	float y = tangent * zNear;
	float x = aspect * y;
	return frustum_inverse(-x, x, -y, y, zNear, zFar);
  }

  inline void set_texgen_planes(GLenum plane_type, const matrix4f & m)
  {
	  GLenum coord[] = {GL_S, GL_T, GL_R, GL_Q };
	  for(int i = 0; i < 4; i++)
	  {
		  glTexGenfv(coord[i], plane_type, m[i]);
	  }
  }

  inline void texgen_mode(GLenum coord, GLint param)
  { glTexGeni(coord, GL_TEXTURE_GEN_MODE, param); }


  inline void texenv_mode(GLint m)
  { glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m); }

} // namespace glh

#endif
