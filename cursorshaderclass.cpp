////////////////////////////////////////////////////////////////////////////////
// Filename: cursorshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cursorshaderclass.h"


CursorShaderClass::CursorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}


CursorShaderClass::CursorShaderClass(const CursorShaderClass& other)
{
}


CursorShaderClass::~CursorShaderClass()
{
}


bool CursorShaderClass::Initialize(ID3D11Device* device, HWND hwnd, bool linearInterpolation)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L".\\omniscaler_files\\cursortexture.vs", L".\\omniscaler_files\\texture.ps", linearInterpolation);
	if (!result)
	{
		return false;
	}

	return true;
}


void CursorShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool CursorShaderClass::Render(
	ID3D11DeviceContext* deviceContext,			// D3D11 device context
	int indexCount,								// Index count
	XMMATRIX worldMatrix,						// World Matrix
	XMMATRIX viewMatrix,						// View Matrix
	XMMATRIX projectionMatrix,					// Projection Matrix
	ID3D11ShaderResourceView* texture,			// Texture
	bool linearInterpolation,					// Linear interpolation filtering
	int gRealScreenWidth,				// screen width
	int gRealScreenHeight,				// screen height
	int gScreenWidth,					// game window width
	int gScreenHeight,					// game window height
	float mouseX,						// Mouse X
	float mouseY,						// Mouse Y
	int mcursor_w						// Mouse Pointer size
)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		texture,
		gRealScreenWidth,
		gRealScreenHeight,
		gScreenWidth,
		gScreenHeight,
		mouseX,
		mouseY,
		mcursor_w
	);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool CursorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, bool linearInterpolation)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	/*
		// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	*/

	// Create the vertex shader from the buffer.
	//result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	result = device->CreateVertexShader(g_cursorTextureVertexShader, sizeof(g_cursorTextureVertexShader), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	//result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	result = device->CreatePixelShader(g_cursorTexturePixelShader, sizeof(g_cursorTexturePixelShader), nullptr, &m_pixelShader);

	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, g_cursorTextureVertexShader,
		sizeof(g_cursorTextureVertexShader), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	//vertexShaderBuffer->Release();
	//vertexShaderBuffer = 0;

	//pixelShaderBuffer->Release();
	//pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	//LINEAR TEXTURE FILTERING
	if (linearInterpolation)
		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, D3D11_FILTER_TYPE_LINEAR, false);
	else
		////NEAREST TEXTURE FILTERING
		samplerDesc.Filter = D3D11_ENCODE_BASIC_FILTER(D3D11_FILTER_TYPE_POINT, D3D11_FILTER_TYPE_POINT, D3D11_FILTER_TYPE_POINT, false);
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void CursorShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void CursorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool CursorShaderClass::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,			// D3D11 device context
	XMMATRIX worldMatrix,						// World Matrix
	XMMATRIX viewMatrix,						// View Matrix
	XMMATRIX projectionMatrix,					// Projection Matrix
	ID3D11ShaderResourceView* texture,			// Texture
	int gRealScreenWidth,				// screen width
	int gRealScreenHeight,				// screen height
	int gScreenWidth,					// game window width
	int gScreenHeight,					// game window height
	float mouseX,						// Mouse X
	float mouseY,						// Mouse Y
	int mcursor_w						// Mouse Pointer size
)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	XMMATRIX identityMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//// Setup the translation matrix.
	//translationMatrix = XMMatrixTranslation((float)mouseX / 800.0f, (float)mouseY / 600.0f, 0.0f);

	//// Create the final matrix and store it in the instances array.
	//worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

	XMVECTOR transformation[4] = 
	{
			{1.0f,		   0.0f,		   0.0f, 0.0f},
			{0.0f,		   1.0f,		   0.0f, 0.0f},
			{0.0f,		   0.0f,		   1.0f, 0.0f},
			{mouseX, mouseY, 0.0f, 1.0f}
	};

	XMMATRIX cursorMatrix(transformation[0], transformation[1], transformation[2], transformation[3]);


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->cursor = cursorMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}


void CursorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}