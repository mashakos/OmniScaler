////////////////////////////////////////////////////////////////////////////////
// Filename: OmniScalerSystem.cpp
////////////////////////////////////////////////////////////////////////////////
#include "OmniScalerSystem.h"

//global variables
int gAllowResize = 0;
int gIsActive = 0;
int isactive = 0;
int gModeWidth = 824;
int gScreenWidth = 824;
int gScreenHeight = 618;
int gRealScreenWidth;
int gRealScreenHeight;
int gRealRefreshRate;
uint32_t frameStart;
int frameTime;
int frameDelay;
int gScreenBits;
WNDPROC origfunc = NULL;
int gLastUpdate = -1;
int gActiveCnt = 0;
int gSmooth = 0;
int gAltWinPos = 0;
int gGDI = 0;
STARTUPINFO si;
PROCESS_INFORMATION pi;
HWND gHwnd = NULL;
TCHAR appClassName[MAX_PATH];
WNDCLASSEX wc;
HWND d3dHwnd = NULL;
HANDLE appHandle;
int tex_wq = 5274;
int tex_hq = 2637;
size_t bitmap_buffer_size = tex_wq * tex_hq;
BYTE* lpPixels = new BYTE[bitmap_buffer_size];
BYTE *bitmap_buffer;
UINT_PTR IDT_TIMER1;
DWORD lpExitCode = 0;
DWORD currExitCode = 0;
HHOOK kb_hhook = NULL;
int glinitvar = 0;
BOOL gHandleKbExit = FALSE;
LPCWSTR g_szClassName[] = { L"d3dWindowClass" };

//RESHADE INIT
static int reshadeinit = -1;
WORD reshadeInitKey = VK_HOME;
int reshadeInitDur = 1000;


//MOUSE AND MOUSE LIMIT
HCURSOR ttc;
int* mouseCursorBmpData;
int mcursor_w = 32;
int mcursor_h = 32;
float mcursor_u;
float mcursor_v;
int mcursor_wq = 256;
int mcursor_hq = 256;
RECT r;
RECT windowRect;
POINT mousePos;
float mousePosRemap_xscale;
float mousePosRemap_yscale;
float mousePosRemap_xoffset;
float mousePosRemap_yoffset;
int mousePointer_size = 32;
float initIntegerScale;
float mouseCursorSize;
float gScreenDynWidth;
float gScreenDynHeight;
float rDynOffset;
float rDynYOffset;
float rScreenWnd;
float rScreenReal;
float rScreenWndScale;
float gHwnd_winwidth;
float gHwnd_winheight;
RECT mouse_area;
BOOL gScreenResized = FALSE;
int gWndTick = 0;


int fullscr_delay = 0;
int alttab_delay = 0;
BOOL isMinimised = FALSE;

//INI VALS
BOOL integerScaling = FALSE;
float integerScalingOverride = 1.0f;
BOOL IntegerScalingOverscan = FALSE;
BOOL linearInterpolation = FALSE;
BOOL modeLauncher = TRUE;
BOOL isFullscreen = TRUE;
BOOL isWindowed = FALSE;
BOOL isStretched = FALSE;
BOOL MousePointerEnable = FALSE;
BOOL MousePointerLinear = FALSE;
BOOL MousePointerBig = FALSE;
wchar_t argv[10000];
float topOffset = 0.0f;
float bottomOffset = 0.0f;
float leftOffset = 0.0f;
float rightOffset = 0.0f;
//INI VALS

//GAME PROCESS
HANDLE hProcess_Game;
DWORD processId_Game;

OmniScalerSystem::OmniScalerSystem()
{
	m_Input = 0;
	m_Graphics = 0;
}

OmniScalerSystem::OmniScalerSystem(const OmniScalerSystem& other)
{
}


OmniScalerSystem::~OmniScalerSystem()
{
}

