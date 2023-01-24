/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   */

#include "deu.h"
#include <mousex.h>

#ifndef SLIM
void DisplayHotKeys(BCINT EditMode) 
{
	BCINT x0, y0 = 0;
	BCINT height = 0;
	BCINT width = 0;
	HideMousePointer();
    if (EditMode == OBJ_THINGS) {
#ifdef GAME_HEXEN
		height = 13 * 10 + 70;
		width = 360;
#else
		height = 9 * 10 + 70;
		width = 300;
#endif
		x0 = (ScrMaxX - width) / 2;
		y0 = (ScrMaxY - height) / 2;
		DrawScreenWindow( x0, y0, width, height, " Help: Special Keys for Thing Mode", GREEN);
		DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
		DrawScreenText( x0 + 14, y0 + 24, " ");
		SetColor(BLACK);
#ifdef GAME_HEXEN
		DrawScreenText( -1, -1, "CTRL+B - Toggle Blind/Dormant Flag");
		DrawScreenText( -1, -1, "CTRL+D - Toggle Deaf Flag");
		DrawScreenText( -1, -1, "CTRL+F - Change Flags");
		DrawScreenText( -1, -1, "CTRL+G - Reset all ArGuments to 0");
		DrawScreenText( -1, -1, "CTRL+T - Change Type");
		DrawScreenText( -1, -1, ".      - Rotate Clockwise");
		DrawScreenText( -1, -1, ",      - Rotate Anti-Clockwise");
		DrawScreenText( -1, -1, ">      - Increase Height By 16");
		DrawScreenText( -1, -1, "<      - Decrease Height By 16");
		DrawScreenText( -1, -1, "B      - Jump to TID");
#else
		DrawScreenText( -1, -1, "CTRL+D - Toggle Deaf Flag");
		DrawScreenText( -1, -1, "CTRL+F - Change Flags");
		DrawScreenText( -1, -1, "CTRL+M - Toggle Multiplayer Flag");
		DrawScreenText( -1, -1, "CTRL+T - Change Type");
		DrawScreenText( -1, -1, ".      - Rotate Clockwise");
		DrawScreenText( -1, -1, ",      - Rotate Anti-Clockwise");
#endif
		DrawScreenText( -1, -1, "F      - Find/Mark/Filter Menu");
		DrawScreenText( -1, -1, "A      - Repeat Last Find");
		DrawScreenText( -1, -1, "F4     - Toggle Graphical Display");
	}
    if (EditMode == OBJ_SECTORS) {
		width = 420;
		height = 9 * 10 + 70;
		x0 = (ScrMaxX - width) / 2;
		y0 = (ScrMaxY - height) / 2;
		DrawScreenWindow( x0, y0, width, height, " Help: Special Keys for Sector Mode", GREEN);
		DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
		DrawScreenText( x0 + 14, y0 + 24, " ");
		SetColor(BLACK);
		DrawScreenText( -1, -1, "CTRL+C - Change Ceiling Height");
		DrawScreenText( -1, -1, "CTRL+F - Change Floor Height");
		DrawScreenText( -1, -1, "CTRL+G - Set LineDef TaG");
		DrawScreenText( -1, -1, "CTRL+N - Set Type to Normal & LineDef Tag to 0");
		DrawScreenText( -1, -1, "CTRL+T - Change Type");
		DrawScreenText( -1, -1, ".      - Increase Floor Height by %d", Increment);
		DrawScreenText( -1, -1, ",      - Decrease Floor Height by %d", Increment);
		DrawScreenText( -1, -1, ">      - Increase Ceiling Height by %d", Increment);
		DrawScreenText( -1, -1, "<      - Decrease Ceiling Height by %d", Increment);
	}
    if (EditMode == OBJ_VERTEXES) {
		width = 420;
		height = 5 * 10 + 70;
		x0 = (ScrMaxX - width) / 2;
		y0 = (ScrMaxY - height) / 2;
		DrawScreenWindow( x0, y0, width, height, " Help: Special Keys for Vertex Mode", GREEN);
		DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
		DrawScreenText( x0 + 14, y0 + 24, " ");
		SetColor(BLACK);
		DrawScreenText( -1, -1, "CTRL+A - Add LineDef & Add Sector");
		DrawScreenText( -1, -1, "CTRL+D - Delete Vertex & Join LineDefs");
		DrawScreenText( -1, -1, "CTRL+J - Delete Vertex & Join LineDefs");
		DrawScreenText( -1, -1, "CTRL+M - Merge Several Vertexes & Join LineDefs");
		DrawScreenText( -1, -1, "F4     - Toggle Dimension Display");
	}
    if (EditMode == OBJ_LINEDEFS) {
		width = 420;
#ifdef GAME_HEXEN
		height = 25 * 10 + 70 ;
#else
		height = 24 * 10 + 70 ;
#endif
		x0 = (ScrMaxX - width) / 2;
		y0 = (ScrMaxY - height) / 2;
		DrawScreenWindow( x0, y0, width, height, " Help: Special Keys for LineDef Mode", GREEN);
		DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
		DrawScreenText( x0 + 14, y0 + 24, " ");
		SetColor(BLACK);
		DrawScreenText( -1, -1, "CTRL+A - Split Selected LineDefs & Add Sector");
		DrawScreenText( -1, -1, "CTRL+B - Toggle Block Sounds Flag");
		DrawScreenText( -1, -1, "CTRL+C - Change Flags");
		DrawScreenText( -1, -1, "CTRL+D - Delete Selected LineDefs & Join Sectors");
#ifdef GAME_HEXEN
		DrawScreenText( -1, -1, "CTRL+E - Change Trigger");
#endif
		DrawScreenText( -1, -1, "CTRL+F - Flip LineDefs");
#ifdef GAME_HEXEN
		DrawScreenText( -1, -1, "CTRL+G - Reset all ArGuments to 0");
#else
		DrawScreenText( -1, -1, "CTRL+G - Change Sector taG");
#endif
		DrawScreenText( -1, -1, "CTRL+H - Toggle Hidden On Map Flag");
		DrawScreenText( -1, -1, "CTRL+J - Delete Selected LineDefs & Join Sectors");
		DrawScreenText( -1, -1, "CTRL+L - Toggle Lower Texture Unpegged Flag");
		DrawScreenText( -1, -1, "CTRL+M - Toggle iMpassable Flag");
		DrawScreenText( -1, -1, "CTRL+N - Set Type To Normal & Sector Tag To 0");
		DrawScreenText( -1, -1, "CTRL+O - Toggle Already On Map Flag");
		DrawScreenText( -1, -1, "CTRL+P - Toggle Monsters Can't Pass Flag");
		DrawScreenText( -1, -1, "CTRL+R - Toggle secRet Flag");
		DrawScreenText( -1, -1, "CTRL+S - Swap SideDefs");
		DrawScreenText( -1, -1, "CTRL+T - Change LineDef Type");
		DrawScreenText( -1, -1, "CTRL+U - Toggle Upper Texture Unpegged Flag");
		DrawScreenText( -1, -1, "CTRL+V - Split LineDefs & Add Vertexes");
		DrawScreenText( -1, -1, "CTRL+X - Align Textures On X Without Checking");
		DrawScreenText( -1, -1, "X      - Align Textures On X With Checking");
		DrawScreenText( -1, -1, "CTRL+Y - Align Textures On Y Without Checking");
		DrawScreenText( -1, -1, "Y      - Align Textures On Y With Checking");
		DrawScreenText( -1, -1, "CTRL+2 - Toggle 2 Sided Flag");
		DrawScreenText( -1, -1, "F4     - Toggle Doom Style Auto Map Display");
	}
	SetColor(YELLOW);
	DrawScreenText( -1, y0 + height - 20, "Press any key to return...");
	bioskey(0);
    if (UseMouse)
		ShowMousePointer();
}

