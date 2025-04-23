// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - nalozenie filtrow przez dx-media

// mixowanie dzwieku przez directx

#include <stdio.h>

#include "FOX_types.h"
#include "FOX_error.h"
#include "FOX_timer.h"
#include "FOX_audio.h"
#include "FOX_audio_c.h"
#include "FOX_dx8audio.h"

// usunac komentarz jezeli chcemy uzywac DirectX 6 DirectSoundNotify interface

//#define USE_POSITION_NOTIFY

// wskazniki dx na funkcje audio

HRESULT (WINAPI *DSoundCreate)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN);

// funkcje sterownika audio

static int DX8_OpenAudio(_THIS, FOX_AudioSpec *spec);
static void DX8_ThreadInit(_THIS);
static void DX8_WaitAudio_BusyWait(_THIS);

// dla dx6 interface

#ifdef USE_POSITION_NOTIFY
	static void DX6_WaitAudio_EventWait(_THIS);
#endif


static void DX8_PlayAudio(_THIS);
static Uint8 *DX8_GetAudioBuf(_THIS);
static void DX8_WaitDone(_THIS);
static void DX8_CloseAudio(_THIS);

// funkcje audio bootstrap

static int Audio_Available(void)
{
	HINSTANCE DSoundDLL;
	int dsound_ok;

	// sprawdzenie wersji dsound.dll
	
	dsound_ok = 0;
	DSoundDLL = LoadLibrary("DSOUND.DLL");

	// do poprawienia - pod 2k wyrzuca jakies smieci (jakie?)
	
	if ( DSoundDLL != NULL ) {

		OSVERSIONINFO ver;
		
		ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		
		GetVersionEx(&ver);
		
		switch (ver.dwPlatformId) 
		{
			case VER_PLATFORM_WIN32_NT:
				if ( ver.dwMajorVersion > 4 ) 
				{
					// win 2K 
					dsound_ok = 1;
				} 
				else 
				{
					// win nt
					dsound_ok = 0;
				}
				break;
			default:
				// win95, win98, winME 
				dsound_ok = 1;
				break;
		}

		// dx ponizej 5 wyrzuci tutaj smieci i przeniesie do dib_audio

		if (dsound_ok) 
		{
			// DirectSoundCaptureCreate
						
			if (!GetProcAddress(DSoundDLL, "DirectSoundCaptureCreate"))
				dsound_ok = 0;

		}
		
		// wyczysc 
				
		FreeLibrary(DSoundDLL);
	}
	return(dsound_ok);
}


// funkcje do dynamicznego ³adowania funkcji directx

static HINSTANCE DSoundDLL = NULL;

// zwolnij

static void DX8_Unload(void)
{
	if ( DSoundDLL != NULL ) 
	{
		FreeLibrary(DSoundDLL);
		DSoundCreate = NULL;
		DSoundDLL = NULL;
	}
}

// zaladuj funkcje dx

static int DX8_Load(void)
{
	int status;

	DX8_Unload();
	
	DSoundDLL = LoadLibrary("DSOUND.DLL");
	
	if ( DSoundDLL != NULL ) 
	{
		DSoundCreate = (void *)GetProcAddress(DSoundDLL,
					"DirectSoundCreate");
	}
	
	if ( DSoundDLL && DSoundCreate ) 
	{
		status = 0;		// ok 
	} 
	else 
	{
		DX8_Unload();	// zaladowane sa smieci - usun je
		status = -1;	// 0xffff
	}
	
	return status;
}

// usun urzadzenie

static void Audio_DeleteDevice(FOX_AudioDevice *device)
{
	DX8_Unload();
	free(device->hidden);
	free(device);
}

// handle, obsluga 

