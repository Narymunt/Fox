// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// implementacja opengl

#include <windows.h>

#include "FOX_error.h"
#include "FOX_lowvideo.h"
#include "FOX_wingl_c.h"

#ifdef HAVE_OPENGL
	#define DEFAULT_GL_DRIVER_PATH "OPENGL32.DLL"
#endif

int WIN_GL_SetupWindow(_THIS)
{
	int retval;
#ifdef HAVE_OPENGL
	int pixel_format;

	// wczytaj sterownik gl
	
	if ( ! this->gl_config.driver_loaded ) 
	{
		// nie ma sterownika, uzywamy wlasnego
		if ( WIN_GL_LoadLibrary(this, NULL) < 0 ) 
		{
			return(-1);
		}
	}

	// pobierz dc dla ogl
	
	GL_hdc = GetDC(FOX_Window);
	
	if ( GL_hdc == NULL ) 
	{
		FOX_SetError("[FOX]: nie mozna przekierowac dc do opengl");
		return(-1);
	}
	
	// przekaz deskryptory
	
	memset(&GL_pfd, 0, sizeof(GL_pfd));
	GL_pfd.nSize = sizeof(GL_pfd);
	GL_pfd.nVersion = 1;
	GL_pfd.dwFlags = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL);
	
	if ( this->gl_config.double_buffer ) 
	{
		GL_pfd.dwFlags |= PFD_DOUBLEBUFFER;
	}
	
	GL_pfd.iPixelType = PFD_TYPE_RGBA;
	GL_pfd.cColorBits = this->gl_config.buffer_size;
	GL_pfd.cRedBits = this->gl_config.red_size;
	GL_pfd.cGreenBits = this->gl_config.green_size;
	GL_pfd.cBlueBits = this->gl_config.blue_size;
	GL_pfd.cAlphaBits = this->gl_config.alpha_size;
	GL_pfd.cAccumRedBits = this->gl_config.accum_red_size;
	GL_pfd.cAccumGreenBits = this->gl_config.accum_green_size;
	GL_pfd.cAccumBlueBits = this->gl_config.accum_blue_size;
	GL_pfd.cAccumAlphaBits = this->gl_config.accum_alpha_size;
	GL_pfd.cAccumBits =
		(GL_pfd.cAccumRedBits + GL_pfd.cAccumGreenBits +
		 GL_pfd.cAccumBlueBits + GL_pfd.cAccumAlphaBits);
	GL_pfd.cDepthBits = this->gl_config.depth_size;
	GL_pfd.cStencilBits = this->gl_config.stencil_size;

	// wybierz najbardziej zblizony format pixela
	
	pixel_format = ChoosePixelFormat(GL_hdc, &GL_pfd);
	
	if ( !pixel_format ) 
	{
		FOX_SetError("[FOX]: nie ma zblizonego formatu pixela w opengl");
		return(-1);
	}

	if( !SetPixelFormat(GL_hdc, pixel_format, &GL_pfd) ) 
	{
		FOX_SetError("[FOX]: nie mozna ustawic formatu pixela hdc w opengl");
		return(-1);
	}
	DescribePixelFormat(GL_hdc, pixel_format, sizeof(GL_pfd), &GL_pfd);

	GL_hrc = this->gl_data->wglCreateContext(GL_hdc);
	
	if( GL_hrc == NULL ) 
	{
		FOX_SetError("[FOX]: nie mozna zainicjalizowac opengl context");
		return(-1);
	}
	gl_active = 1;
#else
	FOX_SetError("[FOX]: system windows nie skofigurowany do pracy z opengl");
#endif
	if ( gl_active ) 
	{
		retval = 0;
	} 
	else 
	{
		retval = -1;
	}
	return(retval);
}

void WIN_GL_ShutDown(_THIS)
{
#ifdef HAVE_OPENGL
	// wyczysc
	
	if ( GL_hrc ) 
	{
		this->gl_data->wglMakeCurrent(NULL, NULL);
		this->gl_data->wglDeleteContext(GL_hrc);
		GL_hrc = NULL;
	}
	
	if ( GL_hdc ) 
	{
		ReleaseDC(FOX_Window, GL_hdc);
		GL_hdc = NULL;
	}
	gl_active = 0;

	WIN_GL_UnloadLibrary(this);
#endif /* HAVE_OPENGL */
}

#ifdef HAVE_OPENGL

// stworz 

int WIN_GL_MakeCurrent(_THIS)
{
	int retval;

	retval = 0;

	if ( ! this->gl_data->wglMakeCurrent(GL_hdc, GL_hrc) ) 
	{
		FOX_SetError("[FOX]: nie mozna przypisac do opengl");
		retval = -1;
	}
	return(retval);
}

// pobierz atrybuty z glx

