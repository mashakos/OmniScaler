////////////////////////////////////////////////////////////////////////////////
// Filename: OmniScalerGraphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "OmniScalerGraphics.h"


OmniScalerGraphics::OmniScalerGraphics()
{
	m_Direct3D = 0;
}


OmniScalerGraphics::OmniScalerGraphics(const OmniScalerGraphics& other)
{
}


OmniScalerGraphics::~OmniScalerGraphics()
{
}


bool OmniScalerGraphics::Initialize(
	HWND& d3dHwnd,						// omniscaler D3D HWND
	HWND& gHwnd,						// game HWND
	HANDLE& appHandle,					// Shared surface handle
	int& gRealScreenWidth,				// screen width
	int& gRealScreenHeight,				// screen height
	int& gRealRefreshRate,			// screen refresh rate
	int& gScreenWidth,					// game window width
	int& gScreenHeight,					// game window height
	float& topOffset,					// top offset
	float& bottomOffset,				// bottom offset
	float& leftOffset,					// left offset
	float& rightOffset,					// right offset
	bool linearInterpolation,			// linear interpolation filtering
	bool MousePointerLinear,			// mouse pointer linear interpolation 
	bool integerScaling,				// integer scaling
	float integerScalingOverride,			// integer scaling scale
	bool IntegerScalingOverscan,		// integer scaling overscan
	int* mouseCursorBmpData,		// mouse cursor bitmap data
	float mousePosRemap_xscale,			// mouse position remap scale X
	float mousePosRemap_yscale,			// mouse position remap scale Y
	int mousePointer_size,				// mouse pointer size
	RECT& mouse_area,					// mouse area RECT
	bool MousePointerEnable				// Enable custom mouse pointer
)
{
	bool result;


	// Create the Direct3D object.
	m_Direct3D = new OmniScalerD3D;
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(VSYNC_ENABLED, d3dHwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, gHwnd, appHandle, gRealScreenWidth, gRealScreenHeight, gRealRefreshRate, gScreenWidth, gScreenHeight);
	if (!result)
	{
		MessageBox(d3dHwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), gHwnd, appHandle, gRealScreenWidth, gRealScreenHeight, gScreenWidth, gScreenHeight, topOffset, bottomOffset, leftOffset, rightOffset, integerScaling, integerScalingOverride, IntegerScalingOverscan, mouseCursorBmpData, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size, mouse_area);
	if (!result)
	{
		MessageBox(gHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), gHwnd, linearInterpolation);
	if (!result)
	{
		MessageBox(gHwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	if (MousePointerEnable)
	{
		// Create the cursor object.
		m_CursorModel = new CursorModelClass;
		if (!m_CursorModel)
		{
			return false;
		}

		// Initialize the cursor object.
		result = m_CursorModel->Initialize(
			m_Direct3D->GetDevice(),
			m_Direct3D->GetDeviceContext(),
			gHwnd,
			appHandle,
			gRealScreenWidth,
			gRealScreenHeight,
			gScreenWidth,
			gScreenHeight,
			topOffset,
			bottomOffset,
			leftOffset,
			rightOffset,
			integerScaling,
			IntegerScalingOverscan,
			mouseCursorBmpData,
			mousePosRemap_xscale,
			mousePosRemap_yscale,
			mousePointer_size,
			mouse_area);
		if (!result)
		{
			MessageBox(gHwnd, L"Could not initialize the cursor object.", L"Error", MB_OK);
			return false;
		}

		// Create the cursor shader object.
		m_CursorShader = new CursorShaderClass;
		if (!m_CursorShader)
		{
			return false;
		}

		// Initialize the cursor shader object.
		result = m_CursorShader->Initialize(m_Direct3D->GetDevice(), gHwnd, MousePointerLinear);
		if (!result)
		{
			MessageBox(gHwnd, L"Could not initialize the cursor shader object.", L"Error", MB_OK);
			return false;
		}
	}


	return true;
}


void OmniScalerGraphics::Shutdown()
{
	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}


bool OmniScalerGraphics::UpdateScreen(
	bool integerScaling,
	bool linearInterpolation,
	float x,
	float y,
	float mousePosRemap_xscale,
	float mousePosRemap_yscale,
	RECT gsr,
	HWND& hwnd,							// game HWND
	int gRealScreenWidth,				// screen width
	int gRealScreenHeight,				// screen height
	int gScreenWidth,					// game window width
	int gScreenHeight,					// game window height
	int mcursor_w,						// Mouse Pointer size
	bool MousePointerEnable				// Enable custom mouse pointer
)
{
	bool result;

	// Render the graphics scene.
	result = Render(
		integerScaling,
		linearInterpolation,
		x,
		y,
		mousePosRemap_xscale,
		mousePosRemap_yscale,
		gsr,
		hwnd,
		gRealScreenWidth,
		gRealScreenHeight,
		gScreenWidth,
		gScreenHeight,
		mcursor_w,
		MousePointerEnable
	);
	if (!result)
	{
		return false;
	}
	return true;
}


bool OmniScalerGraphics::Render(
	bool integerScaling,
	bool linearInterpolation,
	float mouseX,
	float mouseY,
	float mousePosRemap_xscale,
	float mousePosRemap_yscale,
	RECT gsr,
	HWND& hwnd,							// game HWND
	int gRealScreenWidth,				// screen width
	int gRealScreenHeight,				// screen height
	int gScreenWidth,					// game window width
	int gScreenHeight,					// game window height
	int mcursor_w,						// Mouse Pointer size
	bool MousePointerEnable				// Enable custom mouse pointer
)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();


	// Put the game model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Model->GetTexture(), linearInterpolation);
	if (!result)
	{
		return false;
	}

	if (MousePointerEnable)
	{
		// enable alpha blending for mouse cursor transparency
		m_Direct3D->TurnOnAlphaBlending();

		// Put the mouse cursor model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_CursorModel->Render(hwnd, m_Direct3D->GetDeviceContext(), gRealScreenWidth, gRealScreenHeight, gScreenWidth, gScreenHeight, mouseX, mouseY, mcursor_w);

		// Render the model using the mouse cursor texture shader.
		result = m_CursorShader->Render(
			m_Direct3D->GetDeviceContext(),
			m_CursorModel->GetIndexCount(),
			worldMatrix,
			viewMatrix,
			orthoMatrix,
			m_CursorModel->GetTexture(),
			linearInterpolation,
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
		m_Direct3D->TurnOffAlphaBlending();
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}