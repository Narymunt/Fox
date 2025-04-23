// Fox v0.5
// by Jaroslaw Rozynski
//===
// *DIRECTX*
//===
// TODO:

// reszta w FOX_sysevents.c

#include "directx.h"

#include <stdio.h>
#include "FOX_events.h"
#include "FOX_video.h"
#include "FOX_error.h"
#include "FOX_syswm.h"
#include "FOX_sysevents.h"
#include "FOX_events_c.h"
#include "FOX_lowvideo.h"
#include "FOX_dx8video.h"

#ifndef WM_APP
	#define WM_APP	0x8000
#endif

// klwiatura i mysz

#define MAX_INPUTS	16		// maxymalnie 16 urzadzen
#define INPUT_QSIZE	32		// bufor na 32 wiadomosci

static LPDIRECTINPUT dinput = NULL;
static LPDIRECTINPUTDEVICE2 FOX_DIdev[MAX_INPUTS];
static HANDLE               FOX_DIevt[MAX_INPUTS];

static void (*FOX_DIfun[MAX_INPUTS])(const int, DIDEVICEOBJECTDATA *);

static int FOX_DIndev = 0;
static int mouse_lost;
static int mouse_pressed;

// tablica dla translacji

static FOXKey DIK_keymap[256];
static FOX_keysym *TranslateKey(UINT scancode, FOX_keysym *keysym, int pressed);

static WNDPROC userWindowProc = NULL;

// konwersja kodu directinput na text

static void SetDIerror(char *function, int code)
{
	static char *error;
	static char  errbuf[BUFSIZ];

	errbuf[0] = 0;
	switch (code) {
                case DIERR_GENERIC:
                        error = "nieznany blad!";
                        break;
		case DIERR_OLDDIRECTINPUTVERSION:
			error = "twoja wersja DirectInput ma problem";
			break;
		case DIERR_INVALIDPARAM:
                        error = "nieprawidlowe parametry";
                        break;
                case DIERR_OUTOFMEMORY:
                        error = "za malo pamieci";
                        break;
		case DIERR_DEVICENOTREG:
			error = "urzadzenie nie zarejestrowane";
			break;
		case DIERR_NOINTERFACE:
			error = "interfejs nie obslugiwany";
			break;
		case DIERR_NOTINITIALIZED:
			error = "urzadzenie nie zainicjalizowane";
			break;
		default:
			sprintf(errbuf, "%s: nieznany blad DirectInput: 0x%x",
								function, code);
			break;
	}
	if ( ! errbuf[0] ) 
	{
		sprintf(errbuf, "%s: %s", function, error);
	}
	FOX_SetError("%s", errbuf);
	return;
}

// inicjalizacja DirectInput

static void handle_keyboard(const int numevents, DIDEVICEOBJECTDATA *events);
static void handle_mouse(const int numevents, DIDEVICEOBJECTDATA *events);
struct {
	char *name;
	REFGUID guid;
	LPCDIDATAFORMAT format;
	DWORD win_level;
	DWORD raw_level;
	void (*fun)(const int numevents, DIDEVICEOBJECTDATA *events);
} inputs[] = {
	{ "keyboard",
		&GUID_SysKeyboard, &c_dfDIKeyboard,
		(DISCL_FOREGROUND|DISCL_NONEXCLUSIVE),
		(DISCL_FOREGROUND|DISCL_NONEXCLUSIVE), handle_keyboard },
	{ "mouse",
		&GUID_SysMouse, &c_dfDIMouse,
		(DISCL_FOREGROUND|DISCL_NONEXCLUSIVE),
		(DISCL_FOREGROUND|DISCL_EXCLUSIVE), handle_mouse },
	{ NULL, NULL, NULL, 0, 0, NULL }
};
	
