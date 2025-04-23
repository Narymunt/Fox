// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - potrzebne skoro powinien leciec przez dxmedia ? 

#ifndef _FOX_sysaudio_h
#define _FOX_sysaudio_h

#include "FOX_mutex.h"
#include "FOX_thread.h"

// obsluga dzwieku 

typedef struct FOX_AudioDevice FOX_AudioDevice;

// struktura glowna 

#define _THIS	FOX_AudioDevice *_this
#ifndef _STATUS
#define _STATUS	FOX_status *status
#endif

struct FOX_AudioDevice {

	const char *name;		// nazwa sterownika 
	const char *desc;		// opis sterownika 
	
	int  (*OpenAudio)(_THIS, FOX_AudioSpec *spec);	// funkcje 
	
	void (*ThreadInit)(_THIS);						// watek startowy 
	void (*WaitAudio)(_THIS);
	void (*PlayAudio)(_THIS);
	
	Uint8 *(*GetAudioBuf)(_THIS);
	
	void (*WaitDone)(_THIS);
	void (*CloseAudio)(_THIS);

	// zwykle dane dla urzadzen
	
	FOX_AudioSpec spec;		// specyfikacja
	FOX_AudioCVT convert;	// blok konwersji 

	// flagi stanu 
	
	int enabled;
	int paused;
	int opened;

	// fake audio buffer, jezeli sprzet jest zajety 
	
	Uint8 *fake_stream;

	// semafor do blokowania buforow audio 
	
	FOX_mutex *mixer_lock;

	// watek dla audio device (feed)
	
	FOX_Thread *thread;
	Uint32 threadid;

	struct FOX_PrivateAudioData *hidden;	// dane prywatne dla sterownika 
	
	void (*free)(_THIS);		// zwolnienie tej struktury
};
#undef _THIS

// boot strap 

typedef struct AudioBootStrap {
	const char *name;
	const char *desc;
	int (*available)(void);
	FOX_AudioDevice *(*create)(int devindex);
} AudioBootStrap;

//#ifdef DMEDIA_SUPPORT
//extern AudioBootStrap DMEDIA_bootstrap;
//#endif

//#ifdef ENABLE_DIRECTX
extern AudioBootStrap DSOUND_bootstrap;
//#endif

//#ifdef ENABLE_WINDIB
//extern AudioBootStrap WAVEOUT_bootstrap;
//#endif

// to jest nasze aktualne urzadzenie audio 

extern FOX_AudioDevice *current_audio;

#endif 

// end


