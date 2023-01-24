/*
   DETH - Doom Editor for Total Headcases, by Antony J. Burden
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   MAINLOOP.C - Editor routines.
*/

/* the includes */
#include "deu.h"
#include "levels.h"
#include "keys.h"
#include <mousex.h>

void SortSelection(SelPtr *);
#ifndef SLIM
void DrawMenuLetters(BCINT);
void DoHelp(BCINT, BCINT);
void ShowHelpHere(BCINT);
#endif
void GetCount(SelPtr, BCINT);

extern Bool InfoShown;		/* should we display the info bar? */
extern Bool BrowseMap;
Bool BMap;

/*
   the editor main loop
   */

#define REDRAW_NONE			0
#define REDRAW_QUICK		1
#define REDRAW_ALL			2
#define REDRAW_INFO			3

void EditorLoop()
{
  SelPtr Selected = NULL;
  SelPtr n;
  BCINT EditMode = EdMode;
  BCINT CurObject = -1;
  BCINT OldObject = -1;
	BCINT RedrawMap = REDRAW_ALL;
  BCINT LastRedraw = REDRAW_NONE;
  BCINT buttons; 
  BCINT oldbuttons = 0;
  BCINT SelBoxX = 0;
  BCINT SelBoxY = 0;
  BCINT OldPointerX = 0;
  BCINT OldPointerY = 0;
  BCINT val = 0;
  BCINT count = 0;
  BCINT x0, y0; 
  BCINT MaxX = ScrMaxX;
  BCINT MaxY = ScrMaxY;
  BCINT sd1x, sd2x, sd1y, sd2y;
  Bool  RedrawObj = FALSE;
  Bool  InfoDisplayed = FALSE;
#ifndef SLIM
  Bool  LettersDrawn = FALSE;		/*FULL*/
#endif
  Bool  StretchSelBox = FALSE;
  Bool  KeepMenu = FALSE;
  Bool  DrawMode = FALSE;
#ifdef GAME_HEXEN
  Bool  Found = FALSE;			/*HEXEN*/
  int	  i = 0;					/*HEXEN*/
#endif

  int   key, altkey;
  char  keychar;

  DragObject = FALSE; 
  MadeChanges = FALSE;
  MadeMapChanges = FALSE;
  if (InitialScale > 40)
	InitialScale = 40;  
  Scale = (float) (1.0 / InitialScale);
	CenterMapAroundCoords((MapMinX + MapMaxX) / 2, (MapMinY + MapMaxY) / 2);
  if (UseMouse)
	{
		ResetMouseLimits();
		SetMouseCoords( PointerX, PointerY);
		ShowMousePointer();
		oldbuttons = 0;
  }
  else
		FakeCursor = TRUE;

	if (CreatingNewLevel)
	{
		InsertObject( OBJ_SECTORS, -1, 0, 0);
		CreatingNewLevel = FALSE;
	}
    
  for (;;)
	{
		key = 0;
		altkey = 0;
		
		/* get mouse position and button status */
		if (UseMouse)
		{
			if (FakeCursor)
			{
				HideMousePointer();
				DrawPointer();
				ShowMousePointer();
			}
			GetMouseCoords(&PointerX, &PointerY, &buttons);
			if (FakeCursor)
			{
				HideMousePointer();
				DrawPointer();
				ShowMousePointer();
			}
			MaxX = ScrMaxX;
			sd1x = ScrMaxX - 140;
			sd1y = ScrMaxY - 266;
			sd2x = ScrMaxX - 72;
			sd2y = ScrMaxY - 266;
			if (VideoMode < 3)
			{
				sd1y -= 91;
				sd2y -= 91;
			}
			if (InfoShown)
			{
				MaxY = ScrMaxY;
				sd1y -= 13;
				sd2y -= 13;
			}
			else
				MaxY = ScrMaxY + 13;
			if (buttons == 1 && PointerY < 16)
			{
				/* kluge for the menu bar */
				altkey = 0x08;
				if (PointerX < 12)
					Beep();
				else if (PointerX < 42)
					key = 0x2100; /* 'F' */
				else if (PointerX < 80)
					key = 0x1200; /* 'E' */
				else if (PointerX < 135)
					key = 0x1f00; /* 'S' */
				else if (PointerX < 176)
					key = 0x3200; /* 'M' */
				else if (PointerX < 247)
					key = 0x1700; /* 'I' */
				else if (PointerX < 344)
					key = 0x1800; /* 'O' */
				else if (PointerX < 440)
					key = 0x2E00; /* 'C' */
				else if (PointerX < ScrMaxX - 60)
					Beep();
				else
					key = 0x2300; /* 'H' */
			}
			if (buttons != oldbuttons && CurObject >= 0)
			{
				switch (buttons)
				{
					case 1:
						if (SwapButtons)
							key = 0x000D;
						else
						{
							if (DrawMode && (EditMode == OBJ_VERTEXES || EditMode == OBJ_THINGS))
								key = 20992;
							else
								key = 'M';
						}
						break;
					case 2:
						if (!DragObject)
							if (EditMode == OBJ_THINGS)
							{
								Backup (BTHINGS);
								strcpy (BackupText, "Move Things");
							}
							else if (EditMode == OBJ_VERTEXES)
							{
								Backup (BVERTEXES | BLINEDEFS | BLINEDEFS);
								strcpy (BackupText, "Move Vertexes");
							}
							else if (EditMode == OBJ_LINEDEFS)
							{
								Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS);
								strcpy (BackupText, "Move LineDefs");
							}
							else if (EditMode == OBJ_SECTORS)
							{
								Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS);
								strcpy (BackupText, "Move Sectors");
							}
						key = 'D';	  /* Press right button = Drag */
						break;
					case 3:
					case 4:
						if (SwapButtons)
						{
							if (DrawMode && (EditMode == OBJ_VERTEXES || EditMode == OBJ_THINGS))
								key = 20992;
							else 
								key = 'M';
						}
						else
							key = 0x000D;   /* Press middle button = Edit ('Enter') */
						break;
					default:
						if (StretchSelBox) /* Release left button = End Selection Box */
							key = 'M';
						if (DragObject)
						{	/* Release right button = End Drag */
							DisplayObjectInfo( EditMode, CurObject);
							DrawInfoBar(EditMode, GridScale);
							key = 'D'; 
						}
						break;
				}
				altkey = bioskey(2);
			}
			oldbuttons = buttons;
		}
		if (InfoDisplayed && RedrawMap != REDRAW_ALL && !StretchSelBox && !DragObject && CurObject >= 0)
		{
			x0 = ScrMaxX - 140;
			y0 = ScrMaxY - 210;
			if (VideoMode < 3)
				y0 -= 91;
			if (InfoShown) 
				y0 -= 13; 
#ifdef GAME_HEXEN
															if (EditMode == OBJ_THINGS && MouseInArea(2, MaxY - 132, 360, 116))
																RedrawMap = OnScreenEditingThings(CurObject, Selected);
															else if (DispSprite && EditMode == OBJ_THINGS && MouseInArea(MaxX - 120, MaxY - 132, 116, 120)) {
#ifndef SLIM
															ShowHelpHere(ORANGE);
															RedrawMap = OnScreenEditingThings(CurObject, Selected);
															ShowHelpHere(WHITE);
#else
															RedrawMap = OnScreenEditingThings(CurObject, Selected);
#endif
															}
															else if (EditMode == OBJ_LINEDEFS && MouseInArea(2, MaxY - 142, 258, 120))
																RedrawMap = OnScreenEditingLineDefs(CurObject, Selected);
#else
			if (EditMode == OBJ_THINGS && MouseInArea(2, MaxY - 62, 360, 46)) 
				RedrawMap = OnScreenEditingThings(CurObject, Selected);
			else if (DispSprite && EditMode == OBJ_THINGS && MouseInArea(MaxX - 120, MaxY - 132, 116, 120))
			{
#ifndef SLIM
				ShowHelpHere(ORANGE);
				RedrawMap = OnScreenEditingThings(CurObject, Selected);
				ShowHelpHere(WHITE);
#else
				RedrawMap = OnScreenEditingThings(CurObject, Selected);
#endif
			}
			else if (EditMode == OBJ_LINEDEFS && MouseInArea(2, MaxY - 102, 258, 82))
				RedrawMap = OnScreenEditingLineDefs(CurObject, Selected);
#endif
			else if (EditMode == OBJ_LINEDEFS && DispSprite && LineDefs[CurObject].sidedef1 > -1 && MouseInArea(x0, y0, 136, 210))
			{
#ifndef SLIM
				ShowHelpHere(ORANGE);
				RedrawMap = OnScreenEditingSideDefTextures(CurObject, Selected);
				ShowHelpHere(WHITE);
#else
				RedrawMap = OnScreenEditingSideDefTextures(CurObject, Selected);
#endif
			}
			else if (EditMode == OBJ_SECTORS && MouseInArea(2, MaxY - 92, 280, 76))
				RedrawMap = OnScreenEditingSectors(CurObject, Selected);
			else if (EditMode == OBJ_SECTORS && DispSprite && MouseInArea(MaxX - 72, MaxY - 148, 72, 140))
			{
#ifndef SLIM
				ShowHelpHere(ORANGE);
				RedrawMap = OnScreenEditingSectorFlats(CurObject, Selected);
				ShowHelpHere(WHITE);
#else
				RedrawMap = OnScreenEditingSectorFlats(CurObject, Selected);
#endif
			}
			else while (EditMode == OBJ_LINEDEFS && MouseInArea(260, MaxY - 96, 380, 80))
			{
				RedrawMap = REDRAW_NONE;
				GetMouseCoords( &PointerX, &PointerY, &buttons);
				if (LineDefs[CurObject].sidedef1 > -1 && MouseInArea(260, MaxY - 96, 192, 80))
				{
					RedrawMap = OnScreenEditingSideDef1(CurObject, Selected);
					if (RedrawMap > REDRAW_NONE)
						break;
				}
				else if (LineDefs[CurObject].sidedef2 > -1 && MouseInArea(450, MaxY - 96, 190, 80))
				{
					RedrawMap = OnScreenEditingSideDef2(CurObject, Selected);
					if (RedrawMap > REDRAW_NONE)
						break;
				}
				else if (LineDefs[CurObject].sidedef1 == -1 && MouseInArea(304, MaxY - 43, 100, 16) && buttons == 1)
				{ 
					strcpy (BackupText, "Add 1st SideDef");
					Backup (BSIDEDEFS | BLINEDEFS);
					for (n = Selected; n; n = n->next)
						if (LineDefs[ n->objnum].sidedef1 == -1)
						{
							InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
							LineDefs[ n->objnum].sidedef1 = NumSideDefs - 1;
						}
					if (LineDefs[CurObject].sidedef1 == -1)
					{
						InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
						LineDefs[CurObject].sidedef1 = NumSideDefs - 1;
					}
					HideMousePointer();
					DrawScreenButtonIn(304, MaxY - 43, 100, 16, "Add 1st SideDef", TRUE);
					ShowMousePointer();
					RedrawMap = REDRAW_ALL;
					break;
				}
				else if (LineDefs[CurObject].sidedef2 == -1 && MouseInArea(495, MaxY - 43, 100, 16) && buttons == 1)
				{ 
					DrawScreenButtonIn(495, MaxY - 43, 100, 16, "Add 2nd SideDef", TRUE);
					strcpy (BackupText, "Add 2nd SideDef");
					Backup (BSIDEDEFS | BLINEDEFS);
					for (n = Selected; n; n = n->next)
						if (LineDefs[ n->objnum].sidedef2 == -1)
						{
							val = LineDefs[ n->objnum].sidedef1;
							InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
							strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
							strncpy( SideDefs[ val].tex3, "-", 8);
							LineDefs[ n->objnum].sidedef2 = NumSideDefs - 1;
							LineDefs[ n->objnum].flags = 4;
						}
					if (LineDefs[CurObject].sidedef2 == -1)
					{
						val = LineDefs[CurObject].sidedef1;
						InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
						strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
						strncpy( SideDefs[ val].tex3, "-", 8);
						LineDefs[CurObject].sidedef2 = NumSideDefs - 1;
						LineDefs[CurObject].flags = 4;
					}
					HideMousePointer();
					DrawScreenButtonIn(495, MaxY - 43, 100, 16, "Add 2nd SideDef", TRUE);
					ShowMousePointer();
					RedrawMap = REDRAW_ALL;
					break;
				}
			}
		}

		/* drag object(s) */
		if (DragObject) {
			BCINT forgetit = FALSE;
			
			if (IsSelected( Selected, CurObject) == FALSE)
				ForgetSelection( &Selected);
			else if (Selected->objnum != CurObject) {
				/* current object must be first in the list */
				UnSelectObject( &Selected, CurObject);
				SelectObject( &Selected, CurObject);
			}
			if (Selected == NULL && CurObject >= 0) {
				SelectObject( &Selected, CurObject);
				forgetit = TRUE;
			} 
			if (Selected) {
				GetCount(Selected, forgetit ? -1 : 0);
				if (MoveObjectsToCoords( EditMode, Selected, MAPX( PointerX), MAPY( PointerY), GridScale))
					RedrawMap = REDRAW_ALL;
				if (forgetit)
					ForgetSelection( &Selected);
			}
			else {
				Beep();
				DragObject = FALSE;
			}
		}
		else if (StretchSelBox) {
			BCINT x = MAPX( PointerX);
			BCINT y = MAPY( PointerY);
			
			/* draw selection box */
			SetColor( CYAN);
			setlinestyle(1, 0, 1);
			setwritemode( XOR_PUT);
			DrawMapLine( SelBoxX, SelBoxY, SelBoxX, y);
			DrawMapLine( SelBoxX, y, x, y);
			DrawMapLine( x, y, x, SelBoxY);
			DrawMapLine( x, SelBoxY, SelBoxX, SelBoxY);
			delay(25);
			DrawMapLine( SelBoxX, SelBoxY, SelBoxX, y);
			DrawMapLine( SelBoxX, y, x, y);
			DrawMapLine( x, y, x, SelBoxY);
			DrawMapLine( x, SelBoxY, SelBoxX, SelBoxY); 
			setwritemode( COPY_PUT);
			setlinestyle(0, 0, 1);
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (buttons == 0)
				key = 'M';
		}
		else if (!RedrawObj) {
			/* check if there is something near the pointer */
			OldObject = CurObject;
			if (Scale >= 4.0)
				Sensitivity = 10;
			else if (Scale == 2.0)
				Sensitivity = 8;
			else if (Scale == 1.0)
				Sensitivity = 6;
			else if (Scale == 0.5)
				Sensitivity = 5;
			if (Scale <= 0.4)
				Sensitivity = 4;
			if (Scale <= 0.3)
				Sensitivity = 3;
			if (Scale <= 0.2)
				Sensitivity = 2;
			if ((bioskey( 2) & 0x03) == 0x00)  /* no shift keys */
				CurObject = GetCurObject( EditMode, MAPX( PointerX - Sensitivity), MAPY( PointerY - Sensitivity), MAPX( PointerX + Sensitivity), MAPY( PointerY + Sensitivity));
			if (CurObject < 0)
				CurObject = OldObject;
		}

		
		
		/* draw the map */
		if(RedrawMap == REDRAW_NONE && LastRedraw == REDRAW_QUICK)
			RedrawMap = REDRAW_ALL;
		
		if (RedrawMap == REDRAW_ALL) {
			GetCount(Selected, 0);
			HideMousePointer();
			DrawMap(EditMode, GridScale, GridShown);
			ShowMousePointer();
			HighlightSelection(EditMode, Selected); 
		}
		else if(RedrawMap == REDRAW_INFO && InfoDisplayed) {
			DisplayObjectInfo(EditMode, CurObject);
			DrawInfoBar(EditMode, GridScale);
		}
		else if(RedrawMap == REDRAW_QUICK)  {
			GetCount(Selected, 0);
			HideMousePointer();
			DrawMapQuick(MoveSpeed);
			ShowMousePointer();
			InfoDisplayed = FALSE;
		}

		if (DrawMode && (RedrawMap == REDRAW_ALL || RedrawMap == REDRAW_QUICK)) {
			SetColor(YELLOW);
#ifdef SLIM
			DrawScreenText(ScrMaxX - 160, 5, "DM");
#else
			DrawScreenText(ScrMaxX - 220, 5, "DM");
#endif
		}

		
		/* highlight the current object and display the information box */
		if ((CurObject != OldObject || RedrawObj || RedrawMap == REDRAW_ALL) && PointerX < ScrMaxX && PointerX > 0 && PointerY < ScrMaxY && PointerY > 0) {
			RedrawObj = FALSE;
			if (!RedrawMap && OldObject >= 0)
				HighlightObject( EditMode, OldObject, YELLOW); 
			if (CurObject != OldObject) {
				PlaySound( 50, 10);
				OldObject = CurObject;
				if (EditMode == OBJ_SECTORS && Sectors[CurObject].tag > 0)
					LastSectorTag = Sectors[CurObject].tag;
#ifdef GAME_HEXEN
				if (EditMode == OBJ_LINEDEFS && LineDefs[CurObject].arg1 > 0)
					LastLineDefTag = LineDefs[CurObject].arg1;
#else
				if (EditMode == OBJ_LINEDEFS && LineDefs[CurObject].tag > 0)
					LastLineDefTag = LineDefs[CurObject].tag;
#endif
			}
			if (CurObject >= 0)
				HighlightObject( EditMode, CurObject, YELLOW);
			if (bioskey( 1)) /* speedup */
				RedrawObj = TRUE;
			else
				if (!DragObject) {
					DisplayObjectInfo(EditMode, CurObject);
					InfoDisplayed = TRUE;
				}
		}
		if (RedrawMap && FakeCursor) {
			HideMousePointer();
			DrawPointer();
			ShowMousePointer();
		}

		/* display the current pointer coordinates */
		if (RedrawMap == REDRAW_ALL || PointerX != OldPointerX || PointerY != OldPointerY) {
			SetColor(DARKGRAY);
#ifndef SLIM
			DrawScreenBox( ScrMaxX - 185, 5, ScrMaxX - 70, 12);
			SetColor( BLUE);
			DrawScreenText( ScrMaxX - 185, 5, "(%d, %d)", MAPX( PointerX), MAPY( PointerY));
#else
			DrawScreenBox( ScrMaxX - 125, 5, ScrMaxX - 3, 12);
			SetColor( BLUE);
			DrawScreenText( ScrMaxX - 125, 5, "(%d, %d)", MAPX( PointerX), MAPY( PointerY));
#endif
			if (Debug) {
				SetColor( BLACK);
				DrawScreenBox( ScrMaxX - 185, 25, ScrMaxX - 70, 32);
				SetColor( ORANGE);
				DrawScreenText( ScrMaxX - 256, 25, "(%d, %d, %d, %d ,%d, %2f)", OrigY,ScrCenterY,ScrMaxY,MapMinY,MapMaxY,Scale);
			}
			OldPointerX = PointerX;
			OldPointerY = PointerY;
		}
		
		/* the map is up to date */
		LastRedraw = RedrawMap;
		RedrawMap = REDRAW_NONE;