bool OmniScalerSystem::InitScaler(HINSTANCE& m_hInstance, int& argCount, LPWSTR& szArgList, wchar_t *CommandLineW)
{
	bool result;
	TCHAR currDir[1000];
	TCHAR GamePath[1000];
	boost::filesystem::path GameDir;
	boost::filesystem::path LauncherExe;
	boost::filesystem::path GameExe;
	ZeroMemory(&GameDir, sizeof(GameDir));
	ZeroMemory(&GameExe, sizeof(GameExe));
	ZeroMemory(&GamePath, sizeof(GamePath));

	// mouse area and window area
	RECT gsr;
	RECT gsr_win;
	POINT lefttop;
	POINT rightbottom;



	GetCurrentDirectory(1000, currDir);


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION;
	si.dwX = 0;
	si.dwY = 0;
	ZeroMemory(&pi, sizeof(pi));

	MSG tmsg = { 0 };
	TranslateMessage(&tmsg);

	ptree pt;
	srand(time(NULL));
	try
	{
		std:string str;
		ZeroMemory(&str, sizeof(str));

		ini_parser::read_ini("omniscaler.ini", pt);
		if ((pt.get<std::uint16_t>("Fullscreen.Enabled") != 0))
		{
			topOffset = (pt.get<std::float_t>("Fullscreen.offset_top"));
			bottomOffset = (pt.get<std::float_t>("Fullscreen.offset_bottom"));
			//leftOffset = (pt.get<std::float_t>("Fullscreen.offset_left"));
			//rightOffset = (pt.get<std::float_t>("Fullscreen.offset_right"));
			integerScaling = bool(pt.get<std::uint16_t>("Fullscreen.IntegerScaling"));
			integerScalingOverride = (pt.get<std::float_t>("Fullscreen.integerScalingOverride"));
			IntegerScalingOverscan = bool(pt.get<std::uint16_t>("Fullscreen.IntegerScalingOverscan"));
			linearInterpolation = bool(pt.get<std::uint16_t>("Fullscreen.LinearFilter"));
			MousePointerEnable = bool(pt.get<std::uint16_t>("Fullscreen.MousePointerEnable"));
			MousePointerLinear = bool(pt.get<std::uint16_t>("Fullscreen.MousePointerLinear"));
			MousePointerBig = bool(pt.get<std::uint16_t>("Fullscreen.MousePointerBig"));
			isFullscreen = bool(1 - pt.get<std::uint16_t>("Fullscreen.StartWindowed"));
			isWindowed = bool(pt.get<std::uint16_t>("Fullscreen.StartWindowed"));
			isStretched = bool(pt.get<std::uint16_t>("Fullscreen.Stretched"));
		}
		modeLauncher = bool( pt.get<std::string>("Fullscreen.LauncherExe").size() > 0 ? 1 : 0);
		str = pt.get<std::string>("Fullscreen.GameDir");
		if (str.size() > 0)
		{
			std::wstring widestr = std::wstring(str.begin(), str.end());
			GameDir = { widestr.c_str() };
			ZeroMemory(&str, sizeof(str));
		}
		else
		{
			GameDir = { L"." };
		}
		/*
		check if game starts with launcher
		if not, set game exe path directly:
		*/
		if (!modeLauncher)
		{
			str = pt.get<std::string>("Fullscreen.GameExe");
			if (str.size() > 0)
			{
				std::wstring widestr = std::wstring(str.begin(), str.end());
				GameExe = { widestr.c_str() };
				ZeroMemory(&str, sizeof(str));
			}
			if (GameExe.size() > 0 && GameDir.size() > 0)
			{

				wcscat_s(GamePath, L"\"");
				wcscat_s(GamePath, GameDir.c_str());
				wcscat_s(GamePath, L"\\");
				wcscat_s(GamePath, GameExe.c_str());
				wcscat_s(GamePath, L"\"");

				//SetCurrentDirectory(GameDir.c_str());

				//Exe Arguments Parse: Use the Custom Command Line Parser
				wcscpy_s(argv, GamePath);
			}
		}
		/*
		check if game starts with launcher
		if so, set launcher exe in game path:
		*/
		else
		{
			str = pt.get<std::string>("Fullscreen.GameExe");
			if (str.size() > 0)
			{
				std::wstring widestr = std::wstring(str.begin(), str.end());
				GameExe = { widestr.c_str() };
				ZeroMemory(&str, sizeof(str));
			}
			str = pt.get<std::string>("Fullscreen.LauncherExe");
			if (str.size() > 0)
			{
				std::wstring widestr = std::wstring(str.begin(), str.end());
				LauncherExe = { widestr.c_str() };
				ZeroMemory(&str, sizeof(str));
			}
			if (LauncherExe.size() > 0 && GameDir.size() > 0)
			{

				wcscat_s(GamePath, L"\"");
				wcscat_s(GamePath, GameDir.c_str());
				wcscat_s(GamePath, L"\\");
				wcscat_s(GamePath, LauncherExe.c_str());
				wcscat_s(GamePath, L"\"");

				//SetCurrentDirectory(GameDir.c_str());

				//Exe Arguments Parse: Use the Custom Command Line Parser
				wcscpy_s(argv, GamePath);
			}
		}
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	//CommandLineW
	wcscat_s(argv, L" ");
	wcscat_s(argv, CommandLineW);

	//for (int i = 0; i < argCount; i++)
	//{
	//	wcscat_s(argv, L" ");
	//	wcscat_s(argv, L"\"");
	//	wcscat_s(argv, &szArgList[i]);
	//	wcscat_s(argv, L"\"");
	//}

	SetEnvironmentVariable(L"__COMPAT_LAYER", L"HighDpiAware");

	//Exe Arguments Parse: Add the Command Line Arguments 
	//to List controls.

	if (!CreateProcess(
		NULL,               // No module name (use command line)
		argv,               // Command line
		NULL,               // Process handle not inheritable
		NULL,               // Thread handle not inheritable
		FALSE,              // Set handle inheritance to FALSE
		0,                  // No creation flags
		NULL,               // Use parent's environment block
		GameDir.c_str(),    // Use parent's starting directory 
		&si,                // Pointer to STARTUPINFO structure
		&pi)			    // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 1;
	}

	CloseHandle(pi.hThread);

	//SetCurrentDirectory(currDir);

	Sleep(2000);

	//BlockInput(TRUE);

	if (!modeLauncher)
	{
		gHwnd = FindTargetWindow(pi.dwProcessId);
	}
	else
	{
		while (!gHwnd)
		{
			PROCESSENTRY32 entry;
			entry.dwSize = sizeof(PROCESSENTRY32);

			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

			if (Process32First(snapshot, &entry) == TRUE)
			{
				while (Process32Next(snapshot, &entry) == TRUE)
				{
					//convert wchar_t to const char*
					_bstr_t b(entry.szExeFile);
					const char* c = b;
					//convert wchar_t to const char*
					_bstr_t gec(GameExe.c_str());
					const char* GameExeChar = gec;
					if (stricmp(c, GameExeChar) == 0)
					{
						gHwnd = find_main_window(entry.th32ProcessID);
						hProcess_Game = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
					}
				}
			}
		}
	}


	if (gHwnd != NULL)
	{

		if (!modeLauncher)
		{
			SetFocus(gHwnd);
			SetForegroundWindow(gHwnd);
			SetActiveWindow(gHwnd);
			PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
			appHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pi.dwProcessId);
		}
		else
		{
			SetFocus(gHwnd);
			SetForegroundWindow(gHwnd);
			SetActiveWindow(gHwnd);
			PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
			appHandle = hProcess_Game;
		}

		GetClientRect(gHwnd, &gsr);
		GetWindowRect(gHwnd, &gsr_win);
		lefttop = { gsr.left, gsr.top }; // Practicaly both are 0
		ClientToScreen(gHwnd, &lefttop);
		rightbottom = { gsr.right, gsr.bottom };
		ClientToScreen(gHwnd, &rightbottom);
		// Limit mouse to client area on game window
		mouse_area.top = lefttop.y;// +(lefttop.y - gsr_win.top);
		mouse_area.bottom = rightbottom.y;// -(gsr_win.bottom - rightbottom.y);
		mouse_area.left = lefttop.x;
		mouse_area.right = rightbottom.x;


		InitWindow(m_hInstance, d3dHwnd, gHwnd);

		// Create the input object.  This object will be used to handle reading the keyboard input from the user.
		kb_hhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, 0, 0);
		//m_Input = new OmniScalerInput;
		//if (!m_Input)
		//{
		//	return false;
		//}
		//// Initialize the input object.
		//m_Input->Initialize();

		//custom mouse pointer
		//mouseCursorBmpData array buffer size based on texel dimensions
		gHwnd_winwidth = (float)mouse_area.right - (float)mouse_area.left;
		gHwnd_winheight = (float)mouse_area.bottom - (float)mouse_area.top;
		gScreenWidth = gsr.right;
		gScreenHeight = gsr.bottom;

		mcursor_wq = mcursor_w * 4;
		mcursor_hq = mcursor_h * 4;
		int mouseCursorBmpData_size = (mcursor_wq) * (mcursor_hq);
		mouseCursorBmpData = new int[mouseCursorBmpData_size];
		memset(mouseCursorBmpData, 0, sizeof(int) * mcursor_wq * mcursor_hq);
		// u, v of mouse pointer
		mcursor_u = (float)gScreenWidth / (float)mcursor_w;
		mcursor_v = (float)gScreenHeight / (float)mcursor_h;
		//scale mouse co-ordinates from game res to screen res
		rScreenWnd = ((gHwnd_winwidth) / gHwnd_winheight);// / ((float)gRealScreenWidth / (float)gRealScreenHeight);
		rScreenReal = ((float)gRealScreenWidth / (float)gRealScreenHeight);// / ((gHwnd_winwidth) / gHwnd_winheight);
		rScreenWndScale = ((float)gRealScreenHeight / (float)gScreenHeight);
		//mousePointer_size = mcursor_w*mousePosRemap_xscale;
		////Fill mouseCursorBmpData array with bitmap data
		//for (int x = 0; x < mousePointer_size; x++) {
		//	for (int y = 0; y < mousePointer_size; y++) {
		//		mouseCursorBmpData[x + y * mousePointer_size] = (0) | (255 << 8) | (0 << 16) | (1 << 24); //R | G | B | A
		//	}
		//}
		getMouseCursorBitmap(mouseCursorBmpData);

		if (!MousePointerBig)
		{
			mousePosRemap_xscale = 1.0f;
			mousePosRemap_yscale = 1.0f;
			mousePosRemap_xoffset = floor((float)gRealScreenWidth / (float)gScreenWidth);
			mousePosRemap_yoffset = floor((float)gRealScreenHeight / (float)gScreenHeight);
		}
		else
		{
			if (((float)gRealScreenWidth / (float)gScreenWidth) > 2.1)
			{
				mousePosRemap_xscale = ceil(((float)gRealScreenWidth / (float)gScreenWidth) / 2.0f);
				mousePosRemap_yscale = ceil(((float)gRealScreenHeight / (float)gScreenHeight) / 2.0f);
			}
			else
			{
				mousePosRemap_xscale = ceil(((float)gRealScreenWidth / (float)gScreenWidth));
				mousePosRemap_yscale = ceil(((float)gRealScreenHeight / (float)gScreenHeight));
			}
			mousePosRemap_xoffset = 1.0f;
			mousePosRemap_yoffset = 1.0f;
		}
		// size based on scaling mouse pointer 
		mouseCursorSize = ((float)(mousePointer_size) * (mousePosRemap_xscale));
		// scaled width value of window
		gScreenDynWidth = rScreenWnd * (float)gRealScreenHeight;
		// x-axis offset float value for centering window if aspect ratio is narrower than monitor
		rDynOffset = (gScreenDynWidth / (float)gRealScreenWidth) * -1.0f;
		// init integerscale val
		initIntegerScale = 1.0f;
		if (integerScaling)
		{
			initIntegerScale = floor((float)gRealScreenHeight / (float)gScreenHeight) * integerScalingOverride;
			if (IntegerScalingOverscan)
				initIntegerScale = ceil((float)gRealScreenHeight / (float)gScreenHeight) * integerScalingOverride;
			gScreenDynWidth = (float)gScreenWidth * initIntegerScale;
			gScreenDynHeight = (float)gScreenHeight * initIntegerScale;
			rDynOffset = (gScreenDynWidth / (float)gRealScreenWidth) * -1.0f;
			rDynYOffset = (gScreenDynHeight / (float)gRealScreenHeight);
		}


		// Create the graphics object.  This object will handle rendering all the graphics for this application.
		m_Graphics = new OmniScalerGraphics;
		if (!m_Graphics)
		{
			return false;
		}

		// Initialize the graphics object.
		result = m_Graphics->Initialize(d3dHwnd, gHwnd, appHandle, gRealScreenWidth, gRealScreenHeight, gRealRefreshRate, gScreenWidth, gScreenHeight, topOffset, bottomOffset, leftOffset, rightOffset, linearInterpolation, MousePointerLinear, integerScaling, integerScalingOverride, IntegerScalingOverscan, mouseCursorBmpData, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size, mouse_area, MousePointerEnable);
		if (!result)
		{
			return false;
		}
		SetActiveWindow(gHwnd);
		if (!d3dHwnd)
		{
			return false;
		}

		// set framerate delay
		//frameDelay = 1000 / (gRealRefreshRate * 1.5);
		//if (gRealRefreshRate > 119)
		//{
			frameDelay = 1000 / (gRealRefreshRate * 1.5);
		//}

		if (modeLauncher)
		{
			/*
			Workaround for Teknoparrot
			*/
			_bstr_t geTest(LauncherExe.c_str());
			const char* GameExeTest = geTest;
			if (stricmp("TeknoParrotUi.exe", GameExeTest) == 0)
			{
				PROCESSENTRY32 TeknoParrotEntry;
				TeknoParrotEntry.dwSize = sizeof(PROCESSENTRY32);

				HANDLE TeknoParrotSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

				if (Process32First(TeknoParrotSnapshot, &TeknoParrotEntry) == TRUE)
				{
					while (Process32Next(TeknoParrotSnapshot, &TeknoParrotEntry) == TRUE)
					{
						//convert wchar_t to const char*
						_bstr_t b(TeknoParrotEntry.szExeFile);
						const char* c = b;
						//convert wchar_t to const char*
						_bstr_t gec(GameExe.c_str());
						const char* GameExeChar = gec;
						if (stricmp(c, "OpenParrotLoader.exe") == 0)
						{
							HWND plHwnd = find_main_window(TeknoParrotEntry.th32ProcessID);
							PostMessage(gHwnd, WM_ACTIVATE, 0, 0);
							Sleep(100);
							PostMessage(plHwnd, WM_ACTIVATE, 1, 0);
							Sleep(100);
							PostMessage(plHwnd, WM_ACTIVATE, 0, 0);
							Sleep(100);
							PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
						}
					}
				}
			}
			/*
			END of Workaround for Teknoparrot
			*/
			else
			{
				PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
			}
		}


	}

	return true;
}

