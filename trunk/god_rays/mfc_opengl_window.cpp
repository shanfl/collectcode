#include "mfc_opengl_window.h"

// WGL extensions -----------------------------------------------------------------------------------------------------

// WGL_ARB_pixel_format
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

// WGL_EXT_swap_control
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

// OpenGL 21 functions and extensions ---------------------------------------------------------------------------------

// GL_VERSION_1_2
PFNGLBLENDCOLORPROC glBlendColor;
PFNGLBLENDEQUATIONPROC glBlendEquation;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

// GL_VERSION_1_3
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;

// GL_VERSION_1_4
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
PFNGLPOINTPARAMETERFPROC glPointParameterf;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
PFNGLPOINTPARAMETERIPROC glPointParameteri;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv;

// GL_VERSION_1_5
PFNGLGENQUERIESPROC glGenQueries;
PFNGLDELETEQUERIESPROC glDeleteQueries;
PFNGLISQUERYPROC glIsQuery;
PFNGLBEGINQUERYPROC glBeginQuery;
PFNGLENDQUERYPROC glEndQuery;
PFNGLGETQUERYIVPROC glGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;

// GL_VERSION_2_0
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC glGetShaderSource;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMFVPROC glGetUniformfv;
PFNGLGETUNIFORMIVPROC glGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
PFNGLISPROGRAMPROC glIsProgram;
PFNGLISSHADERPROC glIsShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

// GL_VERSION_2_1
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

// GL_ARB_framebuffer_object
PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;

//GL_EXT_framebuffer_object
PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT;

// Global variables ---------------------------------------------------------------------------------------------------

char ModuleDirectory[MAX_PATH];

int gl_version = 0, gl_max_texture_size = 0, gl_max_texture_max_anisotropy_ext = 0;
bool gl_arb_texture_non_power_of_two = false, gl_arb_multisample = false, gl_arb_framebuffer_object = false, gl_ext_framebuffer_object = false;

// CShaderProgram class -----------------------------------------------------------------------------------------------

CShaderProgram::CShaderProgram()
{
	VertexShader = 0;
	FragmentShader = 0;
	Program = 0;
}

CShaderProgram::~CShaderProgram()
{
}

CShaderProgram::operator GLuint ()
{
	return Program;
}

void CShaderProgram::Delete()
{
	glDetachShader(Program, VertexShader);
	glDetachShader(Program, FragmentShader);
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	glDeleteProgram(Program);
	VertexShader = FragmentShader = Program = 0;
}

bool CShaderProgram::Load(char *VertexShaderFileName, char *FragmentShaderFileName, CString *Error)
{
	bool error = false;

	error |= ((VertexShader = LoadShader(GL_VERTEX_SHADER, VertexShaderFileName, Error)) == 0);

	error |= ((FragmentShader = LoadShader(GL_FRAGMENT_SHADER, FragmentShaderFileName, Error)) == 0);

	if(error == true)
	{
		Delete();
		return false;
	}

	Program = glCreateProgram();
	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);
	glLinkProgram(Program);

	int Param = 0;
	glGetProgramiv(Program, GL_LINK_STATUS, &Param);

	if(Param == GL_FALSE)
	{
		Error->AppendFormat("\r\nError linking program (%s, %s)!", VertexShaderFileName, FragmentShaderFileName);

		int InfoLogLength = 0;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
		if(InfoLogLength > 0)
		{
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten  = 0;
			glGetProgramInfoLog(Program, InfoLogLength, &CharsWritten, InfoLog);
			Error->AppendFormat("\r\n%s", InfoLog);
			delete [] InfoLog;
		}

		Delete();

		return false;
	}

	return true;
}

