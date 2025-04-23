// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:

// dostep liniowy do bufora mixowania
// poprawione wince semaphore

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

#include "audio.h"		// definicje
#include "FOX_mutex.h"
#include "FOX_timer.h"		// zegar, jest zalezny
#include "audio_c.h"	// dodatkowe
#include "FOX_dibaudio.h"

// funkcje dla sterownika

static int DIB_OpenAudio(_THIS, FOX_AudioSpec *spec);
static void DIB_ThreadInit(_THIS);
static void DIB_WaitAudio(_THIS);
static Uint8 *DIB_GetAudioBuf(_THIS);
static void DIB_PlayAudio(_THIS);
static void DIB_WaitDone(_THIS);
static void DIB_CloseAudio(_THIS);

// bootstrap 

static int Audio_Available(void)
{
	return(1);
}

// nie uzywaj juz danego urzadzenia

static void Audio_DeleteDevice(FOX_AudioDevice *device)
{
	free(device->hidden);
	free(device);
}

// otworz urzadzenie, stworz strumien, whatever

static FOX_AudioDevice *Audio_CreateDevice(int devindex)
{
	FOX_AudioDevice *this;

	// inicjalizuj i wyczysc
	
	this = (FOX_AudioDevice *)malloc(sizeof(FOX_AudioDevice));
	
	if ( this ) // jest
	{
		memset(this, 0, (sizeof *this));
		this->hidden = (struct FOX_PrivateAudioData *)
				malloc((sizeof *this->hidden));
	}

	if ( (this == NULL) || (this->hidden == NULL) ) 
	{
		FOX_OutOfMemory();	// error
		if ( this ) 
		{
			free(this);
		}
		return(0);
	}

	memset(this->hidden, 0, (sizeof *this->hidden));

	// ustaw wskazniki na funkcje
	
	this->OpenAudio = DIB_OpenAudio;
	this->ThreadInit = DIB_ThreadInit;
	this->WaitAudio = DIB_WaitAudio;
	this->PlayAudio = DIB_PlayAudio;
	this->GetAudioBuf = DIB_GetAudioBuf;
	this->WaitDone = DIB_WaitDone;
	this->CloseAudio = DIB_CloseAudio;

	this->free = Audio_DeleteDevice;

	return this;
}

// !!! nie zmieniac !!!

AudioBootStrap WAVEOUT_bootstrap = {
	"waveout", "Win95/98/NT/2000 WaveOut",
	Audio_Available, Audio_CreateDevice
};

// callback dla win32 - wypelnianie wave device

static void CALLBACK FillSound(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
						DWORD dwParam1, DWORD dwParam2)
{
	FOX_AudioDevice *this = (FOX_AudioDevice *)dwInstance;

	// obsluga tylko wyjatku "buffer done playing"

	if ( uMsg != WOM_DONE )
		return;
	
	// koniec bufora 
	
	ReleaseSemaphore(audio_sem, 1, NULL);

}

// obsluga tego bledu

static void SetMMerror(char *function, MMRESULT code)
{
	int len;
	
	char errbuf[MAXERRORLENGTH];

	sprintf(errbuf, "[FOX]: %s: ", function);
	len = strlen(errbuf);

	waveOutGetErrorText(code, errbuf+len, MAXERRORLENGTH-len);

	FOX_SetError("[FOX]: %s",errbuf);
}

// wysoki priorytet dla watku odtwarzajacego audio

static void DIB_ThreadInit(_THIS)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

// czekaj na skonczenie tego fragmentu (chunk)

void DIB_WaitAudio(_THIS)
{
	
	WaitForSingleObject(audio_sem, INFINITE);
}

// pobierz 

Uint8 *DIB_GetAudioBuf(_THIS)
{
        Uint8 *retval;

	retval = (Uint8 *)(wavebuf[next_buffer].lpData);
	return retval;
}

// odtwarzaj chunk z bufora 

void DIB_PlayAudio(_THIS)
{

	waveOutWrite(sound, &wavebuf[next_buffer], sizeof(wavebuf[0]));
	next_buffer = (next_buffer+1)%NUM_BUFFERS;
}

// czekaj az skonczy

void DIB_WaitDone(_THIS)
{
	int i, left;

	do 
	{
		left = NUM_BUFFERS;
		
		for ( i=0; i<NUM_BUFFERS; ++i ) 
		{
			if ( wavebuf[i].dwFlags & WHDR_DONE ) 
			{
				--left;
			}
		}
		
		if ( left > 0 ) 
		{
			FOX_Delay(100);
		}
	} while ( left > 0 );
}

// zamknij strumien audio