int WIN_GL_GetAttribute(_THIS, FOX_GLattr attrib, int* value)
{
	int retval;

	retval = 0;
	switch( attrib ) {
	    case FOX_GL_RED_SIZE:
		*value = GL_pfd.cRedBits;
		break;
	    case FOX_GL_GREEN_SIZE:
		*value = GL_pfd.cGreenBits;
		break;
	    case FOX_GL_BLUE_SIZE:
		*value = GL_pfd.cBlueBits;
		break;
	    case FOX_GL_ALPHA_SIZE:
		*value = GL_pfd.cAlphaBits;
		break;
	    case FOX_GL_DOUBLEBUFFER:
		if ( GL_pfd.dwFlags & PFD_DOUBLEBUFFER ) 
		{
			*value = 1;
		} 
		else 
		{
			*value = 0;
		}
		break;
	    case FOX_GL_BUFFER_SIZE:
		*value = GL_pfd.cColorBits;
		break;
	    case FOX_GL_DEPTH_SIZE:
		*value = GL_pfd.cDepthBits;
		break;
	    case FOX_GL_STENCIL_SIZE:
		*value = GL_pfd.cStencilBits;
		break;
	    case FOX_GL_ACCUM_RED_SIZE:
		*value = GL_pfd.cAccumRedBits;
		break;
	    case FOX_GL_ACCUM_GREEN_SIZE:
		*value = GL_pfd.cAccumGreenBits;
		break;
	    case FOX_GL_ACCUM_BLUE_SIZE:
		*value = GL_pfd.cAccumBlueBits;
		break;
	    case FOX_GL_ACCUM_ALPHA_SIZE:
		*value = GL_pfd.cAccumAlphaBits;
		break;
	    default:
		retval = -1;
		break;
	}
	return retval;
}

void WIN_GL_SwapBuffers(_THIS)
{
	SwapBuffers(GL_hdc);
}

#endif 

#ifdef HAVE_OPENGL

void WIN_GL_UnloadLibrary(_THIS)
{
	if ( this->gl_config.driver_loaded ) 
	{
		FreeLibrary((HMODULE)this->gl_config.dll_handle);

		this->gl_data->wglGetProcAddress = NULL;
		this->gl_data->wglCreateContext = NULL;
		this->gl_data->wglDeleteContext = NULL;
		this->gl_data->wglMakeCurrent = NULL;

		this->gl_config.dll_handle = NULL;
		this->gl_config.driver_loaded = 0;
	}
}

// przekazywanie sciezki 

int WIN_GL_LoadLibrary(_THIS, const char* path) 
{
	HMODULE handle;

 	if ( gl_active ) {
 		FOX_SetError("[FOX]: context opengl juz zainicjalizowany");
 		return -1;
 	}

	if ( path == NULL ) 
	{
		path = DEFAULT_GL_DRIVER_PATH;
	}
	
	handle = LoadLibrary(path);
	
	if ( handle == NULL ) 
	{
		FOX_SetError("[FOX]: nie mozna wczytac biblioteki opengl");
		return -1;
	}

	// wyladuj stary sterownik i zresetuj wskazniki
	
	WIN_GL_UnloadLibrary(this);
	
	// zaladuj nowe wskazniki do funkcji
	
	this->gl_data->wglGetProcAddress = (void * (WINAPI *)(const char *))
		GetProcAddress(handle, "wglGetProcAddress");
	this->gl_data->wglCreateContext = (HGLRC (WINAPI *)(HDC))
		GetProcAddress(handle, "wglCreateContext");
	this->gl_data->wglDeleteContext = (BOOL (WINAPI *)(HGLRC))
		GetProcAddress(handle, "wglDeleteContext");
	this->gl_data->wglMakeCurrent = (BOOL (WINAPI *)(HDC, HGLRC))
		GetProcAddress(handle, "wglMakeCurrent");

	if ( (this->gl_data->wglGetProcAddress == NULL) ||
	     (this->gl_data->wglCreateContext == NULL) ||
	     (this->gl_data->wglDeleteContext == NULL) ||
	     (this->gl_data->wglMakeCurrent == NULL) ) {
		FOX_SetError("[FOX]: nie mozna pobrac funkcji opengl");
		FreeLibrary(handle);
		return -1;
	}

	this->gl_config.dll_handle = handle;
	strcpy(this->gl_config.driver_path, path);
	this->gl_config.driver_loaded = 1;
	return 0;
}

void *WIN_GL_GetProcAddress(_THIS, const char* proc)
{
	void *func;

	// pobranie rozszerzen
	
	func = this->gl_data->wglGetProcAddress(proc);
	
	if ( ! func ) 
	{
		// prawdopodobnie zwykla funkcja opengl
		func = GetProcAddress(this->gl_config.dll_handle, proc);
	}
	return func;
}

#endif /* HAVE_OPENGL */

//end