GLuint CShaderProgram::LoadShader(GLenum Type, char *ShaderFileName, CString *Error)
{
	FILE *File;

	char FileName[MAX_PATH];

	strcpy_s(FileName, MAX_PATH, ModuleDirectory);
	//strcat_s(FileName, MAX_PATH, "Shaders\\");
	strcat_s(FileName, MAX_PATH, ShaderFileName);

	if(fopen_s(&File, FileName, "rb") != 0)
	{
		Error->AppendFormat("\r\nError opening file %s!", FileName);
		return 0;
	}

	fseek(File, 0, SEEK_END);
	long Size = ftell(File);
	fseek(File, 0, SEEK_SET);
	char *Source = new char[Size + 1];
	fread(Source, 1, Size, File);
	fclose(File);
	Source[Size] = 0;

	GLuint Shader;

	Shader = glCreateShader(Type);
	glShaderSource(Shader, 1, (const char**)&Source, NULL);
	delete [] Source;
	glCompileShader(Shader);

	int Param = 0;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &Param);

	if(Param == GL_FALSE)
	{
		Error->AppendFormat("\r\nError compiling shader %s!", ShaderFileName);

		int InfoLogLength = 0;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
		if(InfoLogLength > 0)
		{
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten  = 0;
			glGetShaderInfoLog(Shader, InfoLogLength, &CharsWritten, InfoLog);
			Error->AppendFormat("\r\n%s", InfoLog);
			delete [] InfoLog;
		}

		glDeleteShader(Shader);

		return 0;
	}

	return Shader;
}

// COpenGLRenderer class ----------------------------------------------------------------------------------------------

COpenGLRenderer::COpenGLRenderer()
{
	MSAAPixelFormat = Samples = 0;
}

COpenGLRenderer::~COpenGLRenderer()
{
}

bool COpenGLRenderer::ExtensionSupported(char *Extension)
{
	return (strstr((char*)glGetString(GL_EXTENSIONS), Extension) != NULL);
}

bool COpenGLRenderer::InitMultiSampleAntiAliasing(int Samples)
{
	if(!gl_arb_multisample || wglChoosePixelFormatARB == NULL)
		return false;

	CDC *cDC = Wnd->GetDC();

	if(cDC == NULL)
		return false;

	while(Samples > 0)
	{
		UINT NumFormats = 0;

		int iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, Samples,
			0
		};
			
		if(wglChoosePixelFormatARB(*cDC, iAttributes, NULL, 1, &MSAAPixelFormat, &NumFormats) == TRUE && NumFormats >  0)
		{
			this->Samples = Samples;
			return true;
		}

		Samples--;
	}

	return false;
}

