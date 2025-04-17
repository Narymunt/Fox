// FileSystem.h: interface for the CFileSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESYSTEM_H__3CE9AB5D_F52B_4407_99AA_946E0E3831F1__INCLUDED_)
#define AFX_FILESYSTEM_H__3CE9AB5D_F52B_4407_99AA_946E0E3831F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

//=== metody kompresji

#define		compr_none		0
#define		compr_lzss		1
#define		compr_lzari		2

//=== parametry kompresji

#define N			4096
#define F			18
#define THRESHOLD   2
#define F2			60
#define M			15
#define Q1			(1UL << M)
#define Q2			(2 * Q1)
#define Q3			(3 * Q1)
#define Q4			(4 * Q1)
#define MAX_CUM		(Q1 - 1)
#define N_CHAR		(256 - THRESHOLD + F2)

class __declspec(dllexport) CFileSystem  
{

public:

	CFileSystem(char name[]);	// wybieramy plik spakowany
	virtual ~CFileSystem();

	virtual int		putc_buffer(int outc);	// wstaw do bufora
	virtual int		getc_lz(void);			// pobierz znak

	virtual void	lzss_decode(void);		// szybkie dekodowanie
	
	// dekompresja pliku
	
	virtual void	lzss_decompress(FILE* fptr1, unsigned char* tbuffer, long comp_size);

	virtual void	PutBit(int bit);		// ustawianie bitu
	virtual void	FlushBitBuffer(void);	// odswiez bufor
	virtual int		GetBit(void);			// pobierz bit 

	virtual void	StartModel(void);		// poczatek budowy modelu
	virtual void	UpdateModel(int sym);	// aktualizuj model

	virtual void	Output(int bit);		// sekwencja bitow na wyjscie

	virtual void	EncodePosition(int position);	// pozycja
	virtual void	EncodeEnd(void);				// tutaj koniec

	virtual int		BinarySearchSym(unsigned int x);	// szukaj
	virtual int		BinarySearchPos(unsigned int x);	// szukaj pozycji
	
	virtual void	StartDecode(void);		// zacznij dekodowanie
	virtual int		DecodeChar(void);		// dekoduj znak
	virtual int		DecodePosition(void);	// dekoduj pozycje

	virtual void	lzari_Decode(void);		// dekoduj arytmetycznie
	virtual void	lzari_decompress(FILE* fptr1, unsigned char* tbuffer, long comp_size);

	virtual void	Select(char name[]);	// plik z plikami
	virtual long	Search(char name[]);	// szukaj pliku, zwraca offset
	virtual void	LoadIn(unsigned char* tbuffer);	// wczytaj
	virtual long	Load(char filename[]);	// wczytaj do bufora

	unsigned char	*pDataBuffer; // rozkompresowane dane

private:

	unsigned long	m_low;	
	unsigned long	m_high;
	unsigned long	m_value;

	int				m_char_to_sym[N_CHAR];
	int				m_sym_to_char[N_CHAR + 1];
	int				m_shifts;	

	unsigned int	m_sym_freq[N_CHAR + 1];
	unsigned int	m_sym_cum[N_CHAR + 1];
	unsigned int	m_position_cum[N + 1];

	unsigned char	m_text_buf [N + F2 - 1];

	unsigned char	*p_from_buffer;
	unsigned char	*p_to_buffer;

	unsigned long	m_from_pointer;
	unsigned long	m_from_max;
	unsigned long	m_to_pointer;
	unsigned long	m_textsize;
	unsigned long	m_codesize;

	unsigned int	m_PB_buffer;
	unsigned int	m_PB_mask;
	
	unsigned int	m_GB_buffer;
	unsigned int	m_GB_mask;

	FILE			*p_lz_fptr;		// wskaznik na nasz plik

	char			*pArchiveName;	// nazwa spakowanego pliku z zasobami

	// identyfikator
	
	struct filesys_header
	{
		char	header[8];			// identyfikator np "filesys"
		short	version;			// wersja
		long	number_files;		// liczba plikow w archiwum
	} filesys_header;

	// struktura z danymi opisujacymi spakowany plik

	struct filesys_indexblock
	{
		char name[13];				// 12 bajtow - nazwa pliku spakowanego
		long lokation;				// +4= 16offset of filedata from end of .exe file
		long length;				// +4= 20filesize (could be compressed size)
		long original_length;		// rozmiar pliku bez kompresji
		short compression;			// typ kompresji
	} filesys_indexblock;		// rozmiar : 18 bajtow

	

};

#endif // !defined(AFX_FILESYSTEM_H__3CE9AB5D_F52B_4407_99AA_946E0E3831F1__INCLUDED_)
