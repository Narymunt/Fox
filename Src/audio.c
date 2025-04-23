// Fox v0.7a
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
// - fox_active => active
// - drobne poprawki w kodzie
//================================================================================================
// TODO:
// - FOX>>fx
// - generowanie sampli
// - reverb 
// - przy strcmp rozroznianie na male i duze litery
// - mixowanie liniowe
// - wszystkie komunikaty do konsoli
// - usunac #if 0
// - windib ? 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fox.h"
#include "audio.h"
#include "fox_timer.h"
#include "FOX_error.h"
#include "audio_c.h"
#include "audiomem.h"
#include "FOX_sysaudio.h"

// dostepne sterowniki / metody
// directmedia - najlepszy, duzo przerzucane na sprzet
// directx - polowa sprzet, polowa software
// windib - najbardziej niskopoziomowy

static AudioBootStrap *bootstrap[] = 
{

#ifdef DMEDIA_SUPPORT
	&DMEDIA_bootstrap,
#endif

#ifdef ENABLE_DIRECTX
	&DSOUND_bootstrap,
#endif
/*
#ifdef ENABLE_WINDIB
	&WAVEOUT_bootstrap,
#endif
*/
	NULL
};
FOX_AudioDevice *current_audio = NULL;

// funkcje lokalne

int FOX_AudioInit(const char *driver_name);
void FOX_AudioQuit(void);

// glowna obsluga watku mixujacego 

int FOX_RunAudio(void *audiop)
{
	FOX_AudioDevice *audio = (FOX_AudioDevice *)audiop;
	Uint8 *stream;
	int    stream_len;
	void  *udata;
	void (*fill)(void *userdata,Uint8 *stream, int len);
	int    silence;

	// ustaw watki 
	
	if ( audio->ThreadInit ) audio->ThreadInit(audio);
	
	audio->threadid = FOX_ThreadID();

	// ustaw funkcje mixujace 

	fill  = audio->spec.callback;
	udata = audio->spec.userdata;

	// potrzebna konwersja ? 

	if ( audio->convert.needed ) 
	{
		if ( audio->convert.src_format == AUDIO_U8 ) silence = 0x80;
		else silence = 0;
		
		stream_len = audio->convert.len;
	} 
	else 
	{
		silence = audio->spec.silence;
		stream_len = audio->spec.size;
	}
	
	stream = audio->fake_stream;
	
	// petla wypelniajaca bufor audio 
	
	while ( audio->enabled ) 
	{
		// czekaj na nowy bufor zanim skonczysz odtwarzac 
		
		if ( stream == audio->fake_stream ) 
			FOX_Delay((audio->spec.samples*1000)/audio->spec.freq);
		else 
			audio->WaitAudio(audio);
		
		// wypelnij bufor dzwiekiem 
		
		if ( audio->convert.needed ) 
		{
			if ( audio->convert.buf ) stream = audio->convert.buf;
			else continue;
		} 
		else 
		{
			stream = audio->GetAudioBuf(audio);
			
			if ( stream == NULL ) stream = audio->fake_stream;
			
		}
		memset(stream, silence, stream_len);

		// jak traktujemy nasz watek ? 

		if ( ! audio->paused ) 
		{
			FOX_mutexP(audio->mixer_lock);
			(*fill)(udata, stream, stream_len);
			FOX_mutexV(audio->mixer_lock);
		}

		// skonwertuj dzwieki jezeli trzeba
		
		if ( audio->convert.needed ) 
		{
			FOX_ConvertAudio(&audio->convert);
			stream = audio->GetAudioBuf(audio);
			
			// jezeli cisza, to udawaj, ze cos robisz

			if ( stream == NULL ) stream = audio->fake_stream;
			
			memcpy(stream, audio->convert.buf,audio->convert.len_cvt);
		}

		// przygotuj bufor na odtwarzanie 
		
		if ( stream != audio->fake_stream ) audio->PlayAudio(audio);
		
	}

	// czekaj az skonczy

	if ( audio->WaitDone ) audio->WaitDone(audio);

	return(0);
}

// inicjalizuj audio 

