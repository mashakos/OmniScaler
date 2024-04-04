////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"
#include <Uxtheme.h>


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(
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
	float integerScalingOverride,			// integer scaling scale
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
	result = InitializeBuffers(hwnd, device, gRealScreenWidth, gRealScreenHeight, gScreenWidth, gScreenHeight, topOffset, bottomOffset, leftOffset, rightOffset, integerScaling, integerScalingOverride, IntegerScalingOverscan, mouse_area);
	if(!result)
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


void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_Texture->GetTextureArray();
}


bool ModelClass::InitializeBuffers(
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
	float integerScalingOverride,		// integer scaling scale
	bool IntegerScalingOverscan,	// integer scaling overscan
	RECT& mouse_area				// mouse area RECT
)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	float left, right, top, bottom;

	RECT gsr;
	RECT gsr_win;
	GetClientRect(hwnd, &gsr);
	GetWindowRect(hwnd, &gsr_win);
	POINT lefttop = { gsr.left, gsr.top }; // Practicaly both are 0
	ClientToScreen(hwnd, &lefttop);
	POINT rightbottom = { gsr.right, gsr.bottom };
	ClientToScreen(hwnd, &rightbottom);

	gScreenWidth = gsr.right;
	gScreenHeight = gsr.bottom - (int)topOffset - (int)bottomOffset;

	// border for gdi
	//GetSystemMetrics(SM_CYEDGE)
	// border for aero
	// GetThemeSysSize(NULL, SM_CXBORDER);
	//title bar for gdi
	// GetSystemMetrics(SM_CYCAPTION)
	// title bar for aero
	// GetThemeSysSize(NULL, SM_CXPADDEDBORDER)
	//frame for gdi
	// GetSystemMetrics(SM_CYSIZEFRAME)
	// frame for aero
	// GetThemeSysSize(NULL, SM_CYSIZE)

	//float gHwnd_border = (float)GetSystemMetrics(SM_CXSIZEFRAME);// -GetSystemMetrics(SM_CYEDGE);
	//int gHwnd_titlebar = GetSystemMetrics(SM_CYCAPTION);
	float gHwnd_winwidth = (float)gsr_win.right - (float)gsr_win.left;
	float gHwnd_winheight = ((float)gsr_win.bottom - (float)gsr_win.top);

	// pixel offsets for window that has extra menus or UI elements
	float topOffsetTex, bottomOffsetTex, leftOffsetTex, rightOffsetTex;
	topOffsetTex = (topOffset + 0.5f) / gHwnd_winheight;
	bottomOffsetTex = (bottomOffset - 1.0f) / gHwnd_winheight;
	leftOffsetTex = (leftOffset + 0.5f) / gHwnd_winwidth;
	rightOffsetTex = (rightOffset - 1.0f) / gHwnd_winwidth;

	//title bar and window frame border values
	float h_offset = (1.0f / gHwnd_winheight);
	//float margin_offset = (topOffset || bottomOffset) > 0 ? (6.0f / gHwnd_winheight) : 0;
	float client_top = ((float)(lefttop.y - gsr_win.top) / gHwnd_winheight); // There is no transparent part
	float client_bottom = (float)(gsr_win.bottom - rightbottom.y) / gHwnd_winheight; // As above
	float client_left = (float)(lefttop.x - gsr_win.left) / gHwnd_winwidth; // Windows 10: includes transparent part
	float client_right = (float)(gsr_win.right - rightbottom.x) / gHwnd_winwidth; // As above

	float gHwnd_clientwidth = (((float)gScreenWidth) / gHwnd_winwidth) + client_right + rightOffsetTex;
	float gHwnd_clientheight = ((((float)gScreenHeight) / gHwnd_winheight) + client_top) + topOffsetTex - h_offset;
	float gHwnd_clienttop = client_top + topOffsetTex ;
	float gHwnd_clientleft = client_left + leftOffsetTex;
	float gHwnd_clientWidthAspect = (float)gScreenWidth / (float)gScreenHeight;
	float d3dScaleFactor = 0.0f;
	float initIntegerScale = 1.0f;
	float orthoHeight = gScreenHeight / 2;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = -orthoHeight * gHwnd_clientWidthAspect;
	// Calculate the screen coordinates of the right side of the bitmap.
	right = orthoHeight * gHwnd_clientWidthAspect;
	// Calculate the screen coordinates of the top of the bitmap.
	top = orthoHeight;
	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = -orthoHeight;

	if (integerScaling)
	{
		initIntegerScale = floor((float)gRealScreenHeight / (float)gScreenHeight) * integerScalingOverride;
		if(IntegerScalingOverscan)
			initIntegerScale = ceil((float)gRealScreenHeight / (float)gScreenHeight) * integerScalingOverride;
		left *= initIntegerScale;
		right *= initIntegerScale;
		top *= initIntegerScale;
		bottom *= initIntegerScale;
	}
	else
	{
		orthoHeight = (gScreenHeight * (float)gRealScreenHeight / (float)gScreenHeight) / 2;
		left = -orthoHeight * gHwnd_clientWidthAspect;
		right = orthoHeight * gHwnd_clientWidthAspect;
		top = orthoHeight;
		bottom = -orthoHeight;
	}


	float gRealScreenAspect = (float)gRealScreenWidth / (float)gRealScreenHeight;
	
	if (gRealScreenAspect < gHwnd_clientWidthAspect && !integerScaling)
	{
		float aspectReduction = gRealScreenAspect / gHwnd_clientWidthAspect;
		left *= aspectReduction;
		right *= aspectReduction;
		top *= aspectReduction;
		bottom *= aspectReduction;
	}

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(left, bottom, d3dScaleFactor);  // Bottom left.
	vertices[0].texture = XMFLOAT2(gHwnd_clientleft, gHwnd_clientheight);

	vertices[1].position = XMFLOAT3(left, top, d3dScaleFactor);  // Top left.
	vertices[1].texture = XMFLOAT2(gHwnd_clientleft, gHwnd_clienttop);

	vertices[2].position = XMFLOAT3(right, top, d3dScaleFactor);  // top right.
	vertices[2].texture = XMFLOAT2(gHwnd_clientwidth, gHwnd_clienttop);

	vertices[3].position = XMFLOAT3(right, bottom, d3dScaleFactor);  // Bottom right.
	vertices[3].texture = XMFLOAT2(gHwnd_clientwidth, gHwnd_clientheight);

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
	if(FAILED(result))
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
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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


bool ModelClass::LoadTexture(
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
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, deviceContext, hwnd, appHandle, mouseCursorBmpData, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size);
	if (!result)
	{
		return false;
	}

	return true;
}


void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}