void OmniScalerSystem::MainLoop()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));


	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{

		// Framerate start
		frameStart = GetTickCount();

		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = UpdateScreen();
			if (!result)
			{
				done = true;
			}
		}

		// Framerate delay
		frameTime = GetTickCount() - frameStart;

		if (frameDelay > frameTime)
		{
			Sleep(frameDelay - frameTime);
		}


	}

	return;
}

bool OmniScalerSystem::UpdateScreen()
{
	bool result;


	//// Check if the user pressed escape and wants to exit the application.
	//if (m_Input->IsKeyDown(VK_ESCAPE))
	//{
	//	return false;
	//}

	// Check if game has closed
	// exit if so
	//if game exited by menu and not alt-f4, handle shutdown.
	if (!gHandleKbExit)
	{
		if(!modeLauncher)
			GetExitCodeProcess(pi.hProcess, &lpExitCode);
		else
			GetExitCodeProcess(hProcess_Game, &lpExitCode);

		//TCHAR szBuffer[16] = { 0 };
		//wsprintf(szBuffer, TEXT("%lu"), lpExitCode);
		//MessageBox(0, szBuffer, L"d3dHwnd alpha value", MB_OK);
		if (lpExitCode != STILL_ACTIVE)
		{
			ClipCursor(NULL);
			ShowCursor(1);
			// Close process and thread handles. 
			if (!modeLauncher)
				CloseHandle(pi.hProcess);
			else
				CloseHandle(hProcess_Game);
			//UnhookWindowsHookEx(kb_hhook);
			PostMessage(d3dHwnd, WM_CLOSE, 0, 0);
			return false;
		}
	}


	/* Check if window was resized */
	gWndTick++;
	if (gWndTick > (83))
	{
		RECT gsr;
		POINT rightbottom;
		GetClientRect(gHwnd, &gsr);
		rightbottom = { gsr.right, gsr.bottom };
		if (gScreenWidth != rightbottom.x || gScreenHeight != (rightbottom.y - (int)topOffset - (int)bottomOffset))
		{
			gScreenResized = TRUE;
			//TCHAR szBuffer[16] = { 0 };
			//wsprintf(szBuffer, TEXT("%lu"), (int)gScreenResized);
			//MessageBox(0, szBuffer, L"d3dHwnd alpha value", MB_OK);
		}
		gWndTick = 0;
	}


	GetCursorPos(&mousePos);
	ScreenToClient(gHwnd, &mousePos);

	// Client coordinates to screen coordinates
	RECT gsr;
	GetClientRect(gHwnd, &gsr);
	float mouseX = mousePos.x;// *mousePosRemap_xscale;
	float mouseY = mousePos.y;// *mousePosRemap_yscale;

	/* re-init scaling if window resized */
	if (gScreenResized)
	{
		m_Graphics->Initialize(d3dHwnd, gHwnd, appHandle, gRealScreenWidth, gRealScreenHeight, gRealRefreshRate, gScreenWidth, gScreenHeight, topOffset, bottomOffset, leftOffset, rightOffset, linearInterpolation, MousePointerLinear, integerScaling, integerScalingOverride, IntegerScalingOverscan, mouseCursorBmpData, mousePosRemap_xscale, mousePosRemap_yscale, mousePointer_size, mouse_area, MousePointerEnable);
		gScreenResized = FALSE;
		if (modeLauncher)
		{
			HWND prevHwnd = GetNextWindow(gHwnd, GW_HWNDNEXT);
			SetFocus(prevHwnd);
			SetForegroundWindow(prevHwnd);
			SetActiveWindow(prevHwnd);
			Sleep(50);
			SetFocus(gHwnd);
			SetForegroundWindow(gHwnd);
			SetActiveWindow(gHwnd);
		}
		else
		{
			SetFocus(gHwnd);
			SetForegroundWindow(gHwnd);
			SetActiveWindow(gHwnd);
			PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
		}
	}

	if (integerScaling)
	{
		// aspect offset for centering + mouse cursor + (mouse position multiplied by integer scale and 2) / window width
		mouseX = rDynOffset + (mouseCursorSize / (float)gRealScreenWidth) + ((((float)(mousePos.x) * (2.0f)) * (initIntegerScale)) / (float)gRealScreenWidth);
		mouseY = rDynYOffset - (mouseCursorSize / (float)gRealScreenHeight) - ((((float)(mousePos.y) * (2.0f)) * (initIntegerScale)) / (float)gRealScreenHeight);
	}
	else
	{
		// Calculate the X and Y pixel position on the screen to start drawing to.
		if (rScreenWnd < rScreenReal)
		{
			// aspect offset for centering + mouse cursor + (mouse position multiplied by monitor to window scale ratio and 2) / window width
			mouseX = rDynOffset + (mouseCursorSize / (float)gRealScreenWidth) + (((float)(mousePos.x * (2.0f)) * rScreenWndScale) / (float)gRealScreenWidth);
			mouseY = 1.0f - (mouseCursorSize / (float)gRealScreenHeight) - (((float)(mousePos.y) * (2.0f)) / (float)gScreenHeight);
		}
		else
		{
			mouseX = -1.0f + (mouseCursorSize / (float)gRealScreenWidth) + (((float)(mousePos.x) * (2.0f)) / (float)gScreenWidth);
			mouseY = 1.0f - (mouseCursorSize / (float)gRealScreenHeight) - (((float)(mousePos.y) * (2.0f)) / (float)gScreenHeight);
		}
	}

	// Do the frame processing for the graphics object.
	result = m_Graphics->UpdateScreen(
		integerScaling,
		linearInterpolation,
		mouseX,
		mouseY,
		mousePosRemap_xscale,
		mousePosRemap_yscale,
		gsr,
		gHwnd,
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

LRESULT CALLBACK OmniScalerSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	//	// Check if a key has been pressed on the keyboard.
	//case WM_KEYDOWN:
	//{
	//	// If a key is pressed send it to the input object so it can record that state.
	//	m_Input->KeyDown((unsigned int)wparam);
	//	return 0;
	//}

	//// Check if a key has been released on the keyboard.
	//case WM_KEYUP:
	//{
	//	// If a key is released then send it to the input object so it can unset the state for that key.
	//	m_Input->KeyUp((unsigned int)wparam);
	//	return 0;
	//}

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void OmniScalerSystem::InitWindow(HINSTANCE d_hInstance, HWND& d3dHwnd, HWND gHwnd)
{
	//LONG hInstanceL = GetWindowLongA(
	//	NULL, GWL_HINSTANCE);
	//HINSTANCE hInstance = (HINSTANCE)hInstanceL;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Give the application a name.
	m_applicationName = L"OMNISCALER";

	//LONG hInstanceL = GetWindowLongA(
	//	NULL, GWL_HINSTANCE);
	//m_hinstance = GetModuleHandle(NULL);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = oscWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = d_hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = CreateSolidBrush(0x00000000);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = *g_szClassName;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	// next up: work area for primary display
	SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

	// best: full screen
	MONITORINFO mon;
	mon.cbSize = sizeof(mon);

	if (GetMonitorInfo(MonitorFromWindow(gHwnd, MONITOR_DEFAULTTOPRIMARY), &mon))
	{
		r = mon.rcMonitor;
	}

	gRealScreenWidth = r.right;
	gRealScreenHeight = r.bottom;

	gAllowResize = 1;

	d3dHwnd = CreateWindowEx(
		WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
		*g_szClassName,
		m_applicationName,
		WS_POPUP | WS_VISIBLE | WS_SYSMENU,
		//NULL,
		0,
		0,
		gRealScreenWidth,
		gRealScreenHeight,
		gHwnd,
		NULL,
		d_hInstance,
		NULL);
	//keep layered window opaque
	SetLayeredWindowAttributes(d3dHwnd, 0, 255, LWA_ALPHA);
	SetWindowPos(d3dHwnd, HWND_NOTOPMOST, 0, 0, 0,
		0, WS_EX_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);



	UpdateWindow(d3dHwnd);
	ShowWindow(d3dHwnd, SW_SHOW);
	SetForegroundWindow(gHwnd);
	SetActiveWindow(gHwnd);

	ClipCursor(&mouse_area);
	ShowCursor(0);

	return;
}


void toggleFullscreen()
{
	if (isWindowed)
	{
		//ShowWindow(d3dHwnd, SW_MINIMIZE);
		isWindowed = bool(1);
		isFullscreen = bool(0);
		//isMinimised = bool(0);
		SetLayeredWindowAttributes(d3dHwnd, 0, 0, LWA_ALPHA);
		++fullscr_delay;
		HWND desktopHwnd = GetDesktopWindow();
		SetFocus(desktopHwnd);
		SetForegroundWindow(desktopHwnd);
		SetActiveWindow(desktopHwnd);
		Sleep(50);
		SetFocus(gHwnd);
		SetForegroundWindow(gHwnd);
		SetActiveWindow(gHwnd);
		ClipCursor(NULL);
		ShowCursor(1);
		return;
	}
	{
		//ShowWindow(d3dHwnd, SW_RESTORE);
		isWindowed = bool(0);
		isFullscreen = bool(1);
		//isMinimised = bool(0);
		SetLayeredWindowAttributes(d3dHwnd, 0, 255, LWA_ALPHA);
		fullscr_delay = 0;
		//make sure d3d window is at top
		SetWindowPos(d3dHwnd, NULL, 0, 0, 0,
			0, WS_EX_NOACTIVATE | WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);

		//SetWindowPos(d3dHwnd, HWND_NOTOPMOST, 0, 0, 0,
		//	0, SWP_NOMOVE | SWP_NOSIZE);
		SetActiveWindow(gHwnd);
		//Sleep(50);
		//make game window active
		//SetFocus(d3dHwnd);
		//SetForegroundWindow(d3dHwnd);
		//SetActiveWindow(d3dHwnd);

		ClipCursor(&mouse_area);
		ShowCursor(0);
		return;
	}
}

LRESULT CALLBACK oscWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int tick = GetTickCount();
	HWND ActiveHwnd;

	if (gLastUpdate == -1)
	{
		gLastUpdate = tick;
	}

	if ((tick - gLastUpdate) > (30) && reshadeinit == 1)
	{
		BYTE hwndAlpa;
		GetLayeredWindowAttributes(hwnd, NULL, &hwndAlpa, NULL);
		ActiveHwnd = GetActiveWindow();

		//TCHAR szBuffer[16] = { 0 };
		//wsprintf(szBuffer, TEXT("%lu"), (int)hwndAlpa);
		//MessageBox(0, szBuffer, L"d3dHwnd alpha value", MB_OK);

		if ((int)hwndAlpa == 255 && !isFullscreen && isWindowed && !isMinimised)
		{
			//PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
			ClipCursor(NULL);
			ShowCursor(1);
			isactive = 1;
		}
		else
			//minimise ogl view when game minimised, restore when game active in view, hwnd is d3dHwnd
			if (ActiveHwnd == gHwnd && (int)hwndAlpa == 0 && isFullscreen && !isWindowed )
			{
				//PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
				ClipCursor(&mouse_area);
				ShowCursor(0);
				isactive = 1;
				SetActiveWindow(gHwnd);
			}
			else
				if ((int)hwndAlpa == 255 && ActiveHwnd != gHwnd && isFullscreen && !isWindowed)
				{
					//PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
					SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
					ClipCursor(NULL);
					ShowCursor(1);
					isactive = 0;
				}
				else
					if ((int)hwndAlpa == 255)
					{
						ClipCursor(&mouse_area);
						//ShowCursor(0);
						SetActiveWindow(gHwnd);
					}
					else
						if (ActiveHwnd != gHwnd && isFullscreen)
						{
							{
								SetFocus(gHwnd);
								SetForegroundWindow(gHwnd);
								SetActiveWindow(gHwnd);
								PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
							}
						}
	}

	//Pause processing so that reshade does not crash
	if (reshadeinit == -1)
	{
		BlockInput(FALSE);
		//ShowCursor(1);
		{
			//make d3d window topmost and unable to take focus
			SetWindowPos(hwnd, NULL, 0, 0, 0,
				0, WS_EX_NOACTIVATE | WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);
			reshadeinit = 1;
			//make game window active
			SetFocus(gHwnd);
			SetForegroundWindow(gHwnd);
			SetActiveWindow(gHwnd);
			PostMessage(gHwnd, WM_ACTIVATE, 1, 0);
		}
		//ClipCursor(&mouse_area);
		//ShowCursor(0);
	}

	switch (msg)
	{
	case WM_CREATE:
	{
		ttc = LoadCursor(NULL, IDC_ARROW);
		SetCursor(ttc);
	}
	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_MINIMIZED:
		{
			gIsActive = 0;
		}
		case SIZE_RESTORED:
		{
			gIsActive = 1;
		}
		}
		break;
	case WM_MOUSEMOVE:
	case WM_MOUSEACTIVATE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	{
		if (reshadeinit == 1)
		{
			//SetCapture(gHwnd);
			return MA_NOACTIVATE;
		}
	}
	//case WM_CLOSE:
	//{
	//	DestroyWindow(hwnd);
	//}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, msg, wParam, lParam);
	}
	}
	return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT LowKey = NULL;
	LRESULT RetVal;
	if (HC_ACTION == nCode)
	{
		HWND tabHwnd;
		LowKey = (PKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
		{
			if ((LowKey->vkCode == VK_TAB) && (GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0))
			{
				tabHwnd = GetActiveWindow();
				if (tabHwnd == gHwnd)
				{
					SetLayeredWindowAttributes(d3dHwnd, 0, 255, LWA_ALPHA);
					RetVal = CallNextHookEx(kb_hhook, nCode, wParam, lParam);
					ClipCursor(&mouse_area);
					ShowCursor(0);
					return RetVal;
				}
				//PostMessage(d3dHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				SetLayeredWindowAttributes(d3dHwnd, 0, 0, LWA_ALPHA);
				//PostMessage(gHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				RetVal = CallNextHookEx(kb_hhook, nCode, wParam, lParam);
				ClipCursor(NULL);
				ShowCursor(1);
				return RetVal;
			}
			/*
			EAT ALT+ENTER
			*/
			if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && isFullscreen && fullscr_delay == 0)
			{
				return 1;
			}
			else
				if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && !isFullscreen && fullscr_delay > 0)
				{
					return 1;
				}
				else
					if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && !isFullscreen && isWindowed)
					{
						return 1;
					}
			/*
			END OF EAT ALT+ENTER
			*/
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			/*
			EAT ALT+ENTER
			*/
			if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && isFullscreen && fullscr_delay == 0)
			{
				return 1;
			}
			else
				if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && !isFullscreen && fullscr_delay > 0)
				{
					return 1;
				}
				else
					if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && !isFullscreen && isWindowed)
					{
						return 1;
					}
			/*
			END OF EAT ALT+ENTER
			*/

		}
		case WM_SYSKEYDOWN:
		//case WM_KEYUP:
		//case WM_SYSKEYUP:
		{
			if ((LowKey->vkCode == VK_F4) && ((LowKey->flags & LLKHF_ALTDOWN) != 0))
			{
				gHandleKbExit = TRUE;
				ClipCursor(NULL);
				ShowCursor(1);
				UnhookWindowsHookEx(kb_hhook);
				PostMessage(gHwnd, WM_CLOSE, 0, 0);
				PostMessage(d3dHwnd, WM_CLOSE, 0, 0);
				return 1;
			}
			else
				if ((LowKey->vkCode == VK_TAB) && ((LowKey->flags & LLKHF_ALTDOWN) != 0))
				{

					tabHwnd = GetActiveWindow();
					if (tabHwnd != gHwnd)
					{
						//PostMessage(d3dHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
						//PostMessage(gHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
						SetLayeredWindowAttributes(d3dHwnd, 0, 0, LWA_ALPHA);
						isMinimised = bool(1);
						//RetVal = CallNextHookEx(kb_hhook, nCode, wParam, lParam);
						//ClipCursor(NULL); 
						//ShowCursor(1);
						//return  RetVal;
					}
				}
				else
					if ((LowKey->vkCode == VK_RETURN))
					{
						DWORD altPressed = LowKey->flags & LLKHF_ALTDOWN;
							if ( (altPressed != 0) && isFullscreen && fullscr_delay == 0)
						{
							isWindowed = bool(1);
							toggleFullscreen();
							return 1;
						}
						if ( (altPressed != 0) && !isFullscreen && fullscr_delay > 0)
						{
							isWindowed = bool(0);
							toggleFullscreen();
							return 1;
						}
						if ( (altPressed != 0) && !isFullscreen && isWindowed)
						{
							fullscr_delay++;
							return 1;
						}
					}
					//if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && isFullscreen && fullscr_delay == 0)
					//{
					//	isWindowed = bool(1);
					//	toggleFullscreen();
					//	return 1;
					//}
					//else
					//	if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && !isFullscreen && fullscr_delay > 0)
					//	{
					//		isWindowed = bool(0);
					//		toggleFullscreen();
					//		return 1;
					//	}
					//	else
					//		if ((LowKey->vkCode == VK_RETURN) && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && !isFullscreen && isWindowed)
					//		{
					//			fullscr_delay++;
					//			return 1;
					//		}
		}
		}
	}

	RetVal = CallNextHookEx(NULL, nCode, wParam, lParam);

	return  RetVal;

}

