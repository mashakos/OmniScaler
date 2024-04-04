////////////////////////////////////////////////////////////////////////////////
// Filename: cursormodelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cursormodelclass.h"
#include <Uxtheme.h>


CursorModelClass::CursorModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_CursorTexture = 0;
}


CursorModelClass::CursorModelClass(const CursorModelClass& other)
{
}


CursorModelClass::~CursorModelClass()
{
}


bool CursorModelClass::Initialize(
	ID3D11Device* device,				// D3D11 device
	ID3D11DeviceContext* deviceContext, // D3D11 device context
	HWND& hwnd,							// game HWND
	HANDLE& appHandle,					// Shared surface handle
	int& gRealScreenWidth,				// screen width
	int& gRealScreenHeight,				// screen height
	int& gScreenWidth,					// game window width
	int& gScreenHeight,					// game window height
	float& topOffset,					// top offset
	float& bottomOffset,				// bottom offset
	float& leftOffset,					// left offset
	float& rightOffset,					// right offset
	bool integerScaling,				// integer scaling
	bool IntegerScalingOverscan,		// integer scaling overscan
	int* mouseCursorBmpData,		// mouse cursor bitmap data
	float mousePosRemap_xscale,			// mouse position remap scale X
	float mousePosRemap_yscale,			// mouse position remap scale Y
	int mousePointer_size,				// mouse pointer size
	RECT& mouse_area					// mouse area RECT
)
{
	bool result;


	// Initialize the vertex and index buffers.
	result = InitializeBuffers(hwnd, device, gRealScreenWidth, gRealScreenHeight, gScreenWidth, gScreenHeight, topOffset, bottomOffset, leftOffset, rightOffset, integerScaling, IntegerScalingOverscan, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size, mouse_area);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, deviceContext, hwnd, appHandle, mouseCursorBmpData, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size);
	if (!result)
	{
		return false;
	}

	return true;
}


void CursorModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}


void CursorModelClass::Render(
	HWND& hwnd,							// game HWND
	ID3D11DeviceContext* deviceContext,	// Device context
	int gRealScreenWidth,				// screen width
	int gRealScreenHeight,				// screen height
	int gScreenWidth,					// game window width
	int gScreenHeight,					// game window height
	float mouseX,
	float mouseY,
	int mcursor_w						// Mouse Pointer size
)
{
	
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int CursorModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* CursorModelClass::GetTexture()
{
	return m_CursorTexture->GetTexture();
}


ID3D11ShaderResourceView** CursorModelClass::GetTextureArray()
{
	return m_CursorTexture->GetTextureArray();
}


bool CursorModelClass::InitializeBuffers(
	HWND& hwnd,						// game HWND
	ID3D11Device* device,			// D3D11 device
	int& gRealScreenWidth,			// screen width
	int& gRealScreenHeight,			// screen height
	int& gScreenWidth,				// game window width
	int& gScreenHeight,				// game window height
	float& topOffset,				// top offset
	float& bottomOffset,			// bottom offset
	float& leftOffset,				// left offset
	float& rightOffset,				// right offset
	bool integerScaling,			// integer scaling
	bool IntegerScalingOverscan,	// integer scaling overscan
	float mousePosRemap_xscale,		// mouse position remap scale X
	float mousePosRemap_yscale,		// mouse position remap scale Y
	int mcursor_w,					// mouse pointer size
	RECT& mouse_area				// mouse area RECT
)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	float mousePointer_size = (float)mcursor_w * mousePosRemap_xscale;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	float left, right, top, bottom;

	RECT gsr;
	RECT gsr_win;
	GetClientRect(hwnd, &gsr);
	GetWindowRect(hwnd, &gsr_win);

	gScreenWidth = gsr.right;
	gScreenHeight = gsr.bottom;

	float gHwnd_winwidth = (float)gsr_win.right - (float)gsr_win.left;
	float gHwnd_winheight = (float)gsr_win.bottom - (float)gsr_win.top;

	float cursorWidth = mousePointer_size;// / gHwnd_winwidth;
	float cursorHeight = mousePointer_size;// / gHwnd_winwidth;
	float cursorTexWidth = 1.0f;// (float)mcursor_w / gHwnd_winwidth;
	float cursorTexHeight = 1.0f;// (float)mcursor_w / gHwnd_winheight;
	float gHwnd_clienttop = 0.0f;
	float gHwnd_clientleft = 0.0f;
	float d3dScaleFactor = 0.0f;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = -cursorWidth / 2.0f;
	// Calculate the screen coordinates of the right side of the bitmap.
	right = cursorWidth / 2.0f;
	// Calculate the screen coordinates of the top of the bitmap.
	top = cursorHeight / 2.0f;
	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = -cursorHeight / 2.0f;

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(left, bottom, d3dScaleFactor);  // Bottom left.
	vertices[0].texture = XMFLOAT2(gHwnd_clientleft, cursorTexHeight);

	vertices[1].position = XMFLOAT3(left, top, d3dScaleFactor);  // Top left.
	vertices[1].texture = XMFLOAT2(gHwnd_clientleft, gHwnd_clienttop);

	vertices[2].position = XMFLOAT3(right, top, d3dScaleFactor);  // top right.
	vertices[2].texture = XMFLOAT2(cursorTexWidth, gHwnd_clienttop);

	vertices[3].position = XMFLOAT3(right, bottom, d3dScaleFactor);  // Bottom right.
	vertices[3].texture = XMFLOAT2(cursorTexWidth, cursorTexHeight);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // top right.

	indices[3] = 0;  // Bottom left.
	indices[4] = 2;  // Top right.
	indices[5] = 3;  // Bottom right.

					 // Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void CursorModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void CursorModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool CursorModelClass::LoadTexture(
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
	bool result;


	// Create the texture object.
	m_CursorTexture = new CursorTextureClass;
	if (!m_CursorTexture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_CursorTexture->Initialize(device, deviceContext, hwnd, appHandle, mouseCursorBmpData, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size);
	if (!result)
	{
		return false;
	}

	return true;
}


void CursorModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_CursorTexture)
	{
		m_CursorTexture->Shutdown();
		delete m_CursorTexture;
		m_CursorTexture = 0;
	}

	return;
}