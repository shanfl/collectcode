//------------------------------------------------------------------------------
//           Name: ogl_fbo_pbo_readback.cpp
//         Author: Kevin Harris
//  Last Modified: 01/21/2011
//    Description: This sample demonstrates how CPU based image processing can 
//                 be made more efficient by rendering off screen to a 
//                 frame-buffer object followed by an asynchronous read-back to 
//                 system memory using two alternating pixel-buffer objects.
//
//                 As a demonstration, a spinning textured cube is rendered 
//                 to a frame-buffer object, we then use two pixel buffer objects
//                 to pull the pixel data back across the bus where we can copy 
//                 it into system memory and manipulate it. We then build a 
//                 new texture from it. This new texture is then used to 
//                 texture a second spinning cube, which will be rendered to the 
//                 application's window in the regular OpenGL way.
//
//   Research:     This sample is based on research from the following links:
//
//                 http://www.comp.nus.edu.sg/~ashwinna/docs/FBO_Readback_using_PBO.pdf
//                 http://www.songho.ca/opengl/gl_pbo.html
//
//   Control Keys: Left Mouse Button  - Spin the large, black cube.
//                 Right Mouse Button - Spin the textured cube being rendered 
//                                      into the frame buffer object.
//------------------------------------------------------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>

using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>

// The Windows SDK used by Visual Studio 2008 has deprecated the glaux lib.
// Therefore, if the Visual Studio version is newer than Visual Studio 2005
// (i.e. _MSC_VER = 1400 for MSVC++ 8.0), we will have to compile and link 
// against our own private copy of glaux.
#if defined(_WIN32) && (_MSC_VER > 1400)
#include "GLAux.h"
#else
#include <GL/glaux.h>
#endif

#include "resource.h"

//------------------------------------------------------------------------------
// FUNCTION POINTERS FOR OPENGL EXTENSIONS
//------------------------------------------------------------------------------

// For convenience, this project ships with its own "glext.h" extension header 
// file. If you have trouble running this sample, it may be that this "glext.h" 
// file is defining something that your hardware doesn’t actually support. 
// Try recompiling the sample using your own local, vendor-specific "glext.h" 
// header file.

#include "glext.h"      // Sample's header file
//#include <GL/glext.h> // Your local header file

// ARB_framebuffer_object - http://www.opengl.org/registry/specs/ARB/framebuffer_object.txt
extern PFNGLISRENDERBUFFERPROC glIsRenderbuffer = NULL;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv = NULL;
extern PFNGLISFRAMEBUFFERPROC glIsFramebuffer = NULL;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D = NULL;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = NULL;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = NULL;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;

// ARB_pixel_buffer_object - http://www.opengl.org/registry/specs/ARB/pixel_buffer_object.txt
extern PFNGLGENBUFFERSPROC glGenBuffers = NULL;
extern PFNGLBINDBUFFERPROC glBindBuffer = NULL;
extern PFNGLBUFFERDATAPROC glBufferData = NULL;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
extern PFNGLMAPBUFFERPROC glMapBuffer = NULL;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------
HWND g_hWnd = NULL;
HDC g_hDC = NULL;
HGLRC g_hRC = NULL;

GLuint g_testTextureID = -1;  // A regular texture.
GLuint g_newTextureID = -1;   // A new texture built from system memory which was read-back with a PBO.

GLuint g_frameBuffer;
GLuint g_colorRenderBuffer;
GLuint g_depthRenderBuffer;

int g_nWindowWidth = 640;
int g_nWindowHeight = 480;

const int RENDERBUFFER_WIDTH = 256;
const int RENDERBUFFER_HEIGHT = 256;

//const GLenum PIXEL_FORMAT = GL_RGB;
const GLenum PIXEL_FORMAT = GL_RGBA;
//const GLenum PIXEL_FORMAT = GL_BGRA; // Why am I unable to get the FBO and PBOs to work with this format?

