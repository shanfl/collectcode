/* 
 * Comparison between old and new GLSL noise implementations.
 * The old implementation is my own, as posted to the GLSL
 * forum on www.opengl.org in November 2004, with some later
 * enhancements.
 * The new implementation is the one by Ian McEwan of Ashima
 * Research, in its improved and optimized version as posted
 * by me and described in a co-authored article still in review
 * as of 2011-04-09. The code was posted to the GLSL forum on
 * www.opengl.org in March 2011 and is actively maintained at
 * the Github repository http://www.github.com/ashima/webgl-noise
 *
 * Shaders are loaded from three external files:
 * "noisebench.vert", "noisebench-old.frag" and "noisebench-new.frag".
 * The program itself draws a single quad to cover a fullscreen
 * viewport and sets a noise-generated fragment color for it.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2004, 2005, 2011
 */

#define NOISEVERSION "2011-04-10"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glfw.h>

#ifdef __WIN32__
// The system level include file for GL extensions might not be up to date.
#include "GL/glext.h"
#else
#include <GL/glext.h>
#endif


#ifdef __APPLE__
// MacOS application bundles have the executable inside a directory structure
#define VERTSHADERFILE "../../../noisebench.vert"
#define FRAGSHADERFILE_CONST "../../../noisebench-const.frag"
#define FRAGSHADERFILE_TEXTURE "../../../noisebench-texture.frag"
#define FRAGSHADERFILE_OLD2DS "../../../noisebench-old2ds.frag"
#define FRAGSHADERFILE_NEW2DS "../../../noisebench-new2ds.frag"
#define FRAGSHADERFILE_OLD2DC "../../../noisebench-old2dc.frag"
#define FRAGSHADERFILE_NEW2DC "../../../noisebench-new2dc.frag"
#define FRAGSHADERFILE_OLD3DS "../../../noisebench-old3ds.frag"
#define FRAGSHADERFILE_NEW3DS "../../../noisebench-new3ds.frag"
#define FRAGSHADERFILE_OLD3DC "../../../noisebench-old3dc.frag"
#define FRAGSHADERFILE_NEW3DC "../../../noisebench-new3dc.frag"
#define FRAGSHADERFILE_OLD4DS "../../../noisebench-old4ds.frag"
#define FRAGSHADERFILE_NEW4DS "../../../noisebench-new4ds.frag"
#define FRAGSHADERFILE_OLD4DC "../../../noisebench-old4dc.frag"
#define FRAGSHADERFILE_NEW4DC "../../../noisebench-new4dc.frag"
#define LOGFILENAME "../../../noisevsnoise.log"
#else
// Windows, Linux and other Unix systems expose executables as naked files
#define VERTSHADERFILE "noisebench.vert"
#define FRAGSHADERFILE_CONST "noisebench-const.frag"
#define FRAGSHADERFILE_TEXTURE "noisebench-texture.frag"
#define FRAGSHADERFILE_OLD2DS "noisebench-old2ds.frag"
#define FRAGSHADERFILE_NEW2DS "noisebench-new2ds.frag"
#define FRAGSHADERFILE_OLD2DC "noisebench-old2dc.frag"
#define FRAGSHADERFILE_NEW2DC "noisebench-new2dc.frag"
#define FRAGSHADERFILE_OLD3DS "noisebench-old3ds.frag"
#define FRAGSHADERFILE_NEW3DS "noisebench-new3ds.frag"
#define FRAGSHADERFILE_OLD3DC "noisebench-old3dc.frag"
#define FRAGSHADERFILE_NEW3DC "noisebench-new3dc.frag"
#define FRAGSHADERFILE_OLD4DS "noisebench-old4ds.frag"
#define FRAGSHADERFILE_NEW4DS "noisebench-new4ds.frag"
#define FRAGSHADERFILE_OLD4DC "noisebench-old4dc.frag"
#define FRAGSHADERFILE_NEW4DC "noisebench-new4dc.frag"
#define LOGFILENAME "noisevsnoise.log"
#endif

