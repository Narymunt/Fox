// FileSystem.cpp: implementation of the CFileSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <FileSystem.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== tworzymy obiekt i od razu ustawiamy sciezki

CFileSystem::CFileSystem(char name[])
{
	// wyzeruj wskazniki
	
	pDataBuffer=NULL;
	pArchiveName=NULL;
	
	// wybierz
		
	Select(name);
}

//=== usuwamy obiekt

CFileSystem::~CFileSystem()
{

	if (pDataBuffer!=NULL) 
	{
		free(pDataBuffer);
		pDataBuffer = NULL;
	}

	if (pArchiveName != NULL) 
	{
		free(pArchiveName);
		pArchiveName = NULL;
	}

}

//=== wstaw do bufora

int CFileSystem::putc_buffer(int outc)
{
  p_to_buffer[m_to_pointer++] = (unsigned char) outc;
  return(1);
}

//=== pobierz znak

int CFileSystem::getc_lz(void)
{
  
  if (m_from_pointer++ == m_from_max)	
  {
	  return(EOF);	// wskaznik rosnie
  }

  return(getc(p_lz_fptr));
}

//=== dekodowanie chunk lzss

void CFileSystem::lzss_decode(void)
{
	int i, j, k, r, c;
	unsigned int flags;

	for (i = 0; i < N - F; i++) 
		m_text_buf[i] = ' ';
	
	r = N - F;  
	flags = 0;
	
	for ( ; ; ) 
	{
		
		if (((flags >>= 1) & 256) == 0) 
		{
			if ((c = getc_lz()) == EOF) break;
			flags = c | 0xff00;
		}
		
		if (flags & 1) 
		{
			if ((c = getc_lz()) == EOF) break;
			putc_buffer(c);  
			m_text_buf[r++] = c;  
			r &= (N - 1);
		} 
		
		else 
		
		{
			if ((i = getc_lz()) == EOF) break;
			if ((j = getc_lz()) == EOF) break;
			
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
		
			for (k = 0; k <= j; k++) 
			{
				c = m_text_buf[(i + k) & (N - 1)];
				putc_buffer(c);  
				m_text_buf[r++] = c;  
				r &= (N - 1);
			}
		}
	
	}	// for

}

//=== dekompresja pliku

void CFileSystem::lzss_decompress(FILE* fptr1, unsigned char* tbuffer, long comp_size)
{
  p_lz_fptr = fptr1;
  m_from_pointer = 0;		// poczatek
  m_from_max = comp_size;
  m_to_pointer = 0;
  p_to_buffer = tbuffer;
  lzss_decode();
}

//=== ustawianie bitu

void CFileSystem::PutBit(int bit)
{
	if (bit) m_PB_buffer |= m_PB_mask;

	if ((m_PB_mask >>= 1) == 0) 
	{
		m_PB_buffer = 0;  
		m_PB_mask = 128;  
		m_codesize++;
	}
}

//=== odswiez bufor z zapisem bitowym

void CFileSystem::FlushBitBuffer(void)
{
	int  i;
	for (i = 0; i < 7; i++) PutBit(0);
}

//=== pobierz bit z bufora

int CFileSystem::GetBit(void)
{
	if ((m_GB_mask >>= 1) == 0) 
	{
		m_GB_buffer = getc_lz(); 
		m_GB_mask = 128;
	}
	return ((m_GB_buffer & m_GB_mask) != 0);
}

//=== zacznij budowac model

void CFileSystem::StartModel(void)
{
	int ch, sym, i;

	m_sym_cum[N_CHAR] = 0;
	
	for (sym = N_CHAR; sym >= 1; sym--) 
	{
		ch = sym - 1;
		
		m_char_to_sym[ch] = sym;  
		m_sym_to_char[sym] = ch;
		
		m_sym_freq[sym] = 1;
		m_sym_cum[sym - 1] = m_sym_cum[sym] + m_sym_freq[sym];
	}
	
	m_sym_freq[0] = 0;
	m_position_cum[N] = 0;
	
	for (i = N; i >= 1; i--)
		m_position_cum[i - 1] = m_position_cum[i] + 10000 / (i + 200);
}

//=== aktualizuj model

void CFileSystem::UpdateModel(int sym)
{
	int i, c, ch_i, ch_sym;

	if (m_sym_cum[0] >= MAX_CUM) 
	{
		c = 0;
		
		for (i = N_CHAR; i > 0; i--) 
		{
			m_sym_cum[i] = c;
			c += (m_sym_freq[i] = (m_sym_freq[i] + 1) >> 1);
		}
		m_sym_cum[0] = c;
	}
	
	for (i = sym; m_sym_freq[i] == m_sym_freq[i - 1]; i--) ;
	
	if (i < sym) 
	{
		ch_i = m_sym_to_char[i];    
		ch_sym = m_sym_to_char[sym];
		
		m_sym_to_char[i] = ch_sym;  
		m_sym_to_char[sym] = ch_i;
		
		m_char_to_sym[ch_i] = sym;  
		m_char_to_sym[ch_sym] = i;
	}
	m_sym_freq[i]++;
	while (--i >= 0) m_sym_cum[i]++;
}

