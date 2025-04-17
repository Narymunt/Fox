// do przeliczania wartosci np. 
// 1x800 2x800 3x800 itd. 

#ifndef fox_ytable
#define fox_ytable

class CYtable  
{
						
public:

	CYtable();
	CYtable(unsigned long ulSize, unsigned long ulResolution);	// size * resolution

	virtual ~CYtable();

private: 

	unsigned long	*m_ulLine;

};

#endif 
