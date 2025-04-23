// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include "FOX_sysvideo.h"


struct FOX_PrivateGLData {
    int gl_active; 

#ifdef HAVE_OPENGL
    PIXELFORMATDESCRIPTOR GL_pfd;
    HDC GL_hdc;
    HGLRC GL_hrc;

    void * (WINAPI *wglGetProcAddress)(const char *proc);

    HGLRC (WINAPI *wglCreateContext)(HDC hdc);

    BOOL (WINAPI *wglDeleteContext)(HGLRC hglrc);

    BOOL (WINAPI *wglMakeCurrent)(HDC hdc, HGLRC hglrc);

#endif /* HAVE_OPENGL */
};

// stare nazwy zmiennych

#define gl_active	(this->gl_data->gl_active)
#define GL_pfd		(this->gl_data->GL_pfd)
#define GL_hdc		(this->gl_data->GL_hdc)
#define GL_hrc		(this->gl_data->GL_hrc)

// funkcje opengl

extern int WIN_GL_SetupWindow(_THIS);
extern void WIN_GL_ShutDown(_THIS);

#ifdef HAVE_OPENGL
extern int WIN_GL_MakeCurrent(_THIS);
extern int WIN_GL_GetAttribute(_THIS, FOX_GLattr attrib, int* value);
extern void WIN_GL_SwapBuffers(_THIS);
extern void WIN_GL_UnloadLibrary(_THIS);
extern int WIN_GL_LoadLibrary(_THIS, const char* path);
extern void *WIN_GL_GetProcAddress(_THIS, const char* proc);
#endif

//end
