// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:


#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "FOX_getenv.h"
#include "FOX_events.h"
#include "FOX_video.h"
#include "FOX_error.h"
#include "FOX_syswm.h"
#include "FOX_sysevents.h"
#include "FOX_events_c.h"
#include "FOX_sysvideo.h"
#include "FOX_lowvideo.h"
#include "FOX_syswm_c.h"
#include "FOX_main.h"

#ifdef WMMSG_DEBUG
#include "wmmsg.h"
#endif

#ifdef _WIN32_WCE
#define NO_GETKEYBOARDSTATE
#endif

// ustaw 

const char *FOX_Appname = NULL;
HINSTANCE FOX_Instance = NULL;
HWND FOX_Window = NULL;
RECT FOX_bounds = {0, 0, 0, 0};
int FOX_resizing = 0;
int mouse_relative = 0;
int posted = 0;

// funkcje wywolywane w czasie kontroli funkcji obslugujacej wiadomosci

LONG
(*HandleMessage)(_THIS, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)=NULL;
void (*WIN_RealizePalette)(_THIS);
void (*WIN_PaletteChanged)(_THIS, HWND window);
void (*WIN_SwapGamma)(_THIS);
void (*WIN_WinPAINT)(_THIS, HDC hdc);

#ifdef WM_MOUSELEAVE

static BOOL (WINAPI *_TrackMouseEvent)(TRACKMOUSEEVENT *ptme) = NULL;

static VOID CALLBACK
TrackMouseTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	RECT rect;
	POINT pt;

	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, NULL, (LPPOINT)&rect, 2);
	GetCursorPos(&pt);

	if ( !PtInRect(&rect, pt) || (WindowFromPoint(pt) != hWnd) ) 
	{
		if ( !KillTimer(hWnd, idEvent) ) 
		{
			// blad w zabijaniu zegara (timer)
		}
		PostMessage(hWnd, WM_MOUSELEAVE, 0, 0);
	}
}

static BOOL WINAPI WIN_TrackMouseEvent(TRACKMOUSEEVENT *ptme)
{
	if ( ptme->dwFlags == TME_LEAVE ) 
	{
		return SetTimer(ptme->hwndTrack, ptme->dwFlags, 100,
		                (TIMERPROC)TrackMouseTimerProc);
	}
	return FALSE;
}
#endif 

// aktualne modyfikatory klawiatury

static void WIN_GetKeyboardState(void)
{
#ifndef NO_GETKEYBOARDSTATE
	FOXMod state;
	BYTE keyboard[256];

	state = KMOD_NONE;

	if ( GetKeyboardState(keyboard) ) 
	{
		if ( keyboard[VK_LSHIFT] & 0x80) 
		{
			state |= KMOD_LSHIFT;
		}
		
		if ( keyboard[VK_RSHIFT] & 0x80) 
		{
			state |= KMOD_RSHIFT;
		}
		
		if ( keyboard[VK_LCONTROL] & 0x80) 
		{
			state |= KMOD_LCTRL;
		}
		
		if ( keyboard[VK_RCONTROL] & 0x80) 
		{
			state |= KMOD_RCTRL;
		}
		
		if ( keyboard[VK_LMENU] & 0x80) 
		{
			state |= KMOD_LALT;
		}
		
		if ( keyboard[VK_RMENU] & 0x80) 
		{
			state |= KMOD_RALT;
		}
		
		if ( keyboard[VK_NUMLOCK] & 0x01) 
		{
			state |= KMOD_NUM;
		}
		
		if ( keyboard[VK_CAPITAL] & 0x01) 
		{
			state |= KMOD_CAPS;
		}
	}
	FOX_SetModState(state);
#endif /* !NO_GETKEYBOARDSTATE */
}

// funkcja do parsowania wiadomosci

LONG CALLBACK WinMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FOX_VideoDevice *this = current_video;
	static int mouse_pressed = 0;
	static int in_window = 0;
#ifdef WMMSG_DEBUG
	fprintf(stderr, "[FOX]: pobieram wiadomosc od windows:  ");
	
	if ( msg > MAX_WMMSG ) 
	{
		fprintf(stderr, "%d", msg);
	} 
	else 
	{
		fprintf(stderr, "%s", wmtab[msg]);
	}
	fprintf(stderr, " -- 0x%X, 0x%X\n", wParam, lParam);
