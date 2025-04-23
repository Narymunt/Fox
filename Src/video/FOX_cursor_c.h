// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:


#include "FOX_mouse.h"

extern int  FOX_CursorInit(Uint32 flags);
extern void FOX_CursorPaletteChanged(void);
extern void FOX_DrawCursor(FOX_Surface *screen);
extern void FOX_DrawCursorNoLock(FOX_Surface *screen);
extern void FOX_EraseCursor(FOX_Surface *screen);
extern void FOX_EraseCursorNoLock(FOX_Surface *screen);
extern void FOX_UpdateCursor(FOX_Surface *screen);
extern void FOX_ResetCursor(void);
extern void FOX_MoveCursor(int x, int y);
extern void FOX_CursorQuit(void);

#define INLINE_MOUSELOCK
#ifdef INLINE_MOUSELOCK

// blokowanie

#include "FOX_mutex.h"

extern FOX_mutex *FOX_cursorlock;

#define FOX_LockCursor()						\
	do {								\
		if ( FOX_cursorlock ) {					\
			FOX_mutexP(FOX_cursorlock);			\
		}							\
	} while ( 0 )
#define FOX_UnlockCursor()						\
	do {								\
		if ( FOX_cursorlock ) {					\
			FOX_mutexV(FOX_cursorlock);			\
		}							\
	} while ( 0 )
#else
extern void FOX_LockCursor(void);
extern void FOX_UnlockCursor(void);
#endif /* INLINE_MOUSELOCK */

// tylko niskopoziomowe

extern FOX_Cursor *FOX_cursor;
extern void FOX_MouseRect(FOX_Rect *area);

// definicja stanu

#define CURSOR_VISIBLE	0x01
#define CURSOR_USINGSW	0x10
#define SHOULD_DRAWCURSOR(X) 						\
			(((X)&(CURSOR_VISIBLE|CURSOR_USINGSW)) ==  	\
					(CURSOR_VISIBLE|CURSOR_USINGSW))

extern volatile int FOX_cursorstate;

// end