void OmniScalerSystem::ShutdownWindow()
{
	// Show the mouse cursor.
	ClipCursor(NULL);
	ShowCursor(1);


	// Remove the window.
	DestroyWindow(d3dHwnd);
	PostMessage(d3dHwnd, WM_CLOSE, 0, 0);
	d3dHwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}



HBITMAP getCursorHBITMAP(HBITMAP *maskBmp)
{
	CURSORINFO pci;
	ICONINFO iconinfo;
	HBITMAP result;

	pci.cbSize = sizeof(pci);
	GetCursorInfo(&pci);

	if (GetIconInfo(LoadCursor(NULL, IDC_ARROW), &iconinfo))
	{
		result = iconinfo.hbmColor;
		if (maskBmp)
			*maskBmp = iconinfo.hbmMask;
	}
	else
		result = NULL;

	return result;
}

void OmniScalerSystem::getMouseCursorBitmap(int* mouseCursorBmpData)
{
	HBITMAP oldBm, cursorBmp, maskBmp, dibbmp;

	cursorBmp = getCursorHBITMAP(&maskBmp);
	if (cursorBmp)
	{
		HDC hdc, mousePointerDC;
		HDC memDC;
		BITMAP bm;

		//mousePointerDC = CreateCompatibleDC(NULL);
		hdc = CreateCompatibleDC(NULL);
		memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, cursorBmp);
		GetObject(cursorBmp, sizeof(bm), &bm);

		mcursor_w = bm.bmWidth;
		mcursor_h = bm.bmHeight;

		HBITMAP pointerbmp = CreateCompatibleBitmap(memDC, mcursor_w, mcursor_h);
		SelectObject(hdc, pointerbmp);

		BITMAPINFO bitmap_info;
		memset(&bitmap_info, 0, sizeof(bitmap_info));
		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = bm.bmWidth;
		bitmap_info.bmiHeader.biHeight = -bm.bmHeight;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;
		bitmap_info.bmiHeader.biSizeImage = mcursor_w * mcursor_h * 4;

		//mouseCursorBmpData = malloc(bitmap_info.bmiHeader.biSizeImage);

		//BitBlt(hdc, 0, 0, mcursor_w, mcursor_h, memDC, 0, 0, SRCCOPY);
		//SelectObject(memDC, oldBm);

		MaskBlt(hdc, 0, 0, mcursor_w, mcursor_h, memDC, 0, 0, maskBmp, 0, 0, MAKEROP4(SRCPAINT, SRCCOPY));
		////flip horizontal
		//StretchBlt(hdc, mcursor_w - 1, 0, -mcursor_w, mcursor_h, hdc, 0, 0, mcursor_w, mcursor_h, SRCCOPY);

		GetDIBits(hdc, pointerbmp, 0, mcursor_h, mouseCursorBmpData, &bitmap_info, DIB_RGB_COLORS);
		//GetBitmapBits(tempbitmap, mcursor_w * mcursor_h * 4, mouseCursorBmpData);

		DeleteDC(hdc);
		DeleteDC(memDC);
		DeleteObject(pointerbmp);
	}
}

