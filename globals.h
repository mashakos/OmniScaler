#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//global variables
extern int gAllowResize = 0;
extern int gIsActive = 0;
extern int isactive = 0;
extern int gModeWidth = 824;
extern int gScreenWidth = 824;
extern int gScreenHeight = 618;
extern int gRealScreenWidth;
extern int gRealScreenHeight;
extern int gScreenBits;
extern WNDPROC origfunc = NULL;
extern int gLastUpdate = -1;
extern int gSmooth = 0;
extern int gAltWinPos = 0;
extern int gGDI = 0;
extern STARTUPINFO si;
extern PROCESS_INFORMATION pi;
extern HWND gHwnd = NULL;
extern TCHAR appClassName[MAX_PATH];
extern WNDCLASSEX wc;
extern HWND d3dHwnd;
extern HANDLE appHandle;
extern int tex_wq = 5274;
extern int tex_hq = 2637;
extern size_t bitmap_buffer_size = tex_wq * tex_hq;
extern BYTE* lpPixels = new BYTE[bitmap_buffer_size];
extern BYTE *bitmap_buffer;
extern UINT_PTR IDT_TIMER1;
extern DWORD lpExitCode = 0;
extern DWORD currExitCode = 0;
extern HHOOK kb_hhook = NULL;
extern int glinitvar = 0;
extern BOOL gHandleKbExit = FALSE;

//RESHADE INIT
static int reshadeinit = -1;
extern WORD reshadeInitKey = VK_HOME;
extern int reshadeInitDur = 1000;


//MOUSE AND MOUSE LIMIT
extern RECT r;
extern HCURSOR ttc;

extern int fullscr_delay = 0;
extern int alttab_delay = 0;
extern BOOL isMinimised = FALSE;

//INI VALS
extern BOOL integerScaling = FALSE;
extern BOOL IntegerScalingOverscan = FALSE;
extern BOOL linearInterpolation = FALSE;
extern BOOL isFullscreen = TRUE;
extern BOOL isWindowed = FALSE;
extern BOOL isStretched = FALSE;
extern wchar_t argv[10000];
//INI VALS
