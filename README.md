

# OmniScaler
Zero Latency DirectX 11 scaling utility.

OmniScaler takes a windowed game or application, displays its window contents to a fullscreen view via Directx 11. 

**Advantages**
 - Zero Latency. Rather than a memory copy or video stream of the window's contents, OmniScaler directly reads from the graphical output generated by windows.
 - True fullscreen. This is true exclusive mode fullscreen, not a window stretched out to fake a fullscreen mode. Negates latency from vsync MS Windows forces on windowed applications.
 - Directx 11. Reshade, SweetFX, Rivatuner, all work now.
 - Supports Windows 7, 8, 10


This application requires Aero to be enabled in Windows.


**Tips:**
- OmniScaler accepts command line arguments. Simply create a shortcut to OmniScaler.exe and add command line arguments in the shortcut propreties.
- If a game gives you a black screen it could be that the game internally tries to disable aero or set a compatibility mode of Windows XP or older. A quick workaround is to set the compatibility mode of both the game and OmniScaler.exe to Windows 8.

This initial alpha release has no frontend, therefore certain requirements need to be met by you before using OmniScaler:
 1. The game/application you want to scale up MUST be windowed. Before beginning anything else make sure the application you want to try OmniScaler with starts up windowed
 2. Extract contents of zip into its own directory. I suggest C:\OmniScaler but anywhere is fine
 3. Once files have been extracted, open gamefs.ini in a text editor
 4. Type in the directory of the game/application you want to run in the GameDir entry.
 
	  For example if my game is C:\Program Files\Microsoft Games\Minesweeper\MineSweeper.exe
	  
	  then, 
	  
	  GameDir=C:\Program Files\Microsoft Games\Minesweeper
	  
 5. Type in the exe file name of the game/application you want to run in the GameExe entry.
	  
	  As example above,
	  
	  GameExe=MineSweeper.exe
	  
 6. Save gamefs.ini
 7. Launch OmniScaler.exe , your game/application will launch fullscreen 

**OPTIONAL:** If after running your game or application you have discovered that there are borders or menu items showing at the corners of the screen, edit these entries in gamefs.ini to crop them out:

offset_top=

offset_bottom=

offset_left=

offset_right=

The values to enter are pixels.

This is a test release. Possible features in the future:
 - Frontend UI
 - Global hotkey
 - Integer scaling, integer overscan scaling
 - CRT or 2D Shaders
 - Borderless windowed mode
