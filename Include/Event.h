// Event.h: interface for the CEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENT_H__5058FACD_CA6F_42E0_99E6_FD5BBDAEF7C9__INCLUDED_)
#define AFX_EVENT_H__5058FACD_CA6F_42E0_99E6_FD5BBDAEF7C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "FileSystem.h"

class CEvent  
{
public:
	
	// wczytaj ze spakowanego pliku serie animacji

	CEvent();
	virtual ~CEvent();

	virtual unsigned char	*pGetNazwaZdarzenia(void);	// zwroc wskaznik

	virtual void			SetLiczbaKlatek(unsigned int m_uiIle);	// ustaw liczbe klatek
	virtual unsigned int	GetLiczbaKlatek(void);	// zwroc liczbe klatek w zdarzeniu

	virtual void			AllocPositions(void);	// zaalokuj pamiec na wspolrzedne

	virtual int				GetPositionX(unsigned long ulIndex); // pobierz wspolrzedne
	virtual void			SetPositionX(unsigned long ulIndex, int iValue);

	virtual int				GetPositionY(unsigned long ulIndex); // pobierz
	virtual void			SetPositionY(unsigned long ulIndex, int iValue);

	virtual void			AllocKlatki(void);	// zaaloku pamiec na liste klatek

	virtual unsigned int	GetNrKlatki(unsigned long ulIndex);	// pobierz numer klatki z listy
	virtual void			SetNrKlatki(unsigned long ulIndex, unsigned int uiNrKlatki);

	// czy odtwarzac wave dla danej klatki

	virtual void			AllocWaveInfo(void);
		
	virtual unsigned char	GetPlayWaveInfo(unsigned long ulIndex);
	virtual void			SetPlayWaveInfo(unsigned long ulIndex, unsigned char ucValue);

	// nazwy bankow i zdarzen 

	virtual void			AllocSFXBankNames();	// zaalokuj pamiec 

	virtual unsigned char	*pGetFileNazwaBanku(void);		// zwroc wskaznik
	virtual unsigned char	*pGetFileNazwaZdarzenia(void);	// zwroc wskaznik

	// czy odtwarzany jest dzwiek ? 

	virtual void			SetIsPlayingSound(bool bState);
	virtual bool			GetIsPlayingSound(void);


	// petla #1 - zdarzenia

private:
	
	unsigned char	*p_ucNazwaZdarzenia;		// 256 bajtow

	unsigned int	m_uiLiczbaKlatek;			// ile klatek w tym zdarzeniu

	// petla #2 - klatki 

	int				*m_iPositionX;				// przeplatane CALA PETLA #2
	int				*m_iPositionY;

	unsigned int	*m_uiNrKlatki;				// numer klatki z EVT

	unsigned char	*m_bPlayWave;				// czy odtwarzac dla tej klatki wave

	// w pliku jest zapisane zawsze 

	unsigned char	*p_ucFileNazwaBanku;		// jezeli nie to puste, file->skip(512)
	unsigned char	*p_ucFileNazwaZdarzenia;	// tak jak wyzej

	// petla #2 - koniec 

	// petla #1 - nastepne zdarzenie

	bool			m_bIsPlayingSound;			// czy odtwarza juz dzwiek  

};

#endif // !defined(AFX_EVENT_H__5058FACD_CA6F_42E0_99E6_FD5BBDAEF7C9__INCLUDED_)
