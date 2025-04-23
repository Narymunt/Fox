// Fox v0.7a!
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
//================================================================================================
// TODO:
// - obsluga innych formatow

// obsluga microsoft wave 

#ifndef DISABLE_FILE	// tak na wszelki wypadek 

#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "audio.h"
#include "FOX_wave.h"
#include "FOX_endian.h"


// ile tego jest ? 

#ifndef NELEMS
	#define NELEMS(array)	((sizeof array)/(sizeof array[0]))
#endif

// odczytaj klatke 

static int ReadChunk(FOX_RWops *src, Chunk *chunk);

// struktura ms

struct MS_ADPCM_decodestate {
	
	Uint8 hPredictor;
	
	Uint16 iDelta;
	
	Sint16 iSamp1;
	Sint16 iSamp2;

};

// struktura dla dekodera 

static struct MS_ADPCM_decoder {

	WaveFMT wavefmt;
	
	Uint16 wSamplesPerBlock;
	Uint16 wNumCoef;
	
	Sint16 aCoeff[7][2];
	
	struct MS_ADPCM_decodestate state[2];

} MS_ADPCM_state;

// struktura dla inicjalizacji 

static int InitMS_ADPCM(WaveFMT *format)
{
	Uint8 *rogue_feel;
	Uint16 extra_info;
	int i;

	// ustaw wskaznik na dane MS_ADPCM
	
	MS_ADPCM_state.wavefmt.encoding = FOX_SwapLE16(format->encoding);

	MS_ADPCM_state.wavefmt.channels = FOX_SwapLE16(format->channels);
	
	MS_ADPCM_state.wavefmt.frequency = FOX_SwapLE32(format->frequency);
	
	MS_ADPCM_state.wavefmt.byterate = FOX_SwapLE32(format->byterate);
	
	MS_ADPCM_state.wavefmt.blockalign = FOX_SwapLE16(format->blockalign);
	
	MS_ADPCM_state.wavefmt.bitspersample =
					 FOX_SwapLE16(format->bitspersample);
	
	rogue_feel = (Uint8 *)format+sizeof(*format);
	
	if ( sizeof(*format) == 16 ) 
	{
		extra_info = ((rogue_feel[1]<<8)|rogue_feel[0]);
		rogue_feel += sizeof(Uint16);
	}
	
	MS_ADPCM_state.wSamplesPerBlock = ((rogue_feel[1]<<8)|rogue_feel[0]);
	
	rogue_feel += sizeof(Uint16);
	
	MS_ADPCM_state.wNumCoef = ((rogue_feel[1]<<8)|rogue_feel[0]);
	
	rogue_feel += sizeof(Uint16);
	
	if ( MS_ADPCM_state.wNumCoef != 7 ) 
	{
		FOX_SetError("[FOX]: nieznane ustawienia MS_ADPCM");
		return(-1);
	}

	for ( i=0; i<MS_ADPCM_state.wNumCoef; ++i ) 
	{
		MS_ADPCM_state.aCoeff[i][0] = ((rogue_feel[1]<<8)|rogue_feel[0]);
	
		rogue_feel += sizeof(Uint16);
		
		MS_ADPCM_state.aCoeff[i][1] = ((rogue_feel[1]<<8)|rogue_feel[0]);
		
		rogue_feel += sizeof(Uint16);
	}
	return(0);
}

// struktura dla czestotliwosci

static Sint32 MS_ADPCM_nibble(struct MS_ADPCM_decodestate *state,
					Uint8 nybble, Sint16 *coeff)
{
	const Sint32 max_audioval = ((1<<(16-1))-1);
	const Sint32 min_audioval = -(1<<(16-1));
	
	const Sint32 adaptive[] = {
		230, 230, 230, 230, 307, 409, 512, 614,
		768, 614, 512, 409, 307, 230, 230, 230
	};
	
	Sint32 new_sample, delta;

	new_sample = ((state->iSamp1 * coeff[0]) +
		      (state->iSamp2 * coeff[1]))/256;
	
	if ( nybble & 0x08 ) 
	{
		new_sample += state->iDelta * (nybble-0x10);
	} 
	else 
	{
		new_sample += state->iDelta * nybble;
	}
	
	if ( new_sample < min_audioval ) 
	{
		new_sample = min_audioval;
	} else
	
	if ( new_sample > max_audioval ) 
	{
		new_sample = max_audioval;
	}
	
	delta = ((Sint32)state->iDelta * adaptive[nybble])/256;
	
	if ( delta < 16 ) 
	{
		delta = 16;
	}
	
	state->iDelta = delta;
	state->iSamp2 = state->iSamp1;
	state->iSamp1 = new_sample;
	
	return(new_sample);
}

