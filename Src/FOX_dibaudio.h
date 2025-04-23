// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:


// liniowy dostêp do bufora audio

#ifndef _FOX_lowaudio_h
#define _FOX_lowaudio_h

#include "FOX_sysaudio.h"

// ukryty wskaŸnik this dla funkcji video

#define _THIS	FOX_AudioDevice *this

// !!! tego nie zmieniac !!!

#define NUM_BUFFERS 2	

// dane 

struct FOX_PrivateAudioData {
	HWAVEOUT sound;
	HANDLE audio_sem;
	Uint8 *mixbuf;					// liniowy, zaalokowany bufor
	WAVEHDR wavebuf[NUM_BUFFERS];	// fragmentu dzwieku audio
	int next_buffer;
};

// stare nazwy zmiennych

#define sound			(this->hidden->sound)
#define audio_sem 		(this->hidden->audio_sem)
#define mixbuf			(this->hidden->mixbuf)
#define wavebuf			(this->hidden->wavebuf)
#define next_buffer		(this->hidden->next_buffer)

#endif 

// end

