#include "OmniScalerSystem.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	OmniScalerSystem* System;
	bool result;



	// Create the system object.
	System = new OmniScalerSystem;
	if (!System)
	{
		return 0;
	}
	LPWSTR *szArgList;
	int argCount;

	wchar_t *CommandLineW;
	int len = MultiByteToWideChar(CP_UTF8, 0, pScmdline, strlen(pScmdline), NULL, 0);
	CommandLineW = new wchar_t[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, pScmdline, strlen(pScmdline), CommandLineW, len);
	CommandLineW[len] = '\0';


	szArgList = CommandLineToArgvW(CommandLineW, &argCount);


	// Initialize and run the system object.
	result = System->InitScaler(hInstance, argCount, *szArgList, CommandLineW);
	if (result)
	{
		System->MainLoop();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}