bool COpenGLRenderer::InitOpenGL(CWnd *Wnd)
{
	if(Wnd == NULL)
	{
		MessageBox(NULL, "InitOpenGL error: Wnd is NULL!", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	this->Wnd = Wnd;

	CDC *cDC = Wnd->GetDC();

	if(cDC == NULL)
	{
		Wnd->MessageBox("InitOpenGL error: cDC is NULL!", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int PixelFormat;

	if((PixelFormat = ChoosePixelFormat(*cDC, &pfd)) == 0)
	{
		Wnd->MessageBox("InitOpenGL error: ChoosePixelFormat failed!", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if(SetPixelFormat(*cDC, MSAAPixelFormat == 0 ? PixelFormat : MSAAPixelFormat, &pfd) == FALSE)
	{
		Wnd->MessageBox("InitOpenGL error: SetPixelFormat failed!", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if((hGLRC = wglCreateContext(*cDC)) == NULL)
	{
		Wnd->MessageBox("InitOpenGL error: wglCreateContext failed!", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if(wglMakeCurrent(*cDC, hGLRC) == FALSE)
	{
		Wnd->MessageBox("InitOpenGL error: wglMakeCurrent failed!", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	glBlendColor = (PFNGLBLENDCOLORPROC)wglGetProcAddress("glBlendColor");
	glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");
	glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)wglGetProcAddress("glDrawRangeElements");
	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
	glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");
	glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)wglGetProcAddress("glCopyTexSubImage3D");

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)wglGetProcAddress("glSampleCoverage");
	glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)wglGetProcAddress("glCompressedTexImage3D");
	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress("glCompressedTexImage2D");
	glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)wglGetProcAddress("glCompressedTexImage1D");
	glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)wglGetProcAddress("glCompressedTexSubImage3D");
	glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)wglGetProcAddress("glCompressedTexSubImage2D");
	glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)wglGetProcAddress("glCompressedTexSubImage1D");
	glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)wglGetProcAddress("glGetCompressedTexImage");

	glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)wglGetProcAddress("glBlendFuncSeparate");
	glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)wglGetProcAddress("glMultiDrawArrays");
	glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)wglGetProcAddress("glMultiDrawElements");
	glPointParameterf = (PFNGLPOINTPARAMETERFPROC)wglGetProcAddress("glPointParameterf");
	glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress("glPointParameterfv");
	glPointParameteri = (PFNGLPOINTPARAMETERIPROC)wglGetProcAddress("glPointParameteri");
	glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)wglGetProcAddress("glPointParameteriv");

	glGenQueries = (PFNGLGENQUERIESPROC)wglGetProcAddress("glGenQueries");
	glDeleteQueries = (PFNGLDELETEQUERIESPROC)wglGetProcAddress("glDeleteQueries");
	glIsQuery = (PFNGLISQUERYPROC)wglGetProcAddress("glIsQuery");
	glBeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQuery");
	glEndQuery = (PFNGLENDQUERYPROC)wglGetProcAddress("glEndQuery");
	glGetQueryiv = (PFNGLGETQUERYIVPROC)wglGetProcAddress("glGetQueryiv");
	glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)wglGetProcAddress("glGetQueryObjectiv");
	glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuiv");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glIsBuffer = (PFNGLISBUFFERPROC)wglGetProcAddress("glIsBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)wglGetProcAddress("glGetBufferSubData");
	glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
	glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetBufferParameteriv");
	glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)wglGetProcAddress("glGetBufferPointerv");

	glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)wglGetProcAddress("glBlendEquationSeparate");
	glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
	glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)wglGetProcAddress("glStencilOpSeparate");
	glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)wglGetProcAddress("glStencilFuncSeparate");
	glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)wglGetProcAddress("glStencilMaskSeparate");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttrib");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");
	glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress("glGetAttachedShaders");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)wglGetProcAddress("glGetShaderSource");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glGetUniformfv = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfv");
	glGetUniformiv = (PFNGLGETUNIFORMIVPROC)wglGetProcAddress("glGetUniformiv");
	glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)wglGetProcAddress("glGetVertexAttribdv");
	glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)wglGetProcAddress("glGetVertexAttribfv");
	glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)wglGetProcAddress("glGetVertexAttribiv");
	glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)wglGetProcAddress("glGetVertexAttribPointerv");
	glIsProgram = (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgram");
	glIsShader = (PFNGLISSHADERPROC)wglGetProcAddress("glIsShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform2i = (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
	glUniform3i = (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i");
	glUniform4i = (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
	glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)wglGetProcAddress("glVertexAttrib1d");
	glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)wglGetProcAddress("glVertexAttrib1dv");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)wglGetProcAddress("glVertexAttrib1s");
	glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)wglGetProcAddress("glVertexAttrib1sv");
	glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)wglGetProcAddress("glVertexAttrib2d");
	glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)wglGetProcAddress("glVertexAttrib2dv");
	glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)wglGetProcAddress("glVertexAttrib2f");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)wglGetProcAddress("glVertexAttrib2s");
	glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)wglGetProcAddress("glVertexAttrib2sv");
	glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)wglGetProcAddress("glVertexAttrib3d");
	glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)wglGetProcAddress("glVertexAttrib3dv");
	glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)wglGetProcAddress("glVertexAttrib3f");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)wglGetProcAddress("glVertexAttrib3s");
	glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)wglGetProcAddress("glVertexAttrib3sv");
	glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)wglGetProcAddress("glVertexAttrib4Nbv");
	glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)wglGetProcAddress("glVertexAttrib4Niv");
	glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)wglGetProcAddress("glVertexAttrib4Nsv");
	glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)wglGetProcAddress("glVertexAttrib4Nub");
	glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)wglGetProcAddress("glVertexAttrib4Nubv");
	glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)wglGetProcAddress("glVertexAttrib4Nuiv");
	glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)wglGetProcAddress("glVertexAttrib4Nusv");
	glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)wglGetProcAddress("glVertexAttrib4bv");
	glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)wglGetProcAddress("glVertexAttrib4d");
	glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)wglGetProcAddress("glVertexAttrib4dv");
	glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)wglGetProcAddress("glVertexAttrib4f");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)wglGetProcAddress("glVertexAttrib4iv");
	glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)wglGetProcAddress("glVertexAttrib4s");
	glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)wglGetProcAddress("glVertexAttrib4sv");
	glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)wglGetProcAddress("glVertexAttrib4ubv");
	glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)wglGetProcAddress("glVertexAttrib4uiv");
	glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)wglGetProcAddress("glVertexAttrib4usv");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

	glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)wglGetProcAddress("glUniformMatrix2x3fv");
	glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)wglGetProcAddress("glUniformMatrix3x2fv");
	glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)wglGetProcAddress("glUniformMatrix2x4fv");
	glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)wglGetProcAddress("glUniformMatrix4x2fv");
	glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)wglGetProcAddress("glUniformMatrix3x4fv");
	glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)wglGetProcAddress("glUniformMatrix4x3fv");

	glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
	glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
	glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
	glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
	glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
	glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
	glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");
	glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)wglGetProcAddress("glRenderbufferStorageMultisample");
	glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)wglGetProcAddress("glFramebufferTextureLayer");

	glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

	gl_arb_texture_non_power_of_two = ExtensionSupported("GL_ARB_texture_non_power_of_two");
	gl_arb_multisample = ExtensionSupported("GL_ARB_multisample");
	gl_arb_framebuffer_object = ExtensionSupported("GL_ARB_framebuffer_object");
	gl_ext_framebuffer_object = ExtensionSupported("GL_EXT_framebuffer_object");

	if(!gl_arb_framebuffer_object && gl_ext_framebuffer_object)
	{
		glIsRenderbuffer = glIsRenderbufferEXT;
		glBindRenderbuffer = glBindRenderbufferEXT;
		glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
		glGenRenderbuffers = glGenRenderbuffersEXT;
		glRenderbufferStorage = glRenderbufferStorageEXT;
		glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
		glIsFramebuffer = glIsFramebufferEXT;
		glBindFramebuffer = glBindFramebufferEXT;
		glDeleteFramebuffers = glDeleteFramebuffersEXT;
		glGenFramebuffers = glGenFramebuffersEXT;
		glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
		glFramebufferTexture1D = glFramebufferTexture1DEXT;
		glFramebufferTexture2D = glFramebufferTexture2DEXT;
		glFramebufferTexture3D = glFramebufferTexture3DEXT;
		glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
		glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
		glGenerateMipmap = glGenerateMipmapEXT;
	}

	char *version = (char*)glGetString(GL_VERSION);

	int major, minor;

	sscanf_s(version, "%d.%d", &major, &minor);

	gl_version = major * 10 + minor;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);

	if(wglSwapIntervalEXT != NULL) wglSwapIntervalEXT(0);

	return Init();
}

