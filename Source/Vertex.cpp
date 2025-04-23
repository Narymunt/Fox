// Vertex.cpp: implementation of the CVertex class.
//
//////////////////////////////////////////////////////////////////////

#include "Vertex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// konstruktor inicjalizuje wierzcholek

CVertex::CVertex(float fp_fX,float fp_fY,float fp_fZ, float fp_fR, float fp_fG, float fp_fB, float fp_fU, float fp_fV)
{
	fX=fp_fX;
	fY=fp_fY;
	fZ=fp_fZ;

	dwColor=D3DCOLOR_XRGB((BYTE)(fp_fR*255), (BYTE)(fp_fG*255), (BYTE)(fp_fB*255));

	fU=fp_fU;
	fV=fp_fV;
}

// destruktor, nic nie robi

CVertex::~CVertex()
{
	

}