// dekodowanie 

static int MS_ADPCM_decode(Uint8 **audio_buf, Uint32 *audio_len)
{
	
	struct MS_ADPCM_decodestate *state[2];
	
	Uint8 *freeable, *encoded, *decoded;
	
	Sint32 encoded_len, samplesleft;
	
	Sint8 nybble, stereo;
	
	Sint16 *coeff[2];
	
	Sint32 new_sample;

	// alokacja pamieci na output buffer

	encoded_len = *audio_len;
	encoded = *audio_buf;
	freeable = *audio_buf;
	
	*audio_len = (encoded_len/MS_ADPCM_state.wavefmt.blockalign) * 
				MS_ADPCM_state.wSamplesPerBlock*
				MS_ADPCM_state.wavefmt.channels*sizeof(Sint16);
	*audio_buf = (Uint8 *)malloc(*audio_len);
	
	if ( *audio_buf == NULL ) 
	{
		FOX_Error(FOX_ENOMEM);
		return(-1);
	}
	
	decoded = *audio_buf;

	// gotowy, odtwarzaj
	
	stereo = (MS_ADPCM_state.wavefmt.channels == 2);

	state[0] = &MS_ADPCM_state.state[0];
	state[1] = &MS_ADPCM_state.state[stereo];
	
	while ( encoded_len >= MS_ADPCM_state.wavefmt.blockalign ) 
	{
		// pobierz info dla tego bloku
		state[0]->hPredictor = *encoded++;
		
		if ( stereo ) 
		{
			state[1]->hPredictor = *encoded++;
		}
		
		state[0]->iDelta = ((encoded[1]<<8)|encoded[0]);
		
		encoded += sizeof(Sint16);
		
		if ( stereo ) 
		{
			state[1]->iDelta = ((encoded[1]<<8)|encoded[0]);
			encoded += sizeof(Sint16);
		}
		
		state[0]->iSamp1 = ((encoded[1]<<8)|encoded[0]);
		encoded += sizeof(Sint16);
		
		if ( stereo ) 
		{
			state[1]->iSamp1 = ((encoded[1]<<8)|encoded[0]);
			encoded += sizeof(Sint16);
		}
		
		state[0]->iSamp2 = ((encoded[1]<<8)|encoded[0]);
		encoded += sizeof(Sint16);
		
		if ( stereo ) 
		{
			state[1]->iSamp2 = ((encoded[1]<<8)|encoded[0]);
			encoded += sizeof(Sint16);
		}
		
		coeff[0] = MS_ADPCM_state.aCoeff[state[0]->hPredictor];
		coeff[1] = MS_ADPCM_state.aCoeff[state[1]->hPredictor];

		// ustaw dwie poczatkowe wartosci
		
		decoded[0] = state[0]->iSamp2&0xFF;
		decoded[1] = state[0]->iSamp2>>8;
		decoded += 2;
		
		if ( stereo ) 
		{
			decoded[0] = state[1]->iSamp2&0xFF;
			decoded[1] = state[1]->iSamp2>>8;
			decoded += 2;
		}
		
		decoded[0] = state[0]->iSamp1&0xFF;
		decoded[1] = state[0]->iSamp1>>8;
		decoded += 2;
		
		if ( stereo ) 
		{
			decoded[0] = state[1]->iSamp1&0xFF;
			decoded[1] = state[1]->iSamp1>>8;
			decoded += 2;
		}

		// dekoduj i zatrzymaj inne sample w tym bloku
		
		samplesleft = (MS_ADPCM_state.wSamplesPerBlock-2)*
					MS_ADPCM_state.wavefmt.channels;
		while ( samplesleft > 0 ) 
		{
			nybble = (*encoded)>>4;
			new_sample = MS_ADPCM_nibble(state[0],nybble,coeff[0]);
			decoded[0] = new_sample&0xFF;
			new_sample >>= 8;
			decoded[1] = new_sample&0xFF;
			decoded += 2;

			nybble = (*encoded)&0x0F;
			new_sample = MS_ADPCM_nibble(state[1],nybble,coeff[1]);
			decoded[0] = new_sample&0xFF;
			new_sample >>= 8;
			decoded[1] = new_sample&0xFF;
			decoded += 2;

			++encoded;
			samplesleft -= 2;
		}
		encoded_len -= MS_ADPCM_state.wavefmt.blockalign;
	}
	free(freeable);
	return(0);
}