const GLenum PIXEL_TYPE = GL_UNSIGNED_BYTE;
//const GLenum PIXEL_TYPE = GL_UNSIGNED_INT_8_8_8_8_REV;

int g_dataSize = 0;
GLuint g_pixelBufferObjectIDs[2];
GLubyte* g_bufferData = NULL;
int g_asyncReadingIndex = 0;
int g_asynchCopyingIndex = 1;
int g_brightnessShift = 0;

float g_fSpinX_L = 0.0f;
float g_fSpinY_L = 0.0f;
float g_fSpinX_R = 0.0f;
float g_fSpinY_R = 0.0f;

struct Vertex
{
    float tu, tv;
    float x, y, z;
};

Vertex g_cubeVertices[] =
{
    { 0.0f,0.0f, -1.0f,-1.0f, 1.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 1.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, 1.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, 1.0f },
   
    { 1.0f,0.0f, -1.0f,-1.0f,-1.0f },
    { 1.0f,1.0f, -1.0f, 1.0f,-1.0f },
    { 0.0f,1.0f,  1.0f, 1.0f,-1.0f },
    { 0.0f,0.0f,  1.0f,-1.0f,-1.0f },
   
    { 0.0f,1.0f, -1.0f, 1.0f,-1.0f },
    { 0.0f,0.0f, -1.0f, 1.0f, 1.0f },
    { 1.0f,0.0f,  1.0f, 1.0f, 1.0f },
    { 1.0f,1.0f,  1.0f, 1.0f,-1.0f },
   
    { 1.0f,1.0f, -1.0f,-1.0f,-1.0f },
    { 0.0f,1.0f,  1.0f,-1.0f,-1.0f },
    { 0.0f,0.0f,  1.0f,-1.0f, 1.0f },
    { 1.0f,0.0f, -1.0f,-1.0f, 1.0f },
   
    { 1.0f,0.0f,  1.0f,-1.0f,-1.0f },
    { 1.0f,1.0f,  1.0f, 1.0f,-1.0f },
    { 0.0f,1.0f,  1.0f, 1.0f, 1.0f },
    { 0.0f,0.0f,  1.0f,-1.0f, 1.0f },
   
    { 0.0f,0.0f, -1.0f,-1.0f,-1.0f },
    { 1.0f,0.0f, -1.0f,-1.0f, 1.0f },
    { 1.0f,1.0f, -1.0f, 1.0f, 1.0f },
    { 0.0f,1.0f, -1.0f, 1.0f,-1.0f }
};

//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void redirectIOToConsole(void);
int checkGLError( const char *glCallName, const char *file, int line );
void loadTexture(void);
void init(void);
void render(void);
void shutDown(void);

#define CHECKGL_MSG( msg ) \
{ \
    if( 1 ) \
    checkGLError( #msg, __FILE__, __LINE__ ); \
}

//------------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//------------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow )
{
    redirectIOToConsole();

	WNDCLASSEX winClass; 
	MSG        uMsg;

    memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
    winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_OPENGL_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_OPENGL_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;
	
	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
		                     "OpenGL - Read-backs Using Frame Buffer Objects and Pixel Buffer Objects",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, 640, 480, NULL, NULL, hInstance, NULL );

	if( g_hWnd == NULL )
		return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow( g_hWnd );

	init();

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
        else
		    render();
	}

	shutDown();

    UnregisterClass( "MY_WINDOWS_CLASS", winClass.hInstance );

	return uMsg.wParam;
}

