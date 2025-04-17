// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include <Sound.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== inicjalizacja obiektu 

CSound::CSound(char pCurrentDirectory[200],CAudio *currentAudio, WCHAR wstrFilename[MAX_PATH])
{
	char	bufor[300];
	WCHAR	wstrMediaPath[MAX_PATH];	// tutaj sciezka pliku
	
	g_pMusicSegmentData = NULL;

	pDirectSoundBuffer  = NULL;
	pDirectAudioPath    = NULL;

	wsprintf(bufor, "%s\\Resource\\Sound\\", pCurrentDirectory);
	MultiByteToWideChar(CP_ACP, 0, bufor, -1, wstrMediaPath, MAX_PATH) ;
	
	currentAudio->g_pMusicLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, wstrMediaPath, FALSE );  

	currentAudio->g_pMusicLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
	 					  	            IID_IDirectMusicSegment8, wstrFilename,
								        (LPVOID*) &g_pMusicSegmentData );
	
	g_pMusicSegmentData->Download( currentAudio->g_pPerformance_Audio );

	currentAudio->g_pPerformance_Audio->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 
                                                 1, TRUE, &pDirectAudioPath );


	pDirectAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, 
                                       GUID_NULL, 0, IID_IDirectSound3DBuffer, 
                                       (LPVOID*) &pDirectSoundBuffer );

}

//=== deinicjalizacja obiektu 

CSound::~CSound()
{

	if( g_pMusicSegmentData != NULL ) 
	{
		g_pMusicSegmentData->Release(); 
		g_pMusicSegmentData = NULL;
	}

	if( pDirectSoundBuffer != NULL ) 
	{
		pDirectSoundBuffer->Release();
		pDirectSoundBuffer = NULL;
	}

	if( pDirectAudioPath != NULL ) 
	{
		pDirectAudioPath->Release();
		pDirectAudioPath = NULL;
	}

}

// odtwarza dzwiek w podanych wspolrzednych
// wspolrzedne w skali -0.5:0.5

void CSound::Play(CAudio *currentAudio,float x_sound, float y_sound, float z_sound)
{

	pDirectSoundBuffer->SetPosition( x_sound, y_sound, z_sound, DS3D_IMMEDIATE );

	currentAudio->g_pPerformance_Audio->PlaySegmentEx( g_pMusicSegmentData, NULL, NULL, DMUS_SEGF_SECONDARY, 
								    0,  NULL, NULL, pDirectAudioPath );

}

