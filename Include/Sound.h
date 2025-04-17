// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__CAB7203C_3599_4F56_9765_98C3E864F8EC__INCLUDED_)
#define AFX_SOUND_H__CAB7203C_3599_4F56_9765_98C3E864F8EC__INCLUDED_

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

#include "Audio.h"

class __declspec(dllexport) CSound  
{

public:
	CSound(char pCurrentDirectory[200], CAudio *currentAudio, WCHAR wstrFilename[MAX_PATH]);

	virtual ~CSound();
	
	virtual void Play(CAudio *currentAudio,float x_sound, float y_sound, float z_sound);

private:

	IDirectMusicSegment8     *g_pMusicSegmentData;
	
	IDirectSound3DBuffer8	 *pDirectSoundBuffer;
	IDirectMusicAudioPath8	 *pDirectAudioPath;

	IDirectMusicPerformance8 *pMusic_Performance;  


};

#endif // !defined(AFX_SOUND_H__CAB7203C_3599_4F56_9765_98C3E864F8EC__INCLUDED_)
