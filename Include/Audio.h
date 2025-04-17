// Audio.h: interface for the CAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIO_H__06420044_8727_455F_ACC8_6888B4CD77D4__INCLUDED_)
#define AFX_AUDIO_H__06420044_8727_455F_ACC8_6888B4CD77D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <dshow.h>
#include <dmusicc.h>
#include <dmusici.h>

// ta klasa sluzy tylko i wylacznie do inicjalizacji karty dzwiekowej
// i odtwarzania muzyki. dla zwyklych sampli jest csound

class __declspec(dllexport) CAudio  
{

public:

	CAudio();
	virtual ~CAudio();

	virtual HRESULT		Init(char pCurrentDirectory[200]);			// inicjalizacja 
	virtual HRESULT		PlayMusic(WCHAR wstrFileName[MAX_PATH]);	// odtwarzaj WAV

	IDirectMusicLoader8		 *g_pMusicLoader;	
	IDirectMusicPerformance8 *g_pPerformance_Audio;

private:

	IDirectMusicSegment8     *g_pMusicSegment_Music;	



};

#endif // !defined(AFX_AUDIO_H__06420044_8727_455F_ACC8_6888B4CD77D4__INCLUDED_)