bool COpenGLRenderer::LoadTexture(char *TextureFileName, GLuint *TextureID, CString *Error)
{
	if(TextureFileName == NULL || TextureID == NULL)
	{
		Error->AppendFormat("\r\nError loading texture %s: FileName or TexureID is NULL!", TextureFileName);
		return false;
	}

	CDC *cDC = Wnd->GetDC();
	
	if(cDC == NULL)
	{
		Error->AppendFormat("\r\nError loading texture %s: cDC is NULL!", TextureFileName);
		return false;
	}

	char *FileType = TextureFileName + strlen(TextureFileName) - 3;

	_strlwr_s(FileType, strlen(FileType) + 1);

	if(strcmp(FileType, "bmp") != 0 && strcmp(FileType, "jpg") != 0  && strcmp(FileType, "gif") != 0 )
	{
		Error->AppendFormat("\r\nError loading texture %s: Unsupported file type!", TextureFileName);
		return false;
	}
	
	HDC hdcTemp;
	HBITMAP hbmpTemp;
	IPicture *pPicture;
	OLECHAR wszPath[MAX_PATH + 1];
	long lWidth, lHeight, lWidthPixels, lHeightPixels;

	char FileName[MAX_PATH];

	strcpy_s(FileName, MAX_PATH, ModuleDirectory);
	//strcat_s(FileName, MAX_PATH, "Textures\\");
	strcat_s(FileName, MAX_PATH, TextureFileName);

	MultiByteToWideChar(CP_ACP, 0, FileName, -1, wszPath, MAX_PATH);

	if(FAILED(OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture)))
	{
		Error->AppendFormat("\r\nError loading texture %s: OleLoadPicturePath failed!", FileName);
		return false;
	}

	if((hdcTemp = CreateCompatibleDC(*cDC)) == NULL)
	{
		pPicture->Release();
		Error->AppendFormat("\r\nError loading texture %s: hdcTemp is NULL!", TextureFileName);
		return false;
	}

	pPicture->get_Width(&lWidth);
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	if(lWidthPixels > gl_max_texture_size) lWidthPixels = gl_max_texture_size;
	if(lHeightPixels > gl_max_texture_size) lHeightPixels = gl_max_texture_size;

	if(!gl_arb_texture_non_power_of_two)
	{
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	}
	
	BITMAPINFO bi = {0};
	DWORD *pBits = 0;

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biWidth = lWidthPixels;
	bi.bmiHeader.biHeight = lHeightPixels;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biPlanes = 1;

	if((hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0)) == NULL)
	{
		DeleteDC(hdcTemp);
		pPicture->Release();
		Error->AppendFormat("\r\nError loading texture %s: hbmpTemp is NULL!", TextureFileName);
		return false;
	}

	SelectObject(hdcTemp, hbmpTemp);

	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	if(gl_version < 12)
	{
		BYTE* pPixel;
		BYTE temp;

		for(long i = 0; i < lWidthPixels * lHeightPixels; i++)
		{
			pPixel = (BYTE*)(&pBits[i]);
			temp = pPixel[0];
			pPixel[0] = pPixel[2];
			pPixel[2] = temp;
			pPixel[3] = 255;
		}
	}

	glGenTextures(1, TextureID);
	glBindTexture(GL_TEXTURE_2D, *TextureID);
	if(gl_version >= 14) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
	if(gl_version >= 14) glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	if(gl_version >= 12) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, lWidthPixels, lHeightPixels, 0, GL_BGRA, GL_UNSIGNED_BYTE, pBits);
	else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);
	glBindTexture(GL_TEXTURE_2D, 0);

	DeleteObject(hbmpTemp);
	DeleteDC(hdcTemp);

	pPicture->Release();

	return true;
}

