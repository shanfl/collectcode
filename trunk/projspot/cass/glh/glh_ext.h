#include <GL/gl.h>
// I included glext.h at the end of gl.h...

#if defined GLH_EXT_H && defined GLH_EXT_SINGLE_FILE
# error "glh_ext.h has already been included in non-single-file mode!  Move the GLH_EXT_SINGLE_FILE and includes closer to the top of the c++ file to eliminate this problem."
#endif

#ifndef GLH_EXT_H
#define GLH_EXT_H

// deal with single/multi file situation
#undef GLH_EXTERN
#ifdef GLH_EXT_SINGLE_FILE
#  define GLH_EXTERN
#else
#  define GLH_EXTERN extern
#endif

#include <glh_linear.h>

namespace glh
{
#ifdef WIN32

#include <windows.h>  // for wglGetProcAddress

# ifdef GL_EXT_blend_color
	GLH_EXTERN PFNGLBLENDCOLOREXTPROC      glBlendColorEXT;
	void init_GL_EXT_blend_color();
# endif // EXT_blend_color


# ifdef GL_EXT_blend_subtract
	GLH_EXTERN PFNGLBLENDEQUATIONEXTPROC      glBlendEquationEXT;
	 void init_GL_EXT_blend_subtract();
# endif // EXT_blend_subtract

# ifdef GL_EXT_secondary_color
	GLH_EXTERN PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT;
	void init_GL_EXT_secondary_color();
# endif

# ifdef GL_ARB_multitexture
	GLH_EXTERN PFNGLACTIVETEXTUREARBPROC      glActiveTextureARB;
	GLH_EXTERN PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
	GLH_EXTERN PFNGLMULTITEXCOORD2FARBPROC    glMultiTexCoord2fARB;
	GLH_EXTERN PFNGLMULTITEXCOORD2FVARBPROC    glMultiTexCoord2fvARB;
	GLH_EXTERN PFNGLMULTITEXCOORD3FARBPROC    glMultiTexCoord3fARB;
	GLH_EXTERN PFNGLMULTITEXCOORD3FVARBPROC    glMultiTexCoord3fvARB;
	GLH_EXTERN PFNGLMULTITEXCOORD4FARBPROC    glMultiTexCoord4fARB;
	GLH_EXTERN PFNGLMULTITEXCOORD4FVARBPROC    glMultiTexCoord4fvARB;
	void init_GL_ARB_multitexture();
# endif // ARB_multitexture

# ifdef GL_NV_register_combiners

	GLH_EXTERN PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
	GLH_EXTERN PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
	GLH_EXTERN PFNGLCOMBINERPARAMETERFNVPROC  glCombinerParameterfNV;
	GLH_EXTERN PFNGLCOMBINERPARAMETERINVPROC  glCombinerParameteriNV;
	GLH_EXTERN PFNGLCOMBINERINPUTNVPROC       glCombinerInputNV;
	GLH_EXTERN PFNGLCOMBINEROUTPUTNVPROC      glCombinerOutputNV;
	GLH_EXTERN PFNGLFINALCOMBINERINPUTNVPROC  glFinalCombinerInputNV;

	GLH_EXTERN PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfvNV;
	GLH_EXTERN PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameterivNV;
	GLH_EXTERN PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfvNV;
	GLH_EXTERN PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameterivNV;
	GLH_EXTERN PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
	GLH_EXTERN PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;

	void init_GL_NV_register_combiners();
# endif // NV_register_combiners


# ifdef GLH_EXT_SINGLE_FILE

#  ifdef GL_EXT_blend_color
    void init_GL_EXT_blend_color()
	{
			glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC) wglGetProcAddress("glBlendColorEXT"); 
	}
#  endif // EXT_blend_color

#  ifdef GL_EXT_blend_subtract
	 void init_GL_EXT_blend_subtract()
	{
			glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC) wglGetProcAddress("glBlendEquationEXT"); 
	}
#  endif // EXT_blend_subtract


#  ifdef GL_EXT_secondary_color
	void init_GL_EXT_secondary_color()
	{
		glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC) wglGetProcAddress("glSecondaryColorPointerEXT");
	}
#  endif
	
#  ifdef GL_ARB_multitexture
	void init_GL_ARB_multitexture()
	{
		glActiveTextureARB         = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB   = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		glMultiTexCoord2fARB       = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord2fvARB      = (PFNGLMULTITEXCOORD2FVARBPROC) wglGetProcAddress("glMultiTexCoord2fvARB");
		glMultiTexCoord3fARB       = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord3fvARB      = (PFNGLMULTITEXCOORD3FVARBPROC) wglGetProcAddress("glMultiTexCoord3fvARB");
		glMultiTexCoord4fARB       = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glMultiTexCoord4fvARB      = (PFNGLMULTITEXCOORD4FVARBPROC) wglGetProcAddress("glMultiTexCoord4fvARB");
	}
