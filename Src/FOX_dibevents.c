// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "FOX_events.h"
#include "FOX_error.h"
#include "FOX_syswm.h"
#include "FOX_sysevents.h"
#include "FOX_events_c.h"
#include "FOX_lowvideo.h"
#include "FOX_dibvideo.h"
#include "FOX_vkeys.h"

#ifndef WM_APP
	#define WM_APP	0x8000
#endif

// tablica translacji

static FOXKey VK_keymap[FOXK_LAST];
static FOX_keysym *TranslateKey(UINT vkey, UINT scancode, FOX_keysym *keysym, int pressed);

#define REPEATED_KEYMASK	(1<<30)
#define EXTENDED_KEYMASK	(1<<24)

static WNDPROC userWindowProc = NULL;

// glowny handler win32 

LONG
 DIB_HandleMessage(_THIS, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	extern int posted;

	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			FOX_keysym keysym;

			// ignoruj powtarzane klawisze
			if ( lParam&REPEATED_KEYMASK ) 
			{
				return(0);
			}
			
			switch (wParam) {
				case VK_CONTROL:
					if ( lParam&EXTENDED_KEYMASK )
						wParam = VK_RCONTROL;
					else
						wParam = VK_LCONTROL;
					break;
				case VK_SHIFT:
					wParam = VK_LSHIFT;
					break;
				case VK_MENU:
					if ( lParam&EXTENDED_KEYMASK )
						wParam = VK_RMENU;
					else
						wParam = VK_LMENU;
					break;
			}
#ifdef NO_GETKEYBOARDSTATE
			
			if ( FOX_TranslateUNICODE ) 
			{
				MSG m;

				m.hwnd = hwnd;
				m.message = msg;
				m.wParam = wParam;
				m.lParam = lParam;
				m.time = 0;
				
				if ( TranslateMessage(&m) && PeekMessage(&m, hwnd, 0, WM_USER, PM_NOREMOVE) && (m.message == WM_CHAR) ) 
				{
					GetMessage(&m, hwnd, 0, WM_USER);
			    		wParam = m.wParam;
				} 
				else 
				{
					wParam = 0;
				}
			} 
			else 
			{
				wParam = 0;
			}
#endif /* NO_GETKEYBOARDSTATE */
			posted = FOX_PrivateKeyboard(FOX_PRESSED,
				TranslateKey(wParam,HIWORD(lParam),&keysym,1));
		}
		return(0);

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			FOX_keysym keysym;

			switch (wParam) {
				case VK_CONTROL:
					if ( lParam&EXTENDED_KEYMASK )
						wParam = VK_RCONTROL;
					else
						wParam = VK_LCONTROL;
					break;
				case VK_SHIFT:
					wParam = VK_LSHIFT;
					break;
				case VK_MENU:
					if ( lParam&EXTENDED_KEYMASK )
						wParam = VK_RMENU;
					else
						wParam = VK_LMENU;
					break;
			}
			posted = FOX_PrivateKeyboard(FOX_RELEASED,
				TranslateKey(wParam,HIWORD(lParam),&keysym,0));
		}
		return(0);

		default: {
			
			if ( FOX_ProcessEvents[FOX_SYSWMEVENT] == FOX_ENABLE ) 
			{
			        FOX_SysWMmsg wmmsg;

				FOX_VERSION(&wmmsg.version);
				wmmsg.hwnd = hwnd;
				wmmsg.msg = msg;
				wmmsg.wParam = wParam;
				wmmsg.lParam = lParam;
				posted = FOX_PrivateSysWMEvent(&wmmsg);
			} else if (userWindowProc) 
			{
				return userWindowProc(hwnd, msg, wParam, lParam);
			}
		}
		break;
	}
	return(DefWindowProc(hwnd, msg, wParam, lParam));
}

void DIB_PumpEvents(_THIS)
{
	MSG msg;

	while ( PeekMessage(&msg, NULL, 0, (WM_APP-1), PM_NOREMOVE) ) 
	{
		if ( GetMessage(&msg, NULL, 0, (WM_APP-1)) > 0 ) 
		{
			DispatchMessage(&msg);
		}
	}
}

