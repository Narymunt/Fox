// Audio.h: interface for the CAudio class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXAUDIO_
#define _FOXAUDIO_

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <dshow.h>
#include <dmusicc.h>
#include <dmusici.h>

//=== ta klasa sluzy tylko i wylacznie do inicjalizacji karty dzwiekowej
//=== i odtwarzania muzyki. dla zwyklych sampli jest csound

class __declspec(dllexport) CAudio  
{

public:

	CAudio();
	virtual ~CAudio();

	virtual HRESULT		Init(char pCurrentDirectory[200],char cAddDirectory[]);			// inicjalizacja 
	virtual HRESULT		Init(char pCurrentDirectory[200]);	// inicjalizacja 


	virtual HRESULT		PlayMusic(WCHAR wstrFileName[MAX_PATH]);		// odtwarzaj WAV
	virtual HRESULT		StopMusic(int iDelay);

	IDirectMusicLoader8		 *g_pMusicLoader;			// loader dla directx 8
	IDirectMusicPerformance8 *g_pPerformance_Audio;		// demon odtwarzaj¹cy

private:

	IDirectMusicSegment8     *g_pMusicSegment_Music;	// segment odtwarzanych danych

};

#endif 