static int DX8_DInputInit(_THIS)
{
	int         i;
	LPDIRECTINPUTDEVICE device;
	HRESULT     result;
	DIPROPDWORD dipdw;

	// stworz obiekt directinput
	
	result = DInputCreate(FOX_Instance, DIRECTINPUT_VERSION,
							&dinput, NULL);
	if ( result != DI_OK ) 
	{
		SetDIerror("DirectInputCreate", result);
		return(-1);
	}

	// stworz wszystkie zainicjalizowane urzadzenia
	
	FOX_DIndev = 0;
	
	for ( i=0; inputs[i].name; ++i ) 
	{
		// stworzy urzadzenie DirectInput 
		result = IDirectInput_CreateDevice(dinput, inputs[i].guid,
								&device, NULL);
		
		if ( result != DI_OK ) 
		{
			SetDIerror("DirectInput::CreateDevice", result);
			return(-1);
		}
		result = IDirectInputDevice_QueryInterface(device,
			&IID_IDirectInputDevice2, (LPVOID *)&FOX_DIdev[i]);
		IDirectInputDevice_Release(device);
		
		if ( result != DI_OK ) 
		{
			SetDIerror("DirectInputDevice::QueryInterface", result);
			return(-1);
		}
		
		result = IDirectInputDevice2_SetCooperativeLevel(FOX_DIdev[i],
					FOX_Window, inputs[i].win_level);
		
		if ( result != DI_OK ) 
		{
			SetDIerror("DirectInputDevice::SetCooperativeLevel",
									result);
			return(-1);
		}
		
		result = IDirectInputDevice2_SetDataFormat(FOX_DIdev[i],
							inputs[i].format);
		
		if ( result != DI_OK ) 
		{
			SetDIerror("DirectInputDevice::SetDataFormat", result);
			return(-1);
		}

		// ustaw bufor
		
		memset(&dipdw, 0, sizeof(dipdw));
		dipdw.diph.dwSize = sizeof(dipdw);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = INPUT_QSIZE;
		result = IDirectInputDevice2_SetProperty(FOX_DIdev[i],
						DIPROP_BUFFERSIZE, &dipdw.diph);

		if ( result != DI_OK ) 
		{
			SetDIerror("DirectInputDevice::SetProperty", result);
			return(-1);
		}

		// stworz watek ktory bedzie obslugiwal to urzadzenie
		// lepiej gdyby to bylo zdarzenie
		
		FOX_DIevt[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		
		if ( FOX_DIevt[i] == NULL ) 
		{
			FOX_SetError("[FOX]: nie moge utworzyc zdarzenia DirectInput");
			return(-1);
		}
		
		result = IDirectInputDevice2_SetEventNotification(FOX_DIdev[i],
								FOX_DIevt[i]);
		
		if ( result != DI_OK ) 
		{
			SetDIerror("DirectInputDevice::SetEventNotification",
									result);
			return(-1);
		}
		FOX_DIfun[i] = inputs[i].fun;

		// zdarzenie z urzadzenia
		
		IDirectInputDevice2_Acquire(FOX_DIdev[i]);

		// zwieksz liczbe urzadzen
	
		++FOX_DIndev;
	}
	mouse_pressed = 0;

	// directinput jest juz gotowy

	return(0);
}

// zmien poziom

void DX8_DInputReset(_THIS, int fullscreen)
{
	DWORD level;
	int i;
	HRESULT result;

	for ( i=0; i<MAX_INPUTS; ++i ) 
	{
		if ( FOX_DIdev[i] != NULL ) 
		{
			if ( fullscreen ) 
			{
				level = inputs[i].raw_level;
			} 
			else 
			{
				level = inputs[i].win_level;
			}
			IDirectInputDevice2_Unacquire(FOX_DIdev[i]);
			
			result = IDirectInputDevice2_SetCooperativeLevel(
					FOX_DIdev[i], FOX_Window, level);
			IDirectInputDevice2_Acquire(FOX_DIdev[i]);
			
			if ( result != DI_OK ) 
			{
				SetDIerror(
			"DirectInputDevice::SetCooperativeLevel", result);
			}
		}
	}
	mouse_lost = 1;
}

// wyczysc directinput

static void DX8_DInputQuit(_THIS)
{
	int i;

	if ( dinput != NULL ) 
	{
		// zamknij i zwolnij wszystkie urzadzenia DirectInput
		
		for ( i=0; i<MAX_INPUTS; ++i ) 
		{
			if ( FOX_DIdev[i] != NULL ) 
			{
				IDirectInputDevice2_Unacquire(FOX_DIdev[i]);
				IDirectInputDevice2_SetEventNotification(
							FOX_DIdev[i], NULL);
			
				if ( FOX_DIevt[i] != NULL ) 
				{
					CloseHandle(FOX_DIevt[i]);
					FOX_DIevt[i] = NULL;
				}
				IDirectInputDevice2_Release(FOX_DIdev[i]);
				FOX_DIdev[i] = NULL;
			}
		}
		
		// zwolnij directinput
	
		IDirectInput_Release(dinput);
		dinput = NULL;
	}
}

// flaga czy potrzebujemy zdarzenia

static int posted = 0;

// wejsciowy handler dla zdarzen

static void handle_keyboard(const int numevents, DIDEVICEOBJECTDATA *keybuf)
{
	int i;
	FOX_keysym keysym;

	// konwersja wiadomosci klawiatury
	
	for ( i=0; i<numevents; ++i ) 
	{
		if ( keybuf[i].dwData & 0x80 ) 
		{
			posted = FOX_PrivateKeyboard(FOX_PRESSED,
				    TranslateKey(keybuf[i].dwOfs, &keysym, 1));
		} 
		else 
		{
			posted = FOX_PrivateKeyboard(FOX_RELEASED,
				    TranslateKey(keybuf[i].dwOfs, &keysym, 0));
		}
	}
}

static void handle_mouse(const int numevents, DIDEVICEOBJECTDATA *ptrbuf)
{
	int i;
	Sint16 xrel, yrel;
	Uint8 state;
	Uint8 button;

	// jezeli jestesmy w trybie okienka, windows sie bawi w obsluge
	
	if ( ! (FOX_PublicSurface->flags & FOX_FULLSCREEN) ) 
	{
		return;
	}
	
	// jezeli mysz utracona, sprawdz stany
	
	if ( mouse_lost ) 
	{
		POINT mouse_pos;
		Uint8 old_state;
		Uint8 new_state;
	
		// ustaw na aktualnej pozycji
		
		GetCursorPos(&mouse_pos);
		ScreenToClient(FOX_Window, &mouse_pos);
		posted = FOX_PrivateMouseMotion(0, 0,
				(Sint16)mouse_pos.x, (Sint16)mouse_pos.y);

		// sprawdz czy zmienil sie stan przyciskow
				
		old_state = FOX_GetMouseState(NULL, NULL);
		new_state = 0;
		{ 
			// nowy stan
			
			DIMOUSESTATE distate;
			HRESULT result;

			result=IDirectInputDevice2_GetDeviceState(FOX_DIdev[1],
						sizeof(distate), &distate);
			
			if ( result != DI_OK ) 
			{
				// sprobuj ponownie nastepnym razem
				
				SetDIerror(
				"IDirectInputDevice2::GetDeviceState", result);
				return;
			}
	
			for ( i=3; i>=0; --i ) 
			{
				if ( (distate.rgbButtons[i]&0x80) == 0x80 ) 
				{
					new_state |= 0x01;
				}
				new_state <<= 1;
			}
		}
		
		for ( i=0; i<8; ++i ) 
		{
			if ( (old_state&0x01) != (new_state&0x01) ) 
			{
				button = (Uint8)(i+1);
						
				if ( button == 2 ) 
				{
					button = 3;
				} 
				else
				if ( button == 3 ) 
				{
					button = 2;
				}
				if ( new_state & 0x01 ) 
				{
					// pobierz mysz
					if ( ++mouse_pressed > 0 ) 
					{
						SetCapture(FOX_Window);
					}
					state = FOX_PRESSED;
				} 
				else 
				{
					// zwolnij mysz
					
					if ( --mouse_pressed <= 0 ) 
					{
						ReleaseCapture();
						mouse_pressed = 0;
					}
					state = FOX_RELEASED;
				}
				posted = FOX_PrivateMouseButton(state, button,
									0, 0);
			}
			old_state >>= 1;
			new_state >>= 1;
		}
		mouse_lost = 0;
		return;
	}

	// przetlumacz wiadomosci myszy
	
	xrel = 0;
	yrel = 0;
	for ( i=0; i<(int)numevents; ++i ) 
	{
		switch (ptrbuf[i].dwOfs) {
			case DIMOFS_X:
				xrel += (Sint16)ptrbuf[i].dwData;
				break;
			case DIMOFS_Y:
				yrel += (Sint16)ptrbuf[i].dwData;
				break;
			case DIMOFS_Z:
				if ( xrel || yrel ) 
				{
					posted = FOX_PrivateMouseMotion(
							0, 1, xrel, yrel);
					xrel = 0;
					yrel = 0;
				}
				if((int)ptrbuf[i].dwData > 0)
					posted = FOX_PrivateMouseButton(
							FOX_PRESSED, 4, 0, 0);
				else if((int)ptrbuf[i].dwData < 0)
					posted = FOX_PrivateMouseButton(
							FOX_PRESSED, 5, 0, 0);
				break;
			case DIMOFS_BUTTON0:
			case DIMOFS_BUTTON1:
			case DIMOFS_BUTTON2:
			case DIMOFS_BUTTON3:
				if ( xrel || yrel ) 
				{
					posted = FOX_PrivateMouseMotion(
							0, 1, xrel, yrel);
					xrel = 0;
					yrel = 0;
				}
				button = (Uint8)(ptrbuf[i].dwOfs-DIMOFS_BUTTON0)+1;

				if ( button == 2 ) 
				{
					button = 3;
				} else
				
				if ( button == 3 ) 
				{
					button = 2;
				}
				
				if ( ptrbuf[i].dwData & 0x80 ) 
				{
					// pobierz mysz
					
					if ( ++mouse_pressed > 0 ) 
					{
						SetCapture(FOX_Window);
					}
					state = FOX_PRESSED;
				} else {
					
					if ( --mouse_pressed <= 0 ) 
					{
						ReleaseCapture();
						mouse_pressed = 0;
					}
					state = FOX_RELEASED;
				}
				posted = FOX_PrivateMouseButton(state, button,
									0, 0);
				break;
		}
	}
	if ( xrel || yrel ) 
	{
		posted = FOX_PrivateMouseMotion( 0, 1, xrel, yrel);
	}
}

// glowny handler zdarzen w32

LONG
 DX8_HandleMessage(_THIS, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_ACTIVATEAPP: {
			int i, active;

			active = (wParam && (GetForegroundWindow() == hwnd));
			if ( active ) 
			{
				for ( i=0; FOX_DIdev[i]; ++i ) 
				{
					IDirectInputDevice2_Acquire(
								FOX_DIdev[i]);
				}
			} 
			else 
			{
				for ( i=0; FOX_DIdev[i]; ++i ) 
				{
					IDirectInputDevice2_Unacquire(
								FOX_DIdev[i]);
				}
				mouse_lost = 1;
			}
		}
		break;

		case WM_DISPLAYCHANGE: {
			WORD BitsPerPixel;
			WORD SizeX, SizeY;

			// zmieniona bpp

			SizeX = LOWORD(lParam);
			SizeY = HIWORD(lParam);
			BitsPerPixel = wParam;
			
		}
		break;

		// klawiatura obslugiwana przez directinput
		
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			// klawisze windowsowe maja byc ignorowane przy pobieraniu wiadomosci
		}
		return(0);

		// screen savery nie moga sie wlaczyc

		case WM_SYSCOMMAND: 
		{
			if ((wParam&0xFFF0)==SC_SCREENSAVE || 
			    (wParam&0xFFF0)==SC_MONITORPOWER)
				return(0);
		}
		goto custom_processing;
		break;

		default: {
		custom_processing:
			// sprawdz tylko zdarzenia ktore nas interesuja
			
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

// ta funkcja sprawdza ktore zdarzenia przechodza przez kolejke directinput

static int DX8_CheckInput(_THIS, int timeout)
{
	MSG msg;
	int      i;
	HRESULT  result;
	DWORD    event;

	// sprawdz 

	posted = 0;
	while ( ! posted &&
	        PeekMessage(&msg, NULL, 0, (WM_APP-1), PM_NOREMOVE) ) 
	{
		if ( GetMessage(&msg, NULL, 0, (WM_APP-1)) > 0 ) 
		{
			DispatchMessage(&msg);
		} 
		else 
		{
			return(-1);
		}
	}
	if ( posted ) 
	{
		return(1);
	}

	// pchnij flagi dla directinput
	
	if ( FOX_GetAppState() & FOX_APPINPUTFOCUS ) 
	{
		for ( i=0; i<FOX_DIndev; ++i ) 
		{
			result = IDirectInputDevice2_Poll(FOX_DIdev[i]);
			if ( (result == DIERR_INPUTLOST) ||
					(result == DIERR_NOTACQUIRED) ) 
			{
				if ( strcmp(inputs[i].name, "mouse") == 0 ) 
				{
					mouse_lost = 1;
				}
				IDirectInputDevice2_Acquire(FOX_DIdev[i]);
				IDirectInputDevice2_Poll(FOX_DIdev[i]);
			}
		}
	}

	// czekaj na wiadomosc
	
	event = MsgWaitForMultipleObjects(FOX_DIndev, FOX_DIevt, FALSE,
							timeout, QS_ALLEVENTS);
	if ((event >= WAIT_OBJECT_0) && (event < (WAIT_OBJECT_0+FOX_DIndev))) 
	{
		DWORD numevents;
		DIDEVICEOBJECTDATA evtbuf[INPUT_QSIZE];

		event -= WAIT_OBJECT_0;
		numevents = INPUT_QSIZE;
		result = IDirectInputDevice2_GetDeviceData(
				FOX_DIdev[event], sizeof(DIDEVICEOBJECTDATA),
							evtbuf, &numevents, 0);
		if ( (result == DIERR_INPUTLOST) ||
					(result == DIERR_NOTACQUIRED) ) 
		{
			if ( strcmp(inputs[event].name, "mouse") == 0 ) 
			{
				mouse_lost = 1;
			}
			IDirectInputDevice2_Acquire(FOX_DIdev[event]);
			result = IDirectInputDevice2_GetDeviceData(
				FOX_DIdev[event], sizeof(DIDEVICEOBJECTDATA),
							evtbuf, &numevents, 0);
		}
		
		// obsluz zdarzenie
				
		if ( result == DI_OK ) 
		{
			// moze ich wystapic kilka
			
			(*FOX_DIfun[event])((int)numevents, evtbuf);
			return(1);
		}
	}
	
	if ( event != WAIT_TIMEOUT ) 
	{
		// moze to byla tylko wiadomosc od windowsa
		
		if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ) 
		{
			if ( GetMessage(&msg, NULL, 0, 0) > 0 ) 
			{
				DispatchMessage(&msg);
			} 
			else 
			{
				return(-1);
			}
			return(1);
		}
	}
	return(0);
}

void DX8_PumpEvents(_THIS)
{
	// czekaj na wiadomosc od directinput
		
	while ( DX8_CheckInput(this, 0) > 0 ) 
	{
		// tutaj petla powtarzajaca i czekajaca ponownie
	}
}

void DX8_InitOSKeymap(_THIS)
{
	int i;

	// konwertuj dik scancodes na fox
	
	for ( i=0; i<FOX_TABLESIZE(DIK_keymap); ++i )
		DIK_keymap[i] = 0;

	// zdefiniowane w dik
	
	DIK_keymap[DIK_ESCAPE] = FOXK_ESCAPE;
	DIK_keymap[DIK_1] = FOXK_1;
	DIK_keymap[DIK_2] = FOXK_2;
	DIK_keymap[DIK_3] = FOXK_3;
	DIK_keymap[DIK_4] = FOXK_4;
	DIK_keymap[DIK_5] = FOXK_5;
	DIK_keymap[DIK_6] = FOXK_6;
	DIK_keymap[DIK_7] = FOXK_7;
	DIK_keymap[DIK_8] = FOXK_8;
	DIK_keymap[DIK_9] = FOXK_9;
	DIK_keymap[DIK_0] = FOXK_0;
	DIK_keymap[DIK_MINUS] = FOXK_MINUS;
	DIK_keymap[DIK_EQUALS] = FOXK_EQUALS;
	DIK_keymap[DIK_BACK] = FOXK_BACKSPACE;
	DIK_keymap[DIK_TAB] = FOXK_TAB;
	DIK_keymap[DIK_Q] = FOXK_q;
	DIK_keymap[DIK_W] = FOXK_w;
	DIK_keymap[DIK_E] = FOXK_e;
	DIK_keymap[DIK_R] = FOXK_r;
	DIK_keymap[DIK_T] = FOXK_t;
	DIK_keymap[DIK_Y] = FOXK_y;
	DIK_keymap[DIK_U] = FOXK_u;
	DIK_keymap[DIK_I] = FOXK_i;
	DIK_keymap[DIK_O] = FOXK_o;
	DIK_keymap[DIK_P] = FOXK_p;
	DIK_keymap[DIK_LBRACKET] = FOXK_LEFTBRACKET;
	DIK_keymap[DIK_RBRACKET] = FOXK_RIGHTBRACKET;
	DIK_keymap[DIK_RETURN] = FOXK_RETURN;
	DIK_keymap[DIK_LCONTROL] = FOXK_LCTRL;
	DIK_keymap[DIK_A] = FOXK_a;
	DIK_keymap[DIK_S] = FOXK_s;
	DIK_keymap[DIK_D] = FOXK_d;
	DIK_keymap[DIK_F] = FOXK_f;
	DIK_keymap[DIK_G] = FOXK_g;
	DIK_keymap[DIK_H] = FOXK_h;
	DIK_keymap[DIK_J] = FOXK_j;
	DIK_keymap[DIK_K] = FOXK_k;
	DIK_keymap[DIK_L] = FOXK_l;
	DIK_keymap[DIK_SEMICOLON] = FOXK_SEMICOLON;
	DIK_keymap[DIK_APOSTROPHE] = FOXK_QUOTE;
	DIK_keymap[DIK_GRAVE] = FOXK_BACKQUOTE;
	DIK_keymap[DIK_LSHIFT] = FOXK_LSHIFT;
	DIK_keymap[DIK_BACKSLASH] = FOXK_BACKSLASH;
	DIK_keymap[DIK_Z] = FOXK_z;
	DIK_keymap[DIK_X] = FOXK_x;
	DIK_keymap[DIK_C] = FOXK_c;
	DIK_keymap[DIK_V] = FOXK_v;
	DIK_keymap[DIK_B] = FOXK_b;
	DIK_keymap[DIK_N] = FOXK_n;
	DIK_keymap[DIK_M] = FOXK_m;
	DIK_keymap[DIK_COMMA] = FOXK_COMMA;
	DIK_keymap[DIK_PERIOD] = FOXK_PERIOD;
	DIK_keymap[DIK_SLASH] = FOXK_SLASH;
	DIK_keymap[DIK_RSHIFT] = FOXK_RSHIFT;
	DIK_keymap[DIK_MULTIPLY] = FOXK_KP_MULTIPLY;
	DIK_keymap[DIK_LMENU] = FOXK_LALT;
	DIK_keymap[DIK_SPACE] = FOXK_SPACE;
	DIK_keymap[DIK_CAPITAL] = FOXK_CAPSLOCK;
	DIK_keymap[DIK_F1] = FOXK_F1;
	DIK_keymap[DIK_F2] = FOXK_F2;
	DIK_keymap[DIK_F3] = FOXK_F3;
	DIK_keymap[DIK_F4] = FOXK_F4;
	DIK_keymap[DIK_F5] = FOXK_F5;
	DIK_keymap[DIK_F6] = FOXK_F6;
	DIK_keymap[DIK_F7] = FOXK_F7;
	DIK_keymap[DIK_F8] = FOXK_F8;
	DIK_keymap[DIK_F9] = FOXK_F9;
	DIK_keymap[DIK_F10] = FOXK_F10;
	DIK_keymap[DIK_NUMLOCK] = FOXK_NUMLOCK;
	DIK_keymap[DIK_SCROLL] = FOXK_SCROLLOCK;
	DIK_keymap[DIK_NUMPAD7] = FOXK_KP7;
	DIK_keymap[DIK_NUMPAD8] = FOXK_KP8;
	DIK_keymap[DIK_NUMPAD9] = FOXK_KP9;
	DIK_keymap[DIK_SUBTRACT] = FOXK_KP_MINUS;
	DIK_keymap[DIK_NUMPAD4] = FOXK_KP4;
	DIK_keymap[DIK_NUMPAD5] = FOXK_KP5;
	DIK_keymap[DIK_NUMPAD6] = FOXK_KP6;
	DIK_keymap[DIK_ADD] = FOXK_KP_PLUS;
	DIK_keymap[DIK_NUMPAD1] = FOXK_KP1;
	DIK_keymap[DIK_NUMPAD2] = FOXK_KP2;
	DIK_keymap[DIK_NUMPAD3] = FOXK_KP3;
	DIK_keymap[DIK_NUMPAD0] = FOXK_KP0;
	DIK_keymap[DIK_DECIMAL] = FOXK_KP_PERIOD;
	DIK_keymap[DIK_F11] = FOXK_F11;
	DIK_keymap[DIK_F12] = FOXK_F12;

	DIK_keymap[DIK_F13] = FOXK_F13;
	DIK_keymap[DIK_F14] = FOXK_F14;
	DIK_keymap[DIK_F15] = FOXK_F15;

	DIK_keymap[DIK_NUMPADEQUALS] = FOXK_KP_EQUALS;
	DIK_keymap[DIK_NUMPADENTER] = FOXK_KP_ENTER;
	DIK_keymap[DIK_RCONTROL] = FOXK_RCTRL;
	DIK_keymap[DIK_DIVIDE] = FOXK_KP_DIVIDE;
	DIK_keymap[DIK_SYSRQ] = FOXK_SYSREQ;
	DIK_keymap[DIK_RMENU] = FOXK_RALT;
	DIK_keymap[DIK_HOME] = FOXK_HOME;
	DIK_keymap[DIK_UP] = FOXK_UP;
	DIK_keymap[DIK_PRIOR] = FOXK_PAGEUP;
	DIK_keymap[DIK_LEFT] = FOXK_LEFT;
	DIK_keymap[DIK_RIGHT] = FOXK_RIGHT;
	DIK_keymap[DIK_END] = FOXK_END;
	DIK_keymap[DIK_DOWN] = FOXK_DOWN;
	DIK_keymap[DIK_NEXT] = FOXK_PAGEDOWN;
	DIK_keymap[DIK_INSERT] = FOXK_INSERT;
	DIK_keymap[DIK_DELETE] = FOXK_DELETE;
	DIK_keymap[DIK_LWIN] = FOXK_LMETA;
	DIK_keymap[DIK_RWIN] = FOXK_RMETA;
	DIK_keymap[DIK_APPS] = FOXK_MENU;
}

static FOX_keysym *TranslateKey(UINT scancode, FOX_keysym *keysym, int pressed)
{
	// ustaw informacje dla keysym
	
	keysym->scancode = (unsigned char)scancode;
	keysym->sym = DIK_keymap[scancode];
	keysym->mod = KMOD_NONE;
	keysym->unicode = 0;
	
	if ( pressed && FOX_TranslateUNICODE ) 
	{
		UINT vkey;
		BYTE keystate[256];
		BYTE chars[2];

		vkey = MapVirtualKey(scancode, 1);
		GetKeyboardState(keystate);
		
		if ( ToAscii(vkey,scancode,keystate,(WORD *)chars,0) == 1 ) 
		{
			keysym->unicode = chars[0];
		}
	}
	return(keysym);
}

int DX8_CreateWindow(_THIS)
{
	int i;

	// wyczysc zmienne directinput 

	for ( i=0; i<MAX_INPUTS; ++i ) 
	{
		FOX_DIdev[i] = NULL;
		FOX_DIevt[i] = NULL;
		FOX_DIfun[i] = NULL;
	}

	// stworz okno 
	
	FOX_RegisterApp("FOX_app", CS_BYTEALIGNCLIENT, 0);

	if ( FOX_windowid ) 
	{
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
			FOX_SetError("[FOX]: nie moge stworzyc okna");
			return(-1);
		}
		ShowWindow(FOX_Window, SW_HIDE);
	}

	// zainicjalizuj directinput
	
	if ( DX8_DInputInit(this) < 0 ) 
	{
		return(-1);
	}
	
	return(0);
}

void DX8_DestroyWindow(_THIS)
{
	// zamknij directinput	
	
	DX8_DInputQuit(this);

	// zniszcz nasze okno

	DestroyWindow(FOX_Window);
}


// end
