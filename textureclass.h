////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(
		ID3D11Device*,			// D3D11 device
		ID3D11DeviceContext*,	// D3D11 device context
		HWND&,					// game HWND
		HANDLE&,				// Shared surface handle
		int*,				// mouse cursor bitmap data
		float,					// mouse position remap scale X
		float,					// mouse position remap scale Y
		int						// mouse pointer size
	);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();

private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_textures[2];
	ID3D11ShaderResourceView* m_textureView;
	ID3D11ShaderResourceView* m_texture1View;
	ID3D11ShaderResourceView* m_textureViews[2];

};

#endif