#endif
	switch (msg) {

		case WM_ACTIVATE: {
			FOX_VideoDevice *this = current_video;
			BOOL minimized;
			Uint8 appstate;

			minimized = HIWORD(wParam);
			if ( !minimized && (LOWORD(wParam) != WA_INACTIVE) ) 
			{
				
				// pobierz aktualne stany

				appstate = FOX_APPACTIVE|FOX_APPINPUTFOCUS;
				
				if ( this->input_grab != FOX_GRAB_OFF ) 
				{
					WIN_GrabInput(this, FOX_GRAB_ON);
				}
				
				if ( !(FOX_GetAppState()&FOX_APPINPUTFOCUS) ) 
				{
					WIN_SwapGamma(this);
				}
				posted = FOX_PrivateAppActive(1, appstate);
				WIN_GetKeyboardState();
			} else 
			{
				// olej aktualne stany
				appstate = FOX_APPINPUTFOCUS;
				
				if ( minimized ) 
				{
					appstate |= FOX_APPACTIVE;
				}
				
				if ( this->input_grab != FOX_GRAB_OFF ) 
				{
					WIN_GrabInput(this, FOX_GRAB_OFF);
				}
				
				if ( FOX_GetAppState() & FOX_APPINPUTFOCUS ) 
				{
					WIN_SwapGamma(this);
				}
				posted = FOX_PrivateAppActive(0, appstate);
			}
			return(0);
		}
		break;

		case WM_MOUSEMOVE: {
			
			// mysz jest obslugiwana przez directinput w fullscreen
			
			if ( FOX_VideoSurface && ! DINPUT_FULLSCREEN() ) 
			{
				Sint16 x, y;

				// mysz w oknie
				if ( ! in_window ) 
				{
#ifdef WM_MOUSELEAVE
					TRACKMOUSEEVENT tme;

					tme.cbSize = sizeof(tme);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = FOX_Window;
					_TrackMouseEvent(&tme);
#endif /* WM_MOUSELEAVE */
					in_window = TRUE;

					posted = FOX_PrivateAppActive(1, FOX_APPMOUSEFOCUS);
				}

				// mysz poza oknem
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				
				if ( mouse_relative ) 
				{
					POINT center;
					center.x = (FOX_VideoSurface->w/2);
					center.y = (FOX_VideoSurface->h/2);
					x -= (Sint16)center.x;
					y -= (Sint16)center.y;
					
					if ( x || y ) 
					{
						ClientToScreen(FOX_Window, &center);
						SetCursorPos(center.x, center.y);
						posted = FOX_PrivateMouseMotion(0, 1, x, y);
					}
				} 
				else 
				{
					posted = FOX_PrivateMouseMotion(0, 0, x, y);
				}
			}
		}
		return(0);

#ifdef WM_MOUSELEAVE
		case WM_MOUSELEAVE: {

			// mysz obslugiwana przez directinput w trybie pelnoeranowym
			
			if ( FOX_VideoSurface && ! DINPUT_FULLSCREEN() ) 
			{
				in_window = FALSE;
				posted = FOX_PrivateAppActive(0, FOX_APPMOUSEFOCUS);
			}
		}
		return(0);
#endif /* WM_MOUSELEAVE */

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP: {
			
			// mysz obslugiwana przez directinput w trybie pelnoeranowym
			
			if ( FOX_VideoSurface && ! DINPUT_FULLSCREEN() ) {
				Sint16 x, y;
				Uint8 button, state;

				SetFocus(FOX_Window);

				switch (msg) {
					case WM_LBUTTONDOWN:
						button = 1;
						state = FOX_PRESSED;
						break;
					case WM_LBUTTONUP:
						button = 1;
						state = FOX_RELEASED;
						break;
					case WM_MBUTTONDOWN:
						button = 2;
						state = FOX_PRESSED;
						break;
					case WM_MBUTTONUP:
						button = 2;
						state = FOX_RELEASED;
						break;
					case WM_RBUTTONDOWN:
						button = 3;
						state = FOX_PRESSED;
						break;
					case WM_RBUTTONUP:
						button = 3;
						state = FOX_RELEASED;
						break;
					default:
						// nieznany przycisk myszy nacisniety
						return(0);
				}
				
				if ( state == FOX_PRESSED ) 
				{
					// pobierz stan
					
					if ( ++mouse_pressed > 0 ) 
					{
						SetCapture(hwnd);
					}
				} 
				else 
				{
					// zwonij
					
					if ( --mouse_pressed <= 0 ) 
					{
						ReleaseCapture();
						mouse_pressed = 0;
					}
				}
				if ( mouse_relative ) 
				{
					x = 0; y = 0;
				} 
				else 
				{
					x = (Sint16)LOWORD(lParam);
					y = (Sint16)HIWORD(lParam);
				}
				posted = FOX_PrivateMouseButton(
							state, button, x, y);
			}
		}
		return(0);


#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
		case WM_MOUSEWHEEL: 
			if ( FOX_VideoSurface && ! DINPUT_FULLSCREEN() ) {
				int move = (short)HIWORD(wParam);
				if ( move ) {
					Uint8 button;
					if ( move > 0 )
						button = 4;
					else
						button = 5;
					posted = FOX_PrivateMouseButton(
						FOX_PRESSED, button, 0, 0);
				}
			}
			return(0);
#endif

#ifdef WM_GETMINMAXINFO
		case WM_GETMINMAXINFO: {
			MINMAXINFO *info;
			RECT        size;
			int x, y;
			int width;
			int height;

			if ( FOX_resizing )
				return(0);

			if ( FOX_PublicSurface &&
				(FOX_PublicSurface->flags & FOX_RESIZABLE) ) {
				return(0);
			}

			// pobierz wspolrzedne okna
			
			GetWindowRect(FOX_Window, &size);
			x = size.left;
			y = size.top;

			// wylicz aktualna rozdzielczosc
			
			size.top = 0;
			size.left = 0;
			
			if ( FOX_PublicSurface != NULL ) 
			{
				size.bottom = FOX_PublicSurface->h;
				size.right = FOX_PublicSurface->w;
			} 
			else 
			{
				size.bottom = 0;
				size.right = 0;
			}
			AdjustWindowRect(&size, GetWindowLong(hwnd, GWL_STYLE),
									FALSE);
			width = size.right - size.left;
			height = size.bottom - size.top;

			// wylicz rozmiar
			info = (MINMAXINFO *)lParam;
			info->ptMaxSize.x = width;
			info->ptMaxSize.y = height;
			info->ptMaxPosition.x = x;
			info->ptMaxPosition.y = y;
			info->ptMinTrackSize.x = width;
			info->ptMinTrackSize.y = height;
			info->ptMaxTrackSize.x = width;
			info->ptMaxTrackSize.y = height;
		}
		return(0);
#endif /* WM_GETMINMAXINFO */

		case WM_MOVE: {
			FOX_VideoDevice *this = current_video;

			GetClientRect(FOX_Window, &FOX_bounds);
			ClientToScreen(FOX_Window, (LPPOINT)&FOX_bounds);
			ClientToScreen(FOX_Window, (LPPOINT)&FOX_bounds+1);
			
			if ( this->input_grab != FOX_GRAB_OFF ) 
			{
				ClipCursor(&FOX_bounds);
			}
		}
		break;
	
		case WM_SIZE: {
			if ( FOX_PublicSurface &&
				(FOX_PublicSurface->flags & FOX_RESIZABLE) ) 
			{
				FOX_PrivateResize(LOWORD(lParam), HIWORD(lParam));
			}
			return(0);
		}
		break;

		// ustaw kursor
		
		case WM_SETCURSOR: 
			{
			Uint16 hittest;

			hittest = LOWORD(lParam);
			if ( hittest == HTCLIENT ) 
			{
				SetCursor(FOX_hcursor);
				return(TRUE);
			}
		}
		break;

		
		case WM_QUERYNEWPALETTE: {
			WIN_RealizePalette(current_video);
			return(TRUE);
		}
		break;

		// inna aplikacja zmienila palete
		
		case WM_PALETTECHANGED: {
			WIN_PaletteChanged(current_video, (HWND)wParam);
		}
		break;

		// odswiez
		
		case WM_PAINT: {
			HDC hdc;
			PAINTSTRUCT ps;

			hdc = BeginPaint(FOX_Window, &ps);
			if ( current_video->screen &&
			     !(current_video->screen->flags & FOX_OPENGL) ) 
			{
				WIN_WinPAINT(current_video, hdc);
			}
			EndPaint(FOX_Window, &ps);
		}
		return(0);
		
		case WM_ERASEBKGND: {
			posted = FOX_PrivateExpose();
		}
		return(0);

		case WM_CLOSE: {
			if ( (posted = FOX_PrivateQuit()) )
				PostQuitMessage(0);
		}
		return(0);

		case WM_DESTROY: {
			PostQuitMessage(0);
		}
		return(0);

		default: {
			
			// inna obsluga 
			
			if (HandleMessage) 
			{
				return(HandleMessage(current_video,
			                     hwnd, msg, wParam, lParam));
			}
		}
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}