void DIB_InitOSKeymap(_THIS)
{
	int i;
	
	for ( i=0; i<FOX_TABLESIZE(VK_keymap); ++i )
		VK_keymap[i] = FOXK_UNKNOWN;

	VK_keymap[VK_BACK] = FOXK_BACKSPACE;
	VK_keymap[VK_TAB] = FOXK_TAB;
	VK_keymap[VK_CLEAR] = FOXK_CLEAR;
	VK_keymap[VK_RETURN] = FOXK_RETURN;
	VK_keymap[VK_PAUSE] = FOXK_PAUSE;
	VK_keymap[VK_ESCAPE] = FOXK_ESCAPE;
	VK_keymap[VK_SPACE] = FOXK_SPACE;
	VK_keymap[VK_APOSTROPHE] = FOXK_QUOTE;
	VK_keymap[VK_COMMA] = FOXK_COMMA;
	VK_keymap[VK_MINUS] = FOXK_MINUS;
	VK_keymap[VK_PERIOD] = FOXK_PERIOD;
	VK_keymap[VK_SLASH] = FOXK_SLASH;
	VK_keymap[VK_0] = FOXK_0;
	VK_keymap[VK_1] = FOXK_1;
	VK_keymap[VK_2] = FOXK_2;
	VK_keymap[VK_3] = FOXK_3;
	VK_keymap[VK_4] = FOXK_4;
	VK_keymap[VK_5] = FOXK_5;
	VK_keymap[VK_6] = FOXK_6;
	VK_keymap[VK_7] = FOXK_7;
	VK_keymap[VK_8] = FOXK_8;
	VK_keymap[VK_9] = FOXK_9;
	VK_keymap[VK_SEMICOLON] = FOXK_SEMICOLON;
	VK_keymap[VK_EQUALS] = FOXK_EQUALS;
	VK_keymap[VK_LBRACKET] = FOXK_LEFTBRACKET;
	VK_keymap[VK_BACKSLASH] = FOXK_BACKSLASH;
	VK_keymap[VK_RBRACKET] = FOXK_RIGHTBRACKET;
	VK_keymap[VK_GRAVE] = FOXK_BACKQUOTE;
	VK_keymap[VK_A] = FOXK_a;
	VK_keymap[VK_B] = FOXK_b;
	VK_keymap[VK_C] = FOXK_c;
	VK_keymap[VK_D] = FOXK_d;
	VK_keymap[VK_E] = FOXK_e;
	VK_keymap[VK_F] = FOXK_f;
	VK_keymap[VK_G] = FOXK_g;
	VK_keymap[VK_H] = FOXK_h;
	VK_keymap[VK_I] = FOXK_i;
	VK_keymap[VK_J] = FOXK_j;
	VK_keymap[VK_K] = FOXK_k;
	VK_keymap[VK_L] = FOXK_l;
	VK_keymap[VK_M] = FOXK_m;
	VK_keymap[VK_N] = FOXK_n;
	VK_keymap[VK_O] = FOXK_o;
	VK_keymap[VK_P] = FOXK_p;
	VK_keymap[VK_Q] = FOXK_q;
	VK_keymap[VK_R] = FOXK_r;
	VK_keymap[VK_S] = FOXK_s;
	VK_keymap[VK_T] = FOXK_t;
	VK_keymap[VK_U] = FOXK_u;
	VK_keymap[VK_V] = FOXK_v;
	VK_keymap[VK_W] = FOXK_w;
	VK_keymap[VK_X] = FOXK_x;
	VK_keymap[VK_Y] = FOXK_y;
	VK_keymap[VK_Z] = FOXK_z;
	VK_keymap[VK_DELETE] = FOXK_DELETE;

	VK_keymap[VK_NUMPAD0] = FOXK_KP0;
	VK_keymap[VK_NUMPAD1] = FOXK_KP1;
	VK_keymap[VK_NUMPAD2] = FOXK_KP2;
	VK_keymap[VK_NUMPAD3] = FOXK_KP3;
	VK_keymap[VK_NUMPAD4] = FOXK_KP4;
	VK_keymap[VK_NUMPAD5] = FOXK_KP5;
	VK_keymap[VK_NUMPAD6] = FOXK_KP6;
	VK_keymap[VK_NUMPAD7] = FOXK_KP7;
	VK_keymap[VK_NUMPAD8] = FOXK_KP8;
	VK_keymap[VK_NUMPAD9] = FOXK_KP9;
	VK_keymap[VK_DECIMAL] = FOXK_KP_PERIOD;
	VK_keymap[VK_DIVIDE] = FOXK_KP_DIVIDE;
	VK_keymap[VK_MULTIPLY] = FOXK_KP_MULTIPLY;
	VK_keymap[VK_SUBTRACT] = FOXK_KP_MINUS;
	VK_keymap[VK_ADD] = FOXK_KP_PLUS;

	VK_keymap[VK_UP] = FOXK_UP;
	VK_keymap[VK_DOWN] = FOXK_DOWN;
	VK_keymap[VK_RIGHT] = FOXK_RIGHT;
	VK_keymap[VK_LEFT] = FOXK_LEFT;
	VK_keymap[VK_INSERT] = FOXK_INSERT;
	VK_keymap[VK_HOME] = FOXK_HOME;
	VK_keymap[VK_END] = FOXK_END;
	VK_keymap[VK_PRIOR] = FOXK_PAGEUP;
	VK_keymap[VK_NEXT] = FOXK_PAGEDOWN;

	VK_keymap[VK_F1] = FOXK_F1;
	VK_keymap[VK_F2] = FOXK_F2;
	VK_keymap[VK_F3] = FOXK_F3;
	VK_keymap[VK_F4] = FOXK_F4;
	VK_keymap[VK_F5] = FOXK_F5;
	VK_keymap[VK_F6] = FOXK_F6;
	VK_keymap[VK_F7] = FOXK_F7;
	VK_keymap[VK_F8] = FOXK_F8;
	VK_keymap[VK_F9] = FOXK_F9;
	VK_keymap[VK_F10] = FOXK_F10;
	VK_keymap[VK_F11] = FOXK_F11;
	VK_keymap[VK_F12] = FOXK_F12;
	VK_keymap[VK_F13] = FOXK_F13;
	VK_keymap[VK_F14] = FOXK_F14;
	VK_keymap[VK_F15] = FOXK_F15;

	VK_keymap[VK_NUMLOCK] = FOXK_NUMLOCK;
	VK_keymap[VK_CAPITAL] = FOXK_CAPSLOCK;
	VK_keymap[VK_SCROLL] = FOXK_SCROLLOCK;
	VK_keymap[VK_RSHIFT] = FOXK_RSHIFT;
	VK_keymap[VK_LSHIFT] = FOXK_LSHIFT;
	VK_keymap[VK_RCONTROL] = FOXK_RCTRL;
	VK_keymap[VK_LCONTROL] = FOXK_LCTRL;
	VK_keymap[VK_RMENU] = FOXK_RALT;
	VK_keymap[VK_LMENU] = FOXK_LALT;
	VK_keymap[VK_RWIN] = FOXK_RSUPER;
	VK_keymap[VK_LWIN] = FOXK_LSUPER;

	VK_keymap[VK_HELP] = FOXK_HELP;
#ifdef VK_PRINT
	VK_keymap[VK_PRINT] = FOXK_PRINT;
#endif
	VK_keymap[VK_SNAPSHOT] = FOXK_PRINT;
	VK_keymap[VK_CANCEL] = FOXK_BREAK;
	VK_keymap[VK_APPS] = FOXK_MENU;
}

