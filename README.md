
# OmniScaler
Zero Latency DirectX 11 scaling utility

OmniScaler takes a windowed game or application, streams its window contents to a fullscreen view via Directx 11. This initial alpha release has no frontend, therefore certain requirements need to be met by you before using OmniScaler:

 1. The game/application you want to scale up MUST be fullscreen. Before beginning anything else make sure the application you want to try OmniScalre with starts up windowed
 2. Extract contents of zip into its own directory. I suggest C:\OmniScaler but anywhere is fine
 3. Once files have been extracted, pen gamefs.ini in a text editor
 4. Type in the directory of the game/application you want to run in GameDir.
	  For example if my game is C:\Program Files\Microsoft Games\Minesweeper\MineSweeper.exe
	  then, 
	  GameDir=C:\Program Files\Microsoft Games\Minesweeper
 5. Type in the exe file name of the game/application you want to run in GameExe
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
