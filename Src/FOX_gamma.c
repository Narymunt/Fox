// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// korekta gammy 

#define USE_MATH_H	

#ifdef USE_MATH_H
#include <math.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_sysvideo.h"

#ifdef USE_MATH_H
static void CalculateGammaRamp(float gamma, Uint16 *ramp)
{
	int i;

	// 0.0 jest czarne
	
	if ( gamma <= 0.0 ) 
	{
		for ( i=0; i<256; ++i ) 
		{
			ramp[i] = 0;
		}
		return;
	} else
	if ( gamma == 1.0 ) 
	{
		for ( i=0; i<256; ++i ) 
		{
			ramp[i] = (i << 8) | i;
		}
		return;
	} 
	else
	{ int value;
		gamma = 1.0f / gamma;
		for ( i=0; i<256; ++i ) 
		{
			value = (int)(pow((double)i/256.0, gamma)*65535.0+0.5);
			if ( value > 65535 ) 
			{
				value = 65535;
			}
			ramp[i] = (Uint16)value;
		}
	}
}

static void CalculateGammaFromRamp(float *gamma, Uint16 *ramp)
{
	float sum = 0.0;
	int i, count = 0;

	*gamma = 1.0;

	for ( i = 1; i < 256; ++i ) 
	{
	    if ( (ramp[i] != 0) && (ramp[i] != 65535) ) 
		{
	        double B = (double)i / 256.0;
	        double A = ramp[i] / 65535.0;
	        sum += (float) ( log(A) / log(B) );
	        count++;
	    }
	}
	if ( count && sum ) 
	{
		*gamma = 1.0f / (sum / count);
	}
}
#endif /* USE_MATH_H */

// lepiej uzywac set gamma ramp

int FOX_SetGamma(float red, float green, float blue)
{
	int succeeded;
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;	

	succeeded = -1;
#ifdef USE_MATH_H
	{
		Uint16 ramp[3][256];

		CalculateGammaRamp(red, ramp[0]);
		CalculateGammaRamp(green, ramp[1]);
		CalculateGammaRamp(blue, ramp[2]);
		succeeded = FOX_SetGammaRamp(ramp[0], ramp[1], ramp[2]);
	}
#else
	FOX_SetError("[FOX]: korekta gammy nie jest obslugiwana");
#endif
	if ( (succeeded < 0) && video->SetGamma ) 
	{
		FOX_ClearError();
		succeeded = video->SetGamma(this, red, green, blue);
	}
	return succeeded;
}

int FOX_GetGamma(float *red, float *green, float *blue)
{
	int succeeded;
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;	

	succeeded = -1;
#ifdef USE_MATH_H
	{
		Uint16 ramp[3][256];

		succeeded = FOX_GetGammaRamp(ramp[0], ramp[1], ramp[2]);
		
		if ( succeeded >= 0 ) 
		{
			CalculateGammaFromRamp(red, ramp[0]);
			CalculateGammaFromRamp(green, ramp[1]);
			CalculateGammaFromRamp(blue, ramp[2]);
		}
	}
#else
	FOX_SetError("[FOX]: korekta gammy nie obslugiwana");
#endif
	if ( (succeeded < 0) && video->GetGamma ) 
	{
		FOX_ClearError();
		succeeded = video->GetGamma(this, red, green, blue);
	}
	return succeeded;
}

int FOX_SetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue)
{
	int succeeded;
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;	
	FOX_Surface *screen = FOX_PublicSurface;

	// sprawdz parametry screenu
	
	if ( !screen ) {
		FOX_SetError("[FOX]: niewlasciwy tryb");
		return -1;
	}
	
	if ( ! video->gamma ) 
	{
		FOX_GetGammaRamp(0, 0, 0);
	}

	// wypelnij tablice gammy

	if ( red ) 
	{
		memcpy(&video->gamma[0*256], red, 256*sizeof(*video->gamma));
	}
	
	if ( green ) 
	{
		memcpy(&video->gamma[1*256], green, 256*sizeof(*video->gamma));
	}
	
	if ( blue ) 
	{
		memcpy(&video->gamma[2*256], blue, 256*sizeof(*video->gamma));
	}
	
	if ( (screen->flags & FOX_HWPALETTE) == FOX_HWPALETTE ) 
	{
		FOX_Palette *pal = screen->format->palette;
	
		if(video->physpal)
		        pal = video->physpal;
		      
		FOX_SetPalette(screen, FOX_PHYSPAL,
			       pal->colors, 0, pal->ncolors);
		return 0;
	}

	// sprobuj ustawic rampe na sterowniku

	succeeded = -1;
	if ( video->SetGammaRamp ) 
	{
		succeeded = video->SetGammaRamp(this, video->gamma);
	} 
	else 
	{
		FOX_SetError("[FOX]: nie mozna sprzetowo manipulowac gamma");
	}
	return succeeded;
}

int FOX_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;	

	// allokuj tablice gammy 
	
	if ( ! video->gamma ) 
	{
		video->gamma = malloc(3*256*sizeof(*video->gamma));
		if ( ! video->gamma ) 
		{
			FOX_OutOfMemory();
			return -1;
		}
		if ( video->GetGammaRamp ) 
		{
			// pobierz gamme od sprzety
			video->GetGammaRamp(this, video->gamma);
		} 
		else 
		{
		
			int i;
			for ( i=0; i<256; ++i ) 
			{
				video->gamma[0*256+i] = (i << 8) | i;
				video->gamma[1*256+i] = (i << 8) | i;
				video->gamma[2*256+i] = (i << 8) | i;
			}
		}
	}
	
	if ( red ) 
	{
		memcpy(red, &video->gamma[0*256], 256*sizeof(*red));
	}
	
	if ( green ) 
	{
		memcpy(green, &video->gamma[1*256], 256*sizeof(*green));
	}
	
	if ( blue ) 
	{
		memcpy(blue, &video->gamma[2*256], 256*sizeof(*blue));
	}
	return 0;
}

// end
