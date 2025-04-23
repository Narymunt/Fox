// Vertex.h: interface for the CVertex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXPROCESSEDVERTEX_
#define _FOXPROCESSEDVERTEX_

#include <windows.h>
#include <d3d8.h>


// obsluga wierzcholka i wierzcholka po transformacjach
// ### na koncu sa definicje ###

class  __declspec(dllexport) CVertex  
{
public:
	
	// stworz wierzcholki

	CVertex(float fp_fX,float fp_fY,float fp_fZ, float fp_fR, float fp_fG, float fp_fB, float fp_fU, float fp_fV);
	virtual ~CVertex();


private:
	
	float fX, fY, fZ;		// wspolrzedne wierzcholka

	DWORD dwColor;			// kolor wierzcholka
	
	float fU, fV;			// wspolrzedne na texturze

};

// wierzcholek po transformacji

class __declspec(dllexport) CProcessedVertex
{
public:
	
	float fX, fY, fZ, fRHW;	// wspolrzedne po transformacjach

	DWORD dwColor;	// diffuse color
	
	float fU, fV;	// wspolrzedne na texturze
};


// wierzcholek FVF

const DWORD D3DFVF_CVertex=D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE;

// wierzcholek po transformacji

const DWORD D3DFVF_CProcessedVertex=D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE;

// nasz powszechny "(c) jaytee" wierzcholek

#define D3D8T_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

#endif 