#ifndef SLIM
		if (bioskey(2) == 40 && !LettersDrawn) {
			DrawMenuLetters(BLACK);
			LettersDrawn = TRUE;
		}
		if (bioskey(2) != 40 && LettersDrawn) {
			DrawMenuLetters(WHITE);
			LettersDrawn = FALSE;
		}
#endif
		
		/* get user input */
		if (bioskey( 1) || key) {
			if (! key) {
				key = bioskey( 0);
				altkey = bioskey( 2);
//				sprintf(BackupText,"%4x %4x",key,altkey);
			}
			
			if (Debug) {
				SetColor(BLACK);
				DrawScreenBox( ScrMaxX - 300 , 25, ScrMaxX - 250, 35);  
				SetColor(ORANGE);
				DrawScreenText( ScrMaxX - 300 , 25, "%d, %d", key, altkey);  
			}


			/* user wants to access the drop-down menus */
			if (altkey & 0x08) {   /* if alt is pressed... */
				KeepMenu = TRUE;
#ifndef SLIM
				DrawMenuLetters(WHITE);
#endif
				while (KeepMenu) {
					x0 = -1;
					y0 = -1;
					if ((key & 0xFF00) == 0x2100) {	   /* Scan code for F */
						if (NestMenus) {
							x0 = 0;
							y0 = 17;
						}
						key = PullDownMenu( x0, y0,
										   "Save           F2", 0x3C00,    (BCINT) 'S', 1,
										   "Save As...     F3", 0x3D00,    (BCINT) 'A', 6,
										   "-", 				0x3D00,    (BCINT) 'A', 1,
										   "Preferences    F5", F5    , (BCINT) 'P', 1,
										   "Save ini File    ",	CTRL_6,    (BCINT) 'i', 6,
										   "-", 				0x3D00,    (BCINT) 'A', 1,
										   "Drop to DOS      ",	ESCAPE,    (BCINT) 'D', 1,
										   "-", 				0x3D00,    (BCINT) 'A', 1,
										   "Quit             ", CTRL_W,    (BCINT) 't', 4,
										   "Quit & Save     Q", CTRL_Q,    (BCINT) 'Q', 1,
										   NULL); 
						if (key == -1 && NestMenus) {
							key = 0x1200; /* 'E' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
	#ifndef SLIM
							key = 0x2300; /* 'H' */
	#else
							key = 0x2E00; /* 'C' */
	#endif
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						KeepMenu = FALSE;
					}
					else if ((key & 0xFF00) == 0x1200) {  /* Scan code for E */
						if (NestMenus) {
							x0 = 43;
							y0 = 17;
						}
						key = PullDownMenu( x0, y0,
											"Copy Object(s)       C", (BCINT) 'O', (BCINT) 'C', 1,
											"Add Object         Ins", INSERT, (BCINT) 'A', 1,

											"Delete Object(s)   Del", DELETE, (BCINT) 'D', 1,
											"-", 				0x3D00,    (BCINT) 'A', 1,
											"Preferences         F5", F5    , (BCINT) 'P', 1,
											"Display Options     F7", F7    , (BCINT) 'O', 9,
										   NULL);
						if (key == -1 && NestMenus) {
							key = 0x1F00; /* 'S' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
							key = 0x2100; /* 'F' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						KeepMenu = FALSE;
					}
					else if ((key & 0xFF00) == 0x1F00) {  /* Scan code for S */
						if (NestMenus) {
							x0 = 81;
							y0 = 17;
						}
						if (EditMode == OBJ_THINGS)
							key = PullDownMenu( x0, y0,
										   "Find/Mark/Filter...  F", (BCINT) 'F', (BCINT) 'F', 1,
										   "Find Again           A", (BCINT) 'A', (BCINT) 'A', 6,
											"-", 				0x3D00,    (BCINT) 'A', 1,
										   "Next Object          N", (BCINT) 'N', (BCINT) 'N', 1,
										   "Prev Object          P", (BCINT) 'P', (BCINT) 'P', 1,
										   "Jump To Object...    J", (BCINT) 'J', (BCINT) 'J', 1,
										   NULL);
						else
							key = PullDownMenu( x0, y0,
										   "Next Object          N", (BCINT) 'N', (BCINT) 'N', 1,
										   "Prev Object          P", (BCINT) 'P', (BCINT) 'P', 1,
										   "Jump To Object...    J", (BCINT) 'J', (BCINT) 'J', 1,
										   NULL);
						if (key == -1 && NestMenus) {
							key = 0x3200; /* 'M' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
							key = 0x1200; /* 'E' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						KeepMenu = FALSE;
					}
					else if ((key & 0xFF00) == 0x3200) {  /* Scan code for M */
						if (NestMenus) {
							x0 = 136;
							y0 = 17;
						}
						key = PullDownMenu( x0, y0,
										   ((EditMode == OBJ_THINGS) ?
											"* Things              T" :
											"  Things              T"), (BCINT) 'T', (BCINT) 'T', 3,
										   ((EditMode == OBJ_LINEDEFS) ?
											"* LineDefs+SideDefs   L" :
											"  LineDefs+SideDefs   L"), (BCINT) 'L', (BCINT) 'L', 3,
										   ((EditMode == OBJ_VERTEXES) ?
											"* Vertexes            V" :
											"  Vertexes            V"), (BCINT) 'V', (BCINT) 'V', 3,
										   ((EditMode == OBJ_SECTORS) ?
											"* Sectors             S" :
											"  Sectors             S"), (BCINT) 'S', (BCINT) 'S', 3,
											"-", 				0x3D00,    (BCINT) 'A', 1,
											"Next Mode           Tab",  0x0009,    (BCINT) 'N', 3,
											"Last Mode     Shift+Tab",  0x0F00,    (BCINT) 'L', 3,
										   NULL);
						if (key == -1 && NestMenus) {
							key = 0x1700; /* 'I' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
							key = 0x1F00; /* 'S' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						KeepMenu = FALSE;
					}
					else if ((key & 0xFF00) == 0x1700) { /* Scan code for I */
						if (NestMenus) {
							x0 = 177;
							y0 = 17;
						}
						key = 0;
						/* code duplicated from 'F8' - I hate to do that */
						if (Selected) { /*JFF mis-blocked code */
							key = MiscOperations( x0, y0, EditMode, &Selected, CurObject);
							if (key == -1) {
								key = 0x1800; /* 'O' */
								KeepMenu = TRUE;
								DrawMap(EditMode, GridScale, GridShown);
								DisplayObjectInfo(EditMode, CurObject);
								continue;
							}
							if (key == -2 && NestMenus) {
								key = 0x3200; /* 'M' */
								KeepMenu = TRUE;
								DrawMap(EditMode, GridScale, GridShown);
								DisplayObjectInfo(EditMode, CurObject);
								continue;
							}
						}/*JFF mis-blocked code */
						else {														 //jff Don't select current
							if (CurObject >= 0) { // when replacing textures
								SelectObject( &Selected, CurObject); // allow empty to mean all
							}
							key = MiscOperations( x0, y0, EditMode, &Selected, CurObject);
							if (key == -1 && NestMenus) {
								key = 0x1800; /* 'O' */
								KeepMenu = TRUE;
								DrawMap(EditMode, GridScale, GridShown);
								DisplayObjectInfo(EditMode, CurObject);
								continue;
							}
							if (key == -2 && NestMenus) {
								key = 0x3200; /* 'M' */
								KeepMenu = TRUE;
								DrawMap(EditMode, GridScale, GridShown);
								DisplayObjectInfo(EditMode, CurObject);
								continue;
							}
							if (CurObject >= 0) {
								UnSelectObject( &Selected, CurObject);
							}
						}
						CurObject = -1;
						DragObject = FALSE;
						StretchSelBox = FALSE;
						MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
						KeepMenu = FALSE;
					}
					else if ((key & 0xFF00) == 0x1800) { /* Scan code for O */
						BCINT savednum, i;
						if (NestMenus) {
							x0 = 248;
							y0 = 17;
						}
						
						key = 0;
						/* don't want to create the object behind the menu bar... */
						if (PointerY < 20) {
							PointerX = ScrCenterX;
							PointerY = ScrCenterY;
						}
						/* code duplicated from 'F9' - I hate to do that */
						savednum = NumLineDefs;
						key = InsertStandardObject( x0, y0, MAPX( PointerX), MAPY( PointerY));
						if (key == -1 && NestMenus) {
							key = 0x2E00; /* 'C' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
							key = 0x1700; /* 'I' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (NumLineDefs > savednum) {
							ForgetSelection( &Selected);
							EditMode = OBJ_LINEDEFS;
							for (i = savednum; i < NumLineDefs; i++) {
								SelectObject( &Selected, i);
							}
							CurObject = NumLineDefs - 1;
							OldObject = -1;
							DragObject = FALSE;
							StretchSelBox = FALSE;
							MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
						}
						KeepMenu = FALSE;
					}
					else if ((key & 0xFF00) == 0x2E00) { /* Scan code for C */
						if (NestMenus) {
							x0 = 344;
							y0 = 17;
						}
						key = 0;
						key = CheckLevel( x0, y0);
						if (key == -1 && NestMenus) {
	#ifndef SLIM
							key = 0x2300; /* 'H' */
	#else
							key = 0x2100; /* 'F' */
	#endif
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
							key = 0x1800; /* 'O' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						KeepMenu = FALSE;
					}
	#ifndef SLIM
					else if ((key & 0xFF00) == 0x2300) {  /* Scan code for H */
						if (NestMenus) {
							x0 = ScrMaxX - 100;
							y0 = 17;
						}
						key = PullDownMenu( x0, y0,
							 "General Keys    F1", F1, (BCINT) 'G', 1,
							 "Special Keys    F6", F6, (BCINT) 'S', 1,
							"-", 				0x3D00,    (BCINT) 'A', 1,
							 "About...", SHIFT_F10,(BCINT) 'A', 1,
							  NULL);
						if (key == -1 && NestMenus) {
							key = 0x2100; /* 'F' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						if (key == -2 && NestMenus) {
							key = 0x2E00; /* 'C' */
							KeepMenu = TRUE;
							DrawMap(EditMode, GridScale, GridShown);
							DisplayObjectInfo(EditMode, CurObject);
							continue;
						}
						KeepMenu = FALSE;
					}
	#endif
					else {
						Beep();
						key = 0;
					}
					KeepMenu = FALSE;
					RedrawMap = REDRAW_ALL;
				}
			}
			
			
			/* simplify the checks later on */
			if (isprint(key & 0x00ff)) 
				keychar = toupper(key & 0x00ff);
			else 
				keychar = '\0';
			
			
			/* erase the (keyboard) pointer */
			if (FakeCursor) {
				HideMousePointer();
				DrawPointer();
				ShowMousePointer();
			}
			
			if(toupper(keychar) == 'F' && EditMode == OBJ_THINGS) {
				ShowSprite = TRUE;
				FindThing(0, &Selected);
				ShowSprite = FALSE;
				RedrawMap = REDRAW_ALL;
			}

			else if(toupper(keychar) == 'A' && EditMode == OBJ_THINGS) {
				ShowSprite = TRUE;
				FindThing(1, NULL);
				ShowSprite = FALSE;
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to exit  */
			else if (key == CTRL_W || (toupper(keychar) == 'Q' && QisQuit)) {
				if (!MadeChanges || Confirm(-1, -1, "You Have Unsaved Changes.  Do You Really Want To Quit?", NULL)) {
					SaveConfigFileOptions();
					break;
				}
				RedrawMap = REDRAW_ALL;
			} 
			else if ((key & 0x00FF) == 0x001B) { /* 'Esc' */
				if (DragObject)
					DragObject = FALSE;						  
				else if (StretchSelBox) {
					StretchSelBox = FALSE;
					MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
				}
				else {
					ForgetSelection( &Selected);
					if (!MadeChanges || Confirm(-1, -1, "You Have Unsaved Changes.  Do You Really Want To Quit?", NULL)) {
						SaveConfigFileOptions();
						break;
					}
					RedrawMap = REDRAW_ALL;
				}
			}
#ifndef SLIM			
			/* user is lost */
			else if (key == F1) {
				DoHelp(EditMode, GridScale);
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants special keys list */
			else if (key == F6) { 
				DisplayHotKeys(EditMode);
				RedrawMap = REDRAW_ALL;
			}

			/* user wants the about box */
			else if (key == SHIFT_F10) { 
				DisplayAbout();
				RedrawMap = REDRAW_ALL;
			}
#endif
			/* user wants to save the level data */
			else if ((key == CTRL_Q || (key & 0xFF00) == 0x3C00 || (toupper(keychar) == 'Q')) && Registered) { /* 'F2' */
				char *outfile = GetWadFileName();
				if (outfile)
				{
					SaveLevelData( outfile);
					if (key == CTRL_Q) {
						SaveConfigFileOptions();
						break;
					}
				}
				RedrawMap = REDRAW_ALL;
				InfoDisplayed = FALSE;
			}
			
			/* user wants to save and change the episode and mission numbers */
			else if ((key & 0xFF00) == 0x3D00 && Registered) { /* 'F3' */
				char *outfile = NULL;
				MDirPtr newLevel, oldl, newl;
				char name[ 9];
				BCINT i;
				
				outfile = GetWadFileName();
				if (outfile) {
					SelectLevel();
					if (*LevelName) {
						/* horrible but it works... */
						sprintf( name, "%s", LevelName);
						newLevel = FindMasterDir( MasterDir, name);
						oldl = Level;
						newl = newLevel;
						for (i = 0; i < 12; i++) {
							newl->wadfile = oldl->wadfile;
							if (i > 0)
								newl->dir = oldl->dir;
							oldl = oldl->next;
							newl = newl->next;
						}
						Level = newLevel;
					}
					SaveLevelData( outfile);
				}
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to get the 'Preferences' menu */
			else if (key == F5) {
				Preferences( -1, -1);
				RedrawMap = REDRAW_ALL;
			}

			/* user wants to get the 'Options' menu */
			else if (key == F7) { 
				Options( -1, -1);
				RedrawMap = REDRAW_ALL;
			}

			else if (key == F4 && EditMode == OBJ_VERTEXES) { 
				if (!ShowDistance)
					ShowDistance = TRUE;
				else
					ShowDistance = FALSE;
				RedrawMap = REDRAW_ALL;
			}

			else if (key == F4 && EditMode == OBJ_LINEDEFS) { 
				if (!AutoMap)
					AutoMap = TRUE;
				else
					AutoMap = FALSE;
				RedrawMap = REDRAW_ALL;
			}

			else if (key == F4 && EditMode == OBJ_THINGS) { 
				if (!ShowThings)
					ShowThings = TRUE;
				else
					ShowThings = FALSE;
				RedrawMap = REDRAW_ALL;
			}

			/* user wants to get the menu of misc. ops */
			else if ((key & 0xFF00) == 0x4200) { /* 'F8' */
				if (Selected)
					MiscOperations( -1, -1, EditMode, &Selected, CurObject);
				else {
					if (CurObject >= 0) {
						SelectObject( &Selected, CurObject);
					}
					MiscOperations( -1, -1, EditMode, &Selected, CurObject);
					if (CurObject >= 0) {
						UnSelectObject( &Selected, CurObject);
					}
				}
				CurObject = -1;
				RedrawMap = REDRAW_ALL;
				DragObject = FALSE;
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
			}
			
			/* user wants to insert a standard shape */
			else if ((key & 0xFF00) == 0x4300) { /* 'F9' */
				BCINT savednum, i;
				Backup (~BTHINGS);
				strcpy (BackupText, "Insert Prefab Object");
				savednum = NumLineDefs;
				InsertStandardObject( -1, -1, MAPX( PointerX), MAPY( PointerY));
				if (NumLineDefs > savednum) {
					ForgetSelection( &Selected);
					EditMode = OBJ_LINEDEFS;
					for (i = savednum; i < NumLineDefs; i++) {
						SelectObject( &Selected, i);
					}
					CurObject = NumLineDefs - 1;
					OldObject = -1;
					DragObject = FALSE;
					StretchSelBox = FALSE;
					MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
				}
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to check his level */
			else if ((key & 0xFF00) == 0x4400) { /* 'F10' */
				CheckLevel( -1, -1);
				RedrawMap = REDRAW_ALL;
			}

			/* user wants to change the edit mode */
			else if ((key & 0x00FF) == 0x0009 || (key & 0xFF00) == 0x0F00 ||
					 toupper(keychar) == 'T' || toupper(keychar) == 'V' || toupper(keychar) == 'L' || toupper(keychar) == 'S') {
				BCINT PrevMode = EditMode;
				SelPtr NewSel;
				
				if ((key & 0x00FF) == 0x0009) { /* 'Tab' */
					switch (EditMode) {
					case OBJ_THINGS:
						EditMode = OBJ_VERTEXES;
						break;
					case OBJ_VERTEXES:
						EditMode = OBJ_LINEDEFS;
						break;
					case OBJ_LINEDEFS:
						EditMode = OBJ_SECTORS;
						break;
					case OBJ_SECTORS:
						EditMode = OBJ_THINGS;
						break;
					}
				}
				else if ((key & 0xFF00) == 0x0F00) { /* 'Shift-Tab' */
					switch (EditMode) {
					case OBJ_THINGS:
						EditMode = OBJ_SECTORS;
						break;
					case OBJ_VERTEXES:
						EditMode = OBJ_THINGS;
						break;
					case OBJ_LINEDEFS:
						EditMode = OBJ_VERTEXES;
						break;
					case OBJ_SECTORS:
						EditMode = OBJ_LINEDEFS;
						break;
					}
				}
				else {
					if (toupper(keychar) == 'T')
						EditMode = OBJ_THINGS;
					else if (toupper(keychar) == 'V')
						EditMode = OBJ_VERTEXES;
					else if (toupper(keychar) == 'L')
						EditMode = OBJ_LINEDEFS;
					else if (toupper(keychar) == 'S')
						EditMode = OBJ_SECTORS;
					ForgetSelection( &Selected);
				}
				EdMode = EditMode;
				DrawMode = FALSE;
				
				/* special cases for the selection list... */
				if (Selected) {
					/* select all LineDefs bound to the selected Sectors */
					if (PrevMode == OBJ_SECTORS && EditMode == OBJ_LINEDEFS) {
						int l, sd;
						
						NewSel = NULL;
						for (l = 0; l < NumLineDefs; l++) {
							sd = LineDefs[ l].sidedef1;
							if (sd >= 0 && IsSelected( Selected, SideDefs[ sd].sector)) {
								SelectObject( &NewSel, l);
							}
							else {
								sd = LineDefs[ l].sidedef2;
								if (sd >= 0 && IsSelected( Selected, SideDefs[ sd].sector)) {
									SelectObject( &NewSel, l);
								}
							}
						}
						ForgetSelection( &Selected);
						Selected = NewSel;
					}
					/* select all Vertices bound to the selected LineDefs */
					else if (PrevMode == OBJ_LINEDEFS && EditMode == OBJ_VERTEXES) {
						NewSel = NULL;
						while (Selected) {
							if (!IsSelected( NewSel, LineDefs[ Selected->objnum].start)) {
								SelectObject( &NewSel, LineDefs[ Selected->objnum].start);
							}
							if (!IsSelected( NewSel, LineDefs[ Selected->objnum].end)) {
								SelectObject( &NewSel, LineDefs[ Selected->objnum].end);
							}
							UnSelectObject( &Selected, Selected->objnum); 
						}
						Selected = NewSel;
					}
					/* select all Sectors that have their LineDefs selected */
					else if (PrevMode == OBJ_LINEDEFS && EditMode == OBJ_SECTORS) {
						int l, sd;
						
						NewSel = NULL;
						/* select all Sectors... */
						for (l = 0; l < NumSectors; l++) {
							SelectObject( &NewSel, l);
						}
						/* ... then unselect those that should not be in the list */
						for (l = 0; l < NumLineDefs; l++)
							if (!IsSelected( Selected, l)) {
								sd = LineDefs[ l].sidedef1;
								if (sd >= 0) {
									UnSelectObject( &NewSel, SideDefs[ sd].sector);
								}
								sd = LineDefs[ l].sidedef2;
								if (sd >= 0) {
									UnSelectObject( &NewSel, SideDefs[ sd].sector);
								}
							}
						ForgetSelection( &Selected);
						Selected = NewSel;
					}
					/* select all LineDefs that have both ends selected */
					else if (PrevMode == OBJ_VERTEXES && EditMode == OBJ_LINEDEFS) {
						int l;
						
						NewSel = NULL;
						for (l = 0; l < NumLineDefs; l++)
							if (IsSelected( Selected, LineDefs[ l].start) && IsSelected( Selected, LineDefs[ l].end)) {
								SelectObject( &NewSel, l);
							}
						ForgetSelection( &Selected);
						Selected = NewSel;
					}
					/* unselect all */
					else
						ForgetSelection( &Selected);
				}
				if (GetMaxObjectNum( EditMode) >= 0 && Select0)
					CurObject = 0;
				else
					CurObject = -1;
				OldObject = -1;
				DragObject = FALSE;
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
				RedrawMap = REDRAW_ALL;
			}
			
			else if (toupper(keychar) == 'U') {
				Undo();
				CurObject = -1;
				ForgetSelection( &Selected);
				RedrawMap = REDRAW_ALL;
			}

			else if (toupper(keychar) == 'R') {
				DrawMode = DrawMode ? FALSE : TRUE;
				if (DrawMode)
					SetColor(YELLOW);
				else
					SetColor(DARKGRAY);
#ifdef SLIM
				DrawScreenText(ScrMaxX - 160, 5, "DM");
#else
				DrawScreenText(ScrMaxX - 220, 5, "DM");
#endif

			}


			/* user wants to display/hide the info box */
			else if (toupper(keychar) == 'I') {
				if ((altkey & 0x03) == 0x00)  /* no shift keys */
					InfoShown = !InfoShown;
				else
					DispSprite = !DispSprite;
				RedrawMap = REDRAW_ALL;
			}

			
			/* user wants to change the scale */
			else if ((keychar == '+' || keychar == '=') && Scale < 32.0) { 
				OrigX += (BCINT) ((PointerX - ScrCenterX) / Scale);
				OrigY += (BCINT) ((ScrCenterY - PointerY) / Scale);
				if (Scale < 1.0)
					Scale = 1.0 / ((1.0 / Scale) - 1.0);
				else
					Scale = Scale * 2.0;
				OrigX -= (BCINT) ((PointerX - ScrCenterX) / Scale);
				OrigY -= (BCINT) ((ScrCenterY - PointerY) / Scale);
				RedrawMap = REDRAW_ALL;
			}
			else if ((keychar == '-' || keychar == '_') && Scale > 0.025) {
				OrigX += (BCINT) ((PointerX - ScrCenterX) / Scale);
				OrigY += (BCINT) ((ScrCenterY - PointerY) / Scale);
				if (Scale < 1.0)
					Scale = 1.0 / ((1.0 / Scale) + 1.0);
				else
					Scale = Scale / 2.0;
				OrigX -= (BCINT) ((PointerX - ScrCenterX) / Scale);
				OrigY -= (BCINT) ((ScrCenterY - PointerY) / Scale);
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to set the scale directly */
			else if (keychar >= '0' && keychar <= '9') {
				OrigX += (BCINT) ((PointerX - ScrCenterX) / Scale);
				OrigY += (BCINT) ((ScrCenterY - PointerY) / Scale);
				if (keychar == '0')
					Scale = 0.1;
				else
					Scale = 1.0 / (keychar - '0');
				OrigX -= (BCINT) ((PointerX - ScrCenterX) / Scale);
				OrigY -= (BCINT) ((ScrCenterY - PointerY) / Scale);
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to move */
			else if ((key & 0xFF00) == 0x4800 && (PointerY - MoveSpeed) >= 0) {
				if (UseMouse)
					SetMouseCoords( PointerX, PointerY - MoveSpeed);
				else
					PointerY -= MoveSpeed;
			}
			else if ((key & 0xFF00) == 0x5000 && (PointerY + MoveSpeed) <= ScrMaxY) {
				if (UseMouse)
					SetMouseCoords( PointerX, PointerY + MoveSpeed);
				else
					PointerY += MoveSpeed;
			}
			else if ((key & 0xFF00) == 0x4B00 && (PointerX - MoveSpeed) >= 0) {
				if (UseMouse)
					SetMouseCoords( PointerX - MoveSpeed, PointerY);
				else
					PointerX -= MoveSpeed;
			}
			else if ((key & 0xFF00) == 0x4D00 && (PointerX + MoveSpeed) <= ScrMaxX) {
				if (UseMouse)
					SetMouseCoords( PointerX + MoveSpeed, PointerY);
				else
					PointerX += MoveSpeed;
			}
			
			/* user wants so scroll the map */
			else if ((key & 0xFF00) == 0x4900 && MAPY( ScrCenterY) < MapMaxY) { 
				OrigY += (BCINT) (ScrCenterY / Scale);
				RedrawMap = REDRAW_ALL;
			}
			else if ((key & 0xFF00) == 0x5100 && MAPY( ScrCenterY) > MapMinY) { 
				OrigY -= (BCINT) (ScrCenterY / Scale);
				RedrawMap = REDRAW_ALL;
			}
			else if ((key & 0xFF00) == 0x4700 && MAPX( ScrCenterX) > MapMinX) {
				OrigX -= (BCINT) (ScrCenterX / Scale);
				RedrawMap = REDRAW_ALL;
			}
			else if ((key & 0xFF00) == 0x4F00 && MAPX( ScrCenterX) < MapMaxX) {
				OrigX += (BCINT) (ScrCenterX / Scale);
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to change the movement speed */
			else if (keychar == ' ')
				MoveSpeed = MoveSpeed == 2 ? DefaultLargeScroll : 2;
			
			/* user wants to change the grid scale */
			else if (toupper(keychar) == 'G') {
				if ((altkey & 0x03) == 0x00) { /* no shift keys */
					if (GridScale == 0)
						GridScale = MaxGrid;
					else if (GridScale > MinGrid)
						GridScale /= 2;
					else
						GridScale = 0;
				}
				else {
					if (GridScale == 0)
						GridScale = MinGrid;
					else if (GridScale < MaxGrid)
						GridScale *= 2;
					else
						GridScale = 0;
				}
				RedrawMap = REDRAW_ALL;
			}
			else if (toupper(keychar) == 'K') {
				if ((altkey & 0x03) == 0x00)  /* no shift keys */
					GridScale = 0;
				RedrawMap = REDRAW_ALL;
			}
			else if (toupper(keychar) == 'H') {
				if ((altkey & 0x03) != 0x00)  /* shift key pressed */
					GridScale = 0;
				else
					GridShown = !GridShown;
				RedrawMap = REDRAW_ALL;
			}

			
			/* user wants to toggle drag mode */
			else if (toupper(keychar) == 'D') {
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
				if (DragObject) {
					DragObject = FALSE;
					if (EditMode == OBJ_VERTEXES) {
						if (Selected == NULL && CurObject >= 0) {
							SelectObject( &Selected, CurObject);
							if (AutoMergeVertices( &Selected)) {
								ForgetSelection( &Selected);
								RedrawMap = REDRAW_ALL;
							}
							ForgetSelection( &Selected);
						}
						if (AutoMergeVertices( &Selected)) {
							ForgetSelection( &Selected);
							RedrawMap = REDRAW_ALL;
						}
					}
					else if (EditMode == OBJ_LINEDEFS) {
						SelPtr NewSel, cur;
						
						NewSel = NULL;
						if (Selected == NULL && CurObject >= 0) {
							SelectObject( &NewSel, LineDefs[ CurObject].start);
							SelectObject( &NewSel, LineDefs[ CurObject].end);
						}
						else {
							for (cur = Selected; cur; cur = cur->next) {
								if (!IsSelected( NewSel, LineDefs[ cur->objnum].start)) {
									SelectObject( &NewSel, LineDefs[ cur->objnum].start);
								}
								if (!IsSelected( NewSel, LineDefs[ cur->objnum].end)) {
									SelectObject( &NewSel, LineDefs[ cur->objnum].end);
								}
							}
						}
						if (AutoMergeVertices( &NewSel))
							RedrawMap = REDRAW_ALL;
						ForgetSelection( &NewSel);
					}
				}
				else {
					DragObject = TRUE;
					if (EditMode == OBJ_THINGS && CurObject >= 0) {
				    	Backup (BTHINGS);
						strcpy (BackupText, "Drag Things");
						MoveObjectsToCoords( EditMode, NULL, Things[ CurObject].xpos, Things[ CurObject].ypos, 0);
					}
					else if (EditMode == OBJ_VERTEXES && CurObject >= 0) {
				    	Backup (~BTHINGS);
						strcpy (BackupText, "Drag Vertexes");
						MoveObjectsToCoords( EditMode, NULL, Vertexes[ CurObject].x, Vertexes[ CurObject].y, 0);
						}
					else {
				    	Backup (~BTHINGS);
						strcpy (BackupText, "Drag Sectors");
						if (EditMode == OBJ_LINEDEFS) 
								strcpy (BackupText, "Drag LineDefs");
						MoveObjectsToCoords( EditMode, NULL, MAPX( PointerX), MAPY( PointerY), GridScale);
					}
				}
			}
			
			/* user wants to select the next or previous object */
			else if (toupper(keychar) == 'N')
			{
				if (CurObject < GetMaxObjectNum(EditMode))
				{
					CurObject++;
					if (Selected)
					{
						while (!IsSelected(Selected,CurObject))
							CurObject = CurObject<GetMaxObjectNum(EditMode)? CurObject+1 : 0;
						GoToObject(EditMode,CurObject);
					}
				}
				else if (GetMaxObjectNum(EditMode) >= 0)
				{
					CurObject = 0;
					if (Selected)
					{
						while (!IsSelected(Selected,CurObject))
							CurObject = CurObject<GetMaxObjectNum(EditMode)? CurObject+1 : 0;
						GoToObject(EditMode,CurObject);
					}
				}

/*
				if (CurObject < GetMaxObjectNum( EditMode))
					CurObject++;
				else if (GetMaxObjectNum( EditMode) >= 0)
					CurObject = 0;
				else
					CurObject = -1;
*/
				RedrawMap = REDRAW_ALL;
			}
			else if (toupper(keychar) == 'P')
			{
				if (CurObject > 0)
				{
					CurObject--;
					if (Selected)
					{
						while (!IsSelected(Selected,CurObject))
							CurObject = CurObject>0? CurObject-1 : GetMaxObjectNum(EditMode);
						GoToObject(EditMode,CurObject);
					}
				}
				else
				{
					CurObject = GetMaxObjectNum(EditMode);
					if (Selected)
					{
						while (!IsSelected(Selected,CurObject))
							CurObject = CurObject>0? CurObject-1 : GetMaxObjectNum(EditMode);
						GoToObject(EditMode,CurObject);
					}
				}
/*
				if (CurObject > 0)
					CurObject--;
				else
					CurObject = GetMaxObjectNum( EditMode);
*/
				RedrawMap = REDRAW_ALL;
			}
			else if (toupper(keychar) == 'J' || keychar == '#')
			{
				OldObject = InputObjectNumber( -1, -1, EditMode, CurObject);
				if (OldObject >= 0)
				{
					CurObject = OldObject;
					GoToObject( EditMode, CurObject);
				}
				else
					OldObject = CurObject;
				RedrawMap = REDRAW_ALL;
			}
#ifdef GAME_HEXEN
			else if (toupper(keychar) == 'B' && EditMode == OBJ_THINGS) {
				val = InputIntegerValue( -1, -1, 0, 255, 1);
				if (val >= 1) {
					i = 0;
					Found = FALSE;
					while (!Found) {
						if (Things[ i].tid == val) {
							Found = TRUE;
							GoToObject( EditMode, i);
							RedrawMap = REDRAW_ALL; 
						}
						++i;
						if (i >= 256) {
							RedrawMap = REDRAW_ALL; 
							break;
						}
					}
				}
			}
#endif

			/* user wants to mark/unmark an object or a group of objects */
			else if (toupper(keychar) == 'M') {
				if (StretchSelBox) {
					SelPtr oldsel;
					
					/* select all objects in the selection box */
					StretchSelBox = FALSE;
					MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
					RedrawMap = REDRAW_ALL;
					/* additive selection box or not? */
					if (!AdditiveSelBox)
						ForgetSelection( &Selected);
					else
						oldsel = Selected;
					Selected = SelectObjectsInBox( EditMode, SelBoxX, SelBoxY, MAPX( PointerX), MAPY( PointerY));
					if (AdditiveSelBox)
						while (oldsel != NULL) {
							if (! IsSelected( Selected, oldsel->objnum)) {
								SelectObject( &Selected, oldsel->objnum);
							}
							UnSelectObject( &oldsel, oldsel->objnum);
						}
					if (Selected) {
						CurObject = Selected->objnum;
						PlaySound( 440, 10);
					}
					else
						CurObject = -1;
				}
				else if ((altkey & 0x03) == 0x00)  { /* no shift keys */
					if (CurObject >= 0) {
						/* mark or unmark one object */
						if (IsSelected( Selected, CurObject)) {
							UnSelectObject( &Selected, CurObject);
						}
						else {
							SelectObject( &Selected, CurObject);
						}
						HighlightObject( EditMode, CurObject, GREEN);
						if (Selected)
							PlaySound( 440, 10);
						DragObject = FALSE;
					}
					else
						Beep();
				}
				else {
					/* begin "stretch selection box" mode */
					SelBoxX = MAPX( PointerX);
					SelBoxY = MAPY( PointerY);
					StretchSelBox = TRUE;
					MouseSetColors(TranslateToGameColor(YELLOW),GrNOCOLOR);
					DragObject = FALSE;
				}
				GetCount(Selected, 0);
				DrawInfoBar(EditMode, GridScale);
			}
			
			/* user wants to clear all marks and redraw the map */
			else if (toupper(keychar) == 'C') {
				ForgetSelection( &Selected);
				RedrawMap = REDRAW_ALL;
				DragObject = FALSE;
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
				DrawMode = FALSE;
			}
			
			/* user wants to copy a group of objects */
			else if (toupper(keychar) == 'O' && CurObject >= 0) {
				/* copy the object(s) */
				if (Selected == NULL) {
					SelectObject( &Selected, CurObject);
				}
					if (EditMode == OBJ_THINGS) {
						Backup (BTHINGS);
						strcpy (BackupText, "Copy Things");
					}
					else if (EditMode == OBJ_VERTEXES) {
						Backup (BVERTEXES);
						strcpy (BackupText, "Copy Vertexes");
					}
					else if (EditMode == OBJ_LINEDEFS) {
						Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS);
						strcpy (BackupText, "Copy LineDefs");
					}
					else if (EditMode == OBJ_SECTORS) {
						Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS | BSECTORS);
						strcpy (BackupText, "Copy Sectors");
					}
				CopyObjects( EditMode, Selected);
				/* enter drag mode */
				DragObject = TRUE;
				CurObject = Selected->objnum;
				if (EditMode == OBJ_THINGS)
					MoveObjectsToCoords( EditMode, NULL, Things[ CurObject].xpos, Things[ CurObject].ypos, 0);
				else if (EditMode == OBJ_VERTEXES) 
					MoveObjectsToCoords( EditMode, NULL, Vertexes[ CurObject].x, Vertexes[ CurObject].y, 0);
				else
					MoveObjectsToCoords( EditMode, NULL, MAPX( PointerX), MAPY( PointerY), GridScale);
				RedrawMap = REDRAW_ALL;
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
			}

			else if ((key == ESCAPE)) { 
				TermGfx();
				printf("Type Exit to return\n");
				system("");
				InitGfx();
				CheckMouseDriver();
				RedrawMap = REDRAW_ALL;
			}

/************/
/* HOT KEYS */
/************/

#ifdef GAME_HEXEN
			else if ((key == CTRL_S) && EditMode == OBJ_THINGS)
				RedrawMap = FunctionThingSpecial(CurObject, Selected);
#endif
			else if ((key == CTRL_T) && EditMode == OBJ_THINGS)
				RedrawMap = FunctionThingType(CurObject, Selected);

			else if ((key == CTRL_T) && EditMode == OBJ_SECTORS)
				RedrawMap = FunctionSectorType(CurObject, Selected);

			else if ((key == CTRL_J || key == CTRL_D) && EditMode == OBJ_VERTEXES) {
				RedrawMap = FunctionVertexDeleteJoin(CurObject, Selected);
				ForgetSelection ( &Selected);
				CurObject = -1;
			}

// CTRL_D already in use
//			else if (key == CTRL_D && EditMode == OBJ_LINEDEFS) {
//				RedrawMap = FunctionRenumberLineDefsLowest(CurObject, Selected);
//				ForgetSelection ( &Selected);
//				CurObject = -1;
//			}
//
//			else if (key == CTRL_D && EditMode == OBJ_SECTORS) {
//				RedrawMap = FunctionRenumberSectorsLowest(CurObject, Selected);
//				ForgetSelection ( &Selected);
//				CurObject = -1;
//			}

			else if ((key == CTRL_T) && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefSpecial(CurObject, Selected);

			else if ((key == CTRL_J || key == CTRL_D) && EditMode == OBJ_LINEDEFS) { 
				RedrawMap = FunctionLineDefDeleteJoin(CurObject, Selected);
				ForgetSelection ( &Selected);
			}

			else if (key == CTRL_V && EditMode == OBJ_LINEDEFS) { 
				RedrawMap = FunctionVertexSplitLineDef(CurObject, Selected);
				ForgetSelection ( &Selected);
			}
			
			else if (key == CTRL_F && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefFlipSideDefs(CurObject, Selected);

			else if (key == CTRL_S && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefSwapSideDefs(CurObject, Selected);

			else if (key == CTRL_M && EditMode == OBJ_VERTEXES) { 
				RedrawMap = FunctionVertexMerge(CurObject, Selected);
				ForgetSelection( &Selected);
			} 

			else if (key == CTRL_X && EditMode == OBJ_LINEDEFS) { 
				Backup (BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Align Textures on X without Checking");
				AlignTexturesX ( &Selected, TRUE, FALSE);
				RedrawMap = REDRAW_ALL; 
			}

			else if (toupper(keychar) == 'X' && EditMode == OBJ_LINEDEFS) {
				Backup (BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Align Textures on X with Checking");
				AlignTexturesX ( &Selected, FALSE, FALSE);
				RedrawMap = REDRAW_ALL; 
			}

			else if (key == CTRL_Y && EditMode == OBJ_LINEDEFS) {
				Backup (BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Align Textures on Y without Checking");
				AlignTexturesY ( &Selected, TRUE, FALSE);
				RedrawMap = REDRAW_ALL; 
			}

			else if (toupper(keychar) == 'Y' && EditMode == OBJ_LINEDEFS) {
				Backup (BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Align Textures on Y with Checking");
				AlignTexturesY ( &Selected, FALSE, FALSE);
				RedrawMap = REDRAW_ALL; 
			}

			else if (key == CTRL_M && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 0);

			else if (key == CTRL_P && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 1);

			else if (key == CTRL_2 && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 2);

			else if (key == CTRL_6) 
				SaveConfigFileOptions();

			else if (key == CTRL_U && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 3);

			else if (key == CTRL_L && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 4);

			else if (key == CTRL_R && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 5);

			else if (key == CTRL_B && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 6);

			else if (key == CTRL_H && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 7);

			else if (key == CTRL_O && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefFlag(CurObject, Selected, 8);

#ifdef GAME_HEXEN
			else if (key == CTRL_G && EditMode == OBJ_LINEDEFS) { 
				SelPtr cur;
				BCINT x0, y0;
				Backup (BLINEDEFS);
				strcpy (BackupText, "Reset LineDef Arguments");
				for(cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) 
						LineDefs[ cur->objnum].arg1 = 0;
						LineDefs[ cur->objnum].arg2 = 0;
						LineDefs[ cur->objnum].arg3 = 0;
						LineDefs[ cur->objnum].arg4 = 0;
						LineDefs[ cur->objnum].arg5 = 0;
				}
				LineDefs[ CurObject].arg1 = 0;
				LineDefs[ CurObject].arg2 = 0;
				LineDefs[ CurObject].arg3 = 0;
				LineDefs[ CurObject].arg4 = 0;
				LineDefs[ CurObject].arg5 = 0;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL; 
			}


			else if (key == CTRL_G && EditMode == OBJ_THINGS) {
				SelPtr cur;
				BCINT x0, y0;
				Backup (BTHINGS);
				strcpy (BackupText, "Reset Thing Arguments");
				for(cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) 
						Things[ cur->objnum].arg1 = 0;
						Things[ cur->objnum].arg2 = 0;
						Things[ cur->objnum].arg3 = 0;
						Things[ cur->objnum].arg4 = 0;
						Things[ cur->objnum].arg5 = 0;
				}
				Things[ CurObject].arg1 = 0;
				Things[ CurObject].arg2 = 0;
				Things[ CurObject].arg3 = 0;
				Things[ CurObject].arg4 = 0;
				Things[ CurObject].arg5 = 0;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL; 
			}
#else

			else if (key == CTRL_G && EditMode == OBJ_LINEDEFS) {
				SelPtr cur;
				BCINT x0, y0;
				Backup (BLINEDEFS);
				strcpy (BackupText, "Change LineDef Sector Tag");
				x0 = ((ScrMaxX - 280) /2);
				y0 = ((ScrMaxY / 2 ) + 29);
				DrawScreenBox3D( x0, y0 , x0 + 25 + (8 * 32), y0 + 55);
				DrawScreenText( x0 + 10, y0 + 10, "Current Tag: %d",LineDefs[CurObject].tag);
				DrawScreenText( -1, y0 + 24, "Next Free Tag: %d",FindFreeTag());
				DrawScreenText( -1, y0 + 38, "Last Highlighted Sector Tag: %d",LastSectorTag);
				if (LineDefs[ CurObject].tag == 0) 		/* jff allow 32767 tags */
					val = InputIntegerValue( -1, -1, 0, 32767, FindFreeTag());
				else 
					val = InputIntegerValue( -1, -1, 0, 32767, LineDefs[ CurObject].tag);
				if (val >= 0) {
					for(cur = Selected; cur; cur = cur->next) {
						if(cur->objnum != CurObject) 
							LineDefs[ cur->objnum].tag = val;
					}
					LineDefs[ CurObject].tag = val;
					LastLineDefTag = LineDefs[CurObject].tag;
					MadeChanges = TRUE;
				}
				RedrawMap = REDRAW_ALL; 
			}
#endif

			else if (key == CTRL_G && EditMode == OBJ_SECTORS) { 
				SelPtr cur;
				BCINT x0, y0;
				Backup (BSECTORS);
				strcpy (BackupText, "Change Sector LineDef Tag");
				x0 = ((ScrMaxX - 280) /2);
				y0 = ((ScrMaxY / 2 ) + 29);
				DrawScreenBox3D( x0, y0 , x0 + 25 + (8 * 32), y0 + 55);
				DrawScreenText( x0 + 10, y0 + 10, "Current Tag: %d",Sectors[CurObject].tag);
				DrawScreenText( -1, y0 + 24, "Next Free Tag: %d",FindFreeTag());
				DrawScreenText( -1, y0 + 38, "Last Highlighted LineDef Tag: %d",LastLineDefTag);
				if (Sectors[ CurObject].tag == 0) 		/* jff allow 999 tags */
					val = InputIntegerValue( -1, -1, 0, 32767, FindFreeTag());
				else 
					val = InputIntegerValue( -1, -1, 0, 32767, Sectors[ CurObject].tag);
				if (val >= 0) {
					for(cur = Selected; cur; cur = cur->next) {
						if(cur->objnum != CurObject) 
							Sectors[ cur->objnum].tag = val;
					}
					Sectors[ CurObject].tag = val;
					LastSectorTag = Sectors[CurObject].tag;
					MadeChanges = TRUE;
				}
				RedrawMap = REDRAW_ALL; 
			}

			else if (key == CTRL_N && EditMode == OBJ_LINEDEFS) {
				SelPtr cur;
				Backup (BLINEDEFS);
				strcpy (BackupText, "Set LineDef Type To Normal And Tag To 0");
				for (cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) {
						LineDefs[ cur->objnum].special = 0;
#ifdef GAME_HEXEN
						LineDefs[ cur->objnum].arg1 = 0;
#else
						LineDefs[ cur->objnum].tag = 0;
#endif
					}
				}
				LineDefs[ CurObject].special = 0;
#ifdef GAME_HEXEN
				LineDefs[ CurObject].arg1 = 0;
#else
				LineDefs[ CurObject].tag = 0;
#endif
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}

			else if (key == CTRL_N && EditMode == OBJ_SECTORS) {
				SelPtr cur;
				Backup (BSECTORS);
				strcpy (BackupText, "Set Sector Type To Normal And Tag To 0");
				for (cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) {
						Sectors[ cur->objnum].special = 0;
						Sectors[ cur->objnum].tag = 0;
					}
				}
				Sectors[ CurObject].special = 0;
				Sectors[ CurObject].tag = 0;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}


			else if (key == CTRL_F && EditMode == OBJ_THINGS)
				RedrawMap = FunctionThingFlags(CurObject, Selected);

			else if (key == CTRL_C && EditMode == OBJ_LINEDEFS)
				RedrawMap = FunctionLineDefFlags(CurObject, Selected);

#ifdef GAME_HEXEN
			else if (key == CTRL_E && EditMode == OBJ_LINEDEFS) 
				RedrawMap = FunctionLineDefTrigger(CurObject, Selected);

			else if (key == CTRL_B && EditMode == OBJ_THINGS)
				RedrawMap = FunctionThingFlag(CurObject, Selected, 5);
#else
			else if (key == CTRL_M && EditMode == OBJ_THINGS)
				RedrawMap = FunctionThingFlag(CurObject, Selected, 5);
#endif


			else if (key == CTRL_D && EditMode == OBJ_THINGS)
				RedrawMap = FunctionThingFlag(CurObject, Selected, 4);

			else if (key == CTRL_A && EditMode == OBJ_LINEDEFS) {
				RedrawMap = FunctionLineDefSplitAdd(CurObject, Selected);
				ForgetSelection( &Selected);
			}

			else if (key == CTRL_K && EditMode == OBJ_LINEDEFS) {
				RedrawMap = FunctionConnectLineDefsSplitDonut(CurObject, Selected);
				ForgetSelection( &Selected);
			}

			else if (key == CTRL_A && EditMode == OBJ_VERTEXES) {
				RedrawMap = FunctionVertexSplitSector(CurObject, Selected);
				ForgetSelection( &Selected);
			}

			/* user wants to rotate things */
			else if(EditMode == OBJ_THINGS && ((keychar == ',') || (keychar == '.'))) {
				BCINT temp, rot = (keychar == ',') ? 45 : 315;
				SelPtr cur;
				Backup (BTHINGS);
				strcpy (BackupText, "Change Thing Angle");
				for(cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) {
						temp = (Things[cur->objnum].angle + rot) % 360;
						Things[cur->objnum].angle = temp;
					}
				}
				temp = (Things[CurObject].angle + rot) % 360;
				Things[CurObject].angle = temp;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}

#ifdef GAME_HEXEN
			/* user wants to change things z position */
			else if(EditMode == OBJ_THINGS && ((keychar == '<') || (keychar == '>'))) {
				BCINT temp, val = (keychar == '<') ? -16 : 16;
				SelPtr cur;
				Backup (BTHINGS);
				strcpy (BackupText, "Change Thing Height");
				for(cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) {
						temp = (Things[cur->objnum].zpos + val);
						Things[cur->objnum].zpos = temp;
					}
				}
				temp = (Things[CurObject].zpos + val);
				Things[CurObject].zpos = temp;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}
#endif

			else if(EditMode == OBJ_SECTORS && key == CTRL_F)
				RedrawMap = FunctionSectorFloorHeight(CurObject, Selected);

			else if(EditMode == OBJ_SECTORS && key == CTRL_C) 
				RedrawMap = FunctionSectorCeilingHeight(CurObject, Selected);

			else if(EditMode == OBJ_SECTORS && key == CTRL_L)
				RedrawMap = FunctionSectorLightLevel(CurObject, Selected);
			

			else if(keychar == '[' || keychar == ']') {
				BCINT angle = (keychar == ']') ? 90 : 270;
				Backup (BTHINGS | BSECTORS | BVERTEXES | BLINEDEFS | BLINEDEFS);
				strcpy (BackupText, "Rotate Objects");
				RotateAndScaleObjects(EditMode, Selected, (double) angle * 0.0174533, (double) 100 * 0.01, 0);
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}
/*
			else if(keychar == '{' || keychar == '}') {
				BCINT temp, scale = (keychar == '}') ? 50 : 200;
				Backup (BTHINGS | BSECTORS | BVERTEXES | BLINEDEFS | BLINEDEFS);
				strcpy (BackupText, "Scale Objects");
				RotateAndScaleObjects(EditMode, Selected, (double) 0 * 0.0174533, (double) scale * 0.01, 0);
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}

*/
			else if(keychar == '/') {
				CenterMapAroundCoords((MapMinX + MapMaxX) / 2, (MapMinY + MapMaxY) / 2);
				RedrawMap = REDRAW_ALL;
			}

			else if(EditMode == OBJ_SECTORS && ((keychar == '<') || (keychar == '>'))) {
				BCINT temp, amount = (keychar == '>') ? Increment : Increment - (2 * Increment);
				SelPtr cur;
				Backup (BSECTORS);
				strcpy (BackupText, "Change Sector Ceiling Heights");
				for(cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) {
						temp = (Sectors[cur->objnum].ceilh + amount);
						Sectors[cur->objnum].ceilh = temp;
					}
				}
				temp = (Sectors[CurObject].ceilh + amount);
				Sectors[CurObject].ceilh = temp;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}


			else if(EditMode == OBJ_SECTORS && ((keychar == ',') || (keychar == '.'))) {
				BCINT temp, amount = (keychar == '.') ? Increment : Increment - (2 * Increment);
				SelPtr cur;
				Backup (BSECTORS);
				strcpy (BackupText, "Change Sector Floor Heights");
				for(cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) {
						temp = (Sectors[cur->objnum].floorh + amount);
						Sectors[cur->objnum].floorh = temp;
					}
				}
				temp = (Sectors[CurObject].floorh + amount);
				Sectors[CurObject].floorh = temp;
				MadeChanges = TRUE;
				RedrawMap = REDRAW_ALL;
			}

			/* user wants to edit the current object */
			else if ((key & 0x00FF) == 0x000D && CurObject >= 0) { /* 'Enter' */
				if (Selected)
					EditObjectsInfo( 0, 30, EditMode, Selected);
				else {
					SelectObject( &Selected, CurObject);
					EditObjectsInfo( 0, 30, EditMode, Selected);
					UnSelectObject( &Selected, CurObject);
				}
				RedrawMap = REDRAW_ALL;
				DragObject = FALSE;
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
			}
			
			/* user wants to delete the current object */
			else if ((key & 0xFF00) == 0x5300 && CurObject >= 0)
			{ /* 'Del' */
				if (EditMode == OBJ_THINGS || Expert || Confirm( -1, -1,
																(Selected ? "Do You Really Want To Delete These Objects?" : "Do You Really Want To Delete This Object?"),
																(Selected ? "This Will Also Delete The Objects Bound To Them." : "This Will Also Delete The Objects Bound To It.")))
				{
					if (EditMode == OBJ_THINGS)
					{
						Backup (BTHINGS);
						strcpy (BackupText, "Delete Things");
					}
					else if (EditMode == OBJ_VERTEXES)
					{
						Backup (~BTHINGS);
						strcpy (BackupText, "Delete Vertexes");
					}
					else if (EditMode == OBJ_LINEDEFS)
					{
						Backup (~BTHINGS);
						strcpy (BackupText, "Delete LineDefs");
					}
					else if (EditMode == OBJ_SECTORS)
					{
						Backup (~BTHINGS);
						strcpy (BackupText, "Delete Sectors");
					}
					if (Selected)
					{
	    			/* SO 23/7/95:
						This is to fix a bug which has been present
						in DEU as far back as I have the source for! */
						SortSelection(&Selected);
						DeleteObjects(EditMode, &Selected);
					}
					else
						DeleteObject(EditMode, CurObject);
					CurObject = -1;
				}
				DragObject = FALSE;
				StretchSelBox = FALSE;
				MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
				RedrawMap = REDRAW_ALL;
			}
			
			/* user wants to insert a new object */
			else if ((key & 0xFF00) == 0x5200)
			{ /* 'Ins' */
				SelPtr cur;

				if (EditMode == OBJ_THINGS)
				{
					Backup (BTHINGS);
					strcpy (BackupText, "Insert Thing");
				}
				else if (EditMode == OBJ_VERTEXES)
				{
					Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS); //jff change duplicate
					strcpy (BackupText, "Insert Vertex");				//BLINEDEFS to BSIDEDEFS
				}
				else if (EditMode == OBJ_LINEDEFS)
				{
					Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS);
					strcpy (BackupText, "Insert LineDef(s)");
				}
				else if (EditMode == OBJ_SECTORS)
				{
					Backup (BVERTEXES | BSIDEDEFS | BLINEDEFS);
					strcpy (BackupText, "Insert Sector");
				}

				/* first special case: if several Vertices are selected, add new LineDefs */
				if (EditMode == OBJ_VERTEXES && Selected != NULL && Selected->next != NULL)
				{
					BCINT firstv;
					
					if (Selected->next->next != NULL)
						firstv = Selected->objnum;
					else
						firstv = -1;
					EditMode = OBJ_LINEDEFS;
					strcpy (BackupText, "Insert LineDef(s) Between Vertexes");
					/* create LineDefs between the Vertices */
					for (cur = Selected; cur->next; cur = cur->next) {
						/* check if there is already a LineDef between the two Vertices */
						for (CurObject = 0; CurObject < NumLineDefs; CurObject++)
							if ((LineDefs[ CurObject].start == cur->next->objnum && LineDefs[ CurObject].end == cur->objnum)
								|| (LineDefs[ CurObject].end == cur->next->objnum && LineDefs[ CurObject].start == cur->objnum))
								break;
						if (CurObject < NumLineDefs)
							cur->objnum = CurObject;
						else {
							InsertObject( OBJ_LINEDEFS, -1, 0, 0);
							CurObject = NumLineDefs - 1;
							LineDefs[ CurObject].start = cur->next->objnum;
							LineDefs[ CurObject].end = cur->objnum;
							cur->objnum = CurObject;
						}
					}
					/* close the polygon if there are more than 2 Vertices */
					if (firstv >= 0 && (altkey & 0x03) != 0x00)  { /* shift key pressed */
						for (CurObject = 0; CurObject < NumLineDefs; CurObject++)
							if ((LineDefs[ CurObject].start == firstv && LineDefs[ CurObject].end == cur->objnum)
								|| (LineDefs[ CurObject].end == firstv && LineDefs[ CurObject].start == cur->objnum))
								break;
						if (CurObject < NumLineDefs)
							cur->objnum = CurObject;
						else {
							InsertObject( OBJ_LINEDEFS, -1, 0, 0);
							CurObject = NumLineDefs - 1;
							LineDefs[ CurObject].start = firstv;
							LineDefs[ CurObject].end = cur->objnum;
							cur->objnum = CurObject;
						}
					}
					else {
						UnSelectObject( &Selected, cur->objnum);
					}
				}
				/* second special case: if several LineDefs are selected, add new SideDefs and one Sector */
				else if (EditMode == OBJ_LINEDEFS && Selected != NULL)
				{
					for (cur = Selected; cur; cur = cur->next)
						if (LineDefs[ cur->objnum].sidedef1 >= 0 && LineDefs[ cur->objnum].sidedef2 >= 0)
						{
							char msg[ 80];
							
							Beep();
							sprintf( msg, "LineDef #%d Already Has Two SideDefs", cur->objnum);
							Notify( -1, -1, "Error: Cannot Add The New Sector", msg);
							break;
						}
					if (cur == NULL)
					{
						EditMode = OBJ_SECTORS;
						strcpy (BackupText, "Insert Sector");
						InsertObject( OBJ_SECTORS, -1, 0, 0);
						CurObject = NumSectors - 1;
						for (cur = Selected; cur; cur = cur->next)
						{
							InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
							SideDefs[ NumSideDefs - 1].sector = CurObject;
							if (LineDefs[ cur->objnum].sidedef1 >= 0)
							{
								BCINT s;
								
								s = SideDefs[ LineDefs[ cur->objnum].sidedef1].sector;
								if (s >= 0)
								{
									Sectors[ CurObject].floorh = Sectors[ s].floorh;
									Sectors[ CurObject].ceilh = Sectors[ s].ceilh;
									strncpy( Sectors[ CurObject].floort, Sectors[ s].floort, 8);
									strncpy( Sectors[ CurObject].ceilt, Sectors[ s].ceilt, 8);
									Sectors[ CurObject].light = Sectors[ s].light;
								}
								LineDefs[ cur->objnum].sidedef2 = NumSideDefs - 1;
								LineDefs[ cur->objnum].flags = 4;
								strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
								strncpy( SideDefs[ LineDefs[ cur->objnum].sidedef1].tex3, "-", 8);
							}
							else
								LineDefs[ cur->objnum].sidedef1 = NumSideDefs - 1;
						}
						ForgetSelection( &Selected);
						SelectObject( &Selected, CurObject);
					}
				}
				/* normal case: add a new object of the current type */
				else
				{
					ForgetSelection( &Selected);
					if (GridScale > 0)
						InsertObject( EditMode, CurObject, (BCINT) ((int)(MAPX( PointerX) + GridScale / 2)) & ((int)(~(GridScale - 1))), (BCINT) ((int)(MAPY( PointerY) + GridScale / 2)) & (int)(~(GridScale - 1)));
					else
						InsertObject( EditMode, CurObject, MAPX( PointerX), MAPY( PointerY));
					CurObject = GetMaxObjectNum( EditMode);
					if (EditMode == OBJ_LINEDEFS)
					{
						if (! Input2VertexNumbers( -1, -1, "Choose The Two Vertices For The New LineDef",
												  &(LineDefs[ CurObject].start), &(LineDefs[ CurObject].end)))
						{
							DeleteObject( EditMode, CurObject);
							CurObject = -1;
						}
					}
					else if (EditMode == OBJ_VERTEXES)
					{
						if (CurObject>-1)
						{
							SelectObject( &Selected, CurObject);
							if (AutoMergeVertices( &Selected))
								RedrawMap = REDRAW_ALL;
							ForgetSelection( &Selected);
						}
					}
				}
				DragObject = FALSE;
				StretchSelBox = FALSE;
 		   	MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
			  RedrawMap = REDRAW_ALL;
			}
			/* user likes music */
			else if (key)
			{
				Beep();
			}

			
			/* redraw the (keyboard) pointer */
			if (FakeCursor)
			{
				HideMousePointer();
				DrawPointer();
				ShowMousePointer();
			}
		}
		
		/* check if Scroll Lock is off */
		if ((bioskey( 2) & 0x10) == 0x00)
		{
			/* move the map if the pointer is near the edge of the screen */
			if (PointerY <= (UseMouse ? 0 : 20))
			{
				if (! UseMouse)
					PointerY += MoveSpeed;
				OrigY += max(((BCINT) (MoveSpeed * 2.0 / Scale)),1); /*jff*/
				RedrawMap = REDRAW_QUICK;	/* prevent scroll by 0 Scale=32 */
			}
			if (PointerY >= ScrMaxY - (UseMouse ? 0 : 20)) {
				if (! UseMouse)
					PointerY -= MoveSpeed;
				OrigY -= max(((BCINT) (MoveSpeed * 2.0 / Scale)),1); /*jff*/
				RedrawMap = REDRAW_QUICK;
			}
			if (PointerX <= (UseMouse ? 0 : 20))
			{
				if (! UseMouse)   
					PointerX += MoveSpeed;
				OrigX -= max(((BCINT) (MoveSpeed * 2.0 / Scale)),1); /*jff*/
				RedrawMap = REDRAW_QUICK;
			}
			if (PointerX >= ScrMaxX - (UseMouse ? 0 : 20))
			{
				if (! UseMouse)
					PointerX -= MoveSpeed;
				OrigX += max(((BCINT) (MoveSpeed * 2.0 / Scale)),1); /*jff*/
				RedrawMap = REDRAW_QUICK;
			}
		}
		GetMouseCoords( &PointerX, &PointerY, &buttons);
		if (buttons == 1 && !StretchSelBox)
		{
			count++;
			if (count >= ((StretchDelay + 3) * 50))
			{
				PlaySound(50,10);
				SelBoxX = MAPX( PointerX);
				SelBoxY = MAPY( PointerY);
				StretchSelBox = TRUE;
				MouseSetColors(TranslateToGameColor(YELLOW),GrNOCOLOR);
				DragObject = FALSE;
			}
		}
		else 
			count = 0;
  }
}

#ifndef SLIM
void DrawMenuLetters(BCINT color)
{
	SetColor(color);
	DrawScreenText( 7,  4, "F    E    S      M     i       O           C");
	DrawScreenText( ScrMaxX - 50, 4, "H");
}

void DoHelp(BCINT EditMode, BCINT GridScale)
{
	BCINT x0, y0, MaxY, MaxX;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	x0 = ScrMaxX - 140;
	y0 = ScrMaxY - 210;
	if (VideoMode < 3)
		y0 -= 91;
	if (InfoShown) 
		y0 -= 13; 
#ifdef GAME_HEXEN
	if (EditMode == OBJ_THINGS && MouseInArea(2, MaxY - 132, 360, 116))
		DisplayHelp( EditMode, GridScale);
	else if (DispSprite && EditMode == OBJ_THINGS && MouseInArea(MaxX - 120, MaxY - 132, 120, 120)) 
		HelpThingBox();
	else if (EditMode == OBJ_LINEDEFS && MouseInArea(2, MaxY - 142, 258, 120))
		DisplayHelp( EditMode, GridScale);
#else
	if (EditMode == OBJ_THINGS && MouseInArea(2, MaxY - 62, 360, 46)) 
		DisplayHelp( EditMode, GridScale);
	else if (DispSprite && EditMode == OBJ_THINGS && MouseInArea(MaxX - 120, MaxY - 132, 120, 120)) 
		HelpThingBox();
	else if (EditMode == OBJ_LINEDEFS && MouseInArea(2, MaxY - 102, 258, 82))
		DisplayHelp( EditMode, GridScale);
#endif
	else if (DispSprite && EditMode == OBJ_LINEDEFS && MouseInArea(x0, y0, 136, 210)) 
		HelpTextureBox();
	else if (EditMode == OBJ_SECTORS && MouseInArea(2, MaxY - 92, 280, 76))
		DisplayHelp( EditMode, GridScale);
	else if (DispSprite && EditMode == OBJ_SECTORS && MouseInArea(MaxX - 72, MaxY - 148, 72, 140)) 
		HelpSectorBox();
	else if (EditMode == OBJ_LINEDEFS && MouseInArea(260, MaxY - 96, 380, 80))
		DisplayHelp( EditMode, GridScale);
	else 
		DisplayHelp( EditMode, GridScale);
}

void ShowHelpHere(BCINT color)
{
	SetColor(color);
	DrawScreenText( ScrMaxX - 50, 4, "Help");
}
#endif

void GetCount(SelPtr Selected, BCINT Start)
{
	SelPtr n;
	Count = Start;
	for(n = Selected; n; n = n->next)
		Count++;
	if (!Selected && DragObject && Count == 1)
		Count = 0;
}



/* end of file */
