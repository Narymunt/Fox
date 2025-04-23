// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:

// glowna obsluga klawiatury 

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_events.h"
#include "FOX_timer.h"
#include "FOX_events_c.h"
#include "FOX_sysevents.h"

// informacja o stanie klawiszy

static Uint8  FOX_KeyState[FOXK_LAST];
static FOXMod FOX_ModState;

int FOX_TranslateUNICODE = 0;

// tablica z przekonwertowanymi znakami

static char *keynames[FOXK_LAST];	

// powtarzanie

struct {
	
	int firsttime;    // sprawdzamy opoznienie
	int delay;        // opoznienie przed powtarzaniem 
	int interval;     // opoznienie pomiedzy klawiszami
	
	Uint32 timestamp; // kiedy po raz pierwszy nacisniete

	FOX_Event evt;    // zdarzenie do powtarzania

} FOX_KeyRepeat;

// funkcje publiczne 

int FOX_KeyboardInit(void)
{

	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	
	Uint16 i;

	// uzywaj konwersji unicode
	
	FOX_EnableUNICODE(DEFAULT_UNICODE_TRANSLATION);

	// inicjalizuj tablice 
	
	FOX_ModState = KMOD_NONE;
	
	for ( i=0; i<FOX_TABLESIZE(keynames); ++i )
		keynames[i] = NULL;
	
	for ( i=0; i<FOX_TABLESIZE(FOX_KeyState); ++i )
		FOX_KeyState[i] = FOX_RELEASED;
	
	video->InitOSKeymap(this);

	FOX_EnableKeyRepeat(0, 0);

	// pozostale 
	
	keynames[FOXK_BACKSPACE] = "backspace";
	keynames[FOXK_TAB] = "tab";
	keynames[FOXK_CLEAR] = "clear";
	keynames[FOXK_RETURN] = "return";
	keynames[FOXK_PAUSE] = "pause";
	keynames[FOXK_ESCAPE] = "escape";
	keynames[FOXK_SPACE] = "space";
	keynames[FOXK_EXCLAIM]  = "!";
	keynames[FOXK_QUOTEDBL]  = "\"";
	keynames[FOXK_HASH]  = "#";
	keynames[FOXK_DOLLAR]  = "$";
	keynames[FOXK_AMPERSAND]  = "&";
	keynames[FOXK_QUOTE] = "'";
	keynames[FOXK_LEFTPAREN] = "(";
	keynames[FOXK_RIGHTPAREN] = ")";
	keynames[FOXK_ASTERISK] = "*";
	keynames[FOXK_PLUS] = "+";
	keynames[FOXK_COMMA] = ",";
	keynames[FOXK_MINUS] = "-";
	keynames[FOXK_PERIOD] = ".";
	keynames[FOXK_SLASH] = "/";
	keynames[FOXK_0] = "0";
	keynames[FOXK_1] = "1";
	keynames[FOXK_2] = "2";
	keynames[FOXK_3] = "3";
	keynames[FOXK_4] = "4";
	keynames[FOXK_5] = "5";
	keynames[FOXK_6] = "6";
	keynames[FOXK_7] = "7";
	keynames[FOXK_8] = "8";
	keynames[FOXK_9] = "9";
	keynames[FOXK_COLON] = ":";
	keynames[FOXK_SEMICOLON] = ";";
	keynames[FOXK_LESS] = "<";
	keynames[FOXK_EQUALS] = "=";
	keynames[FOXK_GREATER] = ">";
	keynames[FOXK_QUESTION] = "?";
	keynames[FOXK_AT] = "@";
	keynames[FOXK_LEFTBRACKET] = "[";
	keynames[FOXK_BACKSLASH] = "\\";
	keynames[FOXK_RIGHTBRACKET] = "]";
	keynames[FOXK_CARET] = "^";
	keynames[FOXK_UNDERSCORE] = "_";
	keynames[FOXK_BACKQUOTE] = "`";
	keynames[FOXK_a] = "a";
	keynames[FOXK_b] = "b";
	keynames[FOXK_c] = "c";
	keynames[FOXK_d] = "d";
	keynames[FOXK_e] = "e";
	keynames[FOXK_f] = "f";
	keynames[FOXK_g] = "g";
	keynames[FOXK_h] = "h";
	keynames[FOXK_i] = "i";
	keynames[FOXK_j] = "j";
	keynames[FOXK_k] = "k";
	keynames[FOXK_l] = "l";
	keynames[FOXK_m] = "m";
	keynames[FOXK_n] = "n";
	keynames[FOXK_o] = "o";
	keynames[FOXK_p] = "p";
	keynames[FOXK_q] = "q";
	keynames[FOXK_r] = "r";
	keynames[FOXK_s] = "s";
	keynames[FOXK_t] = "t";
	keynames[FOXK_u] = "u";
	keynames[FOXK_v] = "v";
	keynames[FOXK_w] = "w";
	keynames[FOXK_x] = "x";
	keynames[FOXK_y] = "y";
	keynames[FOXK_z] = "z";
	keynames[FOXK_DELETE] = "delete";

	keynames[FOXK_WORLD_0] = "world 0";
	keynames[FOXK_WORLD_1] = "world 1";
	keynames[FOXK_WORLD_2] = "world 2";
	keynames[FOXK_WORLD_3] = "world 3";
	keynames[FOXK_WORLD_4] = "world 4";
	keynames[FOXK_WORLD_5] = "world 5";
	keynames[FOXK_WORLD_6] = "world 6";
	keynames[FOXK_WORLD_7] = "world 7";
	keynames[FOXK_WORLD_8] = "world 8";
	keynames[FOXK_WORLD_9] = "world 9";
	keynames[FOXK_WORLD_10] = "world 10";
	keynames[FOXK_WORLD_11] = "world 11";
	keynames[FOXK_WORLD_12] = "world 12";
	keynames[FOXK_WORLD_13] = "world 13";
	keynames[FOXK_WORLD_14] = "world 14";
	keynames[FOXK_WORLD_15] = "world 15";
	keynames[FOXK_WORLD_16] = "world 16";
	keynames[FOXK_WORLD_17] = "world 17";
	keynames[FOXK_WORLD_18] = "world 18";
	keynames[FOXK_WORLD_19] = "world 19";
	keynames[FOXK_WORLD_20] = "world 20";
	keynames[FOXK_WORLD_21] = "world 21";
	keynames[FOXK_WORLD_22] = "world 22";
	keynames[FOXK_WORLD_23] = "world 23";
	keynames[FOXK_WORLD_24] = "world 24";
	keynames[FOXK_WORLD_25] = "world 25";
	keynames[FOXK_WORLD_26] = "world 26";
	keynames[FOXK_WORLD_27] = "world 27";
	keynames[FOXK_WORLD_28] = "world 28";
	keynames[FOXK_WORLD_29] = "world 29";
	keynames[FOXK_WORLD_30] = "world 30";
	keynames[FOXK_WORLD_31] = "world 31";
	keynames[FOXK_WORLD_32] = "world 32";
	keynames[FOXK_WORLD_33] = "world 33";
	keynames[FOXK_WORLD_34] = "world 34";
	keynames[FOXK_WORLD_35] = "world 35";
	keynames[FOXK_WORLD_36] = "world 36";
	keynames[FOXK_WORLD_37] = "world 37";
	keynames[FOXK_WORLD_38] = "world 38";
	keynames[FOXK_WORLD_39] = "world 39";
	keynames[FOXK_WORLD_40] = "world 40";
	keynames[FOXK_WORLD_41] = "world 41";
	keynames[FOXK_WORLD_42] = "world 42";
	keynames[FOXK_WORLD_43] = "world 43";
	keynames[FOXK_WORLD_44] = "world 44";
	keynames[FOXK_WORLD_45] = "world 45";
	keynames[FOXK_WORLD_46] = "world 46";
	keynames[FOXK_WORLD_47] = "world 47";
	keynames[FOXK_WORLD_48] = "world 48";
	keynames[FOXK_WORLD_49] = "world 49";
	keynames[FOXK_WORLD_50] = "world 50";
	keynames[FOXK_WORLD_51] = "world 51";
	keynames[FOXK_WORLD_52] = "world 52";
	keynames[FOXK_WORLD_53] = "world 53";
	keynames[FOXK_WORLD_54] = "world 54";
	keynames[FOXK_WORLD_55] = "world 55";
	keynames[FOXK_WORLD_56] = "world 56";
	keynames[FOXK_WORLD_57] = "world 57";
	keynames[FOXK_WORLD_58] = "world 58";
	keynames[FOXK_WORLD_59] = "world 59";
	keynames[FOXK_WORLD_60] = "world 60";
	keynames[FOXK_WORLD_61] = "world 61";
	keynames[FOXK_WORLD_62] = "world 62";
	keynames[FOXK_WORLD_63] = "world 63";
	keynames[FOXK_WORLD_64] = "world 64";
	keynames[FOXK_WORLD_65] = "world 65";
	keynames[FOXK_WORLD_66] = "world 66";
	keynames[FOXK_WORLD_67] = "world 67";
	keynames[FOXK_WORLD_68] = "world 68";
	keynames[FOXK_WORLD_69] = "world 69";
	keynames[FOXK_WORLD_70] = "world 70";
	keynames[FOXK_WORLD_71] = "world 71";
	keynames[FOXK_WORLD_72] = "world 72";
	keynames[FOXK_WORLD_73] = "world 73";
	keynames[FOXK_WORLD_74] = "world 74";
	keynames[FOXK_WORLD_75] = "world 75";
	keynames[FOXK_WORLD_76] = "world 76";
	keynames[FOXK_WORLD_77] = "world 77";
	keynames[FOXK_WORLD_78] = "world 78";
	keynames[FOXK_WORLD_79] = "world 79";
	keynames[FOXK_WORLD_80] = "world 80";
	keynames[FOXK_WORLD_81] = "world 81";
	keynames[FOXK_WORLD_82] = "world 82";
	keynames[FOXK_WORLD_83] = "world 83";
	keynames[FOXK_WORLD_84] = "world 84";
	keynames[FOXK_WORLD_85] = "world 85";
	keynames[FOXK_WORLD_86] = "world 86";
	keynames[FOXK_WORLD_87] = "world 87";
	keynames[FOXK_WORLD_88] = "world 88";
	keynames[FOXK_WORLD_89] = "world 89";
	keynames[FOXK_WORLD_90] = "world 90";
	keynames[FOXK_WORLD_91] = "world 91";
	keynames[FOXK_WORLD_92] = "world 92";
	keynames[FOXK_WORLD_93] = "world 93";
	keynames[FOXK_WORLD_94] = "world 94";
	keynames[FOXK_WORLD_95] = "world 95";

	keynames[FOXK_KP0] = "[0]";
	keynames[FOXK_KP1] = "[1]";
	keynames[FOXK_KP2] = "[2]";
	keynames[FOXK_KP3] = "[3]";
	keynames[FOXK_KP4] = "[4]";
	keynames[FOXK_KP5] = "[5]";
	keynames[FOXK_KP6] = "[6]";
	keynames[FOXK_KP7] = "[7]";
	keynames[FOXK_KP8] = "[8]";
	keynames[FOXK_KP9] = "[9]";
	keynames[FOXK_KP_PERIOD] = "[.]";
	keynames[FOXK_KP_DIVIDE] = "[/]";
	keynames[FOXK_KP_MULTIPLY] = "[*]";
	keynames[FOXK_KP_MINUS] = "[-]";
	keynames[FOXK_KP_PLUS] = "[+]";
	keynames[FOXK_KP_ENTER] = "enter";
	keynames[FOXK_KP_EQUALS] = "equals";

	keynames[FOXK_UP] = "up";
	keynames[FOXK_DOWN] = "down";
	keynames[FOXK_RIGHT] = "right";
	keynames[FOXK_LEFT] = "left";
	keynames[FOXK_DOWN] = "down";
	keynames[FOXK_INSERT] = "insert";
	keynames[FOXK_HOME] = "home";
	keynames[FOXK_END] = "end";
	keynames[FOXK_PAGEUP] = "page up";
	keynames[FOXK_PAGEDOWN] = "page down";

	keynames[FOXK_F1] = "f1";
	keynames[FOXK_F2] = "f2";
	keynames[FOXK_F3] = "f3";
	keynames[FOXK_F4] = "f4";
	keynames[FOXK_F5] = "f5";
	keynames[FOXK_F6] = "f6";
	keynames[FOXK_F7] = "f7";
	keynames[FOXK_F8] = "f8";
	keynames[FOXK_F9] = "f9";
	keynames[FOXK_F10] = "f10";
	keynames[FOXK_F11] = "f11";
	keynames[FOXK_F12] = "f12";
	keynames[FOXK_F13] = "f13";
	keynames[FOXK_F14] = "f14";
	keynames[FOXK_F15] = "f15";

	keynames[FOXK_NUMLOCK] = "numlock";
	keynames[FOXK_CAPSLOCK] = "caps lock";
	keynames[FOXK_SCROLLOCK] = "scroll lock";
	keynames[FOXK_RSHIFT] = "right shift";
	keynames[FOXK_LSHIFT] = "left shift";
	keynames[FOXK_RCTRL] = "right ctrl";
	keynames[FOXK_LCTRL] = "left ctrl";
	keynames[FOXK_RALT] = "right alt";
	keynames[FOXK_LALT] = "left alt";
	keynames[FOXK_RMETA] = "right meta";
	keynames[FOXK_LMETA] = "left meta";
	keynames[FOXK_LSUPER] = "left super";	// klawisze windowsowe
	keynames[FOXK_RSUPER] = "right super";	
	keynames[FOXK_MODE] = "alt gr";
	keynames[FOXK_COMPOSE] = "compose";

	keynames[FOXK_HELP] = "help";
	keynames[FOXK_PRINT] = "print screen";
	keynames[FOXK_SYSREQ] = "sys req";
	keynames[FOXK_BREAK] = "break";
	keynames[FOXK_MENU] = "menu";
	keynames[FOXK_POWER] = "power";
	keynames[FOXK_EURO] = "euro";
	keynames[FOXK_UNDO] = "undo";
	
	return(0);
}