/*
   display the help screen
   */

void DisplayHelp( BCINT objtype, BCINT grid)
{
	BCINT width = 415;
	BCINT height = 33 * 10 + 70;
	BCINT x0 = (ScrMaxX - width) / 2;
	BCINT y0 = (ScrMaxY - height) / 2;
	DrawScreenWindow( x0, y0, width, height, " Help: General Keys for all Modes", GREEN);
	DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
	DrawScreenText( x0 + 14, y0 + 24, " ");
	SetColor(BLACK);
    if (UseMouse)
		HideMousePointer();
    DrawScreenText( -1, -1, "Delete     - Delete the current object(s)");
    DrawScreenText( -1, -1, "Enter      - Edit object(s)");
    DrawScreenText( -1, -1, "Escape     - Exit without saving changes");
    DrawScreenText( -1, -1, "Insert     - Insert a new object");
    DrawScreenText( -1, -1, "ScrollLock - Stop screen from scrolling");
    DrawScreenText( -1, -1, "Space      - Toggle the scroll speed");
    DrawScreenText( -1, -1, "+/-        - Zoom in/out");
    DrawScreenText( -1, -1, "1 - 0      - Change to preset scale");
    DrawScreenText( -1, -1, " ");
	DrawScreenText( -1, -1, "B          - Jump to TID (Things Mode)");
	DrawScreenText( -1, -1, "C          - Clear selected objects");
	DrawScreenText( -1, -1, "D          - Toggle drag mode");
    DrawScreenText( -1, -1, "G          - Increase grid scale");
    DrawScreenText( -1, -1, "Shift+G    - Decrease grid scale");
    DrawScreenText( -1, -1, "H          - Hide the grid");
    DrawScreenText( -1, -1, "I          - Toggle information line");
    DrawScreenText( -1, -1, "Shift+I    - Toggle graphical information boxes");
    DrawScreenText( -1, -1, "J          - Jump to a specific object (enter #)");
    DrawScreenText( -1, -1, "K          - Kill the grid (set grid to 0)");
    DrawScreenText( -1, -1, "M          - Mark/unmark current object (select)");
    DrawScreenText( -1, -1, "N          - Jump to the next object");
	DrawScreenText( -1, -1, "O          - Copy Objects");
    DrawScreenText( -1, -1, "P          - Jump to the previous object");
    DrawScreenText( -1, -1, "Q          - Quit or SaveAs - See ini file");
    DrawScreenText( -1, -1, "R          - Toggle ruler on/off");
    DrawScreenText( -1, -1, "U          - Undo last operation");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "T          - Thing mode");
    DrawScreenText( -1, -1, "V          - Vertex mode");
    DrawScreenText( -1, -1, "L          - LineDef/SideDef mode");
    DrawScreenText( -1, -1, "S          - Sector mode");
    DrawScreenText( -1, -1, "Tab        - Switch to next edit mode");
    DrawScreenText( -1, -1, "Shift+Tab  - Switch to previous edit mode");
	SetColor(YELLOW);
	DrawScreenText( -1, y0 + height - 20, "Press any key to return...");
	bioskey(0);
    if (UseMouse)
		ShowMousePointer();
}