#  endif // ARB_multitexture


# ifdef GL_NV_register_combiners
	void init_GL_NV_register_combiners()
	{
		glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC) wglGetProcAddress("glCombinerParameterfvNV");
		glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC) wglGetProcAddress("glCombinerParameterivNV");
		glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC) wglGetProcAddress("glCombinerParameterfNV");
		glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC) wglGetProcAddress("glCombinerParameteriNV");
		glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC) wglGetProcAddress("glCombinerInputNV");
		glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC) wglGetProcAddress("glCombinerOutputNV");
		glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC) wglGetProcAddress("glFinalCombinerInputNV");

		glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) wglGetProcAddress("glGetCombinerInputParameterfvNV");
		glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) wglGetProcAddress("glGetCombinerInputParameterivNV");
		glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) wglGetProcAddress("glGetCombinerOutputParameterfvNV");
		glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) wglGetProcAddress("glGetCombinerOutputParameterivNV");
		glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) wglGetProcAddress("glGetFinalCombinerInputParameterfvNV");
		glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) wglGetProcAddress("glGetFinalCombinerInputParameterivNV");
	}
# endif // NV_register_combiners



# endif // GLH_EXT_SINGLE_FILE



#else // ! WIN32

# ifdef GL_EXT_blend_color

	 void init_GL_EXT_blend_color()
#ifdef GLH_EXT_SINGLE_FILE
	{
	}
#else
	;
#endif
# endif // EXT_blend_color


# ifdef GL_EXT_blend_subtract

	 void init_GL_EXT_blend_subtract()
#ifdef GLH_EXT_SINGLE_FILE
	{
	}
#else
	;
#endif
# endif // EXT_blend_subtract

# ifdef GL_EXT_secondary_color

	 void init_GL_EXT_secondary_color()
#ifdef GLH_EXT_SINGLE_FILE
	{
	}
#else
	;
#endif
# endif

# ifdef GL_ARB_multitexture

	 void init_GL_ARB_multitexture()
#ifdef GLH_EXT_SINGLE_FILE
	{
	}
#else
	;
#endif
# endif // ARB_multitexture


# ifdef GL_NV_register_combiners

	 void init_GL_NV_register_combiners()
#ifdef GLH_EXT_SINGLE_FILE
	{
	}
#else
	;
#endif

# endif // NV_register_combiners

#endif // ! WIN32




# ifdef GL_EXT_texture_env_combine
#ifndef GLH_EXT_TEXTURE_ENV_COMBINE
#define GLH_EXT_TEXTURE_ENV_COMBINE
	struct texture_env_combine
	{
		texture_env_combine()
		{
			rgb.combine = GL_MODULATE;
			a.combine = GL_MODULATE;
			rgb.src[0] = GL_TEXTURE;
			rgb.src[1] = GL_PREVIOUS_EXT;
			rgb.src[2] = GL_CONSTANT_EXT;
			a.src[0] = GL_TEXTURE;
			a.src[1] = GL_PREVIOUS_EXT;
			a.src[2] = GL_CONSTANT_EXT;	
			rgb.op[0] = GL_SRC_COLOR;
			rgb.op[1] = GL_SRC_COLOR;
			rgb.op[2] = GL_SRC_COLOR;
			a.op[0] = GL_SRC_ALPHA;
			a.op[1] = GL_SRC_ALPHA;
			a.op[2] = GL_SRC_ALPHA;

		}
		void apply(GLenum part)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
			if(part == GL_RGB)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, rgb.combine);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, rgb.src[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, rgb.src[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, rgb.src[2]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, rgb.op[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, rgb.op[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, rgb.op[2]);
			}
			else if (part == GL_ALPHA)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, a.combine);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, a.src[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, a.src[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, a.src[2]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, a.op[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, a.op[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT, a.op[2]);
			}
		}
		struct state
		{
			GLenum combine;
			GLenum op[3];
			GLenum src[3];
			GLfloat scale;
		};
		state rgb, a;
	};
#endif
# endif


# ifdef GL_NV_texture_env_combine4
#ifndef GLH_EXT_TEXTURE_ENV_COMBINE4
#define GLH_EXT_TEXTURE_ENV_COMBINE4
	struct texture_env_combine4
	{
		texture_env_combine4()
		{
			rgb.combine = GL_MODULATE;
			a.combine = GL_MODULATE;
			rgb.src[0] = GL_TEXTURE;
			rgb.src[1] = GL_PREVIOUS_EXT;
			rgb.src[2] = GL_CONSTANT_EXT;
			rgb.src[3] = GL_ZERO;
			a.src[0] = GL_TEXTURE;
			a.src[1] = GL_PREVIOUS_EXT;
			a.src[2] = GL_CONSTANT_EXT;	
			a.src[3] = GL_ZERO;
			rgb.op[0] = GL_SRC_COLOR;
			rgb.op[1] = GL_SRC_COLOR;
			rgb.op[2] = GL_SRC_COLOR;
			rgb.op[3] = GL_ONE_MINUS_SRC_COLOR;
			a.op[0] = GL_SRC_ALPHA;
			a.op[1] = GL_SRC_ALPHA;
			a.op[2] = GL_SRC_ALPHA;
			a.op[3] = GL_ONE_MINUS_SRC_ALPHA;

		}
		void apply(GLenum part)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
			if(part == GL_RGB)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, rgb.combine);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, rgb.src[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, rgb.src[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, rgb.src[2]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, rgb.op[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, rgb.op[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, rgb.op[2]);
			}
			else if (part == GL_ALPHA)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, a.combine);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, a.src[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, a.src[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, a.src[2]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, a.op[0]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, a.op[1]);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT, a.op[2]);
			}
		}
		struct state
		{
			GLenum combine;
			GLenum op[4];
			GLenum src[4];
			GLfloat scale;
		};
		state rgb, a;
	};
