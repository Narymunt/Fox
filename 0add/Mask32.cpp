// Mask32.cpp: implementation of the CMask32 class.
//
//////////////////////////////////////////////////////////////////////

#include "Mask32.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMask32::CMask32(long lXSize, long lYSize)
{
	m_pBuffer = NULL;
	m_pBuffer = (unsigned char*) malloc (lXSize*lYSize*sizeof(unsigned char));

	m_pYTable = NULL;
	m_pYTable = new CYTable(lXSize, lYSize);

}

CMask32::~CMask32()
{
	if (m_pBuffer!=NULL)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	if (m_pYTable!=NULL)
	{
		delete m_pYTable;
		m_pYTable = NULL;
	}
}

// pinch 2d z 320x200 na 640x480 z kadrem

/*
for (int h1=0;h1<400;h1++)
		{
			for (h2=0;h2<640;h2++)
			{
			 	
				offset = (++xtable->mBitmapMaskData[ 320*(h1/2)+ (h2/2) ]<<8)+
						  ytable->mBitmapMaskData[ 320*(h1/2)+ (h2/2) ]++;

				b=texture->mBitmapDataB[offset];
				g=texture->mBitmapDataG[offset];
				r=texture->mBitmapDataR[offset];

			((long*)screen->pixels)[640*h1+h2+25600] = 
				(r<<screen->format->Rshift)|
				(g<<screen->format->Gshift)|
				(b<<screen->format->Bshift);

			}
		}	
*/