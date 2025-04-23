// Fox v0.7a
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
// - konwersje pomiêdzy signed/unsigned 
//================================================================================================
// TODO:

// funkcje do konwersji audio 

#include <stdio.h>

#include "FOX_error.h"
#include "audio.h"

// mixuj lewy i prawy kanal w jeden 

void FOX_ConvertMono(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Sint32 sample;

	switch (format&0x8018) 
	{

		// unsigned char 

		case AUDIO_U8:					
			{
			Uint8 *src, *dst;

			src = cvt->buf;
			dst = cvt->buf;
			
			for ( i=cvt->len_cvt/2; i; --i ) 
			{
				sample = src[0] + src[1];
			
				// jak przy flarach 

				if ( sample > 255 )	*dst = 255;
					else *dst = sample;
				
				src += 2;
				dst += 1;
			}
		}
		break;

		// signed char

		case AUDIO_S8: 
			{
			Sint8 *src, *dst;

			src = (Sint8 *)cvt->buf;
			dst = (Sint8 *)cvt->buf;
			
			for ( i=cvt->len_cvt/2; i; --i ) 
			{
				sample = src[0] + src[1];
			
				// jak przy flarach 

				if ( sample > 127 )	*dst = 127;
				else
				if ( sample < -128 ) *dst = -128;
				else *dst = sample;
				
				src += 2;
				dst += 1;
			}
		}
		break;

		// unsigned int 32 

		case AUDIO_U16: 
			{
			Uint8 *src, *dst;

			src = cvt->buf;
			dst = cvt->buf;
			
			if ( (format & 0x1000) == 0x1000 ) 
			{
				for ( i=cvt->len_cvt/4; i; --i ) 
				{
					sample = (Uint16)((src[0]<<8)|src[1])+
					         (Uint16)((src[2]<<8)|src[3]);
					if ( sample > 65535 ) 
					{
						dst[0] = 0xFF;
						dst[1] = 0xFF;
					} 
					else 
					{
						dst[1] = (sample&0xFF);
						sample >>= 8;
						dst[0] = (sample&0xFF);
					}
					src += 4;
					dst += 2;
				}
			} 
			else 
			{
				for ( i=cvt->len_cvt/4; i; --i ) 
				{
					sample = (Uint16)((src[1]<<8)|src[0])+
					         (Uint16)((src[3]<<8)|src[2]);
					if ( sample > 65535 ) 
					{
						dst[0] = 0xFF;
						dst[1] = 0xFF;
					} 
					else 
					{
						dst[0] = (sample&0xFF);
						sample >>= 8;
						dst[1] = (sample&0xFF);
					}
					src += 4;
					dst += 2;
				}
			}
		}
		break;

		// signed int 16

		case AUDIO_S16: 
			{
			Uint8 *src, *dst;

			src = cvt->buf;
			dst = cvt->buf;

			if ( (format & 0x1000) == 0x1000 ) 
			{
				for ( i=cvt->len_cvt/4; i; --i ) 
				{
					sample = (Sint16)((src[0]<<8)|src[1])+
					         (Sint16)((src[2]<<8)|src[3]);
					if ( sample > 32767 ) 
					{
						dst[0] = 0x7F;
						dst[1] = 0xFF;
					} 
					else
					if ( sample < -32768 ) 
					{
						dst[0] = 0x80;
						dst[1] = 0x00;
					} 
					else 
					{
						dst[1] = (sample&0xFF);
						sample >>= 8;
						dst[0] = (sample&0xFF);
					}
					src += 4;
					dst += 2;
				}
			} 
			else 
			{
				for ( i=cvt->len_cvt/4; i; --i ) 
				{
					sample = (Sint16)((src[1]<<8)|src[0])+
					         (Sint16)((src[3]<<8)|src[2]);
					if ( sample > 32767 ) 
					{
						dst[1] = 0x7F;
						dst[0] = 0xFF;
					} 
					else
					if ( sample < -32768 ) 
					{
						dst[1] = 0x80;
						dst[0] = 0x00;
					} 
					else 
					{
						dst[0] = (sample&0xFF);
						sample >>= 8;
						dst[1] = (sample&0xFF);
					}
					src += 4;
					dst += 2;
				}
			}
		}
		break;
	}
	cvt->len_cvt /= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
	
}

// mono w dwa kanaly, czyli double mono

void FOX_ConvertStereo(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;

	if ( (format & 0xFF) == 16 ) 
	{
		Uint16 *src, *dst;

		src = (Uint16 *)(cvt->buf+cvt->len_cvt);
		dst = (Uint16 *)(cvt->buf+cvt->len_cvt*2);
	
		for ( i=cvt->len_cvt/2; i; --i ) 
		{
			dst -= 2;
			src -= 1;
			dst[0] = src[0];
			dst[1] = src[0];
		}
	} 
	else 
	{
		Uint8 *src, *dst;

		src = cvt->buf+cvt->len_cvt;
		dst = cvt->buf+cvt->len_cvt*2;
		
		for ( i=cvt->len_cvt; i; --i ) 
		{
			dst -= 2;
			src -= 1;
			dst[0] = src[0];
			dst[1] = src[0];
		}
	}
	
	cvt->len_cvt *= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
	
}

// konwersja z 8bps na 16bps LSB

void FOX_Convert16LSB(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *src, *dst;

	src = cvt->buf+cvt->len_cvt;
	dst = cvt->buf+cvt->len_cvt*2;

	for ( i=cvt->len_cvt; i; --i ) 
	{
		src -= 1;
		dst -= 2;
		dst[1] = *src;
		dst[0] = 0;
	}
	
	format = ((format & ~0x0008) | AUDIO_U16LSB);
	
	cvt->len_cvt *= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
	
}

// konwersja z 8bps na 16bps MSB

void FOX_Convert16MSB(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *src, *dst;

	src = cvt->buf+cvt->len_cvt;
	dst = cvt->buf+cvt->len_cvt*2;

	for ( i=cvt->len_cvt; i; --i ) 
	{
		src -= 1;
		dst -= 2;
		dst[0] = *src;
		dst[1] = 0;
	}
	
	format = ((format & ~0x0008) | AUDIO_U16MSB);
	cvt->len_cvt *= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
	
}

// konwersja 16bps na 8bps 

void FOX_Convert8(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *src, *dst;

	src = cvt->buf;
	dst = cvt->buf;

	if ( (format & 0x1000) != 0x1000 ) ++src;		// little endian
	
	for ( i=cvt->len_cvt/2; i; --i ) 
	{
		*dst = *src;
		src += 2;
		dst += 1;
	}
	
	format = ((format & ~0x9010) | AUDIO_U8);
	
	cvt->len_cvt /= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
}

// signed <-> unsigned 

void FOX_ConvertSign(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *data;

	data = cvt->buf;

	if ( (format & 0xFF) == 16 ) 
	{
		if ( (format & 0x1000) != 0x1000 ) ++data;		// little endian
		
		for ( i=cvt->len_cvt/2; i; --i ) 
		{
			*data ^= 0x80;
			data += 2;
		}
	} 
	else 
	{
		for ( i=cvt->len_cvt; i; --i ) *data++ ^= 0x80;
	}
	
	format = (format ^ 0x8000);
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
}

// konwersja endian 

void FOX_ConvertEndian(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *data, tmp;

	data = cvt->buf;

	for ( i=cvt->len_cvt/2; i; --i ) 
	{
		tmp = data[0];
		data[0] = data[1];
		data[1] = tmp;
		data += 2;
	}
	
	format = (format ^ 0x1000);
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
	
}

// podnies rate x2,x4,x6  itd 

void FOX_RateMUL2(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *src, *dst;

	src = cvt->buf+cvt->len_cvt;
	dst = cvt->buf+cvt->len_cvt*2;

	switch (format & 0xFF) 
	{
		case 8:
			for ( i=cvt->len_cvt; i; --i ) 
			{
				src -= 1;
				dst -= 2;
				dst[0] = src[0];
				dst[1] = src[0];
			}
			break;
		
		case 16:
			for ( i=cvt->len_cvt/2; i; --i ) 
			{
				src -= 2;
				dst -= 4;
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = src[1];
			}
			break;
	}
	
	cvt->len_cvt *= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
	
}

// konwersja shr 2, shr 4, shr 6 itd 

