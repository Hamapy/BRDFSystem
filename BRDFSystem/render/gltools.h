#ifndef __GLTOOLS__LIBRARY
#define __GLTOOLS__LIBRARY

#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include "glee.h"			// OpenGL Extension "autoloader"
#include <gl\gl.h>			// Microsoft OpenGL headers (version 1.1 by themselves)
#include <gl\glu.h>			// OpenGL Utilities
//#include <GL/glut.h>		// Glut (Free-Glut on Windows)
#include "glut.h"

#ifndef WIN32
typedef GLvoid (*CallBack)(...);            // Visual C++ style
#else
typedef GLvoid (_stdcall *CallBack)();      // XCode (GNU) style
#endif

#include <math.h>

// There is a static block allocated for loading shaders to prevent heap fragmentation
#define MAX_SHADER_LENGTH   8192

#define LITTLE_ENDIAN_WORD 
#define LITTLE_ENDIAN_DWORD


//THE LIBRARY....

// Load a .TGA file
GLbyte *gltLoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat);

// Capute the frame buffer and write it as a .tga
GLint gltWriteTGA(const char *szFileName);

// Draw a Torus
void gltDrawTorus(GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);

// Just draw a simple sphere with normals and texture coordinates
void gltDrawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks);

// Draw a 3D unit Axis set
void gltDrawUnitAxes(void);

// Shader loading support
bool bLoadShaderFile(const char *szFile, GLhandleARB shader);
GLhandleARB gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg);

// Get the OpenGL version, returns fals on error
bool gltGetOpenGLVersion(int &nMajor, int &nMinor);

// Check to see if an exension is supported
int gltIsExtSupported(const char *szExtension);

// Get the function pointer for an extension
void *gltGetExtensionPointer(const char *szFunctionName);

//Win32 Only
#ifdef WIN32
int gltIsWGLExtSupported(HDC hDC, const char *szExtension);
#endif


#endif