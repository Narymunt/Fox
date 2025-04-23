// track zawiera ustawienia czasu
// offset start end offset

#ifndef fox_track
#define fox_track

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

class CTrack 
{
public:

	CTrack(char cFilename[]);
	~CTrack();

	void		UpOnly(char str[]);		// zamiana wszystkich literek lancucha na duze

	int			iGetSize(void);			// zwraca liczbe elementow
	int			iGetStartMax(void);		// pobierz maksymalny mozliwy czas rozpoczecia odtwarzania

	int			iGetOffsetStart(int i);	// zwraca wartosci dla elementow
	int			iGetStart(int i);
	int			iGetEnd(int i);
	int			iGetOffsetEnd(int i);

	bool		isPlaying(int i);		// czy powinnismy teraz grac w tej klatce
	int			iGetFrame(int i);		// pobierz wlasciwa klatke dla danego czasu

private:

	vector <int>	m_iOffsetStart;	// offsety poczatkowe
	vector <int>	m_iStart;		// czas startowy
	vector <int>	m_iEnd;			// czas koncowy
	vector <int>	m_iOffsetEnd;	// offset koncowy
};

#endif 
