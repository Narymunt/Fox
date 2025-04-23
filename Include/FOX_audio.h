// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:
//	- mp3
//	- mpeg


// odtwarzanie dzwieku 

#ifndef _FOX_audio_h
#define _FOX_audio_h

#include <stdio.h>

#include "FOX_main.h"
#include "FOX_types.h"
#include "FOX_error.h"
#include "FOX_rwops.h"
#include "FOX_byteorder.h"

// dodatkowy stuff

#include "FOX_begin.h"

// funkcje traktuj jako C

#ifdef __cplusplus
extern "C" {
#endif

// wszystkie wartosci w tej strukturze sa wyliczane przez FOX_OpenAudio()

typedef struct {
	int freq;			// DSP freq - probkowanie na sekunde np 22000khz
	Uint16 format;		// format danych audio
	Uint8  channels;	// ilosc kanalow, 1 mono, 2 stereo
	Uint8  silence;		// wyliczana wartosc ciszy dla bufora
	Uint16 samples;		// rozmiar bufora audio x2
	Uint16 padding;		// pomocnicze
	Uint32 size;		// rozmiar bufora w bajtach
	
	// ta funkcja jest wywolywana kiedy urzadzenie audio potrzebuje wiecej danych
	// zeby moc dalej probkowac, stream jest wskaznikiem na bufer danych, len to dlugosc
	// w bajtach. kiedy funkcja juz wroci, bufor praktycznie nie bedzie potrzebny.
	// sample stereo sa trzymane jako lewy-prawy,lewy-prawy
	
	void (*callback)(void *userdata, Uint8 *stream, int len);
	void  *userdata;
} FOX_AudioSpec;

// flagi dla formatu audio, domyslnie LSB

#define AUDIO_U8		0x0008			// unsigned 8 bit
#define AUDIO_S8		0x8008			// signed 8-bit 
#define AUDIO_U16LSB	0x0010			// unsigned 16-bit 
#define AUDIO_S16LSB	0x8010			// signed 16-bit 
#define AUDIO_U16MSB	0x1010			// jak wyzej, ale big-endian byte order 
#define AUDIO_S16MSB	0x9010			// jak wyzej, ale big-endian byte order
#define AUDIO_U16		AUDIO_U16LSB	
#define AUDIO_S16		AUDIO_S16LSB

// domyslny rozklad 

#if FOX_BYTEORDER == FOX_LIL_ENDIAN

	#define AUDIO_U16SYS	AUDIO_U16LSB
	#define AUDIO_S16SYS	AUDIO_S16LSB

#else

	#define AUDIO_U16SYS	AUDIO_U16MSB
	#define AUDIO_S16SYS	AUDIO_S16MSB

#endif

// struktura do konwersji filtrow i buforow audio

typedef struct FOX_AudioCVT {
	
	int needed;				// ustawione na 1 jezeli konwersja mozliwa
	
	Uint16 src_format;		// format zrodlowy audio
	Uint16 dst_format;		// format docelowy
	
	double rate_incr;		// zwiekszenie czestotliwosci
	
	Uint8 *buf;				// bufor do trzymania wszystkich danych audio
	
	int    len;				// dlugosc orginalnego bufora
	int    len_cvt;			// dlugosc bufora po konwersji
	int    len_mult;		// bufor musi byc dlugosci len*len_mult 
	
	double len_ratio; 		// podana dlugosc, koncowy rozmiar to len*len_ratio
	
	void (*filters[10])(struct FOX_AudioCVT *cvt, Uint16 format);

	int filter_index;		// aktualna konwersja 

} FOX_AudioCVT;

// te funkcje sa uzywane tylko wtedy, gdy do urzadzenia nie odwolujemy sie
// bezposrednio przez directx, tylko przez konkretny driver, normalnie dzwiek
// inicjalizowany jest przez wlaczenie flagi przy FOX_Init, gdyz te parametry
// od razu sa brane pod uwage przy inicjalizacji directx

extern DECLSPEC int FOX_AudioInit(const char *driver_name);

extern DECLSPEC void FOX_AudioQuit(void);

// funkcja wypelnia bufor nazwa sterownika i zwraca wskaznik
// jezeli zaden driver nie byl zainicjalizowany, to wtedy zwraca NULL
// !!! nie uzywac jesli do urzadzenia audio odwolujemy sie przez directx

extern DECLSPEC char *FOX_AudioDriverName(char *namebuf, int maxlen);

// funkcja otwiera (inicjalizuje) urzadzenie audio i zwraca 0 jezeli sie udalo oraz
// ustawia aktualne zmienne, zwraca -1 jezeli nie udalo sie 
// ms=(samples*1000)/freq
 
extern DECLSPEC int FOX_OpenAudio(FOX_AudioSpec *desired, FOX_AudioSpec *obtained);

// podaj aktualny stan audio

typedef enum {
	FOX_AUDIO_STOPPED = 0,
	FOX_AUDIO_PLAYING,
	FOX_AUDIO_PAUSED
} FOX_audiostatus;

extern DECLSPEC FOX_audiostatus FOX_GetAudioStatus(void);

// zatrzymaj (pauza, bez wylaczania urzadzenia) i wznow

extern DECLSPEC void FOX_PauseAudio(int pause_on);

// wczytaj WAVE

extern DECLSPEC FOX_AudioSpec *FOX_LoadWAV_RW(FOX_RWops *src, int freesrc,
		 FOX_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);

// wczytaj WAVE - podejscie drugie ;)

#define FOX_LoadWAV(file, spec, audio_buf, audio_len) \
	FOX_LoadWAV_RW(FOX_RWFromFile(file, "rb"),1, spec,audio_buf,audio_len)

// zwolnij dane wczytane i zaalokowane przez FOX_LoadWAV_RW()

extern DECLSPEC void FOX_FreeWAV(Uint8 *audio_buf);

// funkcja do konwertowania danych w buforze audio
// 0 wszystko ok, -1 jezeli byl blad

extern DECLSPEC int FOX_BuildAudioCVT(FOX_AudioCVT *cvt,
		Uint16 src_format, Uint8 src_channels, int src_rate,
		Uint16 dst_format, Uint8 dst_channels, int dst_rate);

extern DECLSPEC int FOX_ConvertAudio(FOX_AudioCVT *cvt);

// zmiksuj dwa dzwieki, czyli interpolacja buforow
// zakres dzwieku (volume) 0-128 musi byc ustawione w FOX_MIX_MAXVOLUME
// nie zmienia to glosnosci samej karty, tylko zmiejsza w buforze sampla

#define FOX_MIX_MAXVOLUME 128

extern DECLSPEC void FOX_MixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, int volume);

// zablokowanie urzadzenia audio

extern DECLSPEC void FOX_LockAudio(void);

extern DECLSPEC void FOX_UnlockAudio(void);

// wylacz urzadzenie audio, zwolnij bufor

extern DECLSPEC void FOX_CloseAudio(void);

// koniec traktowania funkcji jako C

#ifdef __cplusplus
}
#endif

//=== koncowe smieci

#include "FOX_end.h"

#endif 

// end

