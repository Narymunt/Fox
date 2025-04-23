// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO: 


// operacje do rysowania, itd. 

#ifndef _FOX_video_h
#define _FOX_video_h

#include <stdio.h>

#include "FOX_types.h"		// typy, skroty, itd
#include "FOX_mutex.h"		// mutex
#include "FOX_rwops.h"		// read write 

// poczatkowe smieci

#include "FOX_begin.h"

// traktuj funkcje jako c

#ifdef __cplusplus
extern "C" {
#endif

// definicje przezroczystosci, tj, alpha

#define FOX_ALPHA_OPAQUE 255
#define FOX_ALPHA_TRANSPARENT 0

// rectangle czyli prostokat

typedef struct {

	Sint16 x, y;

	Uint16 w, h;

} FOX_Rect;

// color, !!! skladowa 8 bitowa !!!
// caly czas jest typu unsigned 

typedef struct {

	Uint8 r;	// unsigned char
	Uint8 g;
	Uint8 b;

	Uint8 unused;		// tutaj jakies dodatkowe 

} FOX_Color;

// paleta kolorow, moze byc 256 np dla 8bpp, ale tez
// moze byc np 1024, dla 14 bpp czyli fixowana

typedef struct {

	int       ncolors;
	FOX_Color *colors;

} FOX_Palette;

// wszystko w tej strukturze jest tylko do odczytu
// tutaj dodajemy jakies inne filtry

typedef struct FOX_PixelFormat {

	FOX_Palette *palette;

	Uint8  BitsPerPixel;

	Uint8  BytesPerPixel;

	Uint8  Rloss;		
	Uint8  Gloss;
	Uint8  Bloss;
	Uint8  Aloss;

	Uint8  Rshift;
	Uint8  Gshift;
	Uint8  Bshift;
	Uint8  Ashift;

	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	Uint32 Amask;
	
	Uint32 colorkey;	// informacja o rgb dla klucza
	
	Uint8  alpha;		// alpha

} FOX_PixelFormat;

// kopiowanie surface

struct FOX_Surface;

typedef int (*FOX_blit)(struct FOX_Surface *src, FOX_Rect *srcrect,
			struct FOX_Surface *dst, FOX_Rect *dstrect);

// struktura praktycznie tylko do odczytu za wyjatkiem pixels
// calos balansuje pomiedzy directx

typedef struct FOX_Surface {

	Uint32 flags;				// tylko do odczytu
	
	FOX_PixelFormat *format;	// tylko do odczytu
	
	int w, h;					// tylko do odczytu

	Uint16 pitch;				// tylko do odczytu

	void *pixels;				// odczyt i zapis, 

	int offset;					// nie ruszac !!!

	struct private_hwdata *hwdata;	// informacja o surface charakterystyczna dla sprzetu
									// zwraca info directx
	// clipping

	FOX_Rect clip_rect;			// tylko do odczytu ustawiamy inna funkcja
	Uint32 unused1;				// wypelnienie dla binarki

	// blokowanie surface 
	
	Uint32 locked;				// prywatny, nie zapisywac !
	
	struct FOX_BlitMap *map;	// szybki flip na inny surface

	unsigned int format_version;	// wesja formatu dla blitowania
	
	int refcount;				// referencja przy zwalnianiu surface

} FOX_Surface;

// w jaki sposob ma byc stworzony surface, niektore flagi wykluczaja sie nawzajem !

#define FOX_SWSURFACE	0x00000000	// surface w system memory
#define FOX_HWSURFACE	0x00000001	// surface w video memory (moze sie nie zmiescic)
#define FOX_ASYNCBLIT	0x00000004	// blit bez synchro 

#define FOX_ANYFORMAT	0x10000000	// ustaw dowolna najblizsza rozdzielczosc
#define FOX_HWPALETTE	0x20000000	// zaerzerwuj palete dla surface
#define FOX_DOUBLEBUF	0x40000000	// double buffor, jest plynniej ale wolniej
#define FOX_FULLSCREEN	0x80000000	// fullscreen czy okno

#define FOX_OPENGL      0x00000002  // czy odblokowac opengl
#define FOX_OPENGLBLIT	0x0000000A	// uzywaj opengl do flipowania
#define FOX_RESIZABLE	0x00000010	// czy tryb video moze sie zmieniac
#define FOX_NOFRAME		0x00000020	// bez ramki kiedy to jest okno

// tylko do odczytu praktycznie

#define FOX_HWACCEL		0x00000100	// czy blit jest z akceleracja
#define FOX_SRCCOLORKEY	0x00001000	// blit z sck ? 
#define FOX_RLEACCELOK	0x00002000	// flaga priv
#define FOX_RLEACCEL	0x00004000	// surface jest kodowany w rle
#define FOX_SRCALPHA	0x00010000	// uzywamy alpha blending 
#define FOX_PREALLOC	0x01000000	// preallokujemy pamiec 

// true jezeli surface musi byc zablokowany

#define FOX_MUSTLOCK(surface)	\
  (surface->offset ||		\
  ((surface->flags & (FOX_HWSURFACE|FOX_ASYNCBLIT|FOX_RLEACCEL)) != 0))

// przydatne przy sprawdzaniu co nam sprzet oferuje
// ale moze byc mylace - directx czasem wstawia, ze cos takiego jest, a naprawde
// nie ma tylko jest emulacja

typedef struct {

	Uint32 hw_available :1;	// mozemy stworzyc hardware surface ? 
	Uint32 wm_available :1;	// mozna przekazywac info do windows ? 

	Uint32 UnusedBits1  :6;
	Uint32 UnusedBits2  :1;

	Uint32 blit_hw      :1;	// akcelerowany blit hw:hw
	Uint32 blit_hw_CC   :1;	// akcelerowany blit z ck
	Uint32 blit_hw_A    :1;	// akcelerowany blit z alpha

	Uint32 blit_sw      :1;	// akcelerowany blit sw:hw
	Uint32 blit_sw_CC   :1;	// akcelerowany blit z ck
	Uint32 blit_sw_A    :1;	// akcelerowany blit z alpha

	Uint32 blit_fill    :1;	// akcelerowane wypelnianie

	Uint32 UnusedBits3  :16;

	Uint32 video_mem;		// ile pamieci na karcie

	FOX_PixelFormat *vfmt;	// format surface

} FOX_VideoInfo;

// konwersje na YUV

#define FOX_YV12_OVERLAY  0x32315659	// planar Y+V+U (3)
#define FOX_IYUV_OVERLAY  0x56555949	// planar Y+U+V (3)
#define FOX_YUY2_OVERLAY  0x32595559	// packed Y0+U0+Y1+V0 (1) 
#define FOX_UYVY_OVERLAY  0x59565955	// packed U0+Y0+V0+Y1 (1)
#define FOX_YVYU_OVERLAY  0x55595659	// packed Y0+V0+Y1+U0 (1) 

// struktura dla YUV jako nakladki

typedef struct FOX_Overlay {

	Uint32 format;				// tylko do odczytu

	int w, h;					// tylko do odczytu, ustawia inna funkcja
	int planes;					// tylko do odczytu
	Uint16 *pitches;			// tylko do odczytu
	Uint8 **pixels;				// odczyt i zapis, traktowac tak samo jak surface

	// info wypelniane ze sprzetu
	
	struct private_yuvhwfuncs *hwfuncs;
	struct private_yuvhwdata *hwdata;
	
	Uint32 hw_overlay :1;		// czy ta nakladka moze byc akcelerowana

	Uint32 UnusedBits :31;

} FOX_Overlay;

// ustawienia dla okna opengl 
// !!! nie zmieniac !!!

typedef enum {
    FOX_GL_RED_SIZE,
    FOX_GL_GREEN_SIZE,
    FOX_GL_BLUE_SIZE,
    FOX_GL_ALPHA_SIZE,
    FOX_GL_BUFFER_SIZE,
    FOX_GL_DOUBLEBUFFER,
    FOX_GL_DEPTH_SIZE,
    FOX_GL_STENCIL_SIZE,
    FOX_GL_ACCUM_RED_SIZE,
    FOX_GL_ACCUM_GREEN_SIZE,
    FOX_GL_ACCUM_BLUE_SIZE,
    FOX_GL_ACCUM_ALPHA_SIZE
} FOX_GLattr;

// flagi dla FOX_SetPalette()

#define FOX_LOGPAL 0x01
#define FOX_PHYSPAL 0x02

// tutaj przekazujemy flagi

extern DECLSPEC int FOX_VideoInit(const char *driver_name, Uint32 flags);

// po prostu zamkniecie okna

extern DECLSPEC void FOX_VideoQuit(void);

// pobierz nazwe drivera 

extern DECLSPEC char *FOX_VideoDriverName(char *namebuf, int maxlen);

// pobierz wskaznik dla aktualnego surface

extern DECLSPEC FOX_Surface * FOX_GetVideoSurface(void);

// pobierz wskaznik na aktualny wyswietlany video surface

extern DECLSPEC const FOX_VideoInfo * FOX_GetVideoInfo(void);

// sprawdz czy mozna ustawic taki tryb graficzny z takimi parametrami

extern DECLSPEC int FOX_VideoModeOK(int width, int height, int bpp, Uint32 flags);

// zwraca wskaznik do listy dostepnych trybow graficznych

extern DECLSPEC FOX_Rect ** FOX_ListModes(FOX_PixelFormat *format, Uint32 flags);

// ustaw tryb graficzny, jezeli nie mozna to znajdz najblizszy

extern DECLSPEC FOX_Surface *FOX_SetVideoMode
			(int width, int height, int bpp, Uint32 flags);

// blit surface, jezeli mozliwa akceleracja to korzystamy
// moze sie zawiesic jezeli blitowany surface jest zablokowany

extern DECLSPEC void FOX_UpdateRects
		(FOX_Surface *screen, int numrects, FOX_Rect *rects);

extern DECLSPEC void FOX_UpdateRect
		(FOX_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);

// przerzucenie (kopiowanie) na ekran, wziety pod uwage hardware, double buffer itd

extern DECLSPEC int FOX_Flip(FOX_Surface *screen);

// gamma correction, jak w doomie, rozswietlanie skladowych
// w skali od 0.1 do 10.0

extern DECLSPEC int FOX_SetGamma(float red, float green, float blue);

// rampa dla gammy, czyli stopien przyblizania

extern DECLSPEC int FOX_SetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue);