static FOX_AudioDevice *Audio_CreateDevice(int devindex)
{
	FOX_AudioDevice *this;		// aj, niedobrze!

	// zaladuj dx
	
	if ( DX8_Load() < 0 ) 
	{
		return(NULL);
	}

	// zainicjalizuj wszystkie zmienne, które czyœcimy przy shutdown

	
	this = (FOX_AudioDevice *)malloc(sizeof(FOX_AudioDevice));
	
	if ( this ) 
	{
		memset(this, 0, (sizeof *this));
		this->hidden = (struct FOX_PrivateAudioData *)
				malloc((sizeof *this->hidden));
	}

	if ( (this == NULL) || (this->hidden == NULL) ) 
	{
		FOX_OutOfMemory();
		if ( this ) 
		{
			free(this);
		}
		return(0);
	}

	memset(this->hidden, 0, (sizeof *this->hidden));

	// ustaw wskazniki funkcji
	
	this->OpenAudio = DX8_OpenAudio;
	this->ThreadInit = DX8_ThreadInit;
	this->WaitAudio = DX8_WaitAudio_BusyWait;
	this->PlayAudio = DX8_PlayAudio;
	this->GetAudioBuf = DX8_GetAudioBuf;
	this->WaitDone = DX8_WaitDone;
	this->CloseAudio = DX8_CloseAudio;

	this->free = Audio_DeleteDevice;

	return this;
}

AudioBootStrap DSOUND_bootstrap = {
	"dsound", "Win95/98/2000 DirectSound",
	Audio_Available, Audio_CreateDevice
};

// ustaw wskazniki bledow

static void SetDSerror(const char *function, int code)
{
	static const char *error;
	static char  errbuf[BUFSIZ];

	errbuf[0] = 0;
	switch (code) 
	{
		case E_NOINTERFACE:
			error = "[FOX]: nieznany DirectX";
			break;
		case DSERR_ALLOCATED:
			error = "[FOX]: urzadzenie audio jest juz uzywane";
			break;
		case DSERR_BADFORMAT:
			error = "[FOX]: nie obslugiwany format dzwieku";
			break;
		case DSERR_BUFFERLOST:
			error = "[FOX]: utrata kontroli nad buforem miksujacym";
			break;
		case DSERR_CONTROLUNAVAIL:
			error = "[FOX]: nie mozna kontrolowac urzadzenia audio";
			break;
		case DSERR_INVALIDCALL:
			error = "[FOX]: niewlasciwe wywolanie dla urzadzenia audio";
			break;
		case DSERR_INVALIDPARAM:
			error = "[FOX]: zly parametr dla urzadzenia audio";
			break;
		case DSERR_NODRIVER:
			error = "[FOX]: brak urzadzenia audio";
			break;
		case DSERR_OUTOFMEMORY:
			error = "[FOX]: za malo pamieci by obsluzyc urzadzenie audio";
			break;
		case DSERR_PRIOLEVELNEEDED:
			error = "[FOX]: za maly priorytet dla funkcji audio";
			break;
		case DSERR_UNSUPPORTED:
			error = "[FOX]: funkcja audio nie obslugiwana";
			break;
		default:
			sprintf(errbuf, "[FOX]: %s: nieznany blad DirectSound: 0x%x",
								function, code);
			break;
	}
	
	if ( ! errbuf[0] ) 
	{
		sprintf(errbuf, "[FOX]: %s: %s", function, error);
	}
	
	FOX_SetError("%s", errbuf);
	
	return;
}

// DirectSound musi byc doczepiony do okna

static HWND mainwin = NULL;

void DX8_SoundFocus(HWND hwnd)
{
	mainwin = hwnd;
}

// inicjalizuj watek 

static void DX8_ThreadInit(_THIS)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

// zaczekaj na audio 

