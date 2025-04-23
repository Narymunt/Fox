// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - lepszy clipping, jezeli linia pozioma lub pionowa nie miesci sie, to rysuj tylko to 
//   co sie miesci, a nie przerywaj od razu calego rysowania
// - texturowanie
// - sprzetowe rysowanie prymitywow

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <FOX.h>

#include "FOX_draw2d.h"

// w razie czego usunac, bo nie wiadomo czy kiedy ostrzezenie jest omijane to 
// konwersja jest rowniez dodawana

#pragma warning(disable: 4761)

// alpha ? 

#undef SURFACE_ALPHA_PIXEL

// obcinanie

#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1

// postaw pixel, szybkie, bez blendingu, bez blokowania, z obcinaniem

int FOX_fastPixelColorNolock (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color)
{
  int bpp; 
  Uint8 *p; 

  if ( (x >= clip_xmin(dst)) && 
       (x <= clip_xmax(dst)) && 
       (y >= clip_ymin(dst)) && 
       (y <= clip_ymax(dst)) ) 
  {
		// w jakim formacie ? 
    
    bpp = dst->format->BytesPerPixel;
    p = (Uint8 *)dst->pixels + y * dst->pitch + x * bpp;
    switch(bpp) {
    case 1:
        *p = color;
        break;
    case 2:
        *(Uint16 *)p = color;
        break;
    case 3:
        if(FOX_BYTEORDER == FOX_BIG_ENDIAN) 
		{
            p[0] = (color >> 16) & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = color & 0xff;
        } 
		else 
		{
            p[0] = color & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = (color >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = color;
        break;
    } 
    

  }
  
  return(0);
}

// szybkie stawianie pixela, bez alphy, bez blend, bez obcinania

int FOX_fastPixelColorNolockNoclip (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color)
{
  int bpp; 
  Uint8 *p; 

  bpp = dst->format->BytesPerPixel;

  p = (Uint8 *)dst->pixels + y * dst->pitch + x * bpp;
  
  switch(bpp) {
    case 1:
        *p = color;
        break;
    case 2:
        *(Uint16 *)p = color;
        break;
    case 3:
        if(FOX_BYTEORDER == FOX_BIG_ENDIAN) 
		{
            p[0] = (color >> 16) & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = color & 0xff;
        } 
		else 
		{
            p[0] = color & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = (color >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = color;
        break;
  } 
  
  return(0);
}

// szybkie stawianie pixela bez alphy, z blokowaniem i obcinaniem

int FOX_fastPixelColor (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color)
{
 int result;

  // zablokuj surface
 
 if ( FOX_MUSTLOCK(dst) ) 
 {
  if ( FOX_LockSurface(dst) < 0 ) 
  {
   return(-1);
  }
 }
 
 result=FOX_fastPixelColorNolock (dst,x,y,color);
 
 // odblokuj surface
 
 if ( FOX_MUSTLOCK(dst) ) 
 {
  FOX_UnlockSurface (dst);
 }

 return(result);
}

// szybkie stawianie pixela, bez alphy, z blokowaniem, jako rgb

int FOX_fastPixelRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 Uint32 color;

 color=FOX_MapRGBA(dst->format, r, g, b, a);

 return(FOX_fastPixelColor(dst, x, y, color)); 

}

// szybkie stawianie pixela, bez alphy, bez blokowania z rgb

int FOX_fastPixelRGBANolock (FOX_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 Uint32 color;
 

 color=FOX_MapRGBA(dst->format, r, g, b, a);


 return(FOX_fastPixelColorNolock(dst, x, y, color)); 
}

#ifdef SURFACE_ALPHA_PIXEL

// szybkie stawianie pixela z alpha<255

static FOX_Surface *gfxPrimitivesSinglePixel=NULL;

int FOX_pixelColor (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color) 
{ 
 FOX_Rect srect;
 FOX_Rect drect;
 int result;

// zrodlo
 
 srect.x=0;
 srect.y=0;
 srect.w=1;
 srect.h=1;
 
// cel

 drect.x=x;
 drect.y=y;
 drect.w=1;
 drect.h=1;

// stworz jeden pixel w 32bpp rgba
 
 if (gfxPrimitivesSinglePixel==NULL) 
 {
  gfxPrimitivesSinglePixel=FOX_CreateRGBSurface(FOX_SWSURFACE | FOX_HWSURFACE | FOX_SRCALPHA, 1, 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
 }

// rysuj 
 
 FOX_FillRect (gfxPrimitivesSinglePixel, &srect, color);

 result=FOX_BlitSurface (gfxPrimitivesSinglePixel, &srect, dst, &drect);

 return(result);
}

#else

// pixel z alpha blending, taki sam format bpp

int FOX__putPixelAlpha(FOX_Surface *surface, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha)
{
 Uint32 Rmask = surface->format->Rmask, Gmask = surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
 Uint32 R,G,B,A = 0;
	
 if (x>=clip_xmin(surface) && x<=clip_xmax(surface) && y>=clip_ymin(surface) && y<=clip_ymax(surface)) {

  switch (surface->format->BytesPerPixel) 
  {
			case 1: { // 8bpp
				if( alpha == 255 )
				{
					*((Uint8 *)surface->pixels + y*surface->pitch + x) = color;
				} 
				else 
				{
					Uint8 *pixel = (Uint8 *)surface->pixels + y*surface->pitch + x;
					
					Uint8 dR = surface->format->palette->colors[*pixel].r;
					Uint8 dG = surface->format->palette->colors[*pixel].g;
					Uint8 dB = surface->format->palette->colors[*pixel].b;
					Uint8 sR = surface->format->palette->colors[color].r;
					Uint8 sG = surface->format->palette->colors[color].g;
					Uint8 sB = surface->format->palette->colors[color].b;
					
					dR = dR + ((sR-dR)*alpha >> 8);
					dG = dG + ((sG-dG)*alpha >> 8);
					dB = dB + ((sB-dB)*alpha >> 8);
				
					*pixel = FOX_MapRGB(surface->format, dR, dG, dB);
				}
			}
			break;

			case 2: { // 15bpp i 16bpp
				if ( alpha == 255 ) 
				{
					*((Uint16 *)surface->pixels + y*surface->pitch/2 + x) = color;
				} 
				else 
				{
					Uint16 *pixel = (Uint16 *)surface->pixels + y*surface->pitch/2 + x;
					Uint32 dc = *pixel;
				
					R = ((dc & Rmask) + (( (color & Rmask) - (dc & Rmask) ) * alpha >> 8)) & Rmask;
					G = ((dc & Gmask) + (( (color & Gmask) - (dc & Gmask) ) * alpha >> 8)) & Gmask;
					B = ((dc & Bmask) + (( (color & Bmask) - (dc & Bmask) ) * alpha >> 8)) & Bmask;
					if( Amask )
						A = ((dc & Amask) + (( (color & Amask) - (dc & Amask) ) * alpha >> 8)) & Amask;

					*pixel= R | G | B | A;
				}
			}
			break;

			case 3: { // 24bpp ;) 
				Uint8 *pix = (Uint8 *)surface->pixels + y * surface->pitch + x*3;
				Uint8 rshift8=surface->format->Rshift/8;
				Uint8 gshift8=surface->format->Gshift/8;
				Uint8 bshift8=surface->format->Bshift/8;
				Uint8 ashift8=surface->format->Ashift/8;
				
				
				if ( alpha == 255 ) 
				{
  					*(pix+rshift8) = color>>surface->format->Rshift;
  					*(pix+gshift8) = color>>surface->format->Gshift;
  					*(pix+bshift8) = color>>surface->format->Bshift;
					*(pix+ashift8) = color>>surface->format->Ashift;
				} 
				else 
				{
					Uint8 dR, dG, dB, dA=0;
					Uint8 sR, sG, sB, sA=0;
					
					pix = (Uint8 *)surface->pixels + y * surface->pitch + x*3;
					
					dR = *((pix)+rshift8); 
            				dG = *((pix)+gshift8);
            				dB = *((pix)+bshift8);
					dA = *((pix)+ashift8);
					
					sR = (color>>surface->format->Rshift)&0xff;
					sG = (color>>surface->format->Gshift)&0xff;
					sB = (color>>surface->format->Bshift)&0xff;
					sA = (color>>surface->format->Ashift)&0xff;
					
					dR = dR + ((sR-dR)*alpha >> 8);
					dG = dG + ((sG-dG)*alpha >> 8);
					dB = dB + ((sB-dB)*alpha >> 8);
					dA = dA + ((sA-dA)*alpha >> 8);

					*((pix)+rshift8) = dR; 
            				*((pix)+gshift8) = dG;
            				*((pix)+bshift8) = dB;
					*((pix)+ashift8) = dA;
				}
			}
			break;

			case 4: { // 32bpp
				if ( alpha == 255 ) 
				{
					*((Uint32 *)surface->pixels + y*surface->pitch/4 + x) = color;
				} 
				else 
				{
					Uint32 *pixel = (Uint32 *)surface->pixels + y*surface->pitch/4 + x;
					Uint32 dc = *pixel;
			
					R = ((dc & Rmask) + (( (color & Rmask) - (dc & Rmask) ) * alpha >> 8)) & Rmask;
					G = ((dc & Gmask) + (( (color & Gmask) - (dc & Gmask) ) * alpha >> 8)) & Gmask;
					B = ((dc & Bmask) + (( (color & Bmask) - (dc & Bmask) ) * alpha >> 8)) & Bmask;
					if ( Amask )
						A = ((dc & Amask) + (( (color & Amask) - (dc & Amask) ) * alpha >> 8)) & Amask;
					
					*pixel = R | G | B | A;
				}
			}
			break;
  }
 }

 return(0);
}

// pixel + alpha

int FOX_pixelColor (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color) 
{
 Uint8 alpha;
 Uint32 mcolor;
 int result=0;

 // zablokuj surface
 
 if ( FOX_MUSTLOCK(dst) ) 
 {
  if ( FOX_LockSurface(dst) < 0 ) 
  {
   return(-1);
  }
 }
 
// ustaw kolor 
 
 alpha=color &0x000000ff;
 mcolor=FOX_MapRGBA(dst->format, (color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

 // rysuj

 result = FOX__putPixelAlpha(dst,x,y,mcolor,alpha);

 // odblokuj surface 
 
 if (FOX_MUSTLOCK(dst) ) 
 {
  FOX_UnlockSurface(dst);
 }

 return(result);
}

// wypelniony szescian z alpha,taki sam bpp

int FOX__filledRectAlpha (FOX_Surface *surface, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 alpha)
{
 Uint32 Rmask = surface->format->Rmask, Gmask = surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
 Uint32 R,G,B,A=0;
 Sint16 x,y;

 switch (surface->format->BytesPerPixel) {
		case 1: { // 8bpp
			Uint8 *row, *pixel;
			Uint8 dR, dG, dB;
			
			Uint8 sR = surface->format->palette->colors[color].r;
			Uint8 sG = surface->format->palette->colors[color].g;
			Uint8 sB = surface->format->palette->colors[color].b;
			
			for(y = y1; y<=y2; y++)
			{
				row = (Uint8 *)surface->pixels + y*surface->pitch;
				for(x = x1; x <= x2; x++)
				{
					pixel = row + x;
					
					dR = surface->format->palette->colors[*pixel].r;
					dG = surface->format->palette->colors[*pixel].g;
					dB = surface->format->palette->colors[*pixel].b;
					
					dR = dR + ((sR-dR)*alpha >> 8);
					dG = dG + ((sG-dG)*alpha >> 8);
					dB = dB + ((sB-dB)*alpha >> 8);
				
					*pixel = FOX_MapRGB(surface->format, dR, dG, dB);
				}
			}
		}
		break;

		case 2: { // 15bpp i 16bpp
			Uint16 *row, *pixel;
			Uint32 dR=(color & Rmask),dG=(color & Gmask),dB=(color & Bmask),dA=(color & Amask);
			
			for(y = y1; y<=y2; y++)
			{
				row = (Uint16 *)surface->pixels + y*surface->pitch/2;
				for(x = x1; x <= x2; x++)
				{
					pixel = row + x;

					R = ((*pixel & Rmask) + (( dR - (*pixel & Rmask) ) * alpha >> 8)) & Rmask;
					G = ((*pixel & Gmask) + (( dG - (*pixel & Gmask) ) * alpha >> 8)) & Gmask;
					B = ((*pixel & Bmask) + (( dB - (*pixel & Bmask) ) * alpha >> 8)) & Bmask;
					if( Amask )
						A = ((*pixel & Amask) + (( dA - (*pixel & Amask) ) * alpha >> 8)) & Amask;

					*pixel= R | G | B | A;
				}
			}
		}
		break;

		case 3: { // 24bpp
			Uint8 *row,*pix;
			Uint8 dR, dG, dB, dA;
  			Uint8 rshift8=surface->format->Rshift/8; 
			Uint8 gshift8=surface->format->Gshift/8; 
			Uint8 bshift8=surface->format->Bshift/8;
			Uint8 ashift8=surface->format->Ashift/8;
			
			Uint8 sR = (color>>surface->format->Rshift)&0xff;
			Uint8 sG = (color>>surface->format->Gshift)&0xff;
			Uint8 sB = (color>>surface->format->Bshift)&0xff;
			Uint8 sA = (color>>surface->format->Ashift)&0xff;
				
			for(y = y1; y<=y2; y++)
			{
				row = (Uint8 *)surface->pixels + y * surface->pitch;
				
				for(x = x1; x <= x2; x++)
				{
					pix = row + x*3;

					dR = *((pix)+rshift8); 
            				dG = *((pix)+gshift8);
            				dB = *((pix)+bshift8);
					dA = *((pix)+ashift8);
					
					dR = dR + ((sR-dR)*alpha >> 8);
					dG = dG + ((sG-dG)*alpha >> 8);
					dB = dB + ((sB-dB)*alpha >> 8);
					dA = dA + ((sA-dA)*alpha >> 8);

					*((pix)+rshift8) = dR; 
            				*((pix)+gshift8) = dG;
            				*((pix)+bshift8) = dB;
					*((pix)+ashift8) = dA;
				}
			}
					
		}
		break;

		case 4: { // 32bpp
			Uint32 *row, *pixel;
			Uint32 dR=(color & Rmask),dG=(color & Gmask),dB=(color & Bmask),dA=(color & Amask);
		
			for(y = y1; y<=y2; y++)
			{
				row = (Uint32 *)surface->pixels + y*surface->pitch/4;
				
				for(x = x1; x <= x2; x++)
				{
					pixel = row + x;

					R = ((*pixel & Rmask) + (( dR - (*pixel & Rmask) ) * alpha >> 8)) & Rmask;
					G = ((*pixel & Gmask) + (( dG - (*pixel & Gmask) ) * alpha >> 8)) & Gmask;
					B = ((*pixel & Bmask) + (( dB - (*pixel & Bmask) ) * alpha >> 8)) & Bmask;
					if( Amask )
						A = ((*pixel & Amask) + (( dA - (*pixel & Amask) ) * alpha >> 8)) & Amask;

					*pixel= R | G | B | A;
				}
			}
		}
		break;
 }
	
 return(0);
}

// rysuj szescian z alpha w rgba

int FOX_filledRectAlpha (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
 Uint8 alpha;
 Uint32 mcolor;
 int result=0;

 if ( FOX_MUSTLOCK(dst) ) 
 {
  if ( FOX_LockSurface(dst) < 0 ) 
  {
   return(-1);
  }
 }
  
 alpha = color & 0x000000ff;
 mcolor=FOX_MapRGBA(dst->format, (color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

 // rysuj 
 
 result = FOX__filledRectAlpha(dst,x1,y1,x2,y2,mcolor,alpha);
	
 // odblokuj surface

 if (FOX_MUSTLOCK(dst) ) 
 {
  FOX_UnlockSurface(dst);
 }

 return(result);
}

// rysuj pozioma linie z alpha + rgba

int FOX_HLineAlpha(FOX_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color)
{
 return (FOX_filledRectAlpha(dst, x1, y, x2, y, color));
}

// rysuj pionowa linie

int FOX_VLineAlpha(FOX_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color)
{
 return (FOX_filledRectAlpha(dst, x, y1, x, y2, color));
}

#endif


int FOX_pixelColorWeight (FOX_Surface *dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight) 
{ 
 Uint32 a;

 // pobierz alpha
 
 a=(color & (Uint32)0x000000ff);

 // zmien alpha
 
 a = ((a*weight) >> 8); 
 
 return(FOX_pixelColor (dst,x,y, (color & (Uint32)0xffffff00) | (Uint32)a ));
}

// sweet ;) 

int FOX_pixelRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 Uint32 color;
 
 // sprawdz alpha
 
 if (a==255) {

  color=FOX_MapRGBA(dst->format, r, g, b, a);

  // rysuj
  
  return(FOX_fastPixelColor (dst, x, y, color));
 } 
 else 
 {
  return(FOX_pixelColor (dst, x, y, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a ));
 }
}

// pozioma linia

#ifdef SURFACE_ALPHA_PIXEL
 static FOX_Surface *gfxPrimitivesHline=NULL;
#endif

int FOX_hlineColor (FOX_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color) 
{
 Sint16 left,right,top,bottom;
 Uint8 *pixel,*pixellast;
 int dx;
 int pixx, pixy;
 Sint16 w;
 Sint16 xtmp;
 int result=-1;
 Uint8 *colorptr;
#ifdef SURFACE_ALPHA_PIXEL
 Uint32 a;
 FOX_Rect srect;
 FOX_Rect drect;
#endif
 
 // pobierz obszar przycinania

 left = dst->clip_rect.x;
 right = dst->clip_rect.x+dst->clip_rect.w-1;
 top = dst->clip_rect.y;
 bottom = dst->clip_rect.y+dst->clip_rect.h-1;

 // zamien jezeli potrzebne

 if (x1>x2) 
 {
  xtmp=x1; x1=x2; x2=xtmp;
 }

 // widoczne
 
 if ((x1>right) || (x2<left) || (y<top) || (y>bottom)) {
  return(0);
 }
 
 // obcinanie x

 if (x1<left) 
 { 
  x1=left;
 } 
 
 if (x2>right) 
 {
  x2=right;
 }
 
 // ile tego jest ? 
 
 w=x2-x1;
  
 if (w<0) 
 {
  return(0);
 }
 
 // sprawdz alpha

 if ((color & 255)==255) 
 {

  colorptr=(Uint8 *)&color;

  if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
  {
   color=FOX_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } 
  else 
  {
   color=FOX_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }
  
  // zablokuj surface
  
  FOX_LockSurface(dst);

  // ustawienia

  dx=w;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8*)dst->pixels) + pixx * (int)x1 + pixy * (int)y;
  
 // rysuj
  
  switch(dst->format->BytesPerPixel) 
  {
   case 1:
    memset (pixel, color, dx);
    break;
   case 2:
    pixellast = pixel + dx + dx;
    
	for (; pixel<=pixellast; pixel += pixx) 
	{
     *(Uint16*)pixel = color;
    }
    break;
   case 3:
    pixellast = pixel + dx + dx + dx;
    for (; pixel<=pixellast; pixel += pixx) 
	{
     if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
	 {
      pixel[0] = (color >> 16) & 0xff;
      pixel[1] = (color >> 8) & 0xff;
      pixel[2] = color & 0xff;
     } 
	 else 
	 {
      pixel[0] = color & 0xff;
      pixel[1] = (color >> 8) & 0xff;
      pixel[2] = (color >> 16) & 0xff;
     }
    }
    break;
   default: 
    dx = dx + dx;
    pixellast = pixel + dx + dx;
    
	for (; pixel<=pixellast; pixel += pixx) 
	{
     *(Uint32*)pixel = color;
    }
    break;
  }

  // odblokuj ten surface

  FOX_UnlockSurface(dst);
  
  result=0;

 } 
 else 
 {

#ifdef SURFACE_ALPHA_PIXEL

 // zwieksz
  w++;
 
 // ustaw zrodlo
 
  srect.x=0;
  srect.y=0;
  srect.w=w;
  srect.h=1;
 
 // ustaw cel
 
  drect.x=x1;
  drect.y=y;
  drect.w=w;
  drect.h=1;
 
 // hmmm... 
  
  if ((gfxPrimitivesHline!=NULL) && (gfxPrimitivesHline->w<w) ) 
  {
   FOX_FreeSurface(gfxPrimitivesHline);
   gfxPrimitivesHline=NULL;
  }
 
 // linia
  
  if (gfxPrimitivesHline==NULL) 
  {
   gfxPrimitivesHline=FOX_CreateRGBSurface(FOX_SWSURFACE | FOX_HWSURFACE | FOX_SRCALPHA, w, 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  }

 // pobierz alfa
 
  a=(color & (Uint32)0x000000ff);

 // potrzebny blending ? 
  
  if (a != 255) 
  {
   FOX_SetAlpha (gfxPrimitivesHline, FOX_SRCALPHA, 255); 
  } 
  else 
  {
   FOX_SetAlpha (gfxPrimitivesHline, 0, 255);
  }
 
 // rysuj 
  
  FOX_FillRect (gfxPrimitivesHline, &srect, color);

 // rysuj na celu
  
  result=FOX_BlitSurface (gfxPrimitivesHline, &srect, dst, &drect);

#else

 result=FOX_HLineAlpha (dst, x1, x1+w, y, color);

#endif

 }
  
 return(result);
}

// kolejna pozioma linia

int FOX_hlineRGBA (FOX_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_hlineColor(dst, x1, x2, y, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// pionowe linie

#ifdef SURFACE_ALPHA_PIXEL
 static FOX_Surface *gfxPrimitivesVline=NULL;
#endif

int FOX_vlineColor (FOX_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color) 
{
 Sint16 left,right,top,bottom;
 Uint8 *pixel, *pixellast;
 int dy;
 int pixx, pixy;
 Sint16 h;
 Sint16 ytmp;
 int result=-1;
 Uint8 *colorptr;
#ifdef SURFACE_ALPHA_PIXEL
 FOX_Rect srect;
 FOX_Rect drect;
 Uint32 a;
#endif

 // pobierz obszar obcinania
 
 left = dst->clip_rect.x;
 right = dst->clip_rect.x+dst->clip_rect.w-1;
 top = dst->clip_rect.y;
 bottom = dst->clip_rect.y+dst->clip_rect.h-1;

 // zamien y1 i y2 jezeli trzeba 
 
 if (y1>y2) 
 {
  ytmp=y1; y1=y2; y2=ytmp;
 }

 // widocznosc

 if ((y2<top) || (y1>bottom) || (x<left) || (x>right)) 
 {
  return(0);
 }

 // przycinanie
 
 if (y1<top) 
 {
  y1=top;
 } 

 if (y2>bottom) 
 {
  y2=bottom;
 }
 
 h=y2-y1;

 if (h<0) 
 {
  return(0);
 }

 // sprawdz alpha
 
 if ((color & 255)==255) 
 {

	 // alpha blending nie potrzebny

  colorptr=(Uint8 *)&color;

  if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
  {
   color=FOX_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } 
  else 
  {
   color=FOX_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }
  
  FOX_LockSurface(dst);

  dy=h;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8*)dst->pixels) + pixx * (int)x + pixy * (int)y1;
  pixellast = pixel + pixy*dy;
  
 // rysuj 
  
  switch(dst->format->BytesPerPixel) {
   case 1:
    for (; pixel<=pixellast; pixel += pixy) 
	{
     *(Uint8*)pixel = color;
    }
    break;
   case 2:
    for (; pixel<=pixellast; pixel += pixy) 
	{
     *(Uint16*)pixel = color;
    }
    break;
   case 3:
    for (; pixel<=pixellast; pixel += pixy) 
	{
     if(FOX_BYTEORDER == FOX_BIG_ENDIAN) 
	 {
      pixel[0] = (color >> 16) & 0xff;
      pixel[1] = (color >> 8) & 0xff;
      pixel[2] = color & 0xff;
     } 
	 else 
	 {
      pixel[0] = color & 0xff;
      pixel[1] = (color >> 8) & 0xff;
      pixel[2] = (color >> 16) & 0xff;
     }
    }
    break;
   default: 
    for (; pixel<=pixellast; pixel += pixy) 
	{
     *(Uint32*)pixel = color;
    }
    break;
  }

  // odblokuj surface
  
  FOX_UnlockSurface(dst);

  
  result=0;

 } 
 else 
 {

 // alpha blending 

#ifdef SURFACE_ALPHA_PIXEL
  
  h++;

  // ustaw zrodlo 
  
  srect.x=0;
  srect.y=0;
  srect.w=1;
  srect.h=h;
 
  // ustaw dla linii
  
  drect.x=x;
  drect.y=y1;
  drect.w=1;
  drect.h=h;

  // jezeli rozmiar jest za maly to zwolnij ta powierzchnie 
  
  if ( (gfxPrimitivesVline!=NULL) && (gfxPrimitivesVline->h<h) ) 
  {
   FOX_FreeSurface(gfxPrimitivesVline);
   gfxPrimitivesVline=NULL;
  }
 
  // pozioma linia 
  
  if (gfxPrimitivesVline==NULL) 
  {
   gfxPrimitivesVline=FOX_CreateRGBSurface(FOX_SWSURFACE | FOX_HWSURFACE | FOX_SRCALPHA, 1, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  }

  // pobierz alpha

  a=(color & (Uint32)0x000000ff);

 // jezeli potrzebne to uzyj alpa
  
  if (a != 255) 
  {
   FOX_SetAlpha (gfxPrimitivesVline, FOX_SRCALPHA, 255);
  } 
  else 
  {
   FOX_SetAlpha (gfxPrimitivesVline, 0, 255);
  }
 
  // rysuj 
  
  FOX_FillRect (gfxPrimitivesVline, &srect, color);
 
  // na dest
  
  result=FOX_BlitSurface (gfxPrimitivesVline, &srect, dst, &drect);

#else

 result=FOX_VLineAlpha(dst,x, y1, y1+h, color);

#endif
 
 } 

 return(result);
}

int FOX_vlineRGBA (FOX_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_vlineColor(dst, x, y1, y2, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// prostokat

int FOX_rectangleColor (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) 
{
 int result;
 Sint16 w,h, xtmp, ytmp;

 // zamien x 
 
 if (x1>x2) 
 {
  xtmp=x1; x1=x2; x2=xtmp;
 }
 
 // zamien y

 if (y1>y2) 
 {
  ytmp=y1; y1=y2; y2=ytmp;
 }
 
 // policz h i w
 
 w=x2-x1;
 h=y2-y1;

 if ((w<0) || (h<0)) 
 {
  return(0);
 }

 // skrajne przypadki
 
 if (x1==x2) 
 {
  if (y1==y2) 
  {
   return(FOX_pixelColor(dst, x1, y1, color));
  } 
  else 
  {
   return(FOX_vlineColor(dst, x1, y1, y2, color));
  }
 } 
 else 
 {
  if (y1==y2) 
  {
   return(FOX_hlineColor(dst, x1, x2, y1, color));
  }
 }
 
// rysuj prostokat

 result=0;
 result |= FOX_vlineColor(dst, x1, y1, y2, color);
 result |= FOX_vlineColor(dst, x2, y1, y2, color);
 result |= FOX_hlineColor(dst, x1, x2, y1, color);
 result |= FOX_hlineColor(dst, x1, x2, y2, color);
 
 return(result);

}

// prostokat + alpha 

int FOX_rectangleRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_rectangleColor(dst, x1, y1, x2, y2, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// obcinanie 

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

static int FOX_clipEncode (Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
 int code = 0;

 if (x < left) 
 {
  code |= CLIP_LEFT_EDGE;
 } 
 else if (x > right) 
 {
  code |= CLIP_RIGHT_EDGE;
 }
 
 if (y < top) 
 {   
  code |= CLIP_TOP_EDGE;
 } 
 else if (y > bottom) 
 {
  code |= CLIP_BOTTOM_EDGE;
 }
 return code;
}

// obcinanie linii

static int FOX_clipLine(FOX_Surface *dst, Sint16 *x1, Sint16 *y1, Sint16 *x2, Sint16 *y2)
{
 Sint16 left,right,top,bottom;
 int code1, code2;
 int draw = 0;
 Sint16 swaptmp;
 float m;

 // pobierz obszar obcinania
 
 left = dst->clip_rect.x;
 right = dst->clip_rect.x+dst->clip_rect.w-1;
 top = dst->clip_rect.y;
 bottom = dst->clip_rect.y+dst->clip_rect.h-1;

 while (1) {
  code1 = FOX_clipEncode (*x1, *y1, left, top, right, bottom);
  code2 = FOX_clipEncode (*x2, *y2, left, top, right, bottom);
  
  if (CLIP_ACCEPT(code1, code2)) 
  {
   draw = 1;
   break;
  } 
  else if (CLIP_REJECT(code1, code2))
   break;
  else 
  {
   if(CLIP_INSIDE (code1)) 
   {
    swaptmp = *x2; *x2 = *x1; *x1 = swaptmp;
    swaptmp = *y2; *y2 = *y1; *y1 = swaptmp;
    swaptmp = code2; code2 = code1; code1 = swaptmp;
   }
   if (*x2 != *x1) 
   {     
    m = (*y2 - *y1) / (float)(*x2 - *x1);
   } 
   else 
   {
    m = 1.0f;
   }
   
   if (code1 & CLIP_LEFT_EDGE) 
   {
    *y1 += (Sint16)((left - *x1) * m);
    *x1 = left; 
   } 
   else if (code1 & CLIP_RIGHT_EDGE) 
   {
    *y1 += (Sint16)((right - *x1) * m);
    *x1 = right; 
   } 
   else if (code1 & CLIP_BOTTOM_EDGE) 
   {
    if (*x2 != *x1) 
	{
     *x1 += (Sint16)((bottom - *y1) / m);
    }
    *y1 = bottom;
   } 
   else if (code1 & CLIP_TOP_EDGE) 
   {
    if (*x2 != *x1) 
	{
     *x1 += (Sint16)((top - *y1) / m);
    }
    *y1 = top;
   } 
  }
 }
 
 return draw;
}

// wypelniany prostokat 

#ifdef SURFACE_ALPHA_PIXEL
 static FOX_Surface *gfxPrimitivesBox=NULL;
#endif

int FOX_boxColor (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) 
{
 Uint8 *pixel, *pixellast;
 int x, dx;
 int dy;
 int pixx, pixy;
 Sint16 w,h,tmp;
 int result;
 Uint8 *colorptr;
#ifdef SURFACE_ALPHA_PIXEL
 Uint32 a;
 FOX_Rect srect;
 FOX_Rect drect;
#endif


 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // skrajne przypadki
 
 if (x1==x2) 
 {
  if (y1<y2) 
  {
   return(FOX_vlineColor(dst, x1, y1, y2, color));
  } 
  else if (y1>y2) 
  {
   return(FOX_vlineColor(dst, x1, y2, y1, color));
  } 
  else 
  {
   return(FOX_pixelColor(dst, x1, y1, color));
  }
 }
 
 if (y1==y2) 
 {
  if (x1<x2) 
  {
   return(FOX_hlineColor(dst, x1, x2, y1, color));
  } else if (x1>x2) 
  { 
   return(FOX_hlineColor(dst, x2, x1, y1, color));
  }
 }
  
 // kolejnosc
 
 if (x1>x2) 
 {
  tmp=x1;
  x1=x2;
  x2=tmp;
 }
 
 if (y1>y2) 
 {
  tmp=y1;
  y1=y2;
  y2=tmp;
 }
 
 // wylicz h i w 
 
 w=x2-x1;
 h=y2-y1;
 
 // sprawdz alfe
 
 if ((color & 255)==255) 
 {

  colorptr=(Uint8 *)&color;

  if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
  {
   color=FOX_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } 
  else 
  {
   color=FOX_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }
 
  FOX_LockSurface(dst);

  dx=w;
  dy=h;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8*)dst->pixels) + pixx * (int)x1 + pixy * (int)y1;
  pixellast = pixel + pixx*dx + pixy*dy;
  
 // rysuj

  switch(dst->format->BytesPerPixel) {
   case 1:
    for (; pixel<=pixellast; pixel += pixy) 
	{
     memset(pixel,(Uint8)color,dx);
    }
    break;
   case 2:
    pixy -= (pixx*dx);
    for (; pixel<=pixellast; pixel += pixy) 
	{
     for (x=0; x<dx; x++) 
	 {
      *(Uint16*)pixel = color;
      pixel += pixx;
     }
    }
    break;
   case 3:
    pixy -= (pixx*dx);
    for (; pixel<=pixellast; pixel += pixy) 
	{
     for (x=0; x<dx; x++) 
	 {
      if(FOX_BYTEORDER == FOX_BIG_ENDIAN) 
	  {
       pixel[0] = (color >> 16) & 0xff;
       pixel[1] = (color >> 8) & 0xff;
       pixel[2] = color & 0xff;
      } 
	  else 
	  {
       pixel[0] = color & 0xff;
       pixel[1] = (color >> 8) & 0xff;
       pixel[2] = (color >> 16) & 0xff;
      }
      pixel += pixx;
     }
    }
    break;
   default: 
    pixy -= (pixx*dx);
    for (; pixel<=pixellast; pixel += pixy) 
	{
     for (x=0; x<dx; x++) 
	 {
      *(Uint32*)pixel = color;
      pixel += pixx;
     }
    }
    break;
  }

  FOX_UnlockSurface(dst);
  
  result=0;

 } 
 else 
 {

#ifdef SURFACE_ALPHA_PIXEL
 	
  srect.x=0;
  srect.y=0;
  srect.w=w;
  srect.h=h;
  
  drect.x=x1;
  drect.y=y1;
  drect.w=w;
  drect.h=h;
 
  if ((gfxPrimitivesBox!=NULL) && ((gfxPrimitivesBox->w<w) || (gfxPrimitivesBox->h<h))) 
  {
   FOX_FreeSurface(gfxPrimitivesBox);
   gfxPrimitivesBox=NULL;
  }
   
  if (gfxPrimitivesBox==NULL) 
  {
   gfxPrimitivesBox=FOX_CreateRGBSurface(FOX_SWSURFACE | FOX_HWSURFACE | FOX_SRCALPHA, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  }

  // pobierz alfa
  
  a=(color & (Uint32)0x000000ff);

  // czy alfa blending jest wogole potrzebny?
  
  if (a != 255) 
  {
   FOX_SetAlpha (gfxPrimitivesBox, FOX_SRCALPHA, 255);
  } 
  else 
  {
   FOX_SetAlpha (gfxPrimitivesBox, 0, 255);
  }
 
  // rysuj w formacie 
  
  FOX_FillRect (gfxPrimitivesBox, &srect, color);
 
  // rysuj pixel na docelowym surface
  
  result=FOX_BlitSurface (gfxPrimitivesBox, &srect, dst, &drect);

#else
 
 result=FOX_filledRectAlpha(dst,x1,y1,x1+w,y1+h,color);

#endif

 }
 
 return(result);
}

// wypelniany prostokat z alfa 

int FOX_boxRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1,  Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_boxColor(dst, x1, y1, x2, y2, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// linia bez alphy

#define ABS(a) (((a)<0) ? -(a) : (a))

int FOX_lineColor(FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
 int pixx, pixy;
 int x,y;
 int dx,dy;
 int ax,ay;
 int sx,sy;
 int swaptmp;
 Uint8 *pixel;
 Uint8 *colorptr;

 // obcinanie
 
 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // sprawdz czy to nie jakis skrajny przypadek 
 
 if (x1==x2) 
 {
  if (y1<y2) 
  {
   return(FOX_vlineColor(dst, x1, y1, y2, color));
  } 
  else if (y1>y2) 
  {
   return(FOX_vlineColor(dst, x1, y2, y1, color));
  } 
  else 
  {
   return(FOX_pixelColor(dst, x1, y1, color));
  }
 }
 
 if (y1==y2) 
 {
  if (x1<x2) 
  {
   return(FOX_hlineColor(dst, x1, x2, y1, color));
  } else if (x1>x2) 
  { 
   return(FOX_hlineColor(dst, x2, x1, y1, color));
  }
 }

 // ustawianie zmiennych

 dx = x2 - x1;
 dy = y2 - y1;
 sx = (dx >= 0) ? 1 : -1;
 sy = (dy >= 0) ? 1 : -1;

 // potrzebne blendowanie ?
 
 if ((color & 255)==255) 
 {

  colorptr=(Uint8 *)&color;
  
  if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
  {
   color=FOX_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } 
  else
  {
   color=FOX_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }
  
  FOX_LockSurface(dst);

  dx = sx * dx + 1;
  dy = sy * dy + 1;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8*)dst->pixels) + pixx * (int)x1 + pixy * (int)y1;
  pixx *= sx;
  pixy *= sy;

  if (dx < dy) 
  {
   swaptmp = dx; dx = dy; dy = swaptmp;
   swaptmp = pixx; pixx = pixy; pixy = swaptmp;
  }

  // rysuj 

  x=0;
  y=0;
  switch(dst->format->BytesPerPixel) {
   case 1:
    for(; x < dx; x++, pixel += pixx) 
	{
     *pixel = color;
     y += dy; 
     if (y >= dx) 
	 {
      y -= dx; pixel += pixy;
     }
    }
    break;
   case 2:
    for (; x < dx; x++, pixel += pixx) 
	{
     *(Uint16*)pixel = color;
     y += dy; 
     if (y >= dx) 
	 {
      y -= dx; 
      pixel += pixy;
     }
    }
    break;
   case 3:
    for(; x < dx; x++, pixel += pixx) 
	{
     if(FOX_BYTEORDER == FOX_BIG_ENDIAN) 
	 {
      pixel[0] = (color >> 16) & 0xff;
      pixel[1] = (color >> 8) & 0xff;
      pixel[2] = color & 0xff;
     } 
	 else 
	 {
      pixel[0] = color & 0xff;
      pixel[1] = (color >> 8) & 0xff;
      pixel[2] = (color >> 16) & 0xff;
     }
     y += dy; 
     if (y >= dx) 
	 {
      y -= dx; 
      pixel += pixy;
     }
    }
    break;
   default: 
     for(; x < dx; x++, pixel += pixx) 
	 {
      *(Uint32*)pixel = color;
      y += dy; 
      if (y >= dx) 
	  {
       y -= dx; 
       pixel += pixy;
      }
     }
     break;
  }
  
  FOX_UnlockSurface(dst);

 } 
 else 
 {

	 // alfa blending
  
  ax = ABS(dx) << 1;
  ay = ABS(dy) << 1;
  x = x1;
  y = y1;
  
  if (ax > ay) 
  {
    int d = ay - (ax >> 1);
  
	while (x != x2) 
	{
     FOX_pixelColor(dst, x, y, color);
     
	 if (d > 0 || (d == 0 && sx == 1)) 
	 {
      y += sy;
      d -= ax;
     } 
     x += sx;
     d += ay;
    }
  } 
  else 
  {
    int d = ax - (ay >> 1);
    
	while (y != y2) 
	{
     FOX_pixelColor(dst, x, y, color);
     
	 if (d > 0 || ((d == 0) && (sy == 1))) 
	 {
      x += sx;
      d -= ay;
     }
     y += sy;
     d += ax;
    }
  }
  FOX_pixelColor(dst, x, y, color);

 }
 
 return(0);
}

// linia rgb+alfa

int FOX_lineRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_lineColor(dst, x1, y1, x2, y2, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// linia z anti aliasingiem
// uzywa 32b fixed point

#define AAlevels 256
#define AAbits 8

int FOX_aalineColorInt (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, int draw_endpoint)
{
 Sint32 xx0,yy0,xx1,yy1;
 int result;
 Uint32 intshift, erracc, erradj;
 Uint32 erracctmp, wgt, wgtcompmask;
 int dx, dy, tmp, xdir, y0p1, x0pxdir;

 // clipping
 
 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // dla 32bp

 xx0=x1;
 yy0=y1;
 xx1=x2;
 yy1=y2;
 
 // zapisz punkty jezeli potrzebne 
 
 if (yy0 > yy1) 
 {
  tmp = yy0; yy0 = yy1; yy1 = tmp;
  tmp = xx0; xx0 = xx1; xx1 = tmp;
 }

 // policz dystans
  
 dx = xx1 - xx0;
 dy = yy1 - yy0;

 // dodaj dla 
 
 if (dx >= 0) 
 {
   xdir=1;
 } 
 else 
 {
   xdir=-1;
   dx=(-dx);
 }

 // sprawdz przypadki szczegolne
 
 if (dx==0) 
 {
  return (FOX_vlineColor(dst,x1,y1,y2,color));
 } 
 else if (dy==0) 
 {
	// pozioma linia 
    return (FOX_hlineColor(dst,x1,x2,y1,color));
 } else if (dx==dy) 
 {
	
   return (FOX_lineColor(dst,x1,y1,x2,y2,color));
 } 
  
  result=0;
  
  // akumulator 

  erracc = 0;			
  
  // ile bitow shiftowac
  
  intshift = 32 - AAbits;
  
  // maska uzywana do kopiowania
  
  wgtcompmask = AAlevels - 1;

  // rysuj 
  
  result |= FOX_pixelColor (dst, x1, y1, color);
  
  if (dy > dx) 
  {

    erradj = ((dx << 16) / dy)<<16;

	// rysuj wszystkie pixele inne niz ostatni
    
    x0pxdir=xx0+xdir;
    
	while (--dy) 
	{
      erracctmp = erracc;
      erracc += erradj;
      
	  if (erracc <= erracctmp) 
	  {
	
	xx0=x0pxdir;
	x0pxdir += xdir;
      }
      yy0++;			

      wgt = (erracc >> intshift) & 255;      
      result |= FOX_pixelColorWeight (dst, xx0, yy0, color, 255-wgt);
      result |= FOX_pixelColorWeight (dst, x0pxdir, yy0, color, wgt);      
    }

  } 
  else 
  {

   erradj = ((dy << 16) / dx)<<16;
 
	// rysuj wszystkie pixele inne niz pierwszy i ostatni
   
   y0p1=yy0+1;
   
   while (--dx) 
   {

    erracctmp = erracc;
    erracc += erradj;
    
	if (erracc <= erracctmp) 
	{
    
      yy0=y0p1;
      y0p1++;
    }
    
	xx0 += xdir;			

    wgt = (erracc >> intshift) & 255;
    result |= FOX_pixelColorWeight (dst, xx0, yy0, color, 255-wgt);
    result |= FOX_pixelColorWeight (dst, xx0, y0p1, color, wgt);
  }
 }

  // musimy rysowac punkt koncowy ?
 
 if (draw_endpoint) 
 {
  result |= FOX_pixelColor (dst, x2, y2, color);
 }

 return(result);
}

// anty aliasing linia 

int FOX_aalineColor (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
 return(FOX_aalineColorInt(dst,x1,y1,x2,y1,color,1));
}

// anti aliasing - linia z rgb i alfa

int FOX_aalineRGBA (FOX_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_aalineColorInt(dst, x1, y1, x2, y2, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a, 1)); 
}

// kolko, a raczej okrag ;)

int FOX_circleColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color)
{
 int result;
 Sint16 x1,y1,x2,y2;
 Sint16 cx = 0;
 Sint16 cy = r;
 Sint16 ocx = (Sint16)0xffff;
 Sint16 ocy = (Sint16)0xffff;
 Sint16 df = 1 - r;
 Sint16 d_e = 3;
 Sint16 d_se = -2 * r + 5;
 Sint16 xpcx, xmcx, xpcy, xmcy;
 Sint16 ypcy, ymcy, ypcx, ymcx;
 Uint8 *colorptr;

 if (r<0) 
 {
  return(-1);
 }

 // szczegolny przypadek dla r=0 
 
 if (r==0) 
 {
  return(FOX_pixelColor (dst, x, y, color));  
 }

 // sprawdzaj czy trzeba obcinac 
 
 x1=x-r;
 y1=y-r;
 x2=x+r;
 y2=y+r;
 
 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // rysuj 
 
 result=0; 
 
 // sprawdz alfe, jezeli nie potrzebna to nie bierz jej pod uwage
 
 if ((color & 255) ==255) 
 {
  
  colorptr=(Uint8 *)&color;

  if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
  {
   color=FOX_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } 
  else 
  {
   color=FOX_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }

  // zablokuj surface
  
  FOX_LockSurface(dst);
  
  // rysuj 
  
  do {
   
   if ((ocy!=cy) || (ocx!=cx)) 
   {
    xpcx=x+cx;
    xmcx=x-cx;
   
	if (cy>0) 
	{
     ypcy=y+cy;
     ymcy=y-cy;
     result |= FOX_fastPixelColorNolock(dst,xmcx,ypcy,color);
     result |= FOX_fastPixelColorNolock(dst,xpcx,ypcy,color);
     result |= FOX_fastPixelColorNolock(dst,xmcx,ymcy,color);
     result |= FOX_fastPixelColorNolock(dst,xpcx,ymcy,color);
    } 
	else 
	{
     result |= FOX_fastPixelColorNolock(dst,xmcx,y,color);
     result |= FOX_fastPixelColorNolock(dst,xpcx,y,color);
    }
    
	ocy=cy;
    xpcy=x+cy;
    xmcy=x-cy;
    
	if (cx>0) 
	{
     ypcx=y+cx;
     ymcx=y-cx;
     result |= FOX_fastPixelColorNolock(dst,xmcy,ypcx,color);
     result |= FOX_fastPixelColorNolock(dst,xpcy,ypcx,color);
     result |= FOX_fastPixelColorNolock(dst,xmcy,ymcx,color);
     result |= FOX_fastPixelColorNolock(dst,xpcy,ymcx,color);
    } 
	else 
	{
     result |= FOX_fastPixelColorNolock(dst,xmcy,y,color);
     result |= FOX_fastPixelColorNolock(dst,xpcy,y,color);
    }
    ocx=cx;
   }
   
	// aktualizuj 

   if (df < 0)  
   {
    df += d_e;
    d_e += 2;
    d_se += 2;
   } 
   else 
   {
    df += d_se;
    d_e += 2;
    d_se += 4;
    cy--;
   }
   cx++;
  } while(cx <= cy);

  // odblokuj surface
  
  FOX_UnlockSurface(dst);

 } 
 else 
 {
 
  // uzywaj alfy
  
  do {
   
	// rysuj 

   if ((ocy!=cy) || (ocx!=cx)) 
   {
    xpcx=x+cx;
    xmcx=x-cx;
    
	if (cy>0) 
	{
     ypcy=y+cy;
     ymcy=y-cy;
     result |= FOX_pixelColor(dst,xmcx,ypcy,color);
     result |= FOX_pixelColor(dst,xpcx,ypcy,color);
     result |= FOX_pixelColor(dst,xmcx,ymcy,color);
     result |= FOX_pixelColor(dst,xpcx,ymcy,color);
    } 
	else 
	{
     result |= FOX_pixelColor(dst,xmcx,y,color);
     result |= FOX_pixelColor(dst,xpcx,y,color);
    }
    
	ocy=cy;
    xpcy=x+cy;
    xmcy=x-cy;
    
	if (cx>0) 
	{
     ypcx=y+cx;
     ymcx=y-cx;
     result |= FOX_pixelColor(dst,xmcy,ypcx,color);
     result |= FOX_pixelColor(dst,xpcy,ypcx,color);
     result |= FOX_pixelColor(dst,xmcy,ymcx,color);
     result |= FOX_pixelColor(dst,xpcy,ymcx,color);
    } 
	else 
	{
     result |= FOX_pixelColor(dst,xmcy,y,color);
     result |= FOX_pixelColor(dst,xpcy,y,color);
    }
    ocx=cx;
   }
   
	// aktualizuj 

   if (df < 0)  
   {
    df += d_e;
    d_e += 2;
    d_se += 2;
   } 
   else 
   {
    df += d_se;
    d_e += 2;
    d_se += 4;
    cy--;
   }
    cx++;
  } while(cx <= cy);

 } // koniec sprawdzania alfy
 
 return(result);
}

// okrag z rgb i alfa 

int FOX_circleRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_circleColor(dst, x, y, rad, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// kolko z anty aliasingiem, bardzo wolne, wiec uzywac tylko w skrajnych przypadkach

int FOX_aacircleColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color)
{
 return (FOX_aaellipseColor(dst, x, y, r, r, color));
}

// tak samo jak wyzej tylko z rgb i alfa

int FOX_aacircleRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
 return(FOX_aaellipseColor(dst, x, y, rad, rad, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a));
}

// wypelniony okrag czyli kolo ;)

int FOX_filledCircleColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color)
{
 int result;
 Sint16 x1,y1,x2,y2;
 Sint16 cx = 0;
 Sint16 cy = r;
 Sint16 ocx = (Sint16)0xffff;
 Sint16 ocy = (Sint16)0xffff;
 Sint16 df = 1 - r;
 Sint16 d_e = 3;
 Sint16 d_se = -2 * r + 5;
 Sint16 xpcx, xmcx, xpcy, xmcy;
 Sint16 ypcy, ymcy, ypcx, ymcx;

 // sprawdzenie promienia ;) 

 if (r<0) 
 {
  return(-1);
 }

 // tylko punkt ?
 
 if (r==0) 
 {
  return(FOX_pixelColor (dst, x, y, color));  
 }

 // przycinanie 
 
 x1=x-r;
 y1=y-r;
 x2=x+r;
 y2=y+r;

 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // rysuj 
  
 result=0;
 
 do {
   xpcx=x+cx;
   xmcx=x-cx;
   xpcy=x+cy;
   xmcy=x-cy;
   if (ocy!=cy) 
   {
    if (cy>0) 
	{
     ypcy=y+cy;
     ymcy=y-cy;
     result |= FOX_hlineColor(dst,xmcx,xpcx,ypcy,color);
     result |= FOX_hlineColor(dst,xmcx,xpcx,ymcy,color);
    } 
	else 
	{
     result |= FOX_hlineColor(dst,xmcx,xpcx,y,color);
    }
    ocy=cy;
   }
   if (ocx!=cx) 
   {
    if (cx!=cy) 
	{
     if (cx>0) 
	 {
      ypcx=y+cx;
      ymcx=y-cx;
      result |= FOX_hlineColor(dst,xmcy,xpcy,ymcx,color);
      result |= FOX_hlineColor(dst,xmcy,xpcy,ypcx,color);
     } 
	 else 
	 {
      result |= FOX_hlineColor(dst,xmcy,xpcy,y,color);
     }
    }
    ocx=cx;
   }
   
 // aktualizuj 

   if (df < 0)  
   {
    df += d_e;
    d_e += 2;
    d_se += 2;
   } 
   else 
   {
    df += d_se;
    d_e += 2;
    d_se += 4;
    cy--;
   }
   cx++;
 } while(cx <= cy);

 return(result);
}

// wypelnione kolo

int FOX_filledCircleRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
  return(FOX_filledCircleColor(dst, x, y, rad, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// elipsa 

int FOX_ellipseColor (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color)
{
 int result;
 Sint16 x1, y1, x2, y2;
 int ix, iy;
 int h, i, j, k;
 int oh, oi, oj, ok;
 int xmh, xph, ypk, ymk;
 int xmi, xpi, ymj, ypj;
 int xmj, xpj, ymi, ypi;
 int xmk, xpk, ymh, yph;
 Uint8 *colorptr;
  
 // sprawdzenie promieni 

 if ((rx<0) || (ry<0)) 
 {
  return(-1);
 }  

 // hmm... jeden z promieni jest liniowy
  
 if (rx==0) 
 {
  return(FOX_vlineColor (dst, x, y-ry, y+ry, color));  
 }
 
 // szczegolny przypadek 
 
 if (ry==0) 
 {
  return(FOX_hlineColor (dst, x-rx, x+rx, y, color));  
 }

 // przycinanie 
 
 x1=x-rx;
 y1=y-ry;
 x2=x+rx;
 y2=y+ry;

 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // na start
 
 oh = oi = oj = ok = 0xFFFF;
 
 // rysuj 

 result=0;
 
 // sprawdzaj alfe i wykorzystuj jezeli potrzebna

 if ((color & 255)==255) 
 {
  colorptr=(Uint8 *)&color;
  
  if (FOX_BYTEORDER == FOX_BIG_ENDIAN) 
  {
   color=FOX_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } 
  else 
  {
   color=FOX_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }

 // zablokuj surface

  FOX_LockSurface(dst);
  
  if (rx > ry) 
  {
  	ix = 0;
   	iy = rx * 64;

 		do {
	 		h = (ix + 32) >> 6;
	 		i = (iy + 32) >> 6;
	 		j = (h * ry) / rx;
	 		k = (i * ry) / rx;

			if (((ok!=k) && (oj!=k)) || ((oj!=j) && (ok!=j)) || (k!=j)) 
			{
			 xph=x+h;
			 xmh=x-h;
 			 if (k>0) 
			 {
 			  ypk=y+k;
 			  ymk=y-k;
 			  result |= FOX_fastPixelColorNolock (dst,xmh,ypk, color);
 			  result |= FOX_fastPixelColorNolock (dst,xph,ypk, color);
 			  result |= FOX_fastPixelColorNolock (dst,xmh,ymk, color);
 			  result |= FOX_fastPixelColorNolock (dst,xph,ymk, color);
			 } 
			 else 
			 {
 			  result |= FOX_fastPixelColorNolock (dst,xmh,y, color);
 			  result |= FOX_fastPixelColorNolock (dst,xph,y, color);
			 }
 			 ok=k;
			 xpi=x+i;
			 xmi=x-i;
			 if (j>0) 
			 {
			  ypj=y+j;
			  ymj=y-j;
 			  result |= FOX_fastPixelColorNolock (dst,xmi,ypj, color);
 			  result |= FOX_fastPixelColorNolock (dst,xpi,ypj, color);
 			  result |= FOX_fastPixelColorNolock (dst,xmi,ymj, color);
 			  result |= FOX_fastPixelColorNolock (dst,xpi,ymj, color);
			 } 
			 else 
			 {
 			  result |= FOX_fastPixelColorNolock (dst,xmi,y, color);
 			  result |= FOX_fastPixelColorNolock (dst,xpi,y, color);
			 }
			 oj=j;
			}

			ix = ix + iy / rx;
	 		iy = iy - ix / rx;

		} while (i > h);
  } 
  else 
  {
  	ix = 0;
   	iy = ry * 64;

  	do {
	 		h = (ix + 32) >> 6;
	 		i = (iy + 32) >> 6;
	 		j = (h * rx) / ry;
	 		k = (i * rx) / ry;

			if (((oi!=i) && (oh!=i)) || ((oh!=h) && (oi!=h) && (i!=h))) 
			{
			 xmj=x-j;
			 xpj=x+j;
 			 
			 if (i>0) 
			 {
 			  ypi=y+i;
 			  ymi=y-i;
	 		  result |= FOX_fastPixelColorNolock (dst,xmj,ypi,color);
	 		  result |= FOX_fastPixelColorNolock (dst,xpj,ypi,color);
	 		  result |= FOX_fastPixelColorNolock (dst,xmj,ymi,color);
	 		  result |= FOX_fastPixelColorNolock (dst,xpj,ymi,color);
		  	 } 
			 else 
			 {
	 		  result |= FOX_fastPixelColorNolock (dst,xmj,y,color);
	 		  result |= FOX_fastPixelColorNolock (dst,xpj,y,color);
			 }
			 
			 oi=i;
			 xmk=x-k;
			 xpk=x+k;
			 
			 if (h>0) 
			 {
			  yph=y+h;
			  ymh=y-h;
 	 		  result |= FOX_fastPixelColorNolock (dst,xmk,yph,color);
 	 		  result |= FOX_fastPixelColorNolock (dst,xpk,yph,color);
 	 		  result |= FOX_fastPixelColorNolock (dst,xmk,ymh,color);
 	 		  result |= FOX_fastPixelColorNolock (dst,xpk,ymh,color);
			 } 
			 else 
			 {
 	 		  result |= FOX_fastPixelColorNolock (dst,xmk,y,color);
 	 		  result |= FOX_fastPixelColorNolock (dst,xpk,y,color);			 
			 }
			 oh=h;
			}
			
	 		ix = ix + iy / ry;
	 		iy = iy - ix / ry;

  	} while(i > h);
  }

  // odblokuj surface

  FOX_UnlockSurface(dst);

 } 
 else 
 {
  
 if (rx > ry) 
 {
  	ix = 0;
   	iy = rx * 64;

 		do {
	 		h = (ix + 32) >> 6;
	 		i = (iy + 32) >> 6;
	 		j = (h * ry) / rx;
	 		k = (i * ry) / rx;

			if (((ok!=k) && (oj!=k)) || ((oj!=j) && (ok!=j)) || (k!=j)) 
			{
			 xph=x+h;
			 xmh=x-h;
 			
			 if (k>0) 
			 {
 			  ypk=y+k;
 			  ymk=y-k;
 			  result |= FOX_pixelColor (dst,xmh,ypk, color);
 			  result |= FOX_pixelColor (dst,xph,ypk, color);
 			  result |= FOX_pixelColor (dst,xmh,ymk, color);
 			  result |= FOX_pixelColor (dst,xph,ymk, color);
			 } 
			 else 
			 {
 			  result |= FOX_pixelColor (dst,xmh,y, color);
 			  result |= FOX_pixelColor (dst,xph,y, color);
			 }
 			 ok=k;
			 xpi=x+i;
			 xmi=x-i;
			 
			 if (j>0) 
			 {
			  ypj=y+j;
			  ymj=y-j;
 			  result |= FOX_pixelColor (dst,xmi,ypj, color);
 			  result |= FOX_pixelColor (dst,xpi,ypj, color);
 			  result |= FOX_pixelColor (dst,xmi,ymj, color);
 			  result |= FOX_pixelColor (dst,xpi,ymj, color);
			 } 
			 else 
			 {
 			  result |= FOX_pixelColor (dst,xmi,y, color);
 			  result |= FOX_pixelColor (dst,xpi,y, color);
			 }
			 oj=j;
			}

			ix = ix + iy / rx;
	 		iy = iy - ix / rx;

		} while (i > h);
  } 
 else 
 {
  	ix = 0;
   	iy = ry * 64;

  	do {
	 		h = (ix + 32) >> 6;
	 		i = (iy + 32) >> 6;
	 		j = (h * rx) / ry;
	 		k = (i * rx) / ry;

			if (((oi!=i) && (oh!=i)) || ((oh!=h) && (oi!=h) && (i!=h))) 
			{
			 xmj=x-j;
			 xpj=x+j;
 			
			 if (i>0) 
			 {
 			  ypi=y+i;
 			  ymi=y-i;
	 		  result |= FOX_pixelColor (dst,xmj,ypi,color);
	 		  result |= FOX_pixelColor (dst,xpj,ypi,color);
	 		  result |= FOX_pixelColor (dst,xmj,ymi,color);
	 		  result |= FOX_pixelColor (dst,xpj,ymi,color);
		  	 } 
			 else 
			 {
	 		  result |= FOX_pixelColor (dst,xmj,y,color);
	 		  result |= FOX_pixelColor (dst,xpj,y,color);
			 }
			 
			 oi=i;
			 xmk=x-k;
			 xpk=x+k;
			 
			 if (h>0) 
			 {
			  yph=y+h;
			  ymh=y-h;
 	 		  result |= FOX_pixelColor (dst,xmk,yph,color);
 	 		  result |= FOX_pixelColor (dst,xpk,yph,color);
 	 		  result |= FOX_pixelColor (dst,xmk,ymh,color);
 	 		  result |= FOX_pixelColor (dst,xpk,ymh,color);
			 } 
			 else 
			 {
 	 		  result |= FOX_pixelColor (dst,xmk,y,color);
 	 		  result |= FOX_pixelColor (dst,xpk,y,color);			 
			 }
			 oh=h;
			}
			
	 		ix = ix + iy / ry;
	 		iy = iy - ix / ry;

  	} while(i > h);
 }

 } // koniec sprawdzania alfy
 
 return(result);
}

// elipsa rgb + alfa

int FOX_ellipseRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_ellipseColor(dst, x, y, rx, ry, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// elipsa z anti aliasingiem, bardzo wolna 

int FOX_aaellipseColor(FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color)
{
 int result;
 Sint16 x1, y1, x2, y2;
 double angle;
 double deltaAngle;
 double drx, dry, dr;
 int posX, posY, oldPosX, oldPosY;
 int i, r;

 // sprawdzenie promieni

 if ((rx<0) || (ry<0)) 
 {
  return(-1);
 }  

 if (rx==0) 
 {
  return(FOX_vlineColor (dst, x, y-ry, y+ry, color));  
 }
 
 if (ry==0) 
 {
  return(FOX_hlineColor (dst, x-rx, x+rx, y, color));  
 }

 // przycinanie 

 x1=x-rx;
 y1=y-ry;
 x2=x+rx;
 y2=y+ry;
 
 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // rysujemy 
 
 r=(rx+ry)>>1;
 
 dr=(double)r;
 drx=(double)rx;
 dry=(double)ry;
 
 deltaAngle=(2*M_PI)/dr;
 
 angle=deltaAngle;
 
 oldPosX=x+rx;
 oldPosY=y;

 result=0;
 
 for(i=0; i<r; i++) 
 {
  posX=x+(int)(drx*cos(angle));
  posY=y+(int)(dry*sin(angle));
  result |= FOX_aalineColorInt (dst, oldPosX, oldPosY, posX, posY, color, 1);
  oldPosX=posX;
  oldPosY=posY;
  angle += deltaAngle;
 }

 return (result);
}

// elipsa z antyaliasingiem, rgb + alfa

int FOX_aaellipseRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
 return(FOX_aaellipseColor(dst, x, y, rx, ry, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a));
}

// wypelniona elipsa

int FOX_filledEllipseColor (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color)
{
 int result;
 Sint16 x1,y1,x2,y2;
 int ix, iy;
 int h, i, j, k;
 int oh, oi, oj, ok;
 int xmh, xph;
 int xmi, xpi;
 int xmj, xpj;
 int xmk, xpk;
 
 // sprawdz promienie 

 if ((rx<0) || (ry<0)) 
 {
  return(-1);
 }  
 
 // szczegolne przypadki
 
 if (rx==0) 
 {
  return(FOX_vlineColor (dst, x, y-ry, y+ry, color));  
 }
   
 if (ry==0) 
 {
  return(FOX_hlineColor (dst, x-rx, x+rx, y, color));  
 }

 // przycinanie 

 x1=x-rx;
 y1=y-ry;
 x2=x+rx;
 y2=y+ry;
 
 if (!(FOX_clipLine(dst,&x1,&y1,&x2,&y2))) 
 {
  return(0);
 }

 // inicjalizacja 

 oh = oi = oj = ok = 0xFFFF;

 // petle rysujace 

 result=0;  
 
 if (rx > ry) 
 {
  	ix = 0;
   	iy = rx * 64;

 		do {
	 		h = (ix + 32) >> 6;
	 		i = (iy + 32) >> 6;
	 		j = (h * ry) / rx;
	 		k = (i * ry) / rx;

			if ((ok!=k) && (oj!=k)) 
			{
			 xph=x+h;
			 xmh=x-h;
 			 if (k>0) 
			 {
	 		  result |= FOX_hlineColor (dst,xmh,xph,y+k,color);
	   		  result |= FOX_hlineColor (dst,xmh,xph,y-k,color);
			 } 
			 else 
			 {
	   		  result |= FOX_hlineColor (dst,xmh,xph,y,color);
			 }
 			 ok=k;
			}
			
			if ((oj!=j) && (ok!=j) && (k!=j))  
			{
			 xmi=x-i;
			 xpi=x+i;
			 
			 if (j>0) 
			 {
 	 		  result |= FOX_hlineColor (dst,xmi,xpi,y+j,color);
	  		  result |= FOX_hlineColor (dst,xmi,xpi,y-j,color);
			 } 
			 else 
			 {
	  		  result |= FOX_hlineColor (dst,xmi,xpi,y  ,color);
			 }
			 oj=j;
			}

			ix = ix + iy / rx;
	 		iy = iy - ix / rx;

		} while (i > h);
  } 
 else 
 {
  	ix = 0;
   	iy = ry * 64;

  	do {
	 		h = (ix + 32) >> 6;
	 		i = (iy + 32) >> 6;
	 		j = (h * rx) / ry;
	 		k = (i * rx) / ry;

			if ((oi!=i) && (oh!=i)) 
			{
			 xmj=x-j;
			 xpj=x+j; 

 			 if (i>0) 
			 {
	 		  result |= FOX_hlineColor (dst,xmj,xpj,y+i,color);
	   		  result |= FOX_hlineColor (dst,xmj,xpj,y-i,color);
		  	 } 
			 else 
			 {
	   		  result |= FOX_hlineColor (dst,xmj,xpj,y,color);
			 }
			 oi=i;
			}
			
			if ((oh!=h) && (oi!=h) && (i!=h)) 
			{
			 xmk=x-k;
			 xpk=x+k;
			 
			 if (h>0) 
			 {
 	 		  result |= FOX_hlineColor (dst,xmk,xpk,y+h,color);
	  		  result |= FOX_hlineColor (dst,xmk,xpk,y-h,color);
			 } 
			 else 
			 {
	  		  result |= FOX_hlineColor (dst,xmk,xpk,y  ,color);
			 }
			 oh=h;
			}
			
	 		ix = ix + iy / ry;
	 		iy = iy - ix / ry;

  	} while(i > h);
 }

 return(result);
}

// wypelniona elipsa rgb + alfa

int FOX_filledEllipseRGBA (FOX_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_filledEllipseColor(dst, x, y, rx, ry, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// poligon, a wlasciwie trojkat 

int FOX_polygonColor (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, Uint32 color)
{
 int result;
 int i;
 Sint16 *x1, *y1, *x2, *y2;

 // sprawdzenie 
 
 if (n<3) 
 {
  return(-1);
 }

 // ustawienie wskaznikow punktow

 x1=x2=vx;
 y1=y2=vy;
 x2++;
 y2++;
 
 // rysuj 

 result=0;
 
 for (i=1; i<n; i++) 
 {
  result |= FOX_lineColor (dst, *x1, *y1, *x2, *y2, color);
  x1=x2;
  y1=y2;
  x2++; 
  y2++; 
 }
 result |= FOX_lineColor (dst, *x1, *y1, *vx, *vy, color);

 return(result);
}

// poligon rgb+alfa

int FOX_polygonRGBA (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_polygonColor(dst, vx, vy, n, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// wypelniony poligon, nie uzywac tego przypadkiem przy engine 3d

int FOX_gfxPrimitivesCompareInt(const void *a, const void *b);

static int *gfxPrimitivesPolyInts=NULL;
static int gfxPrimitivesPolyAllocated=0;

int FOX_filledPolygonColor (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, int color)
{
	int result;
	int i;
	int y;
	int miny, maxy;
	int x1, y1;
	int x2, y2;
	int ind1, ind2;
	int ints;
	
	// ilosc bokow 
	
	if (n<3) 
	{
	 return -1;
	}
	
	// tymczasowa tablica 
	
	if (!gfxPrimitivesPolyAllocated) 
	{
	 gfxPrimitivesPolyInts = (int *) malloc(sizeof(int) * n);
	 gfxPrimitivesPolyAllocated = n;
	} 
	else 
	{
	 if (gfxPrimitivesPolyAllocated<n) 
	 {
 	  gfxPrimitivesPolyInts = (int *) realloc(gfxPrimitivesPolyInts, sizeof(int) * n);
	  gfxPrimitivesPolyAllocated = n;
	 }
	}		

	// sprawdz maxymalne 
	
	miny = vy[0];
	maxy = vy[0];
	
	for (i=1; (i < n); i++) 
	{
		if (vy[i] < miny) 
		{
		 miny = vy[i];
		} else if (vy[i] > maxy) 
		{
		 maxy = vy[i];
		}
	}
	
	// rysuj, skanowanie wedlug y
	
	result=0;
	for (y=miny; (y <= maxy); y++) 
	{
		ints = 0;
		for (i=0; (i < n); i++) 
		{
			if (!i) 
			{
				ind1 = n-1;
				ind2 = 0;
			} 
			else 
			{
				ind1 = i-1;
				ind2 = i;
			}
			y1 = vy[ind1];
			y2 = vy[ind2];
			
			if (y1 < y2) 
			{
				x1 = vx[ind1];
				x2 = vx[ind2];
			} 
			else 
			if (y1 > y2) 
			{
				y2 = vy[ind1];
				y1 = vy[ind2];
				x2 = vx[ind1];
				x1 = vx[ind2];
			} 
			else 
			{
				continue;
			}
			
			if ((y >= y1) && (y < y2)) 
			{
				gfxPrimitivesPolyInts[ints++] = (y-y1) * (x2-x1) / (y2-y1) + x1;
			} 
			else 
			if ((y == maxy) && (y > y1) && (y <= y2)) 
			{
				gfxPrimitivesPolyInts[ints++] = (y-y1) * (x2-x1) / (y2-y1) + x1;
			}
		}
		qsort(gfxPrimitivesPolyInts, ints, sizeof(int), FOX_gfxPrimitivesCompareInt);

		for (i=0; (i<ints); i+=2) 
		{
			result |= FOX_hlineColor(dst, gfxPrimitivesPolyInts[i], gfxPrimitivesPolyInts[i+1], y, color);
		}
	}
	
 return (result);
}

// wypelniony poligon rgb + alfa

int FOX_filledPolygonRGBA (FOX_Surface *dst, Sint16 *vx, Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
{
 return(FOX_filledPolygonColor(dst, vx, vy, n, ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | (Uint32)a)); 
}

// to tylko funkcja pomocnicza

int FOX_gfxPrimitivesCompareInt(const void *a, const void *b)
{
 return (*(const int *)a) - (*(const int *)b);
}


// end