void DIB_CloseAudio(_THIS)
{
	int i;

	
	if ( audio_sem ) 
	{
		CloseHandle(audio_sem);
	}
	
	if ( sound ) 
	{
		waveOutClose(sound);
	}

	// wyczysc bufory ze zmiksowanymi dzwiekami

	for ( i=0; i<NUM_BUFFERS; ++i ) 
	{
		if ( wavebuf[i].dwUser != 0xFFFF ) 
		{
			waveOutUnprepareHeader(sound, &wavebuf[i],
						sizeof(wavebuf[i]));
			wavebuf[i].dwUser = 0xFFFF;
		}
	}
	
	// zwolnij bufor z niemiksowanymi danymi

	if ( mixbuf != NULL ) 
	{
		free(mixbuf);
		mixbuf = NULL;
	}
}

// otworz audio

int DIB_OpenAudio(_THIS, FOX_AudioSpec *spec)
{
	MMRESULT result;
	int i;
	WAVEFORMATEX waveformat;

	// inicjalizacja struktury
	
	sound = NULL;
	audio_sem = NULL;
	
	for ( i = 0; i < NUM_BUFFERS; ++i )
		wavebuf[i].dwUser = 0xFFFF;
	mixbuf = NULL;

	// ustaw podstawowe parametry dla wave
	
	memset(&waveformat, 0, sizeof(waveformat));
	waveformat.wFormatTag = WAVE_FORMAT_PCM;

	// pobierz ustawienia z AudioSpec

	switch ( spec->format & 0xFF ) 
	{
		case 8:
			// dane sa jako unsigned 8bit 
			spec->format = AUDIO_U8;
			waveformat.wBitsPerSample = 8;
			break;
		case 16:
			// dane sa jako signed 16bit
			spec->format = AUDIO_S16;
			waveformat.wBitsPerSample = 16;
			break;
		default:
			FOX_SetError("[FOX]: format dzwieku nie obslugiwany");
			return(-1);
	}
	
	waveformat.nChannels = spec->channels;
	waveformat.nSamplesPerSec = spec->freq;
	
	waveformat.nBlockAlign =
		waveformat.nChannels * (waveformat.wBitsPerSample/8);
	
	waveformat.nAvgBytesPerSec = 
		waveformat.nSamplesPerSec * waveformat.nBlockAlign;

	// sprawdz rozmiar bufora - minimum 1/4 sekundy (word align)
	
	if ( spec->samples < (spec->freq/4) )
		spec->samples = ((spec->freq/4)+3)&~3;

	// aktualizuj ten fragment
	
	FOX_CalculateAudioSpec(spec);

	// otwórz urz¹dzenie audio
	
	result = waveOutOpen(&sound, WAVE_MAPPER, &waveformat,
			(DWORD)FillSound, (DWORD)this, CALLBACK_FUNCTION);

	if ( result != MMSYSERR_NOERROR ) 
	{
		SetMMerror("waveOutOpen()", result);
		return(-1);
	}

#ifdef SOUND_DEBUG
	// sprawdz co otworzylismy 
	{
		WAVEOUTCAPS caps;

		result = waveOutGetDevCaps((UINT)sound, &caps, sizeof(caps));
		if ( result != MMSYSERR_NOERROR ) {
			SetMMerror("waveOutGetDevCaps()", result);
			return(-1);
		}
		printf("[FOX]: urzadzenie audio: %s\n", caps.szPname);
	}
#endif

	// stwórz semafor ;)
	
	audio_sem = CreateSemaphore(NULL, NUM_BUFFERS-1, NUM_BUFFERS, NULL);

	if ( audio_sem == NULL ) 
	{
		FOX_SetError("[FOX]: nie moge stworzyc semafora");
		return(-1);
	}

	// otwórz bufor audio
	
	mixbuf = (Uint8 *)malloc(NUM_BUFFERS*spec->size);

	if ( mixbuf == NULL ) 
	{
		FOX_SetError("[FOX]: za malo pamieci w trakcie odtwarzania dzwieku");
		return(-1);
	}
	
	for ( i = 0; i < NUM_BUFFERS; ++i ) 
	{
		memset(&wavebuf[i], 0, sizeof(wavebuf[i]));
	
		wavebuf[i].lpData = (LPSTR) &mixbuf[i*spec->size];
		wavebuf[i].dwBufferLength = spec->size;
		wavebuf[i].dwFlags = WHDR_DONE;
		
		result = waveOutPrepareHeader(sound, &wavebuf[i],
							sizeof(wavebuf[i]));
		
		if ( result != MMSYSERR_NOERROR ) 
		{
			SetMMerror("waveOutPrepareHeader()", result);
			return(-1);
		}
	}

	// gotowe
	
	next_buffer = 0;
	return(0);
}

// end