void DisplayAbout()
{
	BCINT width = 340;
	BCINT height = 15 * 10 + 70;
	BCINT x0 = (ScrMaxX - width) / 2;
	BCINT y0 = (ScrMaxY - height) / 2;
	HideMousePointer();
    DrawScreenWindow( x0, y0, width, height, " Help: About", GREEN);
    DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
    SetColor(BLACK);
#ifdef GAME_HEXEN
    DrawScreen3DText( x0 + 14, y0 + 34, BLACK, TRUE, "HETH - Hexen Editor for Total Headcases");
#else
    DrawScreen3DText( x0 + 14, y0 + 34, BLACK, TRUE, "DETH - Doom Editor for Total Headcases");
#endif
    DrawScreenText( x0 + 14, y0 + 44, " ");
    SetColor(BLUE);
    DrawScreenText( -1, -1, "Written by");
    DrawScreenText( -1, -1, "~~~~~~~~~~");
    SetColor(BLACK);
#ifdef GAME_HEXEN
    DrawScreenText( -1, -1, "Antony J. Burden.");
#else
    DrawScreenText( -1, -1, "Antony J. Burden & Simon Oke.");
#endif
    DrawScreenText( -1, -1, " ");
    SetColor(BLUE);
    DrawScreenText( -1, -1, "Special thanks to");
    DrawScreenText( -1, -1, "~~~~~~~~~~~~~~~~~");
    SetColor(BLACK);
    DrawScreenText( -1, -1, "John W. Anderson & Jim F. Flynn.");
    DrawScreenText( -1, -1, "Rapha‰l Quinet & Brendon J. Wyber.");
    DrawScreenText( -1, -1, " ");
    SetColor(BLUE);
    DrawScreenText( -1, -1, "Also");
    DrawScreenText( -1, -1, "~~~~");
    SetColor(BLACK);
    DrawScreenText( -1, -1, "Sverre A. Kvernmo, Ken F. Henderson.");
    DrawScreenText( -1, -1, "Ty Halderman, Kjell Rilbe.");
    SetColor( YELLOW);
    DrawScreenText( -1, y0 + height - 20, "Press any key to return...");
    bioskey(0);
	ShowMousePointer();
}