void FOX_RateDIV2(FOX_AudioCVT *cvt, Uint16 format)
{
	int i;
	Uint8 *src, *dst;

	src = cvt->buf;
	dst = cvt->buf;

	switch (format & 0xFF) 
	{
		case 8:
			for ( i=cvt->len_cvt/2; i; --i ) 
			{
				dst[0] = src[0];
				src += 2;
				dst += 1;
			}
			break;
		
		case 16:
			for ( i=cvt->len_cvt/4; i; --i ) 
			{
				dst[0] = src[0];
				dst[1] = src[1];
				src += 4;
				dst += 2;
			}
			break;
	}

	cvt->len_cvt /= 2;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
}

// bardzo wolna konwersja, nie uzywac jesli nie potrzebne 

void FOX_RateSLOW(FOX_AudioCVT *cvt, Uint16 format)
{
	double ipos;
	int i, clen;

	clen = (int)((double)cvt->len_cvt / cvt->rate_incr);

	if ( cvt->rate_incr > 1.0 ) 
	{
		switch (format & 0xFF) 
		{
			case 8: 
				{
				Uint8 *output;

				output = cvt->buf;
				ipos = 0.0;
				
				for ( i=clen; i; --i ) 
				{
					*output = cvt->buf[(int)ipos];
					ipos += cvt->rate_incr;
					output += 1;
				}
			}
			break;

			case 16: 
				{
				Uint16 *output;

				clen &= ~1;
				output = (Uint16 *)cvt->buf;
				ipos = 0.0;
				for ( i=clen/2; i; --i ) 
				{
					*output=((Uint16 *)cvt->buf)[(int)ipos];
					ipos += cvt->rate_incr;
					output += 1;
				}
			}
			break;
		}
	} 
	else 
	{
		switch (format & 0xFF) 
		{
			case 8: 
				{
				Uint8 *output;

				output = cvt->buf+clen;
				ipos = (double)cvt->len_cvt;
				for ( i=clen; i; --i ) 
				{
					ipos -= cvt->rate_incr;
					output -= 1;
					*output = cvt->buf[(int)ipos];
				}
			}
			break;

			case 16: 
				{
				Uint16 *output;

				clen &= ~1;
				output = (Uint16 *)(cvt->buf+clen);
				ipos = (double)cvt->len_cvt/2;
				for ( i=clen/2; i; --i ) 
				{
					ipos -= cvt->rate_incr;
					output -= 1;
					*output=((Uint16 *)cvt->buf)[(int)ipos];
				}
			}
			break;
		}
	}
	
	cvt->len_cvt = clen;
	
	if ( cvt->filters[++cvt->filter_index] ) 
		cvt->filters[cvt->filter_index](cvt, format);
}

// konwersja formatu audio 

int FOX_ConvertAudio(FOX_AudioCVT *cvt)
{
	
	// napewno mamy co konwertowac ? 
	
	if ( cvt->buf == NULL ) 
	{
		FOX_SetError("[FOX]: brak zaalokowanego bufora na konwersje...");
		return(-1);
	}
	
	// zwroc ok jezeli konwersja wogole nie jest potrzebna 
	
	cvt->len_cvt = cvt->len;
	
	if ( cvt->filters[0] == NULL ) return(0);

	// ustaw konwersje 
	
	cvt->filter_index = 0;
	cvt->filters[0](cvt, cvt->src_format);
	return(0);
}

// nakladamy filtry audio, zwraca -1 jezeli format nie rozpoznawany, 1 jezeli udalo sie 
// nalozyc filtr 
  