// brak klawiaturki ;) 

void FOX_ResetKeyboard(void)
{
	FOX_keysym keysym;
	FOXKey key;

	memset(&keysym, 0, (sizeof keysym));

	for ( key=FOXK_FIRST; key<FOXK_LAST; ++key ) 
	{
		if ( FOX_KeyState[key] == FOX_PRESSED ) 
		{
			keysym.sym = key;
			FOX_PrivateKeyboard(FOX_RELEASED, &keysym);
		}
	}
	FOX_KeyRepeat.timestamp = 0;
}

// odblokuj konwersje 

int FOX_EnableUNICODE(int enable)
{
	int old_mode;

	old_mode = FOX_TranslateUNICODE;
	
	if ( enable >= 0 ) 
	{
		FOX_TranslateUNICODE = enable;
	}
	
	return(old_mode);
}

// podaj stan klawiszy 

Uint8 * FOX_GetKeyState (int *numkeys)
{
	if ( numkeys != (int *)0 )
		*numkeys = FOXK_LAST;
	return(FOX_KeyState);
}

// podaj stan modyfikatorow shft itd

FOXMod FOX_GetModState (void)
{
	return(FOX_ModState);
}

// ustaw modyfikator 

void FOX_SetModState (FOXMod modstate)
{
	FOX_ModState = modstate;
}