static void DX8_WaitAudio_BusyWait(_THIS)
{
	DWORD status;
	DWORD cursor, junk;
	HRESULT result;

	// musimy czekac, gdyz urzadzenie nie jest teraz dostepne
	
	result = IDirectSoundBuffer_GetCurrentPosition(mixbuf, &cursor, &junk);
	
	if ( result != DS_OK ) 
	{
		if ( result == DSERR_BUFFERLOST ) 
		{
			IDirectSoundBuffer_Restore(mixbuf);
		}

	// tylko do sprawdzenia

#ifdef DEBUG_SOUND
		SetDSerror("[FOX]: DirectSound GetCurrentPosition", result);
#endif
		return;
	}
	
	cursor /= mixlen; // tak na wszelki wypadek 

	while ( cursor == playing ) 
	{
		// !!! do poprawienia - czekaj tylkko tyle ile musisz 
		
		FOX_Delay(10);
		
		// sproboj odtworzyc bufer audio
		
		IDirectSoundBuffer_GetStatus(mixbuf, &status);
		
		if ( (status&DSBSTATUS_BUFFERLOST) ) 
		{
			IDirectSoundBuffer_Restore(mixbuf);
			IDirectSoundBuffer_GetStatus(mixbuf, &status);
		
			if ( (status&DSBSTATUS_BUFFERLOST) ) 
			{
				break;
			}
		}
		
		if ( ! (status&DSBSTATUS_PLAYING) ) 
		{
			result = IDirectSoundBuffer_Play(mixbuf, 0, 0, DSBPLAY_LOOPING);
			
			if ( result == DS_OK ) 
			{
				continue;
			}

#ifdef DEBUG_SOUND
			SetDSerror("DirectSound Play", result);
#endif
			return;
		}

		// znajdz w ktorym miejscu odgrywamy 
		
		result = IDirectSoundBuffer_GetCurrentPosition(mixbuf,
								&cursor, &junk);
		if ( result != DS_OK ) 
		{
			SetDSerror("[FOX]: DirectSound GetCurrentPosition", result);
			return;
		}
		cursor /= mixlen;
	}
}

// uzywamy ? 

#ifdef USE_POSITION_NOTIFY

static void DX6_WaitAudio_EventWait(_THIS)
{
	DWORD status;
	HRESULT result;

	// sprobuj odtworzyc stracony bufor
	
	IDirectSoundBuffer_GetStatus(mixbuf, &status);
	
	if ( (status&DSBSTATUS_BUFFERLOST) ) 
	{
		IDirectSoundBuffer_Restore(mixbuf);
		IDirectSoundBuffer_GetStatus(mixbuf, &status);
	
		if ( (status&DSBSTATUS_BUFFERLOST) ) 
		{
			return;
		}
	}

	if ( ! (status&DSBSTATUS_PLAYING) ) 
	{
		result = IDirectSoundBuffer_Play(mixbuf, 0, 0, DSBPLAY_LOOPING);
		
		if ( result != DS_OK ) 
		{
			#ifdef DEBUG_SOUND
			SetDSerror("[FOX]: DirectSound Play", result);
			#endif
			
			return;
		}
	}
	WaitForSingleObject(audio_event, INFINITE);
}
#endif 

// tak po prostu 

static void DX8_PlayAudio(_THIS)
{

	// odblokuj bufor tak zeby mozna bylo to odtwarzac
		
	if ( locked_buf ) 
	{
		IDirectSoundBuffer_Unlock(mixbuf, locked_buf, mixlen, NULL, 0);
	}

}

// niebezpieczne, ale na razie nie ma innego rozwiazania

static Uint8 *DX8_GetAudioBuf(_THIS)
{
	DWORD   cursor, junk;
	HRESULT result;
	DWORD   rawlen;

	// sprawdz ktory najpierw
	
	locked_buf = NULL;

	result = IDirectSoundBuffer_GetCurrentPosition(mixbuf, &cursor, &junk);
	
	if ( result == DSERR_BUFFERLOST ) 
	{
		IDirectSoundBuffer_Restore(mixbuf);
		result = IDirectSoundBuffer_GetCurrentPosition(mixbuf,
								&cursor, &junk);
	}
	
	if ( result != DS_OK ) 
	{
		SetDSerror("[FOX]: DirectSound GetCurrentPosition", result);
		return(NULL);
	}

	cursor /= mixlen;
	playing = cursor;
	cursor = (cursor+1)%NUM_BUFFERS;
	cursor *= mixlen;

	// zablokuj bufor audio

	result = IDirectSoundBuffer_Lock(mixbuf, cursor, mixlen,
				(LPVOID *)&locked_buf, &rawlen, NULL, &junk, 0);
	
	if ( result == DSERR_BUFFERLOST ) 
	{
		IDirectSoundBuffer_Restore(mixbuf);
		result = IDirectSoundBuffer_Lock(mixbuf, cursor, mixlen,
				(LPVOID *)&locked_buf, &rawlen, NULL, &junk, 0);
	}

	if ( result != DS_OK ) 
	{
		SetDSerror("DirectSound Lock", result);
		return(NULL);
	}
	return(locked_buf);
}