// aplikacja przejmuje

static void *FOX_handle = NULL;

void FOX_SetModuleHandle(void *handle)
{
	FOX_handle = handle;
}
void *FOX_GetModuleHandle(void)
{
	void *handle;

	if ( FOX_handle ) 
	{
		handle = FOX_handle;
	} 
	else 
	{
		// tutaj moze sie sypac, bo handle zwraca do dll a nie aplikacji, wiec 
		// direct input moze sie nie zainicjalizowac

		handle = GetModuleHandle(NULL);
	}
	return(handle);
}

const char *FOX_windowid = NULL;

// zarejestruj klase

int FOX_RegisterApp(char *name, Uint32 style, void *hInst)
{
	static int initialized = 0;
	WNDCLASS class;
#ifdef WM_MOUSELEAVE
	HMODULE handle;
#endif

	// tylko raz
	
	if ( initialized ) 
	{
		return(0);
	}

	// ciag dalszy
	
	if ( ! hInst ) 
	{
		hInst = FOX_GetModuleHandle();
	}

	// zarejestruj klase aplikacji
	
	class.hCursor		= NULL;
#ifdef _WIN32_WCE
    {
	/* WinCE uses the UNICODE version */
	int nLen = strlen(name)+1;
	LPWSTR lpszW = alloca(nLen*2);
	MultiByteToWideChar(CP_ACP, 0, name, -1, lpszW, nLen);
	class.hIcon		= LoadImage(hInst, lpszW, IMAGE_ICON,
	                                    0, 0, LR_DEFAULTCOLOR);
	class.lpszMenuName	= NULL;
	class.lpszClassName	= lpszW;
    }
#else
	class.hIcon		= LoadImage(hInst, name, IMAGE_ICON,
	                                    0, 0, LR_DEFAULTCOLOR);
	class.lpszMenuName	= "(none)";
	class.lpszClassName	= name;
#endif /* _WIN32_WCE */
	class.hbrBackground	= NULL;
	class.hInstance		= hInst;
	class.style		= style;
#ifdef HAVE_OPENGL
	class.style		|= CS_OWNDC;
#endif
	class.lpfnWndProc	= WinMessage;
	class.cbWndExtra	= 0;
	class.cbClsExtra	= 0;
	if ( ! RegisterClass(&class) ) {
		FOX_SetError("Couldn't register application class");
		return(-1);
	}
	FOX_Appname = name;
	FOX_Instance = hInst;

#ifdef WM_MOUSELEAVE
	/* Get the version of TrackMouseEvent() we use */
	_TrackMouseEvent = NULL;
	handle = GetModuleHandle("USER32.DLL");
	if ( handle ) {
		_TrackMouseEvent = (BOOL (WINAPI *)(TRACKMOUSEEVENT *))GetProcAddress(handle, "TrackMouseEvent");
	}
	if ( _TrackMouseEvent == NULL ) {
		_TrackMouseEvent = WIN_TrackMouseEvent;
	}
#endif 

	
	FOX_windowid = getenv("FOX_WINDOWID");

	initialized = 1;
	return(0);
}

// end
