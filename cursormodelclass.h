////////////////////////////////////////////////////////////////////////////////
// Filename: cursormodelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CURSORMODELCLASS_H_
#define _CURSORMODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "cursortextureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: CursorModelClass
////////////////////////////////////////////////////////////////////////////////
class CursorModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	CursorModelClass();
	CursorModelClass(const CursorModelClass&);
	~CursorModelClass();

	bool Initialize(
		ID3D11Device*,			   // D3D11 device
		ID3D11DeviceContext*,	   // D3D11 device context
		HWND&,					   // game HWND
		HANDLE&,				   // Shared surface handle
		int&,					   // screen width
		int&,					   // screen height
		int&,					   // game window width
		int&,					   // game window height
		float&,					   // top offset
		float&,					   // bottom offset
		float&,					   // left offset
		float&,					   // right offset
		bool,					   // integer scaling
		bool,					   // integer scaling overscan
		int*,				   // mouse cursor bitmap data
		float,					   // mouse position remap scale X
		float,					   // mouse position remap scale Y
		int,					   // mouse pointer size
		RECT&					   // mouse area RECT
	);
	void Shutdown();
	void Render(HWND&, ID3D11DeviceContext*, int, int, int, int, float, float, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();

private:
	bool InitializeBuffers(
		HWND&,				// game HWND
		ID3D11Device*,		// D3D11 device
		int&,				// screen width
		int&,				// screen height
		int&,				// game window width
		int&,				// game window height
		float&,				// top offset
		float&,				// bottom offset
		float&,				// left offset
		float&,				// right offset
		bool,				// integer scaling
		bool,				// integer scaling overscan
		float,				// mouse xscale
		float,				// mouse yscale
		int,					// Mouse Pointer size
		RECT&				// mouse area RECT
	);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(
		ID3D11Device*,			// D3D11 device
		ID3D11DeviceContext*,	// D3D11 device context
		HWND&,					// game HWND
		HANDLE&,				// Shared surface handle
		int*,				// mouse cursor bitmap data
		float,					// mouse position remap scale X
		float,					// mouse position remap scale Y
		int						// mouse pointer size
	);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	CursorTextureClass* m_CursorTexture;
	VertexType* vertices;
};

#endif