# endif
# endif

#ifdef GL_NV_register_combiners

	 vec3f range_compress(const vec3f & v)
#ifdef GLH_EXT_SINGLE_FILE
	{ vec3f vret(v); vret *= .5f; vret += .5f; return vret; }
#else
	;
#endif
	 vec3f range_uncompress(const vec3f & v)
#ifdef GLH_EXT_SINGLE_FILE
	{ vec3f vret(v); vret -= .5f; vret *= 2.f; return vret; }
#else
	;
#endif
	 void combiner_parameter(GLenum param, GLfloat * v)
#ifdef GLH_EXT_SINGLE_FILE
	{ glCombinerParameterfvNV(param, v); }
#else
	;
#endif
	 void combiner_parameter(GLenum param, const vec4f & v)
#ifdef GLH_EXT_SINGLE_FILE
	{ glCombinerParameterfvNV(param, & v[0]); }
#else
	;
#endif

#ifdef GLH_EXT_SINGLE_FILE
	 void combiner_parameter(GLenum param, const vec3f & v, float a)
	{ vec4f v4(v[0], v[1], v[2], a); combiner_parameter(param, v4); }
#else
	 void combiner_parameter(GLenum param, const vec3f & v, float a=1.f);
#endif

	 void combiner_parameter(GLenum param, GLfloat f)
#ifdef GLH_EXT_SINGLE_FILE
	{ glCombinerParameterfNV(param, f); }
#else
	;
#endif