// czekaj, az skonczone

static void DX8_WaitDone(_THIS)
{
	Uint8 *stream;

	// poczekaj na skonczenie chunk

	stream = this->GetAudioBuf(this);

	if ( stream != NULL ) 
	{
		memset(stream, silence, mixlen);
		this->PlayAudio(this);
	}
	this->WaitAudio(this);

	// przestan powtarzac 
	
	IDirectSoundBuffer_Stop(mixbuf);
}

// zamknij urzadzenie audio

static void DX8_CloseAudio(_THIS)
{
	if ( sound != NULL ) 
	{
		if ( mixbuf != NULL ) 
		{
			IDirectSoundBuffer_Release(mixbuf);	// wyczysc
			mixbuf = NULL;
		}

		if ( audio_event != NULL ) 
		{
			CloseHandle(audio_event);
			audio_event = NULL;
		}

		IDirectSound_Release(sound);
		sound = NULL;
	}
}

// ta funkcja proboje otworzy primary audio buffer, zwraca ilosc chunk dostepnych

static int CreatePrimary(LPDIRECTSOUND sndObj, HWND focus, 
	LPDIRECTSOUNDBUFFER *sndbuf, WAVEFORMATEX *wavefmt, Uint32 chunksize)
{
	HRESULT result;
	DSBUFFERDESC format;
	DSBCAPS caps;
	int numchunks;

	// sproboj ustawic lepszy priorytet dla mixowania
	
	result = IDirectSound_SetCooperativeLevel(sndObj, focus,
							DSSCL_WRITEPRIMARY);
	if ( result != DS_OK ) 
	{
		#ifdef DEBUG_SOUND
			SetDSerror("[FOX]: DirectSound SetCooperativeLevel", result);
		#endif
		
		return(-1);
	}

	// sproboj stworzy primary buffer
	
	memset(&format, 0, sizeof(format));
	format.dwSize = sizeof(format);
	format.dwFlags=(DSBCAPS_PRIMARYBUFFER|DSBCAPS_GETCURRENTPOSITION2);
	format.dwFlags |= DSBCAPS_STICKYFOCUS;

	#ifdef USE_POSITION_NOTIFY
		format.dwFlags |= DSBCAPS_CTRLPOSITIONNOTIFY;
	#endif

	result = IDirectSound_CreateSoundBuffer(sndObj, &format, sndbuf, NULL);
	
	if ( result != DS_OK ) 
	{
	#ifdef DEBUG_SOUND
		SetDSerror("DirectSound CreateSoundBuffer", result);
	#endif
		return(-1);
	}

	// sprawdz rozmiary
	
	memset(&caps, 0, sizeof(caps));
	caps.dwSize = sizeof(caps);
	result = IDirectSoundBuffer_GetCaps(*sndbuf, &caps);
	
	if ( result != DS_OK ) 
	{
	
	#ifdef DEBUG_SOUND
		SetDSerror("DirectSound GetCaps", result);
	#endif
	
		IDirectSoundBuffer_Release(*sndbuf);
		
		return(-1);
	}

	if ( (chunksize > caps.dwBufferBytes) ||
				((caps.dwBufferBytes%chunksize) != 0) ) {

		IDirectSoundBuffer_Release(*sndbuf);

		FOX_SetError(
"[FOX]: pierwszy bufor ma rozmiar: %d, i nie moze byc rozdzielony na czesci %d bajtowe\n",
					caps.dwBufferBytes, chunksize);
		return(-1);
	}

	numchunks = (caps.dwBufferBytes/chunksize);

	// ustaw format audio 
	
	result = IDirectSoundBuffer_SetFormat(*sndbuf, wavefmt);
	
	if ( result != DS_OK ) 
	{

		#ifdef DEBUG_SOUND
			SetDSerror("DirectSound SetFormat", result);
		#endif
			
			IDirectSoundBuffer_Release(*sndbuf);
		return(-1);
	}
	return(numchunks);
}