#ifdef __WIN32__
/* Global function pointers for everything we need beyond OpenGL 1.1 */
PFNGLACTIVETEXTUREPROC           glActiveTexture      = NULL;
PFNGLCREATEPROGRAMPROC           glCreateProgram      = NULL;
PFNGLDELETEPROGRAMPROC           glDeleteProgram      = NULL;
PFNGLUSEPROGRAMPROC              glUseProgram         = NULL;
PFNGLCREATESHADERPROC            glCreateShader       = NULL;
PFNGLDELETESHADERPROC            glDeleteShader       = NULL;
PFNGLSHADERSOURCEPROC            glShaderSource       = NULL;
PFNGLCOMPILESHADERPROC           glCompileShader      = NULL;
PFNGLGETSHADERIVPROC             glGetShaderiv        = NULL;
PFNGLGETPROGRAMIVPROC            glGetProgramiv       = NULL;
PFNGLATTACHSHADERPROC            glAttachShader       = NULL;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog   = NULL;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog  = NULL;
PFNGLLINKPROGRAMPROC             glLinkProgram        = NULL;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation = NULL;
PFNGLUNIFORM1FVPROC              glUniform1fv         = NULL;
#endif

/*
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void printError(const char *errtype, const char *errmsg) {
  fprintf(stderr, "%s: %s\n", errtype, errmsg);
}


/*
 * Override the Win32 filelength() function with
 * a version that takes a Unix-style file handle as
 * input instead of a file ID number, and which works
 * on platforms other than Windows.
 */
long filelength(FILE *file) {
    long numbytes;
    long savedpos = ftell(file);
    fseek(file, 0, SEEK_END);
    numbytes = ftell(file);
    fseek(file, savedpos, SEEK_SET);
    return numbytes;
}


/*
 * loadExtensions() - Load OpenGL extensions for anything above OpenGL
 * version 1.1. (This is a requirement only on Windows, so on other
 * platforms, this function just checks for the required extensions.)
 */
void loadExtensions() {
    //These extension strings indicate that the OpenGL Shading Language
    // and GLSL shader objects are supported.
    if(!glfwExtensionSupported("GL_ARB_shading_language_100"))
    {
        printError("GL init error", "GL_ARB_shading_language_100 extension was not found");
        return;
    }
    if(!glfwExtensionSupported("GL_ARB_shader_objects"))
    {
        printError("GL init error", "GL_ARB_shader_objects extension was not found");
        return;
    }
    else
    {
#ifdef __WIN32__
        glActiveTexture           = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture");
        glCreateProgram           = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
        glDeleteProgram           = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
        glUseProgram              = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
        glCreateShader            = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
        glDeleteShader            = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
        glShaderSource            = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
        glCompileShader           = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
        glGetShaderiv             = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
        glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
        glAttachShader            = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
        glLinkProgram             = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
        glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
        glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
        glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
        glUniform1fv              = (PFNGLUNIFORM1FVPROC)glfwGetProcAddress("glUniform1fv");

        if( !glCreateProgram || !glDeleteProgram || !glUseProgram ||
            !glCreateShader || !glDeleteShader || !glShaderSource || !glCompileShader || 
            !glGetShaderiv || !glGetShaderInfoLog || !glAttachShader || !glLinkProgram ||
            !glGetProgramiv || !glGetProgramInfoLog || !glGetUniformLocation ||
            !glUniform1fv )
        {
            printError("GL init error", "One or more required OpenGL functions were not found");
            return;
        }
#endif
    }
}


/*
 * readShaderFile(filename) - read a shader source string from a file
 */
unsigned char* readShaderFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        printError("ERROR", "Cannot open shader file!");
  		  return 0;
    }
    int bytesinfile = filelength(file);
    unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
    int bytesread = fread( buffer, 1, bytesinfile, file);
    buffer[bytesread] = 0; // Terminate the string with 0
    fclose(file);
    
    return buffer;
}


/*
 * createShaders() - create, load, compile and link the GLSL shader objects.
 */