static FOX_keysym *TranslateKey(UINT vkey, UINT scancode, FOX_keysym *keysym, int pressed)
{
	
	keysym->scancode = (unsigned char) scancode;
	keysym->sym = VK_keymap[vkey];
	keysym->mod = KMOD_NONE;
	keysym->unicode = 0;
	if ( pressed && FOX_TranslateUNICODE ) 
	{ 
#ifdef NO_GETKEYBOARDSTATE
		
		keysym->unicode = vkey;
#else
		BYTE keystate[256];
		BYTE chars[2];

		GetKeyboardState(keystate);
		if ( ToAscii(vkey,scancode,keystate,(WORD *)chars,0) == 1 ) 
		{
			keysym->unicode = chars[0];
		}
#endif 
	}
	return(keysym);
}

int DIB_CreateWindow(_THIS)
{
	FOX_RegisterApp("FOX_app", CS_BYTEALIGNCLIENT, 0);
	if ( FOX_windowid ) {
		FOX_Window = (HWND)strtol(FOX_windowid, NULL, 0);

      if (FOX_Window) 
	  {
         userWindowProc = (WNDPROC)GetWindowLong(FOX_Window, GWL_WNDPROC);
         SetWindowLong(FOX_Window, GWL_WNDPROC, (LONG)WinMessage);
      }
	} 
	else 
	{
		FOX_Window = CreateWindow(FOX_Appname, FOX_Appname,
                        (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX),
                                 0, 0, 0, 0, NULL, NULL, FOX_Instance, NULL);
		
		if ( FOX_Window == NULL ) 
		{
			FOX_SetError("[FOX]: nie mozna stworzyc okna");
			return(-1);
		}
		ShowWindow(FOX_Window, SW_HIDE);
	}

	return(0);
}

void DIB_DestroyWindow(_THIS)
{
	if ( FOX_windowid == NULL ) 
	{
		DestroyWindow(FOX_Window);
	}
}

// end