void HelpTextureBox()
{
	BCINT width = 590;
	BCINT height = 19 * 10 + 70;
	BCINT x0 = (ScrMaxX - width) / 2;
	BCINT y0 = (ScrMaxY - height) / 2;
	HideMousePointer();
    DrawScreenWindow( x0, y0, width, height, " Help: Texture Display Box", GREEN);
    DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
    SetColor(BLACK);
    DrawScreenText( x0 + 14, y0 + 24, " ");
    DrawScreenText( -1, -1, "This box represents the currently highlighted LineDefs' SideDef");
    DrawScreenText( -1, -1, "textures.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "It shows the Upper, Normal and Lower textures for each SideDef.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "Moving the mouse around the box causes a blue square to highlight one");
    DrawScreenText( -1, -1, "of the above mentioned regions.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "While in a highlighted area, you can:");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "Press '-' to set the texture to nothing.");
    DrawScreenText( -1, -1, "Press 'U' to copy the texture from the Upper texture of that SideDef.");
    DrawScreenText( -1, -1, "Press 'N' to copy the texture from the Normal texture of that SideDef.");
    DrawScreenText( -1, -1, "Press 'L' to copy the texture from the Lower texture of that SideDef.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "If you single click on one of the highlighted regions then you will");
    DrawScreenText( -1, -1, "be presented with a screen of textures, these are all of the textures");
    DrawScreenText( -1, -1, "available to you for selection, it even shows textures that may be");
    DrawScreenText( -1, -1, "present in any external wads that you load.");
    SetColor( YELLOW);
    DrawScreenText( -1, y0 + height - 20, "Press any key to return...");
    bioskey(0);
    ShowMousePointer();
}

void HelpThingBox()
{
	BCINT width = 590;
#ifdef GAME_HEXEN
	BCINT height = 13 * 10 + 70;
#else
	BCINT height = 10 * 10 + 70;
#endif
	BCINT x0 = (ScrMaxX - width) / 2;
	BCINT y0 = (ScrMaxY - height) / 2;
	HideMousePointer();
    DrawScreenWindow( x0, y0, width, height, " Help: Thing Display Box", GREEN);
    DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
    SetColor(BLACK);
    DrawScreenText( x0 + 14, y0 + 24, " ");
    DrawScreenText( -1, -1, "This box represents the currently highlighted Thing.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "From here you can, by clicking on the sprite, change the Thing type.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "You can also click the letters under the sprite to toggle the");
    DrawScreenText( -1, -1, "Things flags:-");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "E - Easy,       M - Medium,       H - Hard");
#ifdef GAME_HEXEN
    DrawScreenText( -1, -1, "F - Fighter,    C - Cleric,       M - Mage");
    DrawScreenText( -1, -1, "S - Single,     D - Cooperative,  D - Deathmatch");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "Although there are also DEAF and BLIND flags, they are not displayed,");
    DrawScreenText( -1, -1, "but they can be toggled quickly via CTRL+D and CTRL+B.");
#else
    DrawScreenText( -1, -1, "DEAF - Deaf enemies.");
    DrawScreenText( -1, -1, "MULT - Appears in multi-player games.");
#endif
    SetColor( YELLOW);
    DrawScreenText( -1, y0 + height - 20, "Press any key to return...");
    bioskey(0);
    ShowMousePointer();
}

void HelpSectorBox()
{
	BCINT width = 500;
	BCINT height = 4 * 10 + 70;
	BCINT x0 = (ScrMaxX - width) / 2;
	BCINT y0 = (ScrMaxY - height) / 2;
	HideMousePointer();
    DrawScreenWindow( x0, y0, width, height, " Help: Sector Display Box", GREEN);
    DrawScreenButtonIn( x0 + 3, y0 + 21, width - 6, height - 24, "", FALSE);
    SetColor(BLACK);
    DrawScreenText( x0 + 14, y0 + 24, " ");
    DrawScreenText( -1, -1, "These boxes show the ceiling and floor textures of the");
    DrawScreenText( -1, -1, "current highlighted Sector.");
    DrawScreenText( -1, -1, " ");
    DrawScreenText( -1, -1, "Click the upper/lower box to change the relevent texture.");
    SetColor( YELLOW);
    DrawScreenText( -1, y0 + height - 20, "Press any key to return...");
    bioskey(0);
    ShowMousePointer();
}

#endif
