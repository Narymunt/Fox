// Terminal.h: interface for the CTerminal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMINAL_H__3AB994D9_4EF2_4874_B8D0_2F99DB7F16A2__INCLUDED_)
#define AFX_TERMINAL_H__3AB994D9_4EF2_4874_B8D0_2F99DB7F16A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// koncowka - obiekt podlegly pod CTree

class CTerminal  
{

public:
	CTerminal();
	virtual ~CTerminal();
	
	virtual void			SetVersion(unsigned char ucValue);	// ustaw wersje pliku
	virtual unsigned char	GetVersion(void);

	virtual void			SetStarting(unsigned char ucValue);
	virtual unsigned char	GetStarting(void);

	virtual void			SetEnding(unsigned char ucValue);
	virtual unsigned char	GetEnding(void);

	virtual void			AllocNazwaKoncowki(void);	// alokuj pamiec 
	virtual char			*pGetNazwaKoncowki(void);	// zwroc wskaznik

	virtual void			AllocNazwaZdarzeniaZAN(void);	// alokuj pamiec
	virtual char			*pGetNazwaZdarzeniaZAN(void); // zwroc wskaznik

	virtual void			AllocNazwaBankuSFX(void);	// alokuj pamiec
	virtual	char			*pGetNazwaBankuSFX(void);

	virtual void			AllocNazwaZdarzeniaSFX(void);	// alokuj
	virtual char			*pGetNazwaZdarzeniaSFX(void);

private:

	unsigned char		m_ucVersion;		// wersja pliku

	char				*p_cNazwaKoncowki;		// 256 znakow
	
	char				*p_cNazwaZdarzeniaZAN;	// 256 znakow
	
	char				*p_cNazwaBankuSFX;		// 256 znakow
	char				*p_cNazwaZdarzeniaSFX;	// 256 znakow

	unsigned char		m_ucStarting;			// czy ma byc starting
	unsigned char		m_ucEnding;				// czy ma byc ending

};

#endif // !defined(AFX_TERMINAL_H__3AB994D9_4EF2_4874_B8D0_2F99DB7F16A2__INCLUDED_)
