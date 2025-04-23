// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:
// - plyta zmiksowana (audio+data) czy jest odtwarzana poprawnie ? 
// - czy w napedzie jest cdrom czy dvd ? 

// odtwarzanie sciezki audio z CD

#ifndef _FOX_cdrom_h
#define _FOX_cdrom_h

#include "FOX_types.h"	// aliasy do unsigned itd. 

#include "FOX_begin.h"	// dodatkowy stuff, alignacja itd. 

// traktuj funkcje jak w c

#ifdef __cplusplus
extern "C" {
#endif

// zeby moc uzywac tych funkcji, musi byc ustawiona flaga FOX_INIT_CDROM, bo
// parametry przekazywane sa do directx

#define FOX_MAX_TRACKS	99 // maxymalna ilosc muzyczek na plycie 

// mozliwe nosniki

#define FOX_AUDIO_TRACK	0x00
#define FOX_DATA_TRACK	0x04

// mozliwe stany w jakich moze znajdowac sie CDROM 
	
typedef enum {
	CD_TRAYEMPTY,
	CD_STOPPED,
	CD_PLAYING,
	CD_PAUSED,
	CD_ERROR = -1
} CDstatus;

// zwraca true jezeli w napedzie jest plyta

#define CD_INDRIVE(status)	((int)status > 0)

typedef struct {
	Uint8 id;			// numer sciezki
	Uint8 type;			// data czy audio?
	Uint16 unused;
	Uint32 length;		// dlugosc we frames tego utworu
	Uint32 offset;		// offset od poczatku plyty
} FOX_CDtrack;

// tylko dla FOX_CDStatus()

typedef struct FOX_CD {
	int id;				// identyfikator sterownika
	CDstatus status;	// status napedu

	// reszta aktualna tylko gdy plyta jest w napedzie
	
	int numtracks;		// ilosc tracks na plycie
	int cur_track;		// aktualna pozycja w sciezce
	int cur_frame;		// aktualny offset klatki w track
	
	FOX_CDtrack track[FOX_MAX_TRACKS+1];

} FOX_CD;

// funkcje i makra do konwersji z klatek na minuty, sekundy i vice versa

#define CD_FPS	75

#define FRAMES_TO_MSF(f, M,S,F)	{			\
	int value = f;							\
	*(F) = value%CD_FPS;					\
	value /= CD_FPS;						\
	*(S) = value%60;						\
	value /= 60;							\
	*(M) = value;							\
}

#define MSF_TO_FRAMES(M, S, F)	((M)*60*CD_FPS+(S)*CD_FPS+(F))

// CD audio funkcje API 

// ile napedow CD jest w systemie ? 
// jezeli zwroci -1 tzn ze FOX_Init nie ma flagi FOX_INIT_CDROM

extern DECLSPEC int FOX_CDNumDrives(void);

// zwraca lancuch okreslajacy naped np. "E:"

extern DECLSPEC const char * FOX_CDName(int drive);

// otwiera urzadzenie cdrom, zwraca handle jezeli sie udalo, 0 jezeli naped
// jest nieprawidlowy albo zajety

extern DECLSPEC FOX_CD * FOX_CDOpen(int drive);

// funkcja zwraca status napedu, jezeli cos jest w srodku to zostaje wypelniona
// struktura FOX_CD

extern DECLSPEC CDstatus FOX_CDStatus(FOX_CD *cdrom);

// odtwarzaj CD, 0 ok, -1 jezeli blad
// caly cd ROM
//	if ( CD_INDRIVE(FOX_CDStatus(cdrom)) )
//		FOX_CDPlayTracks(cdrom, 0, 0, 0, 0);
// ostatni track:
//	if ( CD_INDRIVE(FOX_CDStatus(cdrom)) ) {
//		FOX_CDPlayTracks(cdrom, cdrom->numtracks-1, 0, 0, 0);
//	}
//	pierwszy track i drugi, 10 sekund trzeciego
//	if ( CD_INDRIVE(FOX_CDStatus(cdrom)) )
//		FOX_CDPlayTracks(cdrom, 0, 0, 2, 10);

extern DECLSPEC int FOX_CDPlayTracks(FOX_CD *cdrom,
		int start_track, int start_frame, int ntracks, int nframes);

// odtwarzaj cd zaczynajac od start i length klatek
// 0 = ok, -1 = error

extern DECLSPEC int FOX_CDPlay(FOX_CD *cdrom, int start, int length);

// pause play, 0 = ok, -1 = error

extern DECLSPEC int FOX_CDPause(FOX_CD *cdrom);

// ponow po pauzie odtwarzania, 0 = ok, -1 = error

extern DECLSPEC int FOX_CDResume(FOX_CD *cdrom);

// zatrzymaj odtwarzanie, 0 = ok, -1 = error

extern DECLSPEC int FOX_CDStop(FOX_CD *cdrom);

// wysun cd, 0 = ok, -1 = error 

extern DECLSPEC int FOX_CDEject(FOX_CD *cdrom);

// zamyka uchwyt do cd

extern DECLSPEC void FOX_CDClose(FOX_CD *cdrom);

// koniec deklaracji funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end