// funkcja proboje otworzyc secondary audio buffer, zwraca ilosc chunk

static int CreateSecondary(LPDIRECTSOUND sndObj, HWND focus,
	LPDIRECTSOUNDBUFFER *sndbuf, WAVEFORMATEX *wavefmt, Uint32 chunksize)
{
	const int numchunks = 2;
	HRESULT result;
	DSBUFFERDESC format;
	LPVOID pvAudioPtr1, pvAudioPtr2;
	DWORD  dwAudioBytes1, dwAudioBytes2;

	// ustaw lepszy priorytet
	
	if ( focus ) 
	{
		result = IDirectSound_SetCooperativeLevel(sndObj,
					focus, DSSCL_PRIORITY);
	} 
	else 
	{
		result = IDirectSound_SetCooperativeLevel(sndObj,
					GetDesktopWindow(), DSSCL_NORMAL);
	}
	
	if ( result != DS_OK ) 
	{
	#ifdef DEBUG_SOUND
		SetDSerror("DirectSound SetCooperativeLevel", result);
	#endif
		return(-1);
	}

	// secondary buffer
	
	memset(&format, 0, sizeof(format));
	format.dwSize = sizeof(format);
	format.dwFlags = DSBCAPS_GETCURRENTPOSITION2;

	#ifdef USE_POSITION_NOTIFY
		format.dwFlags |= DSBCAPS_CTRLPOSITIONNOTIFY;
	#endif
	
	if ( ! focus ) 
	{
		format.dwFlags |= DSBCAPS_GLOBALFOCUS;
	} 
	else 
	{
		format.dwFlags |= DSBCAPS_STICKYFOCUS;
	}
	
	format.dwBufferBytes = numchunks*chunksize;
	
	if ( (format.dwBufferBytes < DSBSIZE_MIN) ||
	     (format.dwBufferBytes > DSBSIZE_MAX) ) 
	{
		FOX_SetError("[FOX]: Sound buffer size must be between %d and %d",
				DSBSIZE_MIN/numchunks, DSBSIZE_MAX/numchunks);
		return(-1);
	}
	
	format.dwReserved = 0;
	format.lpwfxFormat = wavefmt;
	result = IDirectSound_CreateSoundBuffer(sndObj, &format, sndbuf, NULL);
	
	if ( result != DS_OK ) 
	{
		SetDSerror("[FOX]: DirectSound CreateSoundBuffer", result);
		return(-1);
	}
	
	IDirectSoundBuffer_SetFormat(*sndbuf, wavefmt);

	// cisza 
	
	result = IDirectSoundBuffer_Lock(*sndbuf, 0, format.dwBufferBytes,
	                                 (LPVOID *)&pvAudioPtr1, &dwAudioBytes1,
	                                 (LPVOID *)&pvAudioPtr2, &dwAudioBytes2,
	                                 DSBLOCK_ENTIREBUFFER);
	if ( result == DS_OK ) 
	{
		if ( wavefmt->wBitsPerSample == 8 ) 
		{
			memset(pvAudioPtr1, 0x80, dwAudioBytes1);
		} 
		else 
		{
			memset(pvAudioPtr1, 0x00, dwAudioBytes1);
		}
		
		IDirectSoundBuffer_Unlock(*sndbuf,
		                          (LPVOID)pvAudioPtr1, dwAudioBytes1,
		                          (LPVOID)pvAudioPtr2, dwAudioBytes2);
	}

	// gotowe - zwracamy chunk
	
	return(numchunks);
}

// funkcja proboje ustawic pozycje, bardzo brzydko napisana !!! 
// potem trzeba zmienic

