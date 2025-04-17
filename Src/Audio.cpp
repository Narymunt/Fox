// Audio.cpp: implementation of the CAudio class.
//
//////////////////////////////////////////////////////////////////////

#include <Audio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor zeruje wskazniki

CAudio::CAudio()
{
	g_pMusicLoader = NULL;
	g_pMusicSegment_Music = NULL;	// sciezka dzwiekowa	
	g_pPerformance_Audio = NULL;

}

//=== destruktor - zwalnia pamiec i zeruje wskazniki

CAudio::~CAudio()
{
	
	if( g_pMusicLoader != NULL ) 
	{
		g_pMusicLoader->Release(); 
		g_pMusicLoader = NULL;
	}
    	
	if( g_pPerformance_Audio != NULL ) 
	{
		g_pPerformance_Audio->CloseDown();
	    g_pPerformance_Audio->Release();
		g_pPerformance_Audio = NULL;
	}
	   
    if( g_pMusicSegment_Music != NULL )
	{
		g_pMusicSegment_Music->Release();
		g_pMusicSegment_Music = NULL;
	}
}

//=== inicjalizacja 

HRESULT CAudio::Init(char pCurrentDirectory[200])
{
	char bufor[300];	// do trzymania aktualnego katalogu

	WCHAR wstrMediaPath[MAX_PATH];

    CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicLoader8, (void**)&g_pMusicLoader );
    
    CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicPerformance8, (void**)&g_pPerformance_Audio );

    g_pPerformance_Audio->InitAudio( NULL, NULL, NULL, 
                                     DMUS_APATH_DYNAMIC_STEREO, 64,
                                     DMUS_AUDIOF_ALL, NULL ); 

	wsprintf(bufor, "%s\\Resource\\Music\\", pCurrentDirectory);
	MultiByteToWideChar(CP_ACP, 0, bufor, -1, wstrMediaPath, MAX_PATH) ;
	g_pMusicLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, wstrMediaPath, FALSE );  

	return S_OK;

}

//=== odtwarzaj WAVE

HRESULT CAudio::PlayMusic(WCHAR wstrFileName[MAX_PATH])
{
	// odtwarzaj muzyke

	g_pMusicLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                        IID_IDirectMusicSegment8,
                                        wstrFileName,
                                        (LPVOID*) &g_pMusicSegment_Music );
	
	g_pMusicSegment_Music->Download( g_pPerformance_Audio );
	g_pMusicSegment_Music->SetRepeats( DMUS_SEG_REPEAT_INFINITE );	// powtarzanie
  	g_pPerformance_Audio->PlaySegmentEx( g_pMusicSegment_Music, 0, NULL, DMUS_SEGF_DEFAULT, 0, 0, NULL, NULL );

	return S_OK;

}