// struktura opisujaca stan zdekodowania

struct IMA_ADPCM_decodestate 
{

	Sint32 sample;
	Sint8 index;

};

// dekoder 

static struct IMA_ADPCM_decoder 
{
	WaveFMT wavefmt;
	Uint16 wSamplesPerBlock;
	
	struct IMA_ADPCM_decodestate state[2];
} IMA_ADPCM_state;

// inicjalizacja 

static int InitIMA_ADPCM(WaveFMT *format)
{
	Uint8 *rogue_feel;
	Uint16 extra_info;

	// ustaw wskaznik na poczatek 
	
	IMA_ADPCM_state.wavefmt.encoding = FOX_SwapLE16(format->encoding);
	IMA_ADPCM_state.wavefmt.channels = FOX_SwapLE16(format->channels);
	IMA_ADPCM_state.wavefmt.frequency = FOX_SwapLE32(format->frequency);
	IMA_ADPCM_state.wavefmt.byterate = FOX_SwapLE32(format->byterate);
	IMA_ADPCM_state.wavefmt.blockalign = FOX_SwapLE16(format->blockalign);
	
	IMA_ADPCM_state.wavefmt.bitspersample =
					 FOX_SwapLE16(format->bitspersample);
	
	rogue_feel = (Uint8 *)format+sizeof(*format);
	
	if ( sizeof(*format) == 16 ) 
	{
		extra_info = ((rogue_feel[1]<<8)|rogue_feel[0]);
		rogue_feel += sizeof(Uint16);
	}
	
	IMA_ADPCM_state.wSamplesPerBlock = ((rogue_feel[1]<<8)|rogue_feel[0]);
	return(0);
}

// nibble - wziete z msdn, moze nie dzialac !!!

static Sint32 IMA_ADPCM_nibble(struct IMA_ADPCM_decodestate *state,Uint8 nybble)
{
	const Sint32 max_audioval = ((1<<(16-1))-1);
	const Sint32 min_audioval = -(1<<(16-1));
	
	const int index_table[16] = 
	{
		-1, -1, -1, -1,
		 2,  4,  6,  8,
		-1, -1, -1, -1,
		 2,  4,  6,  8
	};
	
	const Sint32 step_table[89] = 
	{
		7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31,
		34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130,
		143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408,
		449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282,
		1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
		3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630,
		9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350,
		22385, 24623, 27086, 29794, 32767
	};
	
	Sint32 delta, step;

	// policz roznice pomiedzy tymi wartosciami
	
	step = step_table[state->index];
	delta = step >> 3;

	if ( nybble & 0x04 ) delta += step;
	if ( nybble & 0x02 ) delta += (step >> 1);
	if ( nybble & 0x01 ) delta += (step >> 2);
	if ( nybble & 0x08 ) delta = -delta;
	
	state->sample += delta;

	// aktualizuj index

	state->index += index_table[nybble];
	
	if ( state->index > 88 ) 
	{
		state->index = 88;
	} 
	else
	if ( state->index < 0 ) 
	{
		state->index = 0;
	}

	// sprawdz output
	
	if ( state->sample > max_audioval ) 
	{
		state->sample = max_audioval;
	} 
	else
	if ( state->sample < min_audioval ) 
	{
		state->sample = min_audioval;
	}
	return(state->sample);
}

// wypelnij bufor do dekodowania (8 samples)

static void Fill_IMA_ADPCM_block(Uint8 *decoded, Uint8 *encoded,
	int channel, int numchannels, struct IMA_ADPCM_decodestate *state)
{
	int i;

	Sint8 nybble;
	
	Sint32 new_sample;

	decoded += (channel * 2);
	
	for ( i=0; i<4; ++i ) 
	{
		nybble = (*encoded)&0x0F;
		new_sample = IMA_ADPCM_nibble(state, nybble);
		decoded[0] = new_sample&0xFF;
		new_sample >>= 8;
		decoded[1] = new_sample&0xFF;
		decoded += 2 * numchannels;

		nybble = (*encoded)>>4;
		new_sample = IMA_ADPCM_nibble(state, nybble);
		decoded[0] = new_sample&0xFF;
		new_sample >>= 8;
		decoded[1] = new_sample&0xFF;
		decoded += 2 * numchannels;

		++encoded;
	}
}

// jak wyzej, pomocnicze