//------------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
	static POINT ptLastMousePosit_L;
	static POINT ptCurrentMousePosit_L;
	static bool  bMousing_L;
	
	static POINT ptLastMousePosit_R;
	static POINT ptCurrentMousePosit_R;
	static bool  bMousing_R;

    switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
			}
		}
        break;

		case WM_LBUTTONDOWN:
		{
			ptLastMousePosit_L.x = ptCurrentMousePosit_L.x = LOWORD (lParam);
            ptLastMousePosit_L.y = ptCurrentMousePosit_L.y = HIWORD (lParam);
			bMousing_L = true;
		}
		break;

		case WM_LBUTTONUP:
		{
			bMousing_L = false;
		}
		break;

		case WM_RBUTTONDOWN:
		{
			ptLastMousePosit_R.x = ptCurrentMousePosit_R.x = LOWORD (lParam);
            ptLastMousePosit_R.y = ptCurrentMousePosit_R.y = HIWORD (lParam);
			bMousing_R = true;
		}
		break;

		case WM_RBUTTONUP:
		{
			bMousing_R = false;
		}
		break;

		case WM_MOUSEMOVE:
		{
			ptCurrentMousePosit_L.x = LOWORD (lParam);
			ptCurrentMousePosit_L.y = HIWORD (lParam);
			ptCurrentMousePosit_R.x = LOWORD (lParam);
			ptCurrentMousePosit_R.y = HIWORD (lParam);

			if( bMousing_L )
			{
				g_fSpinX_L -= (ptCurrentMousePosit_L.x - ptLastMousePosit_L.x);
				g_fSpinY_L -= (ptCurrentMousePosit_L.y - ptLastMousePosit_L.y);
			}
			
			if( bMousing_R )
			{
				g_fSpinX_R -= (ptCurrentMousePosit_R.x - ptLastMousePosit_R.x);
				g_fSpinY_R -= (ptCurrentMousePosit_R.y - ptLastMousePosit_R.y);
			}

			ptLastMousePosit_L.x = ptCurrentMousePosit_L.x;
            ptLastMousePosit_L.y = ptCurrentMousePosit_L.y;
			ptLastMousePosit_R.x = ptCurrentMousePosit_R.x;
            ptLastMousePosit_R.y = ptCurrentMousePosit_R.y;
		}
		break;
		
		case WM_SIZE:
		{
			g_nWindowWidth  = LOWORD(lParam); 
			g_nWindowHeight = HIWORD(lParam);
			glViewport(0, 0, g_nWindowWidth, g_nWindowHeight);

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( 45.0, (GLdouble)g_nWindowWidth / (GLdouble)g_nWindowHeight, 0.1, 100.0);
		}
		break;
		
		case WM_CLOSE:
		{
			PostQuitMessage(0);	
		}
		break;

        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;
		
		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

int checkGLError( const char *glCallName, const char *file, int line )
{
    GLenum glError;
    int retCode = 0;

    while( (glError = glGetError()) != GL_NO_ERROR )
    {
        switch( glError )
        {
        case GL_INVALID_ENUM:
            printf( "GL_INVALID_ENUM error after call to %s in file %s at line: %d\n", glCallName, file, line );
            break;

        case GL_INVALID_VALUE:
            printf( "GL_INVALID_VALUE error after call to %s in file %s at line: %d\n", glCallName, file, line );
            break;

        case GL_INVALID_OPERATION:
            printf( "GL_INVALID_OPERATION error after call to %s in file %s at line: %d\n", glCallName, file, line );
            break;

        case GL_STACK_OVERFLOW:
            printf( "GL_STACK_OVERFLOW error after call to %s in file %s at line: %d\n", glCallName, file, line );
            break;

        case GL_STACK_UNDERFLOW:
            printf( "GL_STACK_UNDERFLOW error after call to %s in file %s at line: %d\n", glCallName, file, line );
            break;

        case GL_OUT_OF_MEMORY:
            printf( "GL_OUT_OF_MEMORY error after call to %s in file %s at line: %d\n", glCallName, file, line );
            break;

        default:
            printf( "UNKNOWN ERROR %d after call to %s in file %s at line: %d\n", glError, glCallName, file, line );
        }
    }

    return retCode;
}