#ifndef GLH_EXT_REGISTER_COMBINERS
#define GLH_EXT_REGISTER_COMBINERS
	struct general_combiner
	{
		general_combiner()
		{
			rgb.a = variable(GL_PRIMARY_COLOR_NV);
			rgb.b = variable(    GL_TEXTURE0_ARB);
			rgb.c = variable(            GL_ZERO); 
			rgb.d = variable(            GL_ZERO); 
			a.a =  variable(GL_PRIMARY_COLOR_NV, GL_ALPHA);
			a.b =  variable(    GL_TEXTURE0_ARB, GL_ALPHA);
			a.c =  variable(            GL_ZERO, GL_ALPHA);
			a.d =  variable(            GL_ZERO, GL_ALPHA);
			rgb.ab_output = rgb.cd_output = GL_DISCARD_NV;
			rgb.sum_output = GL_SPARE0_NV;
			rgb.ab_dot_product = rgb.cd_dot_product = rgb.mux_sum = GL_FALSE;
			rgb.scale = rgb.bias = GL_NONE;
			a.ab_output = a.cd_output = GL_DISCARD_NV;
			a.sum_output = GL_SPARE0_NV;
			a.ab_dot_product = a.cd_dot_product = a.mux_sum = GL_FALSE;
			a.scale = a.bias = GL_NONE;
		}

		void apply(GLenum stage, GLenum portion)
		{
			if(portion == GL_RGB)
			{
				glCombinerInputNV(stage, GL_RGB, GL_VARIABLE_A_NV, rgb.a.input, rgb.a.mapping, rgb.a.component);
				glCombinerInputNV(stage, GL_RGB, GL_VARIABLE_B_NV, rgb.b.input, rgb.b.mapping, rgb.b.component);
				glCombinerInputNV(stage, GL_RGB, GL_VARIABLE_C_NV, rgb.c.input, rgb.c.mapping, rgb.c.component);
				glCombinerInputNV(stage, GL_RGB, GL_VARIABLE_D_NV, rgb.d.input, rgb.d.mapping, rgb.d.component);
				glCombinerOutputNV(stage, GL_RGB, rgb.ab_output, rgb.cd_output, rgb.sum_output, rgb.scale, rgb.bias,
								   rgb.ab_dot_product, rgb.cd_dot_product, rgb.mux_sum);
			}
			else if(portion == GL_ALPHA)
			{
				glCombinerInputNV(stage, GL_ALPHA, GL_VARIABLE_A_NV, a.a.input, a.a.mapping, a.a.component);
				glCombinerInputNV(stage, GL_ALPHA, GL_VARIABLE_B_NV, a.b.input, a.b.mapping, a.b.component);
				glCombinerInputNV(stage, GL_ALPHA, GL_VARIABLE_C_NV, a.c.input, a.c.mapping, a.c.component);
				glCombinerInputNV(stage, GL_ALPHA, GL_VARIABLE_D_NV, a.d.input, a.d.mapping, a.d.component);
				glCombinerOutputNV(stage, GL_ALPHA, a.ab_output, a.cd_output, a.sum_output, a.scale, a.bias,
								   GL_FALSE, GL_FALSE, a.mux_sum);
			}
		}
		void zero()
		{
			rgb.zero(variable(GL_ZERO, GL_RGB));
			a.zero(variable(GL_ZERO, GL_ALPHA));
		}

		struct variable
		{
			variable(GLenum i = GL_PRIMARY_COLOR_NV, GLenum c = GL_RGB, GLenum m = GL_UNSIGNED_IDENTITY_NV)
				: input(i), component(c), mapping(m) {}
			GLenum input, mapping, component;
		};
		struct state
		{
			void zero(const variable & v)
			{
				a = b = c = d = v;
				ab_dot_product = cd_dot_product = mux_sum = GL_FALSE;
				scale = bias = GL_NONE;
				ab_output = cd_output = sum_output = GL_DISCARD_NV;
			}
			variable a, b, c, d;
			GLenum ab_output, cd_output, sum_output;
			GLenum scale, bias;
			bool ab_dot_product;
			bool cd_dot_product;
			bool mux_sum;
		};
		state rgb, a;
	};

	struct final_combiner
	{
		final_combiner()
		{
			a = variable(GL_FOG, GL_ALPHA);
			b = variable(GL_SPARE0_PLUS_SECONDARY_COLOR_NV);
			c = variable(GL_FOG);
			d = variable();
			e = variable();
			f = variable();
			g = variable(GL_SPARE0_NV, GL_ALPHA);
		}
		void apply()
		{
			glFinalCombinerInputNV(GL_VARIABLE_A_NV, a.input, a.mapping, a.component);
			glFinalCombinerInputNV(GL_VARIABLE_B_NV, b.input, b.mapping, b.component);
			glFinalCombinerInputNV(GL_VARIABLE_C_NV, c.input, c.mapping, c.component);
			glFinalCombinerInputNV(GL_VARIABLE_D_NV, d.input, d.mapping, d.component);
			glFinalCombinerInputNV(GL_VARIABLE_E_NV, e.input, e.mapping, e.component);
			glFinalCombinerInputNV(GL_VARIABLE_F_NV, f.input, f.mapping, f.component);
			glFinalCombinerInputNV(GL_VARIABLE_G_NV, g.input, g.mapping, g.component);
		}
		void zero()
		{
			a = b = c = d = e = variable();
			g = variable(GL_ZERO, GL_ALPHA);
		}
		struct variable
		{
			variable(GLenum i = GL_ZERO, GLenum c = GL_RGB, GLenum m = GL_UNSIGNED_IDENTITY_NV)
				: input(i), component(c), mapping(m) {}
			GLenum input, mapping, component;
		};
		variable a, b, c, d, e, f, g;
	};
#endif
#endif

#undef GLH_EXT_SINGLE_FILE
}  // namespace glh
#endif