int FOX_AudioInit(const char *driver_name)
{
	FOX_AudioDevice *audio;
	int i = 0, idx;

	// sprawdz czy nie nadpisujemy 
	
	if ( current_audio != NULL ) FOX_AudioQuit();

	// wybierz sterownik 
		
	audio = NULL;
	idx = 0;

	if ( audio == NULL ) 
	{
		if ( driver_name != NULL ) 
		{
#if 0	// to trzeba wymienic 
			
			if ( strrchr(driver_name, ':') != NULL ) idx = atoi(strrchr(driver_name, ':')+1);
#endif

			// tutaj na strcmp, moze byc problem ze wzgledu na duze i male litery
			
			for ( i=0; bootstrap[i]; ++i ) 
			{
				if (strncmp(bootstrap[i]->name, driver_name,
				            strlen(bootstrap[i]->name)) == 0) 
				{
					if ( bootstrap[i]->available() ) 
					{
						audio=bootstrap[i]->create(idx);
						break;
					}
				}
			}
		} 
		else 
		{
			for ( i=0; bootstrap[i]; ++i ) 
			{
				if ( bootstrap[i]->available() ) 
				{
					audio = bootstrap[i]->create(idx);
			
					// tutaj jest juz znaleziony			

					if ( audio != NULL ) break;
					
				}
			}
		}
		
		// tutaj nie ma dzwieku, wiec nie trzeba czekac
		// przeslij sygnal do konsoli

		if ( audio == NULL ) 
		{
			FOX_SetError("[FOX]: brak dostepnego urzadzenia audio...");
#if 0 // i co teraz ?

			return(-1);
#endif
		}
	}

	current_audio = audio;
	
	if ( current_audio ) current_audio->name = bootstrap[i]->name;
	
	return(0);
}

// sprawdz nazwe sterownika, tutaj moze byc babol zwiazany z porownywaniem lancuchow

char *FOX_AudioDriverName(char *namebuf, int maxlen)
{
	if ( current_audio != NULL ) 
	{
		strncpy(namebuf, current_audio->name, maxlen-1);
		namebuf[maxlen-1] = '\0';
		return(namebuf);
	}
	return(NULL);
}

// otworz urzadzenie audio 

int FOX_OpenAudio(FOX_AudioSpec *desired, FOX_AudioSpec *obtained)
{
	FOX_AudioDevice *audio;

	// jakies dodatkowe inicjalizacje ?
	
	if ( ! current_audio ) 
	{
		if ( (FOX_InitSubSystem(FOX_INIT_AUDIO) < 0)||(current_audio == NULL) ) return(-1);
	}
	
	audio = current_audio;

	// sprawdz parametry
	
	if ( desired->callback == NULL ) 
	{
		FOX_SetError("[FOX]: FOX_OpenAudio() zwraca NULL");
		return(-1);
	}

	switch ( desired->channels ) 
	{
	    case 1:			// mono ? 
		case 2:			// stereo 
		break;
	    default:
		FOX_SetError("[FOX]: obslugiwane zarowno 1 (mono) jak i 2 (stereo)");
		return(-1);
	}

	// semafor blokujacy
		
	audio->mixer_lock = FOX_CreateMutex();
	
	if ( audio->mixer_lock == NULL ) 
	{
		FOX_SetError("[FOX]: nie mozna stworzyc blokady bufora mixujacego");
		FOX_CloseAudio();
		return(-1);
	}

	// oblicz czas i rozmiar
	
	FOX_CalculateAudioSpec(desired);

	// otworz podsystem ;) 
	
	memcpy(&audio->spec, desired, sizeof(audio->spec));
	audio->convert.needed = 0;
	audio->enabled = 1;
	audio->paused  = 1;

	// hmm... 

	audio->opened = audio->OpenAudio(audio, &audio->spec)+1;

	// hmm nie mozna otworzyc jako strumien ?

	if ( ! audio->opened ) 
	{
		FOX_CloseAudio();
		return(-1);
	}

	// jezeli audio driver zmienia rozmiar bufora 
	
	if ( audio->spec.samples != desired->samples ) 
	{
		desired->samples = audio->spec.samples;
		FOX_CalculateAudioSpec(desired);
	}

	// alokuj fake mem bufor
	
	audio->fake_stream = FOX_AllocAudioMem(audio->spec.size);

	if ( audio->fake_stream == NULL ) 
	{
		FOX_CloseAudio();
		FOX_OutOfMemory();
		return(-1);
	}

	// znowu konwersja ? 
	
	if ( memcmp(desired, &audio->spec, sizeof(audio->spec)) == 0 ) 
	{
		// po prostu kopiuj 
		if ( obtained != NULL ) memcpy(obtained, &audio->spec, sizeof(audio->spec));
	} 
	else 
	{
		// jezeli mozliwe to kopiuj 
		if ( obtained != NULL ) memcpy(obtained, &audio->spec, sizeof(audio->spec));
		else 
		{
			// zbuduj blok do konwersji 
			
			if ( FOX_BuildAudioCVT(&audio->convert,
				desired->format, desired->channels,
						desired->freq,
				audio->spec.format, audio->spec.channels,
						audio->spec.freq) < 0 ) 
			{
				FOX_CloseAudio();
				return(-1);
			}
			
			// jezeli konwersja potrzebna to konwertuj 

			if ( audio->convert.needed ) 
			{
				audio->convert.len = desired->size;
				
				audio->convert.buf =
					(Uint8 *)FOX_AllocAudioMem(audio->convert.len*audio->convert.len_mult);

				if ( audio->convert.buf == NULL ) 
				{
					FOX_CloseAudio();		// najpierw zamknij 
					FOX_OutOfMemory();		// potem blad o braku pamieci
					return(-1);
				}
			}
		}
	}

	// wystartuj watek audio jezeli potrzebne
	
	switch (audio->opened) 
	{
		case  1:
			// wystartuj watek audio 
			audio->thread = FOX_CreateThread(FOX_RunAudio, audio);
			
			if ( audio->thread == NULL ) 
			{
				FOX_CloseAudio();
				FOX_SetError("[FOX]: nie mozna stworzyc watku dla urzadzenia audio...");
				return(-1);
			}
			break;

		default:
			// juz odtwarzamy 
			break;
	}

	return(0);
}