//=== sekwencja bitow na wyjscie

void CFileSystem::Output(int bit)
{
	PutBit(bit);
	for ( ; m_shifts > 0; m_shifts--) PutBit(! bit);
}

//=== pozycja

void CFileSystem::EncodePosition(int position)
{
	unsigned long int  range;

	range = m_high - m_low;
	
	m_high = m_low + (range * m_position_cum[position]) / m_position_cum[0];
	
	m_low +=       (range * m_position_cum[position + 1]) / m_position_cum[0];
	
	for ( ; ; ) 
	{
		if (m_high <= Q2) Output(0);
		else 
			if (m_low >= Q2) 
			{
			Output(1);  
			m_low -= Q2;  
			m_high -= Q2;
			} 
			else 
			if (m_low >= Q1 && m_high <= Q3) 
			{
			m_shifts++;  
			m_low -= Q1;  
			m_high -= Q1;
			} else break;
		m_low += m_low;  
		m_high += m_high;
	}
}

//=== koniec 

void CFileSystem::EncodeEnd(void)
{
	m_shifts++;
	
	if (m_low < Q1) Output(0);  
	else Output(1);
	FlushBitBuffer();
}

//=== szukaj 

int CFileSystem::BinarySearchSym(unsigned int x)
{
	int i, j, k;

	i = 1;  
	j = N_CHAR;
	
	while (i < j) 
	{
		k = (i + j) / 2;
		if (m_sym_cum[k] > x) i = k + 1;  else j = k;
	}
	return i;
}

//=== szukaj pozycji

int CFileSystem::BinarySearchPos(unsigned int x)
{
	int i, j, k;

	i = 1;  j = N;
	while (i < j) 
	{
		k = (i + j) / 2;
		if (m_position_cum[k] > x) i = k + 1;  else j = k;
	}
	return i - 1;
}

//=== zacznij dekodowanie

void CFileSystem::StartDecode(void)
{
	int i;
	
	for (i = 0; i < M + 2; i++)
		m_value = 2 * m_value + GetBit();
}

//=== dekoduj znak

int CFileSystem::DecodeChar(void)
{
	int	 m_sym, ch;
	unsigned long int  range;

	range = m_high - m_low;
	m_sym = BinarySearchSym((unsigned int)
		(((m_value - m_low + 1) * m_sym_cum[0] - 1) / range));
	
	m_high = m_low + (range * m_sym_cum[m_sym - 1]) / m_sym_cum[0];
	m_low += (range * m_sym_cum[m_sym]) / m_sym_cum[0];
	
	for ( ; ; ) 
	{
		if (m_low >= Q2) 
		{
			m_value -= Q2;  
			m_low -= Q2;  
			m_high -= Q2;
		} 
		else 
			if (m_low >= Q1 && m_high <= Q3) 
			{
				m_value -= Q1;  
				m_low -= Q1;  
				m_high -= Q1;
			} 
			else 
			if (m_high > Q2) break;
		
			m_low += m_low;  
			m_high += m_high;
			m_value = 2 * m_value + GetBit();
	}
	ch = m_sym_to_char[m_sym];
	UpdateModel(m_sym);
	return ch;
}

//=== dekoduj pozycje 

int CFileSystem::DecodePosition(void)
{
	int position;
	unsigned long int  range;

	range = m_high - m_low;
	
	position = BinarySearchPos((unsigned int)
		(((m_value - m_low + 1) * m_position_cum[0] - 1) / range));
	
	m_high = m_low + (range * m_position_cum[position]) / m_position_cum[0];
	m_low += (range * m_position_cum[position + 1]) / m_position_cum[0];
	
	for ( ; ; ) 
	{
		if (m_low >= Q2) 
		{
			m_value -= Q2;  
			m_low -= Q2;  
			m_high -= Q2;
		} 
		else 
			if (m_low >= Q1 && m_high <= Q3) 
			{
				m_value -= Q1;  
				m_low -= Q1;  
				m_high -= Q1;
			} 
			else 
			
			if (m_high > Q2) break;
		m_low += m_low;  
		m_high += m_high;
		m_value = 2 * m_value + GetBit();
	}
	return position;
}

//=== dekoduj arytmetycznie