// Similar to WinWaitActive on AHK. Public Member function
HWND OmniScalerSystem::FindTargetWindow(unsigned long process_id)
{
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(static_enum_windows_callback, reinterpret_cast<LPARAM>(this));

	return data.window_handle;
}


// Similar to WinWaitActive on AHK. Public Member function
HWND OmniScalerSystem::find_main_window(unsigned long process_id)
{
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(static_enum_windows_callback, reinterpret_cast<LPARAM>(this));

	return data.window_handle;
}


//Exe Arguments Parse: The command Line parser will do the copy
void OmniScalerSystem::GetCommandLineParser(
	CStringArray& params,
	CStringArray& switches)
{
	m_cmdParse.GetParams(params);
}


// Similar to WinWaitActive on AHK. Static Private function
BOOL OmniScalerSystem::static_enum_windows_callback(HWND handle, LPARAM lParam)
{
	LONG lStyle = GetWindowLongPtrW(handle, GWL_STYLE);
	if ((lStyle & WS_VISIBLE) && (lStyle & WS_SYSMENU))
	{
		OmniScalerSystem *pThis = reinterpret_cast<OmniScalerSystem *>(lParam);
		unsigned long process_id = 0;
		INT nCloaked;
		GetWindowThreadProcessId(handle, &process_id);
		// On Windows 10, ApplicationFrameWindow may run in the background and
		// WS_VISIBLE will be true even if the window isn't actually visible,
		// for various UWP apps. I don't know of any method for predicting when 
		// this will happen, and for which app(s).
		// 
		// The only way to test if a window is actually *visible* in this case
		// is to test for the DWM CLOAKED attribute.
		//DwmGetWindowAttribute(handle, DWMWA_CLOAKED, &nCloaked, sizeof(INT));
		//if (pThis->data.process_id != process_id || !nCloaked)
		if (pThis->data.process_id != process_id)
			return TRUE;
		pThis->data.window_handle = handle;
		return FALSE;
	}
	return TRUE;
}

void OmniScalerSystem::Shutdown()
{
	return;
}