static int IMA_ADPCM_decode(Uint8 **audio_buf, Uint32 *audio_len)
{
	struct IMA_ADPCM_decodestate *state;
	Uint8 *freeable, *encoded, *decoded;
	Sint32 encoded_len, samplesleft;
	int c, channels;

	// sprawdz czy to sie zmiesci w tablicy, czy mozna odtwarzac na raz tyle kanalow
	
	channels = IMA_ADPCM_state.wavefmt.channels;

	if ( channels > NELEMS(IMA_ADPCM_state.state) ) 
	{
		FOX_SetError("[FOX]: dekoder IMA ADPCM moze obsluzyc tylko %d kanalow",
						NELEMS(IMA_ADPCM_state.state));
		return(-1);
	}
	state = IMA_ADPCM_state.state;

	// alokuj bufor 
	
	encoded_len = *audio_len;
	encoded = *audio_buf;
	freeable = *audio_buf;

	*audio_len = (encoded_len/IMA_ADPCM_state.wavefmt.blockalign) * 
				IMA_ADPCM_state.wSamplesPerBlock*
				IMA_ADPCM_state.wavefmt.channels*sizeof(Sint16);
	
	*audio_buf = (Uint8 *)malloc(*audio_len);
	
	if ( *audio_buf == NULL ) 
	{
		FOX_Error(FOX_ENOMEM);
		return(-1);
	}
	
	decoded = *audio_buf;

	// gotowe ? 
	
	while ( encoded_len >= IMA_ADPCM_state.wavefmt.blockalign ) 
	{
		// pobierz info 
		
		for ( c=0; c<channels; ++c ) 
		{
			// wypelnij 
			state[c].sample = ((encoded[1]<<8)|encoded[0]);
			
			encoded += 2;
			
			if ( state[c].sample & 0x8000 ) 
			{
				state[c].sample -= 0x10000;
			}
			
			state[c].index = *encoded++;
			
			// zarezerwuj 
			
			if ( *encoded++ != 0 ) 
			{
				// !!! obsluga smieci w buforze !!!
			}

			// zatrzymaj 
			
			decoded[0] = state[c].sample&0xFF;
			decoded[1] = state[c].sample>>8;
			decoded += 2;
		}

		// zatrzymaj w tym bloku
		
		samplesleft = (IMA_ADPCM_state.wSamplesPerBlock-1)*channels;
		
		while ( samplesleft > 0 ) 
		{
			for ( c=0; c<channels; ++c ) 
			{
				Fill_IMA_ADPCM_block(decoded, encoded,
						c, channels, &state[c]);
				encoded += 4;
				samplesleft -= 8;
			}
			decoded += (channels * 8 * 2);
		}
		encoded_len -= IMA_ADPCM_state.wavefmt.blockalign;
	}
	free(freeable);
	return(0);
}

// wczytaj wave 

