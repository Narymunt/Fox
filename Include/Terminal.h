// Terminal.h: interface for the CTerminal class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXTERMINAL_
#define _FOXTERMINAL_

// koncowka - obiekt podlegly pod CTree
// TODO:

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

#endif 