void createShader(GLuint *programObject, char *vertexshaderfile, char *fragmentshaderfile) {
     GLuint vertexShader;
     GLuint fragmentShader;

     const char *vertexShaderStrings[1];
     const char *fragmentShaderStrings[1];
     GLint vertexCompiled;
     GLint fragmentCompiled;
     GLint shadersLinked;
     char str[4096]; // For error messages from the GLSL compiler and linker

    // Create the vertex shader.
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    unsigned char *vertexShaderAssembly = readShaderFile( vertexshaderfile );
    vertexShaderStrings[0] = (char*)vertexShaderAssembly;
    glShaderSource( vertexShader, 1, vertexShaderStrings, NULL );
    glCompileShader( vertexShader);
    free((void *)vertexShaderAssembly);

    glGetShaderiv( vertexShader, GL_COMPILE_STATUS,
                               &vertexCompiled );
    if(vertexCompiled  == GL_FALSE)
  	{
        glGetShaderInfoLog(vertexShader, sizeof(str), NULL, str);
        printError("Vertex shader compile error", str);
  	}

  	// Create the fragment shader.
    fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    unsigned char *fragmentShaderAssembly = readShaderFile( fragmentshaderfile );
    fragmentShaderStrings[0] = (char*)fragmentShaderAssembly;
    glShaderSource( fragmentShader, 1, fragmentShaderStrings, NULL );
    glCompileShader( fragmentShader );
    free((void *)fragmentShaderAssembly);

    glGetProgramiv( fragmentShader, GL_COMPILE_STATUS, 
                               &fragmentCompiled );
    if(fragmentCompiled == GL_FALSE)
   	{
        glGetShaderInfoLog( fragmentShader, sizeof(str), NULL, str );
        printError("Fragment shader compile error", str);
    }

    // Create a program object and attach the two compiled shaders.
    *programObject = glCreateProgram();
    glAttachShader( *programObject, vertexShader );
    glAttachShader( *programObject, fragmentShader );

    // Link the program object and print out the info log.
    glLinkProgram( *programObject );
    glGetProgramiv( *programObject, GL_LINK_STATUS, &shadersLinked );

    if( shadersLinked == GL_FALSE )
	{
		glGetProgramInfoLog( *programObject, sizeof(str), NULL, str );
		printError("Program object linking error", str);
	}
}


/*
 * computeFPS() - Calculate, display and return samples per second.
 * Stats are recomputed only once per second.
 */
double computeFPS() {

    static double t0 = 0.0;
    static int frames = 0;
    static double Msamplespersecond = 0.0;
    static char titlestring[200];

    double t, fps;
    int width, height;
    
    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    // If one second has passed, or if this is the very first frame
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (double)frames / (t-t0);
        glfwGetWindowSize( &width, &height );
        // This assumes that multisampling for FSAA is disabled.
        Msamplespersecond = 1e-6*fps*width*height;
        sprintf(titlestring, "GLSL simplex noise (%.1f M samples/s)", Msamplespersecond);
        glfwSetWindowTitle(titlestring);
        printf("Speed: %.1f M samples/s\n", Msamplespersecond);
        t0 = t;
        frames = 0;
    }
    frames ++;
    return Msamplespersecond;
}


/*
 * setupCamera() - set up the OpenGL projection and (model)view matrices
 */
void setupCamera() {

    int width, height;
    
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &width, &height );
    if(height<=0) height=1; // Safeguard against iconified/closed window

    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, width, height ); // The entire window

    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    // 45 degrees FOV, same aspect ratio as viewport, depth range 1 to 100
    glOrtho( -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f );

    // Select and setup the modelview matrix.
    glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
    glLoadIdentity(); // Reset the matrix to identity
}


/*
 * initDisplayList(GLuint *listID, GLdouble scale) - create a display list
 * to render the demo geometry more efficently than by glVertex() calls.
 * (This is currently just as fast as a VBO, and I'm a bit lazy.)
 */
void initDisplayList(GLuint *listID)
{
  *listID = glGenLists(1);
  
  glNewList(*listID, GL_COMPILE);
    glColor3f(1.0f, 1.0f, 1.0f); // White base color
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-1.0f, -1.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(1.0f, -1.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-1.0f, 1.0f, 0.0f);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();
  glEndList();
}


/*
 * initPermTexture(GLuint *texID) - create and load a 2D texture for
 * a combined index permutation and gradient lookup table.
 * This texture is used for 2D and 3D noise, both classic and simplex.
 */