FOX_AudioSpec * FOX_LoadWAV_RW (FOX_RWops *src, int freesrc,
		FOX_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
	int was_error;
	Chunk chunk;
	int lenread;
	int MS_ADPCM_encoded, IMA_ADPCM_encoded;
	int samplesize;

	// naglowek pliku 
	
	Uint32 RIFFchunk;
	Uint32 wavelen;
	Uint32 WAVEmagic;

	// FMT chunk
	
	WaveFMT *format = NULL;

	// napewno jest ok ? 
	
	was_error = 0;

	if ( src == NULL ) 
	{
		was_error = 1;
		goto done;			// aj, jaj, jaj !!! 
	}

	// sprawdz naglowek 

	
	RIFFchunk	= FOX_ReadLE32(src);
	wavelen		= FOX_ReadLE32(src);
	
	// naglowek wczytany 

	if ( wavelen == WAVE ) 
	{ 
		WAVEmagic = wavelen;
		wavelen   = RIFFchunk;
		RIFFchunk = RIFF;
	} 
	else 
	{
		WAVEmagic = FOX_ReadLE32(src);
	}
	
	if ( (RIFFchunk != RIFF) || (WAVEmagic != WAVE) ) 
	{
		FOX_SetError("[FOX]: nieznany typ .wav");
		was_error = 1;
		goto done;
	}

	// wczytaj dane 
	
	chunk.data = NULL;
	do 
	{
		if ( chunk.data != NULL ) 
		{
			free(chunk.data);
		}
		
		lenread = ReadChunk(src, &chunk);
		
		if ( lenread < 0 ) 
		{
			was_error = 1;
			goto done;
		}
	} while ( (chunk.magic == FACT) || (chunk.magic == LIST) );

	// zdekoduj 
		
	format = (WaveFMT *)chunk.data;
	
	if ( chunk.magic != FMT ) 
	{
		FOX_SetError("[FOX]: nie obslugiwany typ dzwieku .wav <complex wave>");
		was_error = 1;
		goto done;
	}
	
	MS_ADPCM_encoded = IMA_ADPCM_encoded = 0;
	
	// ok, dzwiek mamy 

	switch (FOX_SwapLE16(format->encoding)) 
	{
		case PCM_CODE:
			// to mozna odtworzyc 
			break;
		case MS_ADPCM_CODE:
			// to sprobujemy odkodowac
			
			if ( InitMS_ADPCM(format) < 0 ) 
			{
				was_error = 1;
				goto done;
			}
			MS_ADPCM_encoded = 1;
			break;
		case IMA_ADPCM_CODE:
			// to sprobujemy odkodowac
			
			if ( InitIMA_ADPCM(format) < 0 ) 
			{
				was_error = 1;
				goto done;
			}
			IMA_ADPCM_encoded = 1;
			break;
		default:
			FOX_SetError("[FOX]: nieznany format danych wave: 0x%.4x",
					FOX_SwapLE16(format->encoding));
			was_error = 1;
			goto done;
	}
	
	memset(spec, 0, (sizeof *spec));
	
	spec->freq = FOX_SwapLE32(format->frequency);
	
	switch (FOX_SwapLE16(format->bitspersample)) {
		case 4:
			if ( MS_ADPCM_encoded || IMA_ADPCM_encoded ) 
			{
				spec->format = AUDIO_S16;
			} 
			else 
			{
				was_error = 1;
			}
			break;
		case 8:
			spec->format = AUDIO_U8;
			break;
		case 16:
			spec->format = AUDIO_S16;
			break;
		default:
			was_error = 1;
			break;
	}
	
	if ( was_error ) 
	{
		FOX_SetError("[FOX]: nieznany %d-bit format danych PCM w pliku .wav",
			FOX_SwapLE16(format->bitspersample));
		goto done;
	}
	spec->channels = (Uint8)FOX_SwapLE16(format->channels);
	
	spec->samples = 4096;		// !!! mozna zmieniac !!!

	// wczytaj audio chunka
	
	
	*audio_buf = NULL;
	
	do 
	{
		if ( *audio_buf != NULL ) 
		{
			free(*audio_buf);
		}
		
		lenread = ReadChunk(src, &chunk);
		
		if ( lenread < 0 ) 
		{
			was_error = 1;
			goto done;
		}
		*audio_len = lenread;
		*audio_buf = chunk.data;
	} while ( chunk.magic != DATA );

	if ( MS_ADPCM_encoded ) 
	{
		if ( MS_ADPCM_decode(audio_buf, audio_len) < 0 ) 
		{
			was_error = 1;
			goto done;
		}
	}
	
	if ( IMA_ADPCM_encoded ) 
	{
		if ( IMA_ADPCM_decode(audio_buf, audio_len) < 0 ) 
		{
			was_error = 1;
			goto done;
		}
	}

	// nie zwracaj bufora 
	
	samplesize = ((spec->format & 0xFF)/8)*spec->channels;
	*audio_len &= ~(samplesize-1);

done:
	if ( format != NULL ) 
	{
		free(format);
	}
	
	if ( freesrc && src ) 
	{
		FOX_RWclose(src);
	}
	
	if ( was_error ) 
	{
		spec = NULL;
	}
	return(spec);
}

// zwalnianie wspoldzielonej pamieci po dzwieku

void FOX_FreeWAV(Uint8 *audio_buf)
{
	if ( audio_buf != NULL ) 
	{
		free(audio_buf);
	}
}

// wczytaj chunk

static int ReadChunk(FOX_RWops *src, Chunk *chunk)
{
	chunk->magic	= FOX_ReadLE32(src);
	chunk->length	= FOX_ReadLE32(src);
	chunk->data = (Uint8 *)malloc(chunk->length);
	
	if ( chunk->data == NULL ) 
	{
		FOX_Error(FOX_ENOMEM);
		return(-1);
	}
	
	if ( FOX_RWread(src, chunk->data, chunk->length, 1) != 1 ) 
	{
		FOX_Error(FOX_EFREAD);
		free(chunk->data);
		return(-1);
	}
	return(chunk->length);
}

#endif 

// end