int FOX_BuildAudioCVT(FOX_AudioCVT *cvt,
	Uint16 src_format, Uint8 src_channels, int src_rate,
	Uint16 dst_format, Uint8 dst_channels, int dst_rate)
{

	// won jezeli konwersja nie jest potrzebna 
	
	cvt->needed = 0;
	cvt->filter_index = 0;
	cvt->filters[0] = NULL;
	cvt->len_mult = 1;
	cvt->len_ratio = 1.0;

	// pierwszy filtr, konwersja endian	1234::3412
	
	if ((src_format & 0x1000)!=(dst_format & 0x1000)&&((src_format & 0xff)!=8)) 
		cvt->filters[cvt->filter_index++] = FOX_ConvertEndian;
	
	// drugi filtr - konwersja signed/unsigned 
	
	if ((src_format & 0x8000)!=(dst_format & 0x8000)) 
		cvt->filters[cvt->filter_index++] = FOX_ConvertSign;

	// konwersja 16bps - 8bps 
	
	if ( (src_format & 0xFF) != (dst_format & 0xFF) ) 
	{
		switch (dst_format&0x10FF) 
		{
			case AUDIO_U8:
				cvt->filters[cvt->filter_index++] =
							 FOX_Convert8;
				cvt->len_ratio /= 2;
				break;
			case AUDIO_U16LSB:
				cvt->filters[cvt->filter_index++] =
							FOX_Convert16LSB;
				cvt->len_mult *= 2;
				cvt->len_ratio *= 2;
				break;
			case AUDIO_U16MSB:
				cvt->filters[cvt->filter_index++] =
							FOX_Convert16MSB;
				cvt->len_mult *= 2;
				cvt->len_ratio *= 2;
				break;
		}
	}

	// ostatni filtr - konwersja mono / stereo 
	
	if ( src_channels != dst_channels ) 
	{
		while ( (src_channels*2) <= dst_channels ) 
		{
			cvt->filters[cvt->filter_index++] = 
						FOX_ConvertStereo;
			cvt->len_mult *= 2;
			src_channels *= 2;
			cvt->len_ratio *= 2;
		}
		
		// tutaj zakladamy ze mamy 4 kanaly
		
		while ( ((src_channels%2) == 0) &&
				((src_channels/2) >= dst_channels) ) 
		{
			cvt->filters[cvt->filter_index++] =
						 FOX_ConvertMono;
			src_channels /= 2;
			cvt->len_ratio /= 2;
		}
		
		if ( src_channels != dst_channels ) 
			fprintf(stderr,"[FOX]: po konwersji audio src_channels!=dst_channels..."); // hmmm, i co teraz ?! 
	
	}

	// konwertuj 
	
	cvt->rate_incr = 0.0;
	
	if ( (src_rate/100) != (dst_rate/100) ) 
	{
		Uint32 hi_rate, lo_rate;
		int len_mult;
		double len_ratio;
		void (*rate_cvt)(FOX_AudioCVT *cvt, Uint16 format);

		if ( src_rate > dst_rate ) 
		{
			hi_rate = src_rate;
			lo_rate = dst_rate;
			rate_cvt = FOX_RateDIV2;
			len_mult = 1;
			len_ratio = 0.5;
		} 
		else 
		{
			hi_rate = dst_rate;
			lo_rate = src_rate;
			rate_cvt = FOX_RateMUL2;
			len_mult = 2;
			len_ratio = 2.0;
		}
		
		// jezeli hi_rate = lo_rate*2^x to konwersja jest o wiele prostsza
				
		while ( ((lo_rate*2)/100) <= (hi_rate/100) ) 
		{
			cvt->filters[cvt->filter_index++] = rate_cvt;
			cvt->len_mult *= len_mult;
			lo_rate *= 2;
			cvt->len_ratio *= len_ratio;
		}
		
		// czasem trzeba uzyc wolnej 
		
		if ( (lo_rate/100) != (hi_rate/100) ) 
		{
#if 1

		// tutaj niespodzianka, moze sie wysypac 
		// mniejmy nadzieje, ze jednak tak nie bedzie
#else
			if ( src_rate < dst_rate ) 
			{
				cvt->rate_incr = (double)lo_rate/hi_rate;
				cvt->len_mult *= 2;
				cvt->len_ratio /= cvt->rate_incr;
			} 
			else 
			{
				cvt->rate_incr = (double)hi_rate/lo_rate;
				cvt->len_ratio *= cvt->rate_incr;
			}
			cvt->filters[cvt->filter_index++] = FOX_RateSLOW;
#endif
		}
	}

	// ustaw info dla filtrow
	
	if ( cvt->filter_index != 0 ) 
	{
		cvt->needed = 1;
		cvt->src_format = src_format;
		cvt->dst_format = dst_format;
		cvt->len = 0;
		cvt->buf = NULL;
		cvt->filters[cvt->filter_index] = NULL;
	}
	return(cvt->needed);
}

// end
