////////////////////////////////////////////////////////////////////////////////
// Filename: OmniScalerD3D.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <DWMapi.h>
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: OmniScalerD3D
////////////////////////////////////////////////////////////////////////////////
class OmniScalerD3D
{
public:
	OmniScalerD3D();
	OmniScalerD3D(const OmniScalerD3D&);
	~OmniScalerD3D();

	bool Initialize(
		bool,			// vsync
		HWND,			// omniscaler D3D HWND
		bool,			// fullscreen
		float,			// screen depth
		float,			// screen near clip
		HWND&,			// game HWND
		HANDLE&,		// Shared surface handle
		int&,			// screen width
		int&,			// screen height
		int&,			// screen refresh rate
		int&,			// game window width
		int&			// game window height
	);
	void Shutdown();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();
private:
	HANDLE GetDWMSharedHandle(HWND&, LUID&, HMONITOR&);
	void GetWindowSharedSurfaceHandle(
		HWND&,				// game HWND
		HANDLE&,			// Shared surface handle
		DWORD*,				// D3D Format
		LUID&,				// Graphics card adapter ID
		HMONITOR&			// Monitor ID
	);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	ID3D11DepthStencilState* m_depthDisabledStencilState;

	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

};

#endif