void initPermTexture(GLuint *texID)
{
  char *pixels;
  int i,j;

  int perm[256]= {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

  int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
                     {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
                     {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
                     {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16

  glGenTextures(1, texID); // Generate a unique texture ID
  glBindTexture(GL_TEXTURE_2D, *texID); // Bind the texture to texture unit 0

  pixels = (char*)malloc( 256*256*4 );
  for(i = 0; i<256; i++)
    for(j = 0; j<256; j++) {
      int offset = (i*256+j)*4;
      char value = perm[(j+perm[i]) & 0xFF];
      pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;   // Gradient x
      pixels[offset+1] = grad3[value & 0x0F][1] * 64 + 64; // Gradient y
      pixels[offset+2] = grad3[value & 0x0F][2] * 64 + 64; // Gradient z
      pixels[offset+3] = value;                     // Permuted index
    }
  
  // GLFW texture loading functions won't work here - we need GL_NEAREST lookup.
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

/*
 * initGradTexture(GLuint *texID) - create and load a 2D texture
 * for a 4D gradient lookup table. This is used for 4D noise only.
 */
void initGradTexture(GLuint *texID)
{
  char *pixels;
  int i,j;
  
  int perm[256]= {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

  int grad4[32][4]= {{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1}, // 32 tesseract edges
                   {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
                   {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
                   {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
                   {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
                   {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
                   {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
                   {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};
  glActiveTexture(GL_TEXTURE1); // Activate a different texture unit (unit 1)

  glGenTextures(1, texID); // Generate a unique texture ID
  glBindTexture(GL_TEXTURE_2D, *texID); // Bind the texture to texture unit 2

  pixels = (char*)malloc( 256*256*4 );
  for(i = 0; i<256; i++)
    for(j = 0; j<256; j++) {
      int offset = (i*256+j)*4;
      char value = perm[(j+perm[i]) & 0xFF];
      pixels[offset] = grad4[value & 0x1F][0] * 64 + 64;   // Gradient x
      pixels[offset+1] = grad4[value & 0x1F][1] * 64 + 64; // Gradient y
      pixels[offset+2] = grad4[value & 0x1F][2] * 64 + 64; // Gradient z
      pixels[offset+3] = grad4[value & 0x1F][3] * 64 + 64; // Gradient w
    }
  
  // GLFW texture loading functions won't work here - we need GL_NEAREST lookup.
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glActiveTexture(GL_TEXTURE0); // Switch active texture unit back to 0 again
}


/*
 * renderScene() - draw the scene with the shader active
 */
void renderScene( GLuint listID, GLuint programObject )
{
  GLint location_time = -1;
  GLint location_permTexture = -1;
  GLint location_gradTexture = -1;
  float time = 0.0f;

  // Use vertex and fragment shaders.
  glUseProgram( programObject );
  // Update the uniform time variable.
  location_time = glGetUniformLocation( programObject, "time" );
  // glUniform1f() is bugged in Linux Nvidia driver 260.19.06,
  // so we use glUniform1fv() instead to work around the bug.
  if ( location_time != -1 ) {
    time = (float)glfwGetTime();
    glUniform1fv( location_time, 1, &time );
  }
  location_permTexture = glGetUniformLocation( programObject, "permTexture" );
  // glUniform1fv() is OK to use also to set an integer value
  if ( location_permTexture != -1 ) {
    float temp = 0.0;
    glUniform1fv( location_permTexture, 1, &temp );
  }
  location_gradTexture = glGetUniformLocation( programObject, "gradTexture" );
  // glUniform1fv() is OK to use also to set an integer value
  if ( location_gradTexture != -1 ) {
    float temp = 1.0;
    glUniform1fv( location_gradTexture, 1, &temp );
  }
  // Render with the shaders active.
  glCallList( listID );
  // Deactivate the shaders.
  glUseProgram(0);
}


/*
 * main(argc, argv) - the standard C entry point for the program
 */
int main(int argc, char *argv[]) {

    GLuint displayList;
    GLuint programObject;
    GLuint permTextureID;
    GLuint gradTextureID;

    double performance = 0.0;
    int activeshader = 0; // Currently active version of noise shader
    FILE *logfile;
    GLFWvidmode vidmode;

    GLboolean running = GL_TRUE; // Main loop exits when this is set to GL_FALSE
    
    // Initialise GLFW
    glfwInit();

    // Open a temporary OpenGL window just to determine the desktop size
    if( !glfwOpenWindow(256, 256, 8,8,8,8, 32,0, GLFW_WINDOW) )
    {
        glfwTerminate(); // glfwOpenWindow failed, quit the program.
        return 1;
    }
    glfwGetDesktopMode(&vidmode);
    glfwCloseWindow();

    // Open a fullscreen window using the current desktop resolution
    if( !glfwOpenWindow(vidmode.Width, vidmode.Height, 8,8,8,8, 32,0, GLFW_FULLSCREEN) )
    {
        glfwTerminate(); // glfwOpenWindow failed, quit the program.
        return 1;
    }
    
    // Load the extensions for GLSL - note that this has to be done
    // *after* the window has been opened, or we won't have a GL context
    // to query for those extensions and connect to instances of them.
    loadExtensions();
    
    logfile = fopen(LOGFILENAME, "w");

    fprintf(logfile, "GL vendor:     %s\n", glGetString(GL_VENDOR));
    fprintf(logfile, "GL renderer:   %s\n", glGetString(GL_RENDERER));
    fprintf(logfile, "GL version:    %s\n", glGetString(GL_VERSION));
    fprintf(logfile, "Desktop size:  %d x %d pixels\n", vidmode.Width, vidmode.Height);
	fprintf(logfile, "Noise version: %s\n\n", NOISEVERSION);

    // Disable Z buffering for this simple 2D shader benchmark
    glDisable(GL_DEPTH_TEST); // Use the Z buffer

    glfwSwapInterval(0); // Do not wait for screen refresh between frames

    // Compile a display list for the demo geometry, to render it efficiently
    initDisplayList(&displayList);
    
    // Create the two textures required for the old noise implementations
    initPermTexture(&permTextureID);
    initGradTexture(&gradTextureID);
    
    // Main loop
    while(running)
    {
		double benchmarktime = 3.0; // Total time to run each shader
        // Switch between the different versions of noise
		if(activeshader == 0) {
			createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_CONST);
			activeshader++;
            fprintf(logfile, "Constant color shading, ");
		}
        if((activeshader == 1) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_TEXTURE);
			activeshader++;
            fprintf(logfile, "Single texture shading, ");
        }
        if((activeshader == 2) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
			createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_OLD2DS);
			activeshader++;
            fprintf(logfile, "Texture LUT 2D simplex noise, ");
		}
        if((activeshader == 3) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_NEW2DS);
			activeshader++;
            fprintf(logfile, "Textureless 2D simplex noise, ");
        }
        if((activeshader == 4) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_OLD2DC);
			activeshader++;
            fprintf(logfile, "Texture LUT 2D classic noise, ");
        }
        if((activeshader == 5) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_NEW2DC);
			activeshader++;
            fprintf(logfile, "Textureless 2D classic noise, ");
        }
        if((activeshader == 6) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_OLD3DS);
			activeshader++;
            fprintf(logfile, "Texture LUT 3D simplex noise, ");
        }
        if((activeshader == 7) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_NEW3DS);
			activeshader++;
            fprintf(logfile, "Textureless 3D simplex noise, ");
        }
        if((activeshader == 8) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_OLD3DC);
			activeshader++;
            fprintf(logfile, "Texture LUT 3D classic noise, ");
        }
        if((activeshader == 9) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_NEW3DC);
			activeshader++;
            fprintf(logfile, "Textureless 3D classic noise, ");
        }
        if((activeshader == 10) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_OLD4DS);
			activeshader++;
            fprintf(logfile, "Texture LUT 4D simplex noise, ");
        }
        if((activeshader == 11) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_NEW4DS);
			activeshader++;
            fprintf(logfile, "Textureless 4D simplex noise, ");
        }
        if((activeshader == 12) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_OLD4DC);
			activeshader++;
            fprintf(logfile, "Texture LUT 4D classic noise, ");
        }
        if((activeshader == 13) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            createShader(&programObject, VERTSHADERFILE, FRAGSHADERFILE_NEW4DC);
			activeshader++;
            fprintf(logfile, "Textureless 4D classic noise, ");
        }
        if((activeshader == 14) && (glfwGetTime() > benchmarktime * activeshader)) {
            fprintf(logfile, "%.1f Msamples/s\n", performance);
            running = GL_FALSE;
        }

        // Exit prematurely if the ESC key is pressed or the window is closed.
        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)) {
          running = GL_FALSE;
        }

        // Calculate and update the frames per second (FPS) display
        performance = computeFPS();

        // Do not clear the buffers - this is a raw shader benchmark.
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        // Set up the camera projection.
        setupCamera();
        
        // Draw the scene.
        renderScene(displayList, programObject);

        // Swap buffers, i.e. display the image and prepare for next frame.
        glfwSwapBuffers();
    }

    // Close the OpenGL window and terminate GLFW.
    glfwTerminate();

    fclose(logfile);

    return 0;
}
