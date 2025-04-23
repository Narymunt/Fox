// Fox v0.5 
// by Jaroslaw Rozynski
// TODO:
// - kolorowy kursor 

// obsluga myszy

#ifndef _FOX_mouse_h
#define _FOX_mouse_h

#include "FOX_types.h"
#include "FOX_video.h"

// poczatkowe smieci

#include "FOX_begin.h"

// funkcje jako C

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WMcursor WMcursor;	// zaleznie od implementacji

typedef struct {

	FOX_Rect area;			// obszar kursora myszy

	Sint16 hot_x, hot_y;	// goracy punkt

	Uint8 *data;			// B/W data
	Uint8 *mask;			// B/W mask
	Uint8 *save[2];			// miejsce kopii

	WMcursor *wm_cursor;	// windows manager kursor

} FOX_Cursor;

// podaj stan myszy

extern DECLSPEC Uint8 FOX_GetMouseState(int *x, int *y);

// podaj relatywne pozycje myszy

extern DECLSPEC Uint8 FOX_GetRelativeMouseState(int *x, int *y);

// ustaw wspolrzedne myszy

extern DECLSPEC void FOX_WarpMouse(Uint16 x, Uint16 y);

// nowy kursor, uzycie maski
// data  mask    resulting pixel on screen
//   0     1       White
//   1     1       Black
//   0     0       Transparent
//   1     0       Inverted color if possible, black if not.
 
extern DECLSPEC FOX_Cursor *FOX_CreateCursor
		(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);

// zmiana kursora

extern DECLSPEC void FOX_SetCursor(FOX_Cursor *cursor);

// zwraca aktualny kursor

extern DECLSPEC FOX_Cursor * FOX_GetCursor(void);

// zwalnia miejsce po FOX_CreateCursor()

extern DECLSPEC void FOX_FreeCursor(FOX_Cursor *cursor);

// pokaz kursor, 1 jezeli wyswietlony, 0 jezeli nie
// -1 zmienia stan

extern DECLSPEC int FOX_ShowCursor(int toggle);

// maski do sprawdzania myszy
// 1 lewy
// 2 srodkowy
// 3 prawy

#define FOX_BUTTON(X)		(FOX_PRESSED<<(X-1))
#define FOX_BUTTON_LEFT		1
#define FOX_BUTTON_MIDDLE	2
#define FOX_BUTTON_RIGHT	3
#define FOX_BUTTON_LMASK	FOX_BUTTON(FOX_BUTTON_LEFT)
#define FOX_BUTTON_MMASK	FOX_BUTTON(FOX_BUTTON_MIDDLE)
#define FOX_BUTTON_RMASK	FOX_BUTTON(FOX_BUTTON_RIGHT)

// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koniec smieci

#include "FOX_end.h"

#endif 

// end

