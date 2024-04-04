////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"


TextureClass::TextureClass()
{
	m_targaData = 0;
	//m_texture = 0;
	m_textureView = 0;
	m_textures[0] = 0;
	m_textures[1] = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(
	ID3D11Device* device,					// D3D11 device
	ID3D11DeviceContext* deviceContext,		// D3D11 device context
	HWND& hwnd,								// game HWND
	HANDLE& appHandle,						// Shared surface handle
	int* mouseCursorBmpData,			// mouse cursor bitmap data
	float mousePosRemap_xscale,				// mouse position remap scale X
	float mousePosRemap_yscale,				// mouse position remap scale Y
	int mousePointer_size					// mouse pointer size
)
{
	HRESULT hResult;
	DWORD	d3dFormat;
	ID3D11Resource  *tempResource;
	//ID3D11Texture2D *m_texture = NULL;

	//hwnd is game handle
	//assign using below in initscaler:
	//HANDLE appHandle;
	//appHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pi.dwProcessId);
	//TCHAR appClassName[MAX_PATH];
	//GetClassName(hwnd, appClassName, _countof(appClassName));
	//appHandle = FindWindowA((LPCSTR)appClassName, NULL);


	HRESULT hrs = device->OpenSharedResource(appHandle, __uuidof(ID3D11Resource), (void**)&tempResource);
	//if (SUCCEEDED(hrs))
	{
		hrs = tempResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&m_textures[0]);
		D3D11_TEXTURE2D_DESC textureDesc;
		m_textures[0]->GetDesc(&textureDesc);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		D3D11_RESOURCE_DIMENSION type;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels - 1;
		hResult = device->CreateShaderResourceView(m_textures[0], &srvDesc, &m_textureView);
		if (FAILED(hResult))
		{
			return false;
		}

		m_textureViews[0] = m_textureView;
		//m_textureViews[1] = m_texture1View;

		// Generate mipmaps for this texture.
		deviceContext->GenerateMips(m_textureView);
		//deviceContext->GenerateMips(m_texture1View);
		tempResource->Release();
	}

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}
	if (m_texture1View)
	{
		m_texture1View->Release();
		m_texture1View = 0;
	}

	// Release the texture.
	if (m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = 0;
	}
	if (m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = 0;
	}

	// Release the targa data.
	if(m_targaData)
	{
		delete [] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

ID3D11ShaderResourceView** TextureClass::GetTextureArray()
{
	return m_textureViews;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if(bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if(!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];
	if(!m_targaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete [] targaImage;
	targaImage = 0;

	return true;
}