// podaj nazwe klawisza 

char *FOX_GetKeyName(FOXKey key)
{
	char *keyname;

	keyname = NULL;
	if ( key < FOXK_LAST ) 
	{
		keyname = keynames[key];
	}
	
	if ( keyname == NULL ) 
	{
		keyname = "unknown key";
	}
	return(keyname);
}

// globalne dla FOX_eventloop.c

int FOX_PrivateKeyboard(Uint8 state, FOX_keysym *keysym)
{
	FOX_Event event;
	int posted, repeatable;
	Uint16 modstate;

	memset(&event, 0, sizeof(event));

#if 0
printf("[FOX]: klawisz '%s' %s\n", FOX_GetKeyName(keysym->sym), 
				state == FOX_PRESSED ? "nacisniety" : "zwolniony");
#endif
	
	// ustawienia dla keysym

	modstate = (Uint16)FOX_ModState;

	repeatable = 0;

	if ( state == FOX_PRESSED ) 
	{
		keysym->mod = (FOXMod)modstate;
		switch (keysym->sym) 
		{
			case FOXK_NUMLOCK:
				modstate ^= KMOD_NUM;
				if ( ! (modstate&KMOD_NUM) )
					state = FOX_RELEASED;
				keysym->mod = (FOXMod)modstate;
				break;
			case FOXK_CAPSLOCK:
				modstate ^= KMOD_CAPS;
				if ( ! (modstate&KMOD_CAPS) )
					state = FOX_RELEASED;
				keysym->mod = (FOXMod)modstate;
				break;
			case FOXK_LCTRL:
				modstate |= KMOD_LCTRL;
				break;
			case FOXK_RCTRL:
				modstate |= KMOD_RCTRL;
				break;
			case FOXK_LSHIFT:
				modstate |= KMOD_LSHIFT;
				break;
			case FOXK_RSHIFT:
				modstate |= KMOD_RSHIFT;
				break;
			case FOXK_LALT:
				modstate |= KMOD_LALT;
				break;
			case FOXK_RALT:
				modstate |= KMOD_RALT;
				break;
			case FOXK_LMETA:
				modstate |= KMOD_LMETA;
				break;
			case FOXK_RMETA:
				modstate |= KMOD_RMETA;
				break;
			case FOXK_MODE:
				modstate |= KMOD_MODE;
				break;
			default:
				repeatable = 1;
				break;
		}
	} 
	else 
	{
		switch (keysym->sym) 
		{
			case FOXK_NUMLOCK:
			case FOXK_CAPSLOCK:
				// tylko dla klawiszy wysylajacych
				return(0);
			case FOXK_LCTRL:
				modstate &= ~KMOD_LCTRL;
				break;
			case FOXK_RCTRL:
				modstate &= ~KMOD_RCTRL;
				break;
			case FOXK_LSHIFT:
				modstate &= ~KMOD_LSHIFT;
				break;
			case FOXK_RSHIFT:
				modstate &= ~KMOD_RSHIFT;
				break;
			case FOXK_LALT:
				modstate &= ~KMOD_LALT;
				break;
			case FOXK_RALT:
				modstate &= ~KMOD_RALT;
				break;
			case FOXK_LMETA:
				modstate &= ~KMOD_LMETA;
				break;
			case FOXK_RMETA:
				modstate &= ~KMOD_RMETA;
				break;
			case FOXK_MODE:
				modstate &= ~KMOD_MODE;
				break;
			default:
				break;
		}
		keysym->mod = (FOXMod)modstate;
	}

	// jakie to jest zdarzenie
		
	switch (state) 
	{
		case FOX_PRESSED:
			event.type = FOX_KEYDOWN;
			break;
		case FOX_RELEASED:
			event.type = FOX_KEYUP;
	
			if ( FOX_KeyRepeat.timestamp ) 
			{
				FOX_KeyRepeat.timestamp = 0;
			}
			break;
		default:
			
			// zly, albo nieznany stan
			
			return(0);
	}

	// porzuc zdarzenie i nie zmieniaj stanu
	
	if ( FOX_KeyState[keysym->sym] == state ) 
	{
		return(0);
	}

	// aktualizuj stan klawiatury
	
	FOX_ModState = (FOXMod)modstate;
	FOX_KeyState[keysym->sym] = state;

	// pchnij zdarzenie jezeli potrzebne 
	
	posted = 0;

	if ( FOX_ProcessEvents[event.type] == FOX_ENABLE ) 
	{
		event.key.state = state;
		event.key.keysym = *keysym;
	
		if (repeatable && (FOX_KeyRepeat.delay != 0)) 
		{
			FOX_KeyRepeat.evt = event;
			FOX_KeyRepeat.firsttime = 1;
			FOX_KeyRepeat.timestamp=FOX_GetTicks();
		}
		
		if ( (FOX_EventOK == NULL) || FOX_EventOK(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	return(posted);
}

// sprawdz czy klawisz przycisniety i znak sie powtarza

void FOX_CheckKeyRepeat(void)
{
	if ( FOX_KeyRepeat.timestamp ) 
	{
		Uint32 now, interval;

		now = FOX_GetTicks();
		interval = (now - FOX_KeyRepeat.timestamp);
	
		if ( FOX_KeyRepeat.firsttime ) 
		{
			if ( interval > (Uint32)FOX_KeyRepeat.delay ) 
			{
				FOX_KeyRepeat.timestamp = now;
				FOX_KeyRepeat.firsttime = 0;
			}
		} 
		else 
		{
			if ( interval > (Uint32)FOX_KeyRepeat.interval ) 
			{
				FOX_KeyRepeat.timestamp = now;
				if ( (FOX_EventOK == NULL) || FOX_EventOK(&FOX_KeyRepeat.evt) ) 
				{
					FOX_PushEvent(&FOX_KeyRepeat.evt);
				}
			}
		}
	}
}

// odblokuj powtarzanie 

int FOX_EnableKeyRepeat(int delay, int interval)
{
	if ( (delay < 0) || (interval < 0) ) 
	{
		FOX_SetError("[FOX]: wartosc powtarzania klawisza mniejsza od 0 !!! to niemozliwe !!!");
		return(-1);
	}
	FOX_KeyRepeat.firsttime = 0;
	FOX_KeyRepeat.delay = delay;
	FOX_KeyRepeat.interval = interval;
	FOX_KeyRepeat.timestamp = 0;
	return(0);
}

// end

