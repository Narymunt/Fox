// Fox v0.7
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
//
//================================================================================================
// TODO:
// - notacja wêgierska


// funkcje i zmienne wyeksportowane z FOX_audio.c tylko dla FOX_sysaudio.c

// funkcje aby pobrac liste close 

extern Uint16 FOX_FirstAudioFormat(Uint16 format);
extern Uint16 FOX_NextAudioFormat(void);

// policz rozmiar dla FOX_AudioSpec

extern void FOX_CalculateAudioSpec(FOX_AudioSpec *spec);

// aktualnie mixujacy watek 

extern int FOX_RunAudio(void *audiop);

// end
