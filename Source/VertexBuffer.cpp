// VertexBuffer.cpp: implementation of the CVertexBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "VertexBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// trzeba podac device jako parametr od glownego d3d

CVertexBuffer::CVertexBuffer(IDirect3DDevice8 *pDevice, int fp_iVertexCount, DWORD fp_dwFVF, DWORD fp_dwPrimitiveType, int fp_iVertexSize)
{
	iVertexCount=fp_iVertexCount;
	dwFVF=fp_dwFVF;
	iVertexSize=fp_iVertexSize;
	dwPrimitiveType=fp_dwPrimitiveType;

	pDevice->CreateVertexBuffer(iVertexSize * iVertexCount, NULL, dwFVF, D3DPOOL_MANAGED, &pVertexBuffer);

}

// destruktor, zwalnia bufor

CVertexBuffer::~CVertexBuffer()
{
	if (pVertexBuffer != NULL)
	{
		pVertexBuffer->Release();
		pVertexBuffer=NULL;
	}
}

// pomocnicza

void CVertexBuffer::Create(IDirect3DDevice8 *pDevice, int fp_iVertexCount, DWORD fp_dwFVF, DWORD fp_dwPrimitiveType, int fp_iVertexSize)
{
	iVertexCount=fp_iVertexCount;
	dwFVF=fp_dwFVF;
	iVertexSize=fp_iVertexSize;
	dwPrimitiveType=fp_dwPrimitiveType;

	pDevice->CreateVertexBuffer(iVertexSize * iVertexCount, NULL, dwFVF, D3DPOOL_MANAGED, &pVertexBuffer);

}


// wypelnij 

void CVertexBuffer::FillProcessedBuffer(IDirect3DDevice8 *pDevice, CVertexBuffer *fp_pBuffer, int fp_iCount, int fp_iStartSrc, int fp_iStartDest)
{
	HRESULT hr;

	hr=pDevice->SetStreamSource(0, pVertexBuffer, iVertexSize);
	hr=pDevice->SetVertexShader(dwFVF);
	hr=pDevice->ProcessVertices(fp_iStartSrc, fp_iStartDest, fp_iCount, fp_pBuffer->pVertexBuffer, NULL);
}

// zwraca wskaznik do wierzcholka

void *CVertexBuffer::GetMemoryPointer(int fp_iStart, int fp_iCount)
{
	void *pReturn;

	pVertexBuffer->Lock(fp_iStart * iVertexSize, fp_iCount * iVertexSize, (BYTE **)&pReturn, NULL); 
	
	return pReturn;
}

// zwolnij wskaznik

void CVertexBuffer::ReleaseMemoryPointer()
{
	pVertexBuffer->Unlock();
}

// spawn

void CVertexBuffer::SpawnProcessedBuffer(IDirect3DDevice8 *pDevice, CVertexBuffer *fp_pBuffer)
{
	DWORD	dwProcessedFVF=D3DFVF_XYZRHW;
	BOOL	bLighting;
	BOOL	bSpecular;
	DWORD	dwFogMode;
	int		iSize=16; // 4x float * 4 bajty


	pDevice->GetRenderState(D3DRS_LIGHTING, (DWORD *)&bLighting);
	pDevice->GetRenderState(D3DRS_SPECULARENABLE, (DWORD *)&bSpecular);
	pDevice->GetRenderState(D3DRS_FOGVERTEXMODE, &dwFogMode);

	if (bLighting || (dwFVF & D3DFVF_DIFFUSE))
	{
		dwProcessedFVF |= D3DFVF_DIFFUSE;
		iSize+=4;
	}
	
	if ((bLighting & bSpecular) || (dwProcessedFVF & D3DFVF_SPECULAR) || (dwFogMode!=D3DFOG_NONE))
	{
		dwProcessedFVF |= D3DFVF_SPECULAR;
		iSize+=4;
	}

	if ((dwFVF & D3DFVF_TEX0) == D3DFVF_TEX0 )
	{
		dwProcessedFVF |= D3DFVF_TEX0;
		iSize+=0;
	}
	
	if ((dwFVF & D3DFVF_TEX1) == D3DFVF_TEX1)
	{
		dwProcessedFVF |= D3DFVF_TEX1;
		iSize+=8;
	}
	
	if ((dwFVF & D3DFVF_TEX2)== D3DFVF_TEX2)
	{
		dwProcessedFVF |= D3DFVF_TEX2;
		iSize+=16;
	}
	
	if ((dwFVF & D3DFVF_TEX3)== D3DFVF_TEX3)
	{
		dwProcessedFVF |= D3DFVF_TEX3;
		iSize+=24;
	}	
	
	if ((dwFVF & D3DFVF_TEX4)== D3DFVF_TEX4)
	{
		dwProcessedFVF |= D3DFVF_TEX4;
		iSize+=32;
	}
	
	if ((dwFVF & D3DFVF_TEX5)== D3DFVF_TEX5)
	{
		dwProcessedFVF |= D3DFVF_TEX5;
		iSize+=40;
	}	
	
	if ((dwFVF & D3DFVF_TEX6)== D3DFVF_TEX6)
	{
		dwProcessedFVF |= D3DFVF_TEX6;
		iSize+=48;
	}
	
	if ((dwFVF & D3DFVF_TEX7)== D3DFVF_TEX7)
	{
		dwProcessedFVF |= D3DFVF_TEX7;
		iSize+=56;
	}	
	
	if ((dwFVF & D3DFVF_TEX8)== D3DFVF_TEX8)
	{
		dwProcessedFVF |= D3DFVF_TEX8;
		iSize+=64;
	}

//	fp_pBuffer->Create(fp_pDirect3D, iVertexCount, dwProcessedFVF, dwPrimitiveType, iSize);
	fp_pBuffer->Create(pDevice, iVertexCount, dwProcessedFVF, dwPrimitiveType, iSize);
}

// rysuj 

void CVertexBuffer::Render(IDirect3DDevice8 *pDevice, int fp_iPrimCount, int fp_iStart)
{
	HRESULT hr;
	
	hr=pDevice->SetStreamSource(0, pVertexBuffer, iVertexSize);
	hr=pDevice->SetVertexShader(dwFVF);
	hr=pDevice->DrawPrimitive((D3DPRIMITIVETYPE)dwPrimitiveType, fp_iStart, fp_iPrimCount);
}