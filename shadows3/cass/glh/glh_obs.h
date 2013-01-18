// This is a file for simple GL helper classes...

#ifndef GLH_OBS_H
#define GLH_OBS_H

#ifdef WIN32
# include <windows.h>
#endif

#include <GL/gl.h>

namespace glh
{
	class display_list
	{
	public:
		display_list() 
			: valid(false) {}
		
		virtual ~display_list()
		{ del(); }
		
		void call_list()
		{ if(valid) glCallList(dlist); }
		
		void new_list(GLenum mode)
		{ if(!valid) gen(); glNewList(dlist, mode); }
		
		void end_list()
		{ glEndList(); }
		
		void del()
		{ if(valid) glDeleteLists(dlist, 1); valid = false; }
		
		bool is_valid() const { return valid; }
		
	private:
		
		void gen() { dlist = glGenLists(1); valid=true; }
		
		bool valid;
		GLuint dlist;
	};
	
	
	class tex_object
	{
	public:
		tex_object(GLenum tgt) 
			: target(tgt), valid(false) {}
		
		virtual ~tex_object()
		{ del(); }
		
		void bind()
		{ if(!valid) gen(); glBindTexture(target, texture); }
		
		void unbind()
		{ glBindTexture(target, 0); }

		// convenience methods
		void parameter(GLenum pname, GLint i)
		{ glTexParameteri(target, pname, i); }

		void parameter(GLenum pname, GLfloat f)
		{ glTexParameterf(target, pname, f); }

		void parameter(GLenum pname, GLint * ip)
		{ glTexParameteriv(target, pname, ip); }

		void parameter(GLenum pname, GLfloat * fp)
		{ glTexParameterfv(target, pname, fp); }

		void enable() { glEnable(target); }
		void disable() { glDisable(target); }

		void del()
		{ if(valid) glDeleteTextures(1, &texture); valid = false; }
		
		bool is_valid() const { return valid; }
		
	private:
		
		void gen() { glGenTextures(1, &texture); valid=true; }
		
		bool valid;
		GLuint texture;
		GLenum target;
	};
	
	class tex_object_1D : public tex_object
	{ public: tex_object_1D() : tex_object(GL_TEXTURE_1D) {} };
	
	class tex_object_2D : public tex_object
	{ public: tex_object_2D() : tex_object(GL_TEXTURE_2D) {} };

# ifdef GL_EXT_texture_cube_map
	class tex_object_cube_map : public tex_object
	{ public: tex_object_cube_map() : tex_object(GL_TEXTURE_CUBE_MAP_EXT) {} };
# endif
}
#endif