bool COpenGLRenderer::Init()
{
	if(gl_version < 21)
	{
		Wnd->MessageBox("OpenGL 2.1 not supported!", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	CString csError = "Errors:\r\n";
	bool bError = false;

	if(!gl_arb_texture_non_power_of_two)
	{
		csError.Append("\r\nGL_ARB_texture_non_power_of_two not supported!");
		bError = true;
	}

	if(!ExtensionSupported("GL_ARB_depth_texture"))
	{
		csError.Append("\r\nGL_ARB_depth_texture not supported!");
		bError = true;
	}

	if(!gl_arb_framebuffer_object && !gl_ext_framebuffer_object)
	{
		csError.Append("GL_EXT_framebuffer_object not supported!");
		bError = true;
	}

	bError |= !PerPixelLighting.Load("per_pixel_lighting.vs", "per_pixel_lighting.fs", &csError);

	bError |= !GodRays.Load("god_rays.vs", "god_rays.fs", &csError);

	if(bError)
	{
		Wnd->MessageBox(csError, "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	Stop = false;
	Blur = true;
	WireFrame = false;

	View = ViewMatrix(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 100.0f));

	// generating helix vertices and normals

	int twists = 5, ures = 64, vres = 32;
	float radius = 5.0f, thickness = 5.0f, length = 45.0f;

	VerticesCount = vres * ures * twists * 4;

	vec3 *Vertices = new vec3[VerticesCount];
	vec3 *Normals = new vec3[VerticesCount];

	float uinc = M_PI * 2.0f / ures, vinc = M_PI * 2.0f / vres, zinc = length / twists / ures;

	vec3 z = vec3(0.0f, 0.0f, 1.0f);
	vec3 a = vec3(radius, 0.0f, 0.0f), b = rotate(a, z, uinc), c = rotate(b, z, uinc);
	vec3 x1 = vec3(thickness / 2.0f, 0.0f, 0.0f), x2 = rotate(x1, z, uinc);

	float az = -length / 2.0f, bz = az + zinc, cz = bz + zinc;

	int vertex = 0;

	for(float u = 0.0f; u < 360.0f * twists; u += 360.0f / ures)
	{
		vec3 m1 = vec3(a.x, a.y, az);
		vec3 m2 = vec3(b.x, b.y, bz);
		vec3 m3 = vec3(c.x, c.y, cz);

		vec3 y1 = normalize(m2 - m1);
		vec3 y2 = normalize(m3 - m2);

		for(float v = 0.0f; v < 360.0f; v += 360.0f / vres)
		{
			vec3 x1a = x1, x1b = rotate(x1, y1, vinc), x2a = x2, x2b = rotate(x2, y2, vinc);

			Normals[vertex] = normalize(x1a); Vertices[vertex] = m1 + x1a; vertex++;
			Normals[vertex] = normalize(x1b); Vertices[vertex] = m1 + x1b; vertex++;
			Normals[vertex] = normalize(x2b); Vertices[vertex] = m2 + x2b; vertex++;
			Normals[vertex] = normalize(x2a); Vertices[vertex] = m2 + x2a; vertex++;

			x1 = x1b; x2 = x2b;
		}

		x1 = rotate(x1, z, uinc);
		x2 = rotate(x2, z, uinc);

		a = b; az = bz;
		b = c; bz = cz;
		c = rotate(c, z, uinc); cz += zinc;
	}

	glGenBuffers(1, &VerticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, VerticesVBO);
	glBufferData(GL_ARRAY_BUFFER, VerticesCount * 3 * 4, Vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &NormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, VerticesCount * 3 * 4, Normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete [] Vertices;
	delete [] Normals;

	glGenTextures(1, &ColorBuffer);
	glGenTextures(1, &DepthBuffer);

	glGenFramebuffers(1, &FBO);

	return true;
}

void COpenGLRenderer::Render(float FrameTime)
{
	if(Blur) glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&View);

	glColor3f(1.0f, 1.0f, 1.0f);

	static float a = 0.0f, b = 0.0f, c = 7.5f;

	glTranslatef(0.0f, 0.0f, b);
	glRotatef(a, 0.0f, 1.0f, 0.0f);
	glRotatef(a * 2.0f, 1.0f, 0.0f, 0.0f);

	if(!Stop)
	{
		a += 30.0f * FrameTime;
		b += c * FrameTime;
		if(b <= -75.0f){ b = -75.0f; c *= -1.0f; }
		if(b >= 75.0f){ b = 75.0f; c *= -1.0f; }
	}

	glEnable(GL_DEPTH_TEST);

	if(WireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, VerticesVBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
	glNormalPointer(GL_FLOAT, 0, NULL);
	
	glUseProgram(PerPixelLighting);
	glColor3f(0.4f, 0.2f, 0.8f);
	glDrawArrays(GL_QUADS, 0, VerticesCount);
	glUseProgram(0);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	if(WireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glDisable(GL_DEPTH_TEST);

	if(Blur)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, ColorBuffer);

		glUseProgram(GodRays);

		glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glVertex2f(1.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);
		glEnd();

		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void COpenGLRenderer::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;

	glViewport(0, 0, Width, Height);

	Projection = PerspectiveProjectionMatrix(45.0f, Width, Height, 0.125f, 512.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&Projection);

	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void COpenGLRenderer::Destroy()
{
	if(gl_version >= 21)
	{
		PerPixelLighting.Delete();
		GodRays.Delete();

		glDeleteBuffers(1, &VerticesVBO);
		glDeleteBuffers(1, &NormalsVBO);
	}

	glDeleteTextures(1, &ColorBuffer);
	glDeleteTextures(1, &DepthBuffer);

	if(gl_arb_framebuffer_object || gl_ext_framebuffer_object)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hGLRC);
}

// CMyWnd class -------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMyWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

bool CMyWnd::InitMultiSampleAntiAliasing(int Samples)
{
	return OpenGLRenderer.InitMultiSampleAntiAliasing(Samples);
}

int CMyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetWindowText(WindowText);

	if(!OpenGLRenderer.InitOpenGL(this))
		return -1;

	return 0;
}

void CMyWnd::OnDestroy()
{
	OpenGLRenderer.Destroy();
}

void CMyWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
		case VK_F1:
			OpenGLRenderer.Blur = !OpenGLRenderer.Blur;
			break;

		case VK_F2:
			OpenGLRenderer.WireFrame = !OpenGLRenderer.WireFrame;
			break;

		case VK_SPACE:
			OpenGLRenderer.Stop = !OpenGLRenderer.Stop;
			break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyWnd::OnPaint()
{
	CPaintDC dc(this);

	static DWORD Start = GetTickCount(), Begin = Start;
	static int FPS = 0;

	DWORD End = GetTickCount();

	float FrameTime = (End - Begin) * 0.001f;
	Begin = End;

	if(End - Start > 1000)
	{
		CString Text = WindowText;
		Text.AppendFormat(" - OpenGL %d.%d - %dx%d - MSAA %dx - ATF %dx - FPS: %d - %s", gl_version / 10, gl_version % 10, Width, Height, OpenGLRenderer.Samples, gl_max_texture_max_anisotropy_ext, FPS, glGetString(GL_RENDERER));
		SetWindowText(Text);
		Start = End;
		FPS = 0;
	}
	else
	{
		FPS++;
	}

	OpenGLRenderer.Render(FrameTime);

	SwapBuffers(dc);

	Invalidate(FALSE);
}

void CMyWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	Width = cx;
	Height = cy;

	OpenGLRenderer.Resize(Width, Height);
}

// CMyWinApp class ----------------------------------------------------------------------------------------------------

BOOL CMyWinApp::InitInstance()
{
	GetModuleFileName(GetModuleHandle(NULL), ModuleDirectory, MAX_PATH);
	*(strrchr(ModuleDirectory, '\\') + 1) = 0;

	LPCTSTR WindowClassName = AfxRegisterWndClass(CS_OWNDC | CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), 0, ::LoadIcon(NULL, IDI_APPLICATION));

	char *WindowName = "God rays";
	DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	int Width = 800, Height = 600;

	if(Wnd.CreateEx(WS_EX_APPWINDOW, WindowClassName, WindowName, Style, 0, 0, Width, Height, NULL, 0) == FALSE)
		return FALSE;

	/*if(Wnd.InitMultiSampleAntiAliasing(4))
	{
		Wnd.DestroyWindow();

		if(Wnd.CreateEx(WS_EX_APPWINDOW, WindowClassName, WindowName, Style, 0, 0, Width, Height, NULL, 0) == FALSE)
			return FALSE;
	}*/

	RECT dRect, wRect, cRect;

	GetWindowRect(GetDesktopWindow(), &dRect);
	Wnd.GetWindowRect(&wRect);
	Wnd.GetClientRect(&cRect);

	wRect.right += Width - cRect.right;
	wRect.bottom += Height - cRect.bottom;
	
	wRect.right -= wRect.left;
	wRect.bottom -= wRect.top;

	wRect.left = dRect.right / 2 - wRect.right / 2;
	wRect.top = dRect.bottom / 2 - wRect.bottom / 2;

	Wnd.MoveWindow(wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

	Wnd.ShowWindow(SW_SHOWNORMAL);

	m_pMainWnd = &Wnd;

	return TRUE;
}

CMyWinApp Application;