//-----------------------------------------------------------------------------
// Name: redirectIOToConsole()
// Desc: 
//-----------------------------------------------------------------------------
void redirectIOToConsole( void )
{
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;

    // Allocate a console for this app
    AllocConsole();

    // Set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &coninfo );
    //coninfo.dwSize.Y = 40; // Number of console lines
    SetConsoleScreenBufferSize( GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize );

    // Redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle( STD_OUTPUT_HANDLE );
    hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // Redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle( STD_INPUT_HANDLE );
    hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // Redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle( STD_ERROR_HANDLE );
    hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

    // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
    // point to console as well.
    std::ios::sync_with_stdio();
}

//------------------------------------------------------------------------------
// Name: loadTexture()
// Desc: 
//------------------------------------------------------------------------------
void loadTexture( void )	
{
	AUX_RGBImageRec *pTextureImage = auxDIBImageLoad( ".\\test.bmp" );

    if( pTextureImage != NULL )
	{
		glGenTextures( 1, &g_testTextureID );

		glBindTexture( GL_TEXTURE_2D, g_testTextureID);

		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, pTextureImage->sizeX, pTextureImage->sizeY, 0,
			GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data );
	}

	if( pTextureImage != NULL )
	{
		if( pTextureImage->data != NULL )
			free( pTextureImage->data );

		free( pTextureImage );
	}
}

