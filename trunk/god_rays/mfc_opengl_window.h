#include "glmath.h"

#include <gl/gl.h>

#include <gl/glext.h> // http://www.opengl.org/registry/#headers
#include <gl/wglext.h> // http://www.opengl.org/registry/#headers

#pragma comment(lib, "opengl32.lib")

// CShaderProgram class -----------------------------------------------------------------------------------------------

class CShaderProgram
{
protected:
	GLuint VertexShader, FragmentShader, Program;

public:
	CShaderProgram();
	~CShaderProgram();

	operator GLuint ();

	void Delete();
	bool Load(char *VertexShaderFileName, char *FragmentShaderFileName, CString *Error);

protected:
	GLuint LoadShader(GLenum Type, char *ShaderFileName, CString *Error);
};

// COpenGLRenderer class ----------------------------------------------------------------------------------------------

class COpenGLRenderer
{
protected:
	CWnd *Wnd;
	HGLRC hGLRC;
	int Width, Height;
	int MSAAPixelFormat;

	mat4x4 Projection, View;

	GLuint ColorBuffer, DepthBuffer, FBO;
	GLuint VerticesVBO, NormalsVBO, VerticesCount;

	CShaderProgram PerPixelLighting, GodRays;

public:
	int Samples;
	bool Stop, Blur, WireFrame;

	COpenGLRenderer();
	~COpenGLRenderer();

	bool ExtensionSupported(char *Extension);
	bool InitMultiSampleAntiAliasing(int Samples);
	bool InitOpenGL(CWnd *Wnd);
	bool LoadTexture(char *TextureFileName, GLuint *TextureID, CString *Error);

	bool Init();
	void Render(float FrameTime);
	void Resize(int Width, int Height);
	void Destroy();
};

// CMyWnd class -------------------------------------------------------------------------------------------------------

class CMyWnd : public CWnd
{
protected:
	CString WindowText;
	COpenGLRenderer OpenGLRenderer;
	int Width, Height;

public:
	bool InitMultiSampleAntiAliasing(int Samples);

DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

// CMyWinApp class ----------------------------------------------------------------------------------------------------

class CMyWinApp : public CWinApp
{
protected:
	CMyWnd Wnd;

public:
	virtual BOOL InitInstance();
};