void CFileSystem::lzari_Decode(void)
{
	int  i, j, k, r, c;
	unsigned long int  count, temp;

	m_textsize = 0;
	temp = getc_lz() << 24;
	m_textsize |= temp;
	temp = getc_lz() << 16;
	m_textsize |= temp;
	temp = getc_lz() << 8;
	m_textsize |= temp;
	temp = getc_lz();
	m_textsize |= temp;

	if (m_textsize == 0) return;
	
	StartDecode();  
	StartModel();
	
	for (i = 0; i < N - F2; i++) m_text_buf[i] = ' ';
	r = N - F2;
	
	for (count = 0; count < m_textsize; ) 
	{
		c = DecodeChar();
		
		if (c < 256) 
		{
			putc_buffer(c);  
			m_text_buf[r++] = c;
			r &= (N - 1);  
			count++;
		} 
		else 
		{
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			
			for (k = 0; k < j; k++) 
			{
				c = m_text_buf[(i + k) & (N - 1)];
				putc_buffer(c);  
				m_text_buf[r++] = c;
				r &= (N - 1);  
				count++;
			}
		}
	}
}

//=== dekompresuj 

void CFileSystem::lzari_decompress(FILE* fptr1, unsigned char* tbuffer, long comp_size)
{
  p_lz_fptr = fptr1;
  
  m_textsize = 0;
  m_codesize = 0;
  m_from_pointer = 0;
  m_to_pointer = 0;
  
  m_PB_buffer = 0;
  m_PB_mask = 128;
  m_GB_buffer = 0;
  m_GB_mask = 0;
  
  m_low = 0;
  m_high = Q4;
  m_value = 0;
  m_shifts = 0;
  
  m_from_max = comp_size;
  p_to_buffer = tbuffer;
  lzari_Decode();
  return;
}

//=== wybierz plik z ktorego bedziemy wczytywac

void CFileSystem::Select(char name[])
{

	if (pArchiveName != NULL) 
	{
		free(pArchiveName);
	}

	pArchiveName = (char *) malloc(strlen(name)+1);

	strcpy(pArchiveName, name);	// hmm... to ciekawe, bo ze strcmp sa czasem problemy
}

//=== szukaj pliku w spakowanym

long CFileSystem::Search(char name[])
{
  FILE* fptr;
  long i, seeklong;
//  int j,ok;

  fptr = fopen(pArchiveName, "rb"); // bez sprawdzania czy plik istnieje, wiec moga byc bugi
  seeklong = sizeof(filesys_header);
  fseek(fptr, -seeklong, SEEK_END);
  fread(&filesys_header, sizeof(filesys_header), 1, fptr);
  
  seeklong-=2;
  
  for (i = 0; i < filesys_header.number_files; i++)
	{
	
	seeklong += sizeof(filesys_indexblock)-5;	// pomylka w offsetach ? 
	
	fseek(fptr, -seeklong, SEEK_END);
	
	fread(&filesys_indexblock.name, 13, 1, fptr);
	fread(&filesys_indexblock.lokation,1,sizeof(filesys_indexblock.lokation),fptr);
	fread(&filesys_indexblock.length,1,sizeof(filesys_indexblock.length),fptr);
	fread(&filesys_indexblock.original_length,1,sizeof(filesys_indexblock.original_length),fptr);
	fread(&filesys_indexblock.compression,1,sizeof(filesys_indexblock.compression),fptr);

	if ((int)strcmp(filesys_indexblock.name, name) == 0)
	  break;
	}
  fclose(fptr);
  return(filesys_indexblock.original_length);
}

//=== wczytanie 

void CFileSystem::LoadIn(unsigned char* tbuffer)
{
  FILE* fptr;
  fptr = fopen(pArchiveName, "rb");
  fseek(fptr, -filesys_indexblock.lokation, SEEK_END);

  switch (filesys_indexblock.compression)
	{
	case compr_lzari:
	  lzari_decompress(fptr, tbuffer, filesys_indexblock.length);
	  break;
	case compr_lzss:
	  lzss_decompress(fptr, tbuffer, filesys_indexblock.length);
	  break;
	case compr_none:
	  fread(tbuffer, filesys_indexblock.length, 1, fptr);
	  break;
	}
  fclose(fptr);
  return;
}

// wczytuj, zwracaj 0 jezeli blad jezeli nie, to dlugosc pliku

long CFileSystem::Load(char filename[])
{
	long datalen;	// jak dlugi jest rozkompresowany rekord

	datalen = Search(filename);

	if (datalen==0) return 0; // nie znalazl, albo plik 0 dlugosci, a takich nie ma

	if (pDataBuffer!=NULL)
	{
		free(pDataBuffer);
	}

	pDataBuffer = (unsigned char*) malloc(datalen);

	LoadIn(pDataBuffer);

	return datalen;
}
