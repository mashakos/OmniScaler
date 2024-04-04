////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

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
		float,					   // integer scaling scale
		bool,					   // integer scaling overscan
		int*,				   // mouse cursor bitmap data
		float,					   // mouse position remap scale X
		float,					   // mouse position remap scale Y
		int,					   // mouse pointer size
		RECT&					   // mouse area RECT
	);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

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
		float,				// integer scaling scale
		bool,				// integer scaling overscan
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
	TextureClass* m_Texture;
};

#endif