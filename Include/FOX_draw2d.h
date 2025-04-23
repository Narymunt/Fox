// Fox v0.5
// by Jaroslaw Rozynski
//====
// TODO:

#ifndef _FOX_draw2d_h
#define _FOX_draw2d_h

#include <math.h>

#ifndef M_PI
 #define M_PI	3.141592654
#endif

#include <FOX.h>

// funkcje exportowane w stylu C

#ifdef __cplusplus
extern "C" {
#endif

// wszystkie zmienne color maja byc w formacie 0xrrggbbaa

// stawianie pixeli

extern DECLSPEC int FOX_pixelColor (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color);
extern DECLSPEC int FOX_pixelRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// pozioma linia

extern DECLSPEC int FOX_hlineColor (FOX_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);
extern DECLSPEC int FOX_hlineRGBA (FOX_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// pionowa linia

extern DECLSPEC int FOX_vlineColor (FOX_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color);
extern DECLSPEC int FOX_vlineRGBA (FOX_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// prostokat

extern DECLSPEC int FOX_rectangleColor (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
extern DECLSPEC int FOX_rectangleRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// wypelniony prostokat

extern DECLSPEC int FOX_boxColor (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
extern DECLSPEC int FOX_boxRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// linia

extern DECLSPEC int FOX_lineColor(FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
extern DECLSPEC int FOX_lineRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// anty aliasing linia

extern DECLSPEC int FOX_aalineColor (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
extern DECLSPEC int FOX_aalineRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// okrag

extern DECLSPEC int FOX_circleColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
extern DECLSPEC int FOX_circleRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// anty aliasing okrag

extern DECLSPEC int FOX_aacircleColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
extern DECLSPEC int FOX_aacircleRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// wypelniony okrag

extern DECLSPEC int FOX_filledCircleColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
extern DECLSPEC int FOX_filledCircleRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// elipsa 

extern DECLSPEC int FOX_ellipseColor (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
extern DECLSPEC int FOX_ellipseRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// elipsa z anty aliasingiem

extern DECLSPEC int FOX_aaellipseColor (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
extern DECLSPEC int FOX_aaellipseRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// wypelniona elipsa

extern DECLSPEC int FOX_filledEllipseColor (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
extern DECLSPEC int FOX_filledEllipseRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// poligon, bez wypelniania

extern DECLSPEC int FOX_polygonColor (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, Uint32 color);
extern DECLSPEC int FOX_polygonRGBA (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// wypelniony poligon, bardzo wolne

extern DECLSPEC int FOX_filledPolygonColor (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, int color);
extern DECLSPEC int FOX_filledPolygonRGBA (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// koniec jako c

#ifdef __cplusplus
};
#endif

#endif

// end
