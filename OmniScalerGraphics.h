////////////////////////////////////////////////////////////////////////////////
// Filename: OmniScalerGraphics.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "OmniScalerD3D.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "cursormodelclass.h"
#include "textureshaderclass.h"
#include "cursorshaderclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// Class name: OmniScalerGraphics
////////////////////////////////////////////////////////////////////////////////
class OmniScalerGraphics
{
public:
	OmniScalerGraphics();
	OmniScalerGraphics(const OmniScalerGraphics&);
	~OmniScalerGraphics();

	bool Initialize(
		HWND&,			// omniscaler D3D HWND
		HWND&,			// game HWND
		HANDLE&,		// Shared surface handle
		int&,			// screen width
		int&,			// screen height
		int&,			// screen refresh rate
		int&,			// game window width
		int&,			// game window height
		float&,			// top offset
		float&,			// bottom offset
		float&,			// left offset
		float&,			// right offset
		bool,			// linear interpolation filtering
		bool,			// mouse pointer linear interpolation
		bool,			// integer scaling 
		float,			// integer scaling scale
		bool,			// integer scaling overscan
		int*,		// mouse cursor bitmap data
		float,			// mouse position remap scale X
		float,			// mouse position remap scale Y
		int,			// mouse pointer size
		RECT&,			// mouse area RECT
		bool			// Enable custom mouse pointer
	);
	void Shutdown();
	bool UpdateScreen(
		bool,
		bool,
		float,
		float,
		float,
		float,
		RECT,
		HWND&,			// game HWND
		int,			// screen width
		int,			// screen height
		int,			// game window width
		int,			// game window height
		int,			// Mouse Pointer size
		bool			// Enable custom mouse pointer
		);

private:
	bool Render(
		bool,
		bool,
		float,
		float,
		float,
		float,
		RECT,
		HWND&,			// game HWND
		int,			// screen width
		int,			// screen height
		int,			// game window width
		int,			// game window height
		int,			// Mouse Pointer size
		bool			// Enable custom mouse pointer
	);


private:
	OmniScalerD3D* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	CursorModelClass* m_CursorModel;
	TextureShaderClass* m_TextureShader;
	CursorShaderClass* m_CursorShader;

};

#endif