// status dzwieku, zwracany jako struktura 

FOX_audiostatus FOX_GetAudioStatus(void)
{
	FOX_AudioDevice *audio = current_audio;
	FOX_audiostatus status;

	status = FOX_AUDIO_STOPPED;
	
	if ( audio && audio->enabled ) 
	{
		if ( audio->paused ) status = FOX_AUDIO_PAUSED;
		else status = FOX_AUDIO_PLAYING;
	}
	return(status);
}

// zatrzymaj audio, tylko pauza

void FOX_PauseAudio (int pause_on)
{
	FOX_AudioDevice *audio = current_audio;

	if ( audio ) audio->paused = pause_on;
}

// zablokuj audio 

void FOX_LockAudio (void)
{
	FOX_AudioDevice *audio = current_audio;

	// hmmm... zablokowane bufory ? 
	
	if ( audio ) 
	{
		if ( audio->thread && (FOX_ThreadID() == audio->threadid) ) return;

		FOX_mutexP(audio->mixer_lock);
	}
}

// odblokuj audio 

void FOX_UnlockAudio (void)
{
	FOX_AudioDevice *audio = current_audio;

	// zwolnij blokade na mixowanych buforach 
	
	if ( audio ) 
	{
		if ( audio->thread && (FOX_ThreadID() == audio->threadid) ) return;
		
		FOX_mutexV(audio->mixer_lock);
	}
}

// zamknij strumien audio 

void FOX_CloseAudio (void)
{
	FOX_QuitSubSystem(FOX_INIT_AUDIO);
}

// zwolnij urzadzenie i sterowniki 

void FOX_AudioQuit(void)
{
	FOX_AudioDevice *audio = current_audio;
	
	// o ile wogole dzwiek byl zainicjalizowany

	if ( audio ) 
	{
		audio->enabled = 0;
		
		if ( audio->thread != NULL ) FOX_WaitThread(audio->thread, NULL);
		if ( audio->mixer_lock != NULL ) FOX_DestroyMutex(audio->mixer_lock);
		if ( audio->fake_stream != NULL ) FOX_FreeAudioMem(audio->fake_stream);
		if ( audio->convert.needed ) FOX_FreeAudioMem(audio->convert.buf);

		// jezeli teraz wszystko jest ok to zamykamy 

		if ( audio->opened ) 
		{
			audio->CloseAudio(audio);
			audio->opened = 0;
		}

		// zwolnij dane po sterowniku
				
		audio->free(audio);
		current_audio = NULL;
	}
}

// dodatkowe definicje 

#define NUM_FORMATS	6

static int format_idx;
static int format_idx_sub;

// brakuje gus i tym podobnych, tak wiec dziala tylko na urzadzeniach kompatybilnych

static Uint16 format_list[NUM_FORMATS][NUM_FORMATS] = {
 { AUDIO_U8, AUDIO_S8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB },
 { AUDIO_S8, AUDIO_U8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB },
 { AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8 },
 { AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8 },
 { AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U8, AUDIO_S8 },
 { AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_U8, AUDIO_S8 },
};

// sprawdz format 

Uint16 FOX_FirstAudioFormat(Uint16 format)
{
	for ( format_idx=0; format_idx < NUM_FORMATS; ++format_idx ) 
	{
		if ( format_list[format_idx][0] == format ) break;
	}

	format_idx_sub = 0;
	return(FOX_NextAudioFormat());

}

// nastepny format audio 

Uint16 FOX_NextAudioFormat(void)
{
	if ( (format_idx == NUM_FORMATS) || (format_idx_sub == NUM_FORMATS) ) return(0);
	
	return(format_list[format_idx][format_idx_sub++]);
}

// sprawdz czy ta karta moze odtwarzac taki format

void FOX_CalculateAudioSpec(FOX_AudioSpec *spec)
{
	switch (spec->format) 
	{
		case AUDIO_U8:	spec->silence = 0x80;		break;
		default:		spec->silence = 0x00;		break;
	}

	spec->size = (spec->format&0xFF)/8;		// w gwoli scislosci 
	spec->size *= spec->channels;
	spec->size *= spec->samples;
}

// end