// pobierz rampe 

extern DECLSPEC int FOX_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue);

// ustaw kolor 

extern DECLSPEC int FOX_SetColors(FOX_Surface *surface, 
			FOX_Color *colors, int firstcolor, int ncolors);

// ustaw palete

extern DECLSPEC int FOX_SetPalette(FOX_Surface *surface, int flags,
				   FOX_Color *colors, int firstcolor,
				   int ncolors);

// przemapuj kolor o rgb na dany pixel 

extern DECLSPEC Uint32 FOX_MapRGB
			(FOX_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b);

// tak samo tylko rgba
			
extern DECLSPEC Uint32 FOX_MapRGBA(FOX_PixelFormat *format,
				   Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// pobierz pixel i oddaj rgb

extern DECLSPEC void FOX_GetRGB(Uint32 pixel, FOX_PixelFormat *fmt,
				Uint8 *r, Uint8 *g, Uint8 *b);

// pobierz pixel i oddaj rgba
				
extern DECLSPEC void FOX_GetRGBA(Uint32 pixel, FOX_PixelFormat *fmt,
				 Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);

// stworz i zaalokuj pamiec dla surface

#define FOX_AllocSurface    FOX_CreateRGBSurface

extern DECLSPEC FOX_Surface *FOX_CreateRGBSurface
			(Uint32 flags, int width, int height, int depth, 
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

extern DECLSPEC FOX_Surface *FOX_CreateRGBSurfaceFrom(void *pixels,
			int width, int height, int depth, int pitch,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

// zwolnij pamiec po surface

extern DECLSPEC void FOX_FreeSurface(FOX_Surface *surface);

// zablokuj surface
 
extern DECLSPEC int FOX_LockSurface(FOX_Surface *surface);

// odblokuj surface

extern DECLSPEC void FOX_UnlockSurface(FOX_Surface *surface);

// wszytaj bmp do surface

extern DECLSPEC FOX_Surface * FOX_LoadBMP_RW(FOX_RWops *src, int freesrc);

// makro, zeby bylo szybciej i przyjemniej

#define FOX_LoadBMP(file)	FOX_LoadBMP_RW(FOX_RWFromFile(file, "rb"), 1)

// zapisz surface jako bmp

extern DECLSPEC int FOX_SaveBMP_RW
		(FOX_Surface *surface, FOX_RWops *dst, int freedst);

// ponownie makro zeby bylo szybciej

#define FOX_SaveBMP(surface, file) \
		FOX_SaveBMP_RW(surface, FOX_RWFromFile(file, "wb"), 1)

// ustaw ck dla blitowania surface

extern DECLSPEC int FOX_SetColorKey
			(FOX_Surface *surface, Uint32 flag, Uint32 key);

// ustaw alpha dla surface

extern DECLSPEC int FOX_SetAlpha(FOX_Surface *surface, Uint32 flag, Uint8 alpha);

// ustaw clipping 

extern DECLSPEC FOX_bool FOX_SetClipRect(FOX_Surface *surface, const FOX_Rect *rect);

// clipping pobierz

extern DECLSPEC void FOX_GetClipRect(FOX_Surface *surface, FOX_Rect *rect);

// dodaj nowy surface

extern DECLSPEC FOX_Surface *FOX_ConvertSurface
			(FOX_Surface *src, FOX_PixelFormat *fmt, Uint32 flags);

// szybki blit

// nie stosowac na zablokowanych surface'ach

#define FOX_BlitSurface FOX_UpperBlit

// drugi blit

extern DECLSPEC int FOX_UpperBlit
			(FOX_Surface *src, FOX_Rect *srcrect,
			 FOX_Surface *dst, FOX_Rect *dstrect);

// bardzo low-level blit

extern DECLSPEC int FOX_LowerBlit
			(FOX_Surface *src, FOX_Rect *srcrect,
			 FOX_Surface *dst, FOX_Rect *dstrect);

// blit z kolorem albo surface

extern DECLSPEC int FOX_FillRect
		(FOX_Surface *dst, FOX_Rect *dstrect, Uint32 color);

// zwroc format surface

extern DECLSPEC FOX_Surface * FOX_DisplayFormat(FOX_Surface *surface);

// prawie to samo

extern DECLSPEC FOX_Surface * FOX_DisplayFormatAlpha(FOX_Surface *surface);

// naloz YUV surface

extern DECLSPEC FOX_Overlay *FOX_CreateYUVOverlay(int width, int height,
				Uint32 format, FOX_Surface *display);

// zablokuj nakladki YUV

extern DECLSPEC int FOX_LockYUVOverlay(FOX_Overlay *overlay);

// odblokuj nakladke

extern DECLSPEC void FOX_UnlockYUVOverlay(FOX_Overlay *overlay);

// blit surface nakladka

extern DECLSPEC int FOX_DisplayYUVOverlay(FOX_Overlay *overlay, FOX_Rect *dstrect);

// zwolnij nakladke

extern DECLSPEC void FOX_FreeYUVOverlay(FOX_Overlay *overlay);

// opengl funkcje, driver ladowany dynamicznie

extern DECLSPEC int FOX_GL_LoadLibrary(const char *path);

// pobierz adres funkcji opengl

extern DECLSPEC void *FOX_GL_GetProcAddress(const char* proc);

// podaj atrybut opengl przed inicjalizacja

extern DECLSPEC int FOX_GL_SetAttribute(FOX_GLattr attr, int value);

// pobierz atrybut dla interface np glX

extern DECLSPEC int FOX_GL_GetAttribute(FOX_GLattr attr, int* value);

// swap buforow, jezeli uzywamy double buffer

extern DECLSPEC void FOX_GL_SwapBuffers(void);

// wewnetrzne funkcje, nie wywolywac dopoki nie zaladowane

extern DECLSPEC void FOX_GL_UpdateRects(int numrects, FOX_Rect* rects);

// zablokuj gl surface

extern DECLSPEC void FOX_GL_Lock(void);

// odblokuj gl surface

extern DECLSPEC void FOX_GL_Unlock(void);

// tutaj zmiany na windowsie - usuwanie i wstawianie ramki z tytulem okna

extern DECLSPEC void FOX_WM_SetCaption(const char *title, const char *icon);

extern DECLSPEC void FOX_WM_GetCaption(char **title, char **icon);

// ikonka dla ramki

extern DECLSPEC void FOX_WM_SetIcon(FOX_Surface *icon, Uint8 *mask);

// ikonifuj okno, zminimalizuj

extern DECLSPEC int FOX_WM_IconifyWindow(void);

// przelacz pomiedzy fullscreen a okno

extern DECLSPEC int FOX_WM_ToggleFullScreen(FOX_Surface *surface);

// pobierz grab state aplikacji

typedef enum {

	FOX_GRAB_QUERY = -1,

	FOX_GRAB_OFF = 0,
	FOX_GRAB_ON = 1,
	
	FOX_GRAB_FULLSCREEN	

} FOX_GrabMode;

extern DECLSPEC FOX_GrabMode FOX_WM_GrabInput(FOX_GrabMode mode);

// !!! nie uzywac !!!

extern DECLSPEC int FOX_SoftStretch(FOX_Surface *src, FOX_Rect *srcrect,
                                    FOX_Surface *dst, FOX_Rect *dstrect);
                    
// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end

