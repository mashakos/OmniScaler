////////////////////////////////////////////////////////////////////////////////
// Filename: cursortextureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cursortextureclass.h"


CursorTextureClass::CursorTextureClass()
{
	m_targaData = 0;
	//m_texture = 0;
	m_textureView = 0;
	m_texture = 0;
}


CursorTextureClass::CursorTextureClass(const CursorTextureClass& other)
{
}


CursorTextureClass::~CursorTextureClass()
{
}


bool CursorTextureClass::Initialize(
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


	//if (SUCCEEDED(hrs))
	{
		//MOUSE POINTER TEXTURE
		// Create texture data
		D3D11_SUBRESOURCE_DATA subrecData;
		subrecData.pSysMem = mouseCursorBmpData;
		subrecData.SysMemPitch = mousePointer_size * sizeof(int);
		subrecData.SysMemSlicePitch = 0;
		// Create texture descriptor
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = mousePointer_size;
		desc.Height = mousePointer_size;
		desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		// Create texture
		device->CreateTexture2D(&desc, &subrecData, &m_texture);
		// Create texture resource
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		srvd.Texture2D.MostDetailedMip = 0;
		hResult = device->CreateShaderResourceView(m_texture, &srvd, &m_textureView);
		if (FAILED(hResult))
		{
			return false;
		}

		m_textureViews[0] = m_textureView;
		//m_textureViews[1] = m_texture1View;

		// Generate mipmaps for this texture.
		deviceContext->GenerateMips(m_textureView);
		//deviceContext->GenerateMips(m_texture1View);
	}

	return true;
}

void CursorTextureClass::Shutdown()
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
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* CursorTextureClass::GetTexture()
{
	return m_textureView;
}

ID3D11ShaderResourceView** CursorTextureClass::GetTextureArray()
{
	return m_textureViews;
}
