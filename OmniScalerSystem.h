////////////////////////////////////////////////////////////////////////////////
// Filename: OmniScalerSystem.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_
#include "stdafx.h"
//////////////
// INCLUDES //
//////////////
#include <afxwin.h>
#include <varargs.h>
#include <iostream>
#include <DWMapi.h>
#include <Tlhelp32.h>
#include <dinput.h>
#include <math.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <comdef.h>  // for game launcher code

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CommandParse.h"
#include "OmniScalerGraphics.h"
#include "OmniScalerD3D.h"
#include "OmniScalerInput.h"

using std::string;
using std::pair;
using namespace boost;
using namespace property_tree;



////////////////////////////////////////////////////////////////////////////////
// Class name: OmniScalerSystem
////////////////////////////////////////////////////////////////////////////////
class OmniScalerSystem :public CWinApp
{
public:
	OmniScalerSystem();
	OmniScalerSystem(const OmniScalerSystem&);
	~OmniScalerSystem();

	//Exe Arguments Parse: Fill the passed in array structures.
	void GetCommandLineParser(
		CStringArray& params,
		CStringArray& switches);
	HWND find_main_window(unsigned long process_id);
	struct handle_data {
		unsigned long process_id;
		HWND window_handle;
	} data;
	MSG Msg;


	bool InitScaler(HINSTANCE&, int&, LPWSTR&, wchar_t*);
	void Shutdown();
	void MainLoop();
	void InitWindow(HINSTANCE, HWND&, HWND);

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	HINSTANCE m_hinstance;

	void getMouseCursorBitmap(int*);

private:
	bool UpdateScreen();
	void ShutdownWindow();

private:
	HWND FindTargetWindow(unsigned long process_id);
	static BOOL CALLBACK static_enum_windows_callback(HWND handle, LPARAM lParam);
	CCommandParse m_cmdParse;

	LPCWSTR m_applicationName;
	HWND* m_hwnd;

	OmniScalerInput* m_Input;
	OmniScalerGraphics* m_Graphics;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK oscWndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static OmniScalerSystem* ApplicationHandle = 0;


#endif