#ifdef USE_POSITION_NOTIFY
static int CreateAudioEvent(_THIS)
{
	LPDIRECTSOUNDNOTIFY notify;
	DSBPOSITIONNOTIFY *notify_positions;
	int i, retval;
	HRESULT result;

	// domyslne ustawienie - blad i wyjscie 
	
	retval = -1;
	notify = NULL;

	// zapytaj o interfaces
	
	result = IDirectSoundBuffer_QueryInterface(mixbuf,
			&IID_IDirectSoundNotify, (void *)&notify);
	
	if ( result != DS_OK ) 
	{
		goto done;
	}

	// zaalokuj struktury
	
	notify_positions = (DSBPOSITIONNOTIFY *)malloc(NUM_BUFFERS*
					sizeof(*notify_positions));
	
	if ( notify_positions == NULL ) 
	{
		goto done;
	}

	// stworz watek 
	
	audio_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	if ( audio_event == NULL ) 
	{
		goto done;
	}

	// ustaw struktury
	
	for ( i=0; i<NUM_BUFFERS; ++i ) 
	{
		notify_positions[i].dwOffset = i*mixlen;
		notify_positions[i].hEventNotify = audio_event;
	}
	
	result = IDirectSoundNotify_SetNotificationPositions(notify,
					NUM_BUFFERS, notify_positions);
	
	if ( result == DS_OK ) 
	{
		retval = 0;
	}

done:

	if ( notify != NULL ) 
	{
		IDirectSoundNotify_Release(notify);
	}
	return(retval);
}
#endif 

// otworz audio 

static int DX8_OpenAudio(_THIS, FOX_AudioSpec *spec)
{
	HRESULT      result;
	WAVEFORMATEX waveformat;

	// ustaw podstawowe parametry dla WAVE
	
	memset(&waveformat, 0, sizeof(waveformat));
	waveformat.wFormatTag = WAVE_FORMAT_PCM;

	// ustaw parametry wg AudioSpec
	
	switch ( spec->format & 0xFF ) 
	{
		case 8:
			spec->format = AUDIO_U8;		// unsigned 8 bit audio data
			silence = 0x80;
			waveformat.wBitsPerSample = 8;
			break;
		
		case 16:
			spec->format = AUDIO_S16;		// signed 16 bit audio data
			silence = 0x00;
			waveformat.wBitsPerSample = 16;
			break;
		default:
			FOX_SetError("[FOX]: nie obslugiwany format dzwieku");	// jakis inny 
			return(-1);
	}

	waveformat.nChannels = spec->channels;
	waveformat.nSamplesPerSec = spec->freq;

	waveformat.nBlockAlign =
		waveformat.nChannels * (waveformat.wBitsPerSample/8);
	
	waveformat.nAvgBytesPerSec = 
		waveformat.nSamplesPerSec * waveformat.nBlockAlign;

	// aktualizuj fragment 
	
	FOX_CalculateAudioSpec(spec);

	// otworz audio device 
	
	result = DSoundCreate(NULL, &sound, NULL);

	if ( result != DS_OK ) 
	{
		SetDSerror("DirectSoundCreate", result);
		return(-1);
	}

	// otworz bufor audio do ktorego bedziemy zapisywac
	
	NUM_BUFFERS = -1;

	if ( mainwin ) 
	{
		NUM_BUFFERS = CreatePrimary(sound, mainwin, &mixbuf,
						&waveformat, spec->size);
	}
	
	if ( NUM_BUFFERS < 0 ) 
	{
		NUM_BUFFERS = CreateSecondary(sound, mainwin, &mixbuf,
						&waveformat, spec->size);
		if ( NUM_BUFFERS < 0 ) 
		{
			return(-1);
		}

		#ifdef DEBUG_SOUND
		fprintf(stderr, "[FOX]: uzywam drugoplanowego bufora audio\n");
		#endif
	}

		#ifdef DEBUG_SOUND
	
		else
		fprintf(stderr, "[FOX]: uzywam pierwszoplanowego bufora audio\n");
		
		#endif

	// bufor wystartuje automatycznie DX8_WaitAudio()
		
	playing = 0;
	mixlen = spec->size;

#ifdef USE_POSITION_NOTIFY
	
	// zobacz co z event notification
		
	if ( CreateAudioEvent(this) == 0 ) 
	{
		this->WaitAudio = DX6_WaitAudio_EventWait;
	} 
	else 
	{
		this->WaitAudio = DX8_WaitAudio_BusyWait;
	}

#endif
	return(0);

}

//end
