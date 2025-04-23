// VertexBuffer.h: interface for the CVertexBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXVERTEXBUFFER_
#define _FOXVERTEXBUFFER_

#include <windows.h>
#include <d3d8.h>


// bufor na wierzcholki

class __declspec(dllexport) CVertexBuffer  
{
public:
	CVertexBuffer(IDirect3DDevice8 *pDevice, int fp_iVertexCount, DWORD fp_dwFVF, DWORD fp_dwPrimitiveType, int fp_iVertexSize);
	
	virtual ~CVertexBuffer();

	virtual void Create(IDirect3DDevice8 *pDevice, int fp_iVertexCount, DWORD fp_dwFVF, DWORD fp_dwPrimitiveType, int fp_iVertexSize);

	virtual void *GetMemoryPointer(int fp_iStart=0, int fp_iCount=0);
	virtual void ReleaseMemoryPointer();
	virtual void SpawnProcessedBuffer(IDirect3DDevice8 *pDevice, CVertexBuffer *fp_pBuffer);
	virtual void FillProcessedBuffer(IDirect3DDevice8 *pDevice, CVertexBuffer *fp_pBuffer, int fp_iCount=0, int fp_iStartSrc=0, int fp_iStartDest=0);
	virtual void Render(IDirect3DDevice8 *pDevice, int fp_iPrimCount, int fp_iStart=0);

private:

	IDirect3DVertexBuffer8 *pVertexBuffer;
	
	int iVertexCount;		// ile wierzcholkow
	int iVertexSize;		// ile zajmuje
	
	DWORD dwFVF;
	DWORD dwPrimitiveType;


};

#endif // !defined(AFX_VERTEXBUFFER_H__77F86B20_C78D_4514_BDE7_34E34D2A94EA__INCLUDED_)
