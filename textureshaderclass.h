////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "TexturePixelShader.h"
#include "TextureVertexShader.h"

using namespace DirectX;
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class TextureShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	bool Initialize(ID3D11Device*, HWND, bool);
	void Shutdown();
	bool Render(
		ID3D11DeviceContext*,				// D3D11 device context
		int,								// Index count
		XMMATRIX,							// World Matrix
		XMMATRIX,							// View Matrix
		XMMATRIX,							// Projection Matrix
		ID3D11ShaderResourceView*,			// Texture
		bool								// Linear interpolation filtering
	);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, bool);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(
		ID3D11DeviceContext*,				// D3D11 device context
		XMMATRIX,							// World Matrix
		XMMATRIX,							// View Matrix
		XMMATRIX,							// Projection Matrix
		ID3D11ShaderResourceView*			// Texture
	);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif