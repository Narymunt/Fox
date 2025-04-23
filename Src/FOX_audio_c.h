// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// funkcje i zmienne wyeksportowane z FOX_audio.c tylko dla FOX_sysaudio.c

// funkcje aby pobrac liste close 

extern Uint16 FOX_FirstAudioFormat(Uint16 format);
extern Uint16 FOX_NextAudioFormat(void);

// policz rozmiar dla FOX_AudioSpec

extern void FOX_CalculateAudioSpec(FOX_AudioSpec *spec);

// aktualnie mixujacy watek 

extern int FOX_RunAudio(void *audiop);


// end
