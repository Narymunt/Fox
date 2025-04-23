#if !defined(FOX_SHADER__H)
#define FOX_SHADER__H

#include <windows.h>
#include <d3d8.h>

// to powinno sie znalezc
//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
//CMyD3DApplication::CMyD3DApplication()
//{
//    m_strWindowTitle    = _T("Start");
//    m_dwCreationWidth		= 800;   // Width used to create window
//    m_dwCreationHeight		= 600;
//    m_bUseDepthBuffer   = TRUE;
//    m_pFont        = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
//    m_pVB  = NULL;
//	m_pIB  = NULL;
//	m_dwVertexShader	= 0xffffffff;
//}


// definicje dla vertex shaderow

DWORD VertexShaderdwDecl[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,0  
    D3DVSD_END()
};

//=== prosty shader
//      reg c4-7	= WorldViewProj matrix
//      reg c8      = constant color
//	    reg v0		= input register

const char BasicVertexShader[] =
"vs.1.1             //Shader version 1.1			\n"\
"dp4 oPos.x, v0, c4 //emit projected position	    \n"\
"dp4 oPos.y, v0, c5 //emit projected position	    \n"\
"dp4 oPos.z, v0, c6 //emit projected position	    \n"\
"dp4 oPos.w, v0, c7 //emit projected position	    \n"\
"mov oD0, c6	//Diffuse color = c8            \n";


#endif
