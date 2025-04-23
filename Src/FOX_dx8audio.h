// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:


// obsluga dzwieku przez directx 

#ifndef _FOX_lowaudio_h
#define _FOX_lowaudio_h

#include "../../directx.h"

#include "FOX_sysaudio.h"

// ukryty wskaznik this dla funkcji video 

#define _THIS	FOX_AudioDevice *this

// obiekty DirectSound 

struct FOX_PrivateAudioData {

	LPDIRECTSOUND sound;
	LPDIRECTSOUNDBUFFER mixbuf;
	
	int NUM_BUFFERS;
	int mixlen, silence;
	
	DWORD playing;
	Uint8 *locked_buf;
	HANDLE audio_event;
};

// stare nazwy zmiennych 

#define sound			(this->hidden->sound)
#define mixbuf			(this->hidden->mixbuf)
#define NUM_BUFFERS		(this->hidden->NUM_BUFFERS)
#define mixlen			(this->hidden->mixlen)
#define silence			(this->hidden->silence)
#define playing			(this->hidden->playing)
#define locked_buf		(this->hidden->locked_buf)
#define audio_event		(this->hidden->audio_event)

#endif 

// end