//------------------------------------------------------------------------------
// Name: init()
// Desc: 
//------------------------------------------------------------------------------
void init( void )
{
	GLuint PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 16;
    pfd.cDepthBits = 16;
	
	g_hDC = GetDC( g_hWnd );
	PixelFormat = ChoosePixelFormat( g_hDC, &pfd );
	SetPixelFormat( g_hDC, PixelFormat, &pfd);
	g_hRC = wglCreateContext( g_hDC );
	wglMakeCurrent( g_hDC, g_hRC );

	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, (GLdouble)g_nWindowWidth / g_nWindowHeight, 0.1, 100.0 );

	//
	// If the required extensions are present, get the addresses for the
	// functions that we wish to use...
	//

	//
	// ARB_framebuffer_object
	//

	char *ext = (char*)glGetString( GL_EXTENSIONS );

	if( strstr( ext, "ARB_framebuffer_object" ) == NULL )
	{
		MessageBox(NULL,"ARB_framebuffer_object extension was not found",
			"ERROR",MB_OK|MB_ICONEXCLAMATION);
		exit(-1);
	}
	else
	{
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

		if( !glIsRenderbuffer || !glBindRenderbuffer || !glDeleteRenderbuffers || 
			!glGenRenderbuffers || !glRenderbufferStorage || !glGetRenderbufferParameteriv || 
			!glIsFramebuffer || !glBindFramebuffer || !glDeleteFramebuffers || 
			!glGenFramebuffers || !glCheckFramebufferStatus || !glFramebufferTexture1D || 
			!glFramebufferTexture2D || !glFramebufferTexture3D || !glFramebufferRenderbuffer||  
			!glGetFramebufferAttachmentParameteriv || !glGenerateMipmap )
		{
			MessageBox(NULL,"One or more ARB_framebuffer_object functions were not found",
				"ERROR",MB_OK|MB_ICONEXCLAMATION);
			exit(-1);
		}
	}

    if( strstr( ext, "ARB_pixel_buffer_object" ) == NULL )
    {
        MessageBox(NULL,"ARB_pixel_buffer_object extension was not found",
            "ERROR",MB_OK|MB_ICONEXCLAMATION);
        exit(-1);
    }
    else
    {
        glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
        glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetBufferParameteriv");
        glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
        glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");

        if( !glGenBuffers || !glBindBuffer || !glBufferData || !glBufferSubData || 
            !glDeleteBuffers || !glGetBufferParameteriv || !glMapBuffer || !glUnmapBuffer )
        {
            MessageBox(NULL,"One or more ARB_pixel_buffer_object functions were not found",
                "ERROR",MB_OK|MB_ICONEXCLAMATION);
            exit(-1);
        }
    }

	//
	// Create a frame-buffer object and a render-buffer object...
	//

	glGenFramebuffers( 1, &g_frameBuffer );
    glGenRenderbuffers( 1, &g_colorRenderBuffer );
    glGenRenderbuffers( 1, &g_depthRenderBuffer );

    // Initialize the color render-buffer for usage as a color buffer.
    glBindRenderbuffer( GL_RENDERBUFFER, g_colorRenderBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, PIXEL_FORMAT, RENDERBUFFER_WIDTH, RENDERBUFFER_HEIGHT );

	// Initialize the render-buffer for usage as a depth buffer.
	// We don't really need this to render things into the frame-buffer object,
	// but without it the geometry will not be sorted properly.
	glBindRenderbuffer( GL_RENDERBUFFER, g_depthRenderBuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, RENDERBUFFER_WIDTH, RENDERBUFFER_HEIGHT );

	//
	// Check for errors...
	//

	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	switch( status )
	{
		case GL_FRAMEBUFFER_COMPLETE:
			//MessageBox(NULL,"GL_FRAMEBUFFER_COMPLETE!","SUCCESS",MB_OK|MB_ICONEXCLAMATION);
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			MessageBox(NULL,"GL_FRAMEBUFFER_UNSUPPORTED!","ERROR",MB_OK|MB_ICONEXCLAMATION);
			exit(0);
			break;

		default:
			exit(0);
	}

    //
    // Create 2 pixel buffer objects and allocate a buffer to store our pixel 
    // data in.
    //

    if( PIXEL_FORMAT == GL_RGB )
        g_dataSize = RENDERBUFFER_WIDTH * RENDERBUFFER_HEIGHT * 3;
    else if( PIXEL_FORMAT == GL_RGBA || PIXEL_FORMAT == GL_BGRA )
        g_dataSize = RENDERBUFFER_WIDTH * RENDERBUFFER_HEIGHT * 4;

    glGenBuffers( 1, &g_pixelBufferObjectIDs[0] );                       // Ask OpenGL for a valid PBO Id.
    glBindBuffer( GL_PIXEL_PACK_BUFFER, g_pixelBufferObjectIDs[0] );     // Bind to the PBO Id.
    glBufferData( GL_PIXEL_PACK_BUFFER, g_dataSize, 0, GL_STREAM_READ ); // Create the PBO.

    glGenBuffers( 1, &g_pixelBufferObjectIDs[1] );                       // Ask OpenGL for a valid PBO Id.
    glBindBuffer( GL_PIXEL_PACK_BUFFER, g_pixelBufferObjectIDs[1] );     // Bind to the PBO Id.
    glBufferData( GL_PIXEL_PACK_BUFFER, g_dataSize, 0, GL_STREAM_READ ); // Create the PBO.

    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 ); // Unbind all PBOs.

    // Allocate some system memory so we can store off the contents of our 
    // PBOs for CPU based image processing.
    g_bufferData = new GLubyte[g_dataSize];
    memset( g_bufferData, 255, g_dataSize );

    //
    // We'll create a texture that will be reloaded every frame with pixels 
    // stored in the PBO. This means it is "dynamic" in the sense that it 
    // changes, but it is not "dynamic" on the card like the dynamic texture 
    // associated with the FBO - we manually load it on the CPU. We do this as a 
    // proof of concept that we were successful in pulling the pixel data back 
    // across the bus, which it the goal of this sample.
    //

    glGenTextures( 1, &g_newTextureID );
    glBindTexture( GL_TEXTURE_2D, g_newTextureID );
    glTexImage2D( GL_TEXTURE_2D, 0, PIXEL_FORMAT, 
		          RENDERBUFFER_WIDTH, RENDERBUFFER_HEIGHT, 
		          0, PIXEL_FORMAT, PIXEL_TYPE, 0 );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );

	// Finally, load a regular texture.
	loadTexture();
}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: 
//-----------------------------------------------------------------------------
void shutDown( void )
{
    delete [] g_bufferData;
    g_bufferData = NULL;

    glDeleteTextures( 1, &g_testTextureID );
    glDeleteTextures( 1, &g_newTextureID );

	glDeleteFramebuffers( 1, &g_frameBuffer );
    glDeleteRenderbuffers( 1, &g_colorRenderBuffer );
	glDeleteRenderbuffers( 1, &g_depthRenderBuffer );

    glDeleteBuffers( 1, &g_pixelBufferObjectIDs[0] );
    glDeleteBuffers( 1, &g_pixelBufferObjectIDs[1] );

	if( g_hRC != NULL )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( g_hRC );
		g_hRC = NULL;
	}

	if( g_hDC != NULL )
	{
		ReleaseDC( g_hWnd, g_hDC );
		g_hDC = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name: doImageProcessingOnTheCPU()
// Desc: As a proof of concept, we will manipulate the image's brightness.
//-----------------------------------------------------------------------------
void doImageProcessingOnTheCPU( GLubyte* src, int width, int height, 
                                int brightnessShift, GLubyte* dst )
{
    if( !src || !dst )
        return;

    int value;

    for( int i = 0; i < height; ++i )
    {
        for( int j = 0; j < width; ++j )
        {
            // First color component...

            value = *src + brightnessShift;

            if( value > 255 )
                *dst = (GLubyte)255;
            else
                *dst = (GLubyte)value;

            ++src;
            ++dst;

            // Second color component...

            value = *src + brightnessShift;

            if( value > 255 )
                *dst = (GLubyte)255;
            else
                *dst = (GLubyte)value;

            ++src;
            ++dst;

            // Third color component...

            value = *src + brightnessShift;

            if( value > 255 )
                *dst = (GLubyte)255;
            else
                *dst = (GLubyte)value;

            ++src;
            ++dst;

            // Alpha component (optional)...

            if( PIXEL_FORMAT == GL_RGBA || PIXEL_FORMAT == GL_BGRA )
            {
                ++src;
                ++dst;
            }
        }
    }
}

//------------------------------------------------------------------------------
// Name: render()
// Desc: 
//------------------------------------------------------------------------------
void render( void )
{
    //--------------------------------------------------------------------------
    // Write to FBO...
    //--------------------------------------------------------------------------

	//
	// Bind the frame-buffer object and attach to it a render-buffer object 
	// set up as a depth-buffer.
	//

	glBindFramebuffer( GL_FRAMEBUFFER, g_frameBuffer );
    CHECKGL_MSG( "glBindFramebuffer" );

    glBindRenderbuffer( GL_RENDERBUFFER, g_colorRenderBuffer );
    CHECKGL_MSG( "glBindRenderbuffer" );
	glBindRenderbuffer( GL_RENDERBUFFER, g_depthRenderBuffer );
    CHECKGL_MSG( "glBindRenderbuffer" );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, g_colorRenderBuffer );
    CHECKGL_MSG( "glFramebufferRenderbuffer" );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_depthRenderBuffer );
    CHECKGL_MSG( "glFramebufferRenderbuffer" );

	//
	// Set up the frame-buffer object just like you would set up a window.
	//

	glViewport( 0, 0, RENDERBUFFER_WIDTH, RENDERBUFFER_HEIGHT );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//
	// Let the user spin the cube about with the right mouse button, so our 
	// dynamic texture will show motion.
	//

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0.0f, 0.0f, -5.0f );
	glRotatef( -g_fSpinY_R, 1.0f, 0.0f, 0.0f );
	glRotatef( -g_fSpinX_R, 0.0f, 1.0f, 0.0f );

	//
	// Now, render the cube to the frame-buffer object just like you we would
	// have done with a regular window.
	//

	glBindTexture( GL_TEXTURE_2D, g_testTextureID );
	glInterleavedArrays( GL_T2F_V3F, 0, g_cubeVertices );
	glDrawArrays( GL_QUADS, 0, 24 );

	//--------------------------------------------------------------------------
	// Read from FBO into PBO...
	//--------------------------------------------------------------------------

    // For each frame - swap or alternate which PBO does which job.
    if( g_asyncReadingIndex == 0 )
    {
        g_asyncReadingIndex  = 1;
        g_asynchCopyingIndex = 0;
    }
    else
    {
        g_asyncReadingIndex  = 0;
        g_asynchCopyingIndex = 1;
    }

    //
    // Start an asynchronous reading of pixels from our FBO to one of our 
    // alternating PBOs. Switching back and forth between our two PBOs, 
    // guarantees that we always have access to a PBO that has finished reading.
    //

    glBindBuffer( GL_PIXEL_PACK_BUFFER, g_pixelBufferObjectIDs[g_asyncReadingIndex] );
    CHECKGL_MSG( "glBindBuffer" );

    // If we bind a FBO and then call glReadPixels with a 0 passed for the 
    // *pixels argument - the call will return immediately and behave asynchronously!
    glReadPixels( 0, 0, RENDERBUFFER_WIDTH, RENDERBUFFER_HEIGHT, PIXEL_FORMAT, PIXEL_TYPE, 0 );
    CHECKGL_MSG( "glReadPixels" );

    //
    // While the other PBO is asynchronously reading pixels - map the other PBO 
    // and copy out its pixels so we can make a new texture from it.
    //

    glBindBuffer( GL_PIXEL_PACK_BUFFER, g_pixelBufferObjectIDs[g_asynchCopyingIndex] );
    CHECKGL_MSG( "glBindBuffer" );

    GLubyte* bufferData = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );
    CHECKGL_MSG( "glMapBufferARB" );

    if( bufferData != NULL )
    {
        //
        // As a proof of concept, copy out the FBO's contents into system memory,
        // perform some image processing on it, and then create a new texture 
        // from it.
        //

        // If we don't desire to process the image on the CPU, we can just memcpy() it.
        //memcpy( g_bufferData, bufferData, g_dataSize );

        g_brightnessShift = ++g_brightnessShift % 200;
        doImageProcessingOnTheCPU( bufferData, RENDERBUFFER_WIDTH, RENDERBUFFER_HEIGHT, g_brightnessShift, g_bufferData );

        glUnmapBuffer( GL_PIXEL_PACK_BUFFER ); // Release pointer to the mapped buffer.
        CHECKGL_MSG( "glUnmapBuffer" );

        glBindTexture( GL_TEXTURE_2D, g_newTextureID );
        glTexImage2D( GL_TEXTURE_2D, 0, PIXEL_FORMAT, RENDERBUFFER_HEIGHT, RENDERBUFFER_WIDTH, 0,
            PIXEL_FORMAT, PIXEL_TYPE, g_bufferData );
    }

    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
    CHECKGL_MSG( "glBindBuffer" );

    //
    // Unbind the frame-buffer and render-buffer objects.
    //

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    CHECKGL_MSG( "glBindFramebuffer" );

    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    CHECKGL_MSG( "glBindRenderbuffer" );

    //--------------------------------------------------------------------------
    // Now, set up the regular window for rendering...
    //--------------------------------------------------------------------------

	glViewport( 0, 0, g_nWindowWidth, g_nWindowHeight );
	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//
	// Let the user spin the cube about with the left mouse button.
	//

	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	glTranslatef( 0.0f, 0.0f, -5.0f );
    glRotatef( -g_fSpinY_L, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX_L, 0.0f, 1.0f, 0.0f );

    //
    // Finally, we'll use our new texture as a proof of concept that we were 
    // indeed successful in performing a read-back from a FBO to a PBO and then
    // manipulated it on the CPU.
    //

    glBindTexture( GL_TEXTURE_2D, g_newTextureID );

    glInterleavedArrays( GL_T2F_V3F, 0, g_cubeVertices );
    glDrawArrays( GL_QUADS, 0, 24 );

	SwapBuffers( g_hDC );
}
