#include "deu.h"
#include "levels.h"
#include "keys.h"

#define REDRAW_NONE			0
#define REDRAW_QUICK		1
#define REDRAW_ALL			2
#define REDRAW_INFO			3


#ifdef GAME_HEXEN
BCINT OnScreenEditingThings(BCINT CurObject, SelPtr Selected)
{
	BCINT val, MouseX, MouseY, buttons, key;
	BCINT MaxX, MaxY;
	Bool hl1, hl2, hl3, hl4, hl5, hl6, hl7, hl8, hl9, hl10;
	Bool hl1a;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl4 = FALSE;
	hl5 = FALSE;
	hl6 = FALSE;
	hl7 = FALSE;
	hl8 = FALSE;
	hl9 = FALSE;
	hl10 = FALSE;
	hl1a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(2, MaxY -132, 360, 116) || MouseInArea(MaxX - 120, MaxY - 132, 118, 120)) {
		if (bioskey(1))
			return bioskey(1);
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(2, MaxY - 113, 360, 9) && !hl1) {
			ScreenHighlightArea(2, MaxY - 115, 76, 11, "Type", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 113, 360, 9) && hl1) {
			ScreenHighlightArea(2, MaxY - 115, 76, 11, "Type", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(2, MaxY - 103, 360, 9) && !hl1a) {
			ScreenHighlightArea(2, MaxY - 105, 76, 11, "Position", DARKERGRAY, BLACK);
			hl1a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 103, 360, 9) && hl1a) {
			ScreenHighlightArea(2, MaxY - 105, 76, 11, "Position", DARKGRAY, BLACK);
			hl1a = FALSE;
		}
		if (MouseInArea(2, MaxY - 93, 360, 9) && !hl2) {
			ScreenHighlightArea(2, MaxY - 95, 76, 11, "Flags", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 93, 360, 9) && hl2) {
			ScreenHighlightArea(2, MaxY - 95, 76, 11, "Flags", DARKGRAY, BLACK);
			hl2 = FALSE;
		}
		if (MouseInArea(2, MaxY - 83, 360, 9) && !hl3) {
			ScreenHighlightArea(2, MaxY - 85, 76, 11, "TID", BLUE, WHITE);
			hl3 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 83, 360, 9) && hl3) {
			ScreenHighlightArea(2, MaxY - 85, 76, 11, "TID", DARKGRAY, BLACK);
			hl3 = FALSE;
		}
		if (MouseInArea(2, MaxY - 73, 360, 9) && !hl4) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Special", BLUE, WHITE);
			hl4 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 73, 360, 9) && hl4) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Special", DARKGRAY, BLACK);
			hl4 = FALSE;
		}
		if (MouseInArea(2, MaxY - 63, 360, 9) && !hl5) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, GetArg1Text(Things[CurObject].special), BLUE, WHITE);
			hl5 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 63, 360, 9) && hl5) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, GetArg1Text(Things[CurObject].special), DARKGRAY, BLACK);
			hl5 = FALSE;
		}
		if (MouseInArea(2, MaxY - 53, 360, 9) && !hl6) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, GetArg2Text(Things[CurObject].special), BLUE, WHITE);
			hl6 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 53, 360, 9) && hl6) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, GetArg2Text(Things[CurObject].special), DARKGRAY, BLACK);
			hl6 = FALSE;
		}
		if (MouseInArea(2, MaxY - 43, 360, 9) && !hl7) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, GetArg3Text(Things[CurObject].special), BLUE, WHITE);
			hl7 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 43, 360, 9) && hl7) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, GetArg3Text(Things[CurObject].special), DARKGRAY, BLACK);
			hl7 = FALSE;
		}
		if (MouseInArea(2, MaxY - 33, 360, 9) && !hl8) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, GetArg4Text(Things[CurObject].special), BLUE, WHITE);
			hl8 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 33, 360, 9) && hl8) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, GetArg4Text(Things[CurObject].special), DARKGRAY, BLACK);
			hl8 = FALSE;
		}
		if (MouseInArea(2, MaxY - 23, 360, 9) && !hl9) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, GetArg5Text(Things[CurObject].special), BLUE, WHITE);
			hl9 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 23, 360, 9) && hl9) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, GetArg5Text(Things[CurObject].special), DARKGRAY, BLACK);
			hl9 = FALSE;
		}
		if (DispSprite && MouseInArea(MaxX - 120, MaxY - 132, 120, 120) && !hl10) {
			ScreenBoxArea(MaxX - 118, MaxY - 131, 116, 116, BLUE);
			ScreenBoxArea(MaxX - 117, MaxY - 130, 114, 114, BLUE);
			hl10 = TRUE;
		}
		if (DispSprite && !MouseInArea(MaxX - 120, MaxY - 132, 120, 120) && hl10) {
			ScreenBoxArea(MaxX - 118, MaxY - 131, 116, 116, DARKGRAY);
			ScreenBoxArea(MaxX - 117, MaxY - 130, 114, 114, DARKGRAY);
			hl10 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(2, MaxY - 113, 360, 9))
			return FunctionThingType(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 93, 360, 9))
			return FunctionThingFlags(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 83, 360, 9))  {
			GetMouseCoords(&MouseX, &MouseY, &buttons);
			return FunctionThingTID(CurObject, Selected);
		}
		if (buttons == 1 && MouseClickedArea(2, MaxY - 73, 360, 9))
			return FunctionThingSpecial(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 63, 360, 9))
			return FunctionThingArg(CurObject, Selected, 1);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 53, 360, 9))
			return FunctionThingArg(CurObject, Selected, 2);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 43, 360, 9))
			return FunctionThingArg(CurObject, Selected, 3);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 33, 360, 9))
			return FunctionThingArg(CurObject, Selected, 4);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 23, 360, 9))
			return FunctionThingArg(CurObject, Selected, 5);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 112, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 1);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 100, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 2);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 88, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 3);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 74, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 6);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 62, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 7);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 50, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 8);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 36, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 9);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 24, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 10);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 12, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 11);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 120, MaxY - 132, 120, 100)) 
			return FunctionThingType(CurObject, Selected);
	}
	ScreenHighlightArea(2, MaxY - 115, 76, 11, "Type", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 105, 76, 11, "Position", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 95, 76, 11, "Flags", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 85, 76, 11, "TID", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 75, 76, 11, "Special", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 65, 76, 11, GetArg1Text(Things[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 55, 76, 11, GetArg2Text(Things[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 45, 76, 11, GetArg3Text(Things[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 35, 76, 11, GetArg4Text(Things[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 25, 76, 11, GetArg5Text(Things[CurObject].special), DARKGRAY, BLACK);
	if (DispSprite) {
		ScreenBoxArea(MaxX - 118, MaxY - 131, 116, 116, DARKGRAY);
		ScreenBoxArea(MaxX - 117, MaxY - 130, 114, 114, DARKGRAY);
	}
	return 0;
}

BCINT OnScreenEditingLineDefs(BCINT CurObject, SelPtr Selected)
{
	BCINT val, MouseX, MouseY, buttons, key;
	BCINT MaxX, MaxY;
	Bool hl1, hl2, hl3, hl4, hl5, hl6, hl7, hl8;
	Bool hl1a, hl2a, hl3a;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl4 = FALSE;
	hl5 = FALSE;
	hl6 = FALSE;
	hl7 = FALSE;
	hl8 = FALSE;
	hl1a = FALSE;
	hl2a = FALSE;
	hl3a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(2, MaxY -142, 258, 126)) {
		if (bioskey(1))
			return bioskey(1); 
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(2, MaxY - 123, 258, 9) && !hl1) {
			ScreenHighlightArea(2, MaxY - 125, 76, 11, "Flags", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 123, 258, 9) && hl1) {
			ScreenHighlightArea(2, MaxY - 125, 76, 11, "Flags", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(2, MaxY - 113, 258, 9) && !hl2) {
			ScreenHighlightArea(2, MaxY - 115, 76, 11, "Trigger", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 113, 258, 9) && hl2) {
			ScreenHighlightArea(2, MaxY - 115, 76, 11, "Trigger", DARKGRAY, BLACK);
			hl2 = FALSE;
		}
		if (MouseInArea(2, MaxY - 103, 258, 9) && !hl1a) {
			ScreenHighlightArea(2, MaxY - 105, 76, 11, "Vertexes", DARKERGRAY, BLACK);
			hl1a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 103, 258, 9) && hl1a) {
			ScreenHighlightArea(2, MaxY - 105, 76, 11, "Vertexes", DARKGRAY, BLACK);
			hl1a = FALSE;
		}
		if (MouseInArea(2, MaxY - 93, 258, 9) && !hl2a) {
			ScreenHighlightArea(2, MaxY - 95, 76, 11, "Length", DARKERGRAY, LIGHTGRAY);
			hl2a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 93, 258, 9) && hl2a) {
			ScreenHighlightArea(2, MaxY - 95, 76, 11, "Length", DARKGRAY, LIGHTGRAY);
			hl2a = FALSE;
		}
		if (MouseInArea(2, MaxY - 83, 258, 9) && !hl3a) {
			ScreenHighlightArea(2, MaxY - 85, 76, 11, "SideDefs", DARKERGRAY, BLACK);
			hl3a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 83, 258, 9) && hl3a) {
			ScreenHighlightArea(2, MaxY - 85, 76, 11, "SideDefs", DARKGRAY, BLACK);
			hl3a = FALSE;
		}
		if (MouseInArea(2, MaxY - 73, 258, 9) && !hl3) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Special", BLUE, WHITE);
			hl3 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 73, 258, 9) && hl3) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Special", DARKGRAY, BLACK);
			hl3 = FALSE;
		}
		if (MouseInArea(2, MaxY - 63, 258, 9) && !hl4) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, GetArg1Text(LineDefs[CurObject].special), BLUE, WHITE);
			hl4 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 63, 258, 9) && hl4) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, GetArg1Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
			hl4 = FALSE;
		}
		if (MouseInArea(2, MaxY - 53, 258, 9) && !hl5) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, GetArg2Text(LineDefs[CurObject].special), BLUE, WHITE);
			hl5 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 53, 258, 9) && hl5) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, GetArg2Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
			hl5 = FALSE;
		}
		if (MouseInArea(2, MaxY - 43, 258, 9) && !hl6) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, GetArg3Text(LineDefs[CurObject].special), BLUE, WHITE);
			hl6 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 43, 258, 9) && hl6) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, GetArg3Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
			hl6 = FALSE;
		}
		if (MouseInArea(2, MaxY - 33, 258, 9) && !hl7) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, GetArg4Text(LineDefs[CurObject].special), BLUE, WHITE);
			hl7 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 33, 258, 9) && hl7) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, GetArg4Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
			hl7 = FALSE;
		}
		if (MouseInArea(2, MaxY - 23, 258, 9) && !hl8) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, GetArg5Text(LineDefs[CurObject].special), BLUE, WHITE);
			hl8 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 23, 258, 9) && hl8) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, GetArg5Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
			hl8 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(2, MaxY - 123, 258, 9))
			return FunctionLineDefFlags(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 113, 258, 9))
			return FunctionLineDefTrigger(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 73, 258, 9))
			return FunctionLineDefSpecial(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 63, 258, 9))
			return FunctionLineDefArg(CurObject, Selected, 1);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 53, 258, 9))
			return FunctionLineDefArg(CurObject, Selected, 2);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 43, 258, 9))
			return FunctionLineDefArg(CurObject, Selected, 3);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 33, 258, 9))
			return FunctionLineDefArg(CurObject, Selected, 4);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 23, 258, 9))
			return FunctionLineDefArg(CurObject, Selected, 5);
	}
	ScreenHighlightArea(2, MaxY - 125, 76, 11, "Flags", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 115, 76, 11, "Trigger", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 105, 76, 11, "Vertexes", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 95, 76, 11, "Length", DARKGRAY, LIGHTGRAY);
	ScreenHighlightArea(2, MaxY - 85, 76, 11, "SideDefs", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 75, 76, 11, "Special", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 65, 76, 11, GetArg1Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 55, 76, 11, GetArg2Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 45, 76, 11, GetArg3Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 35, 76, 11, GetArg4Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 25, 76, 11, GetArg5Text(LineDefs[CurObject].special), DARKGRAY, BLACK);
	return 0;
}
#else
BCINT OnScreenEditingThings(BCINT CurObject, SelPtr Selected)
{
	BCINT MouseX, MouseY, buttons;
	BCINT MaxX, MaxY;
	Bool hl1, hl2, hl3;
	Bool hl1a;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl1a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(2, MaxY - 62, 360, 46) || MouseInArea(MaxX - 120, MaxY - 132, 118, 120)) {
		if (bioskey(1))
			return bioskey(1);
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(2, MaxY - 43, 360, 9) && !hl1) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, "Type", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 43, 360, 9) && hl1) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, "Type", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(2, MaxY - 33, 360, 9) && !hl1a) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, "Position", DARKERGRAY, BLACK);
			hl1a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 33, 360, 9) && hl1a) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, "Position", DARKGRAY, BLACK);
			hl1a = FALSE;
		}
		if (MouseInArea(2, MaxY - 23, 360, 9) && !hl2) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, "Flags", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 23, 360, 9) && hl2) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, "Flags", DARKGRAY, BLACK);
			hl2 = FALSE;
		}
		if (DispSprite && MouseInArea(MaxX - 120, MaxY - 132, 120, 120) && !hl3) {
			ScreenBoxArea(MaxX - 118, MaxY - 131, 116, 116, BLUE);
			ScreenBoxArea(MaxX - 117, MaxY - 130, 114, 114, BLUE);
			hl3 = TRUE;
		}
		if (DispSprite && !MouseInArea(MaxX - 120, MaxY - 132, 120, 120) && hl3) {
			ScreenBoxArea(MaxX - 118, MaxY - 131, 116, 116, DARKGRAY);
			ScreenBoxArea(MaxX - 117, MaxY - 130, 114, 114, DARKGRAY);
			hl3 = FALSE;
		}


		if (buttons == 1 && MouseClickedArea(2, MaxY - 43, 360, 9))
			return FunctionThingType(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 23, 360, 9))
			return FunctionThingFlags(CurObject, Selected);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 112, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 1);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 100, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 2);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 88, MaxY - 25, 10, 10)) 
			return FunctionThingFlag(CurObject, Selected, 3);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 74, MaxY - 25, 30, 10)) 
			return FunctionThingFlag(CurObject, Selected, 4);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 36, MaxY - 25, 30, 10)) 
			return FunctionThingFlag(CurObject, Selected, 5);
		if (DispSprite && buttons == 1 && MouseClickedArea(MaxX - 120, MaxY - 132, 120, 100)) 
			return FunctionThingType(CurObject, Selected);
	}
	ScreenHighlightArea(2, MaxY - 45, 76, 11, "Type", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 35, 76, 11, "Position", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 25, 76, 11, "Flags", DARKGRAY, BLACK);
	if (DispSprite) {
		ScreenBoxArea(MaxX - 118, MaxY - 131, 116, 116, DARKGRAY);
		ScreenBoxArea(MaxX - 117, MaxY - 130, 114, 114, DARKGRAY);
	}
	return 0;
}
BCINT OnScreenEditingLineDefs(BCINT CurObject, SelPtr Selected)
{
	BCINT MouseX, MouseY, buttons;
	BCINT MaxX, MaxY;
	Bool hl1, hl2, hl3;
	Bool hl1a, hl2a, hl3a;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl1a = FALSE;
	hl2a = FALSE;
	hl3a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(2, MaxY -102, 258, 86)) {
		if (bioskey(1))
			return bioskey(1); 
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(2, MaxY - 83, 258, 9) && !hl1) {
			ScreenHighlightArea(2, MaxY - 85, 76, 11, "Flags", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 83, 258, 9) && hl1) {
			ScreenHighlightArea(2, MaxY - 85, 76, 11, "Flags", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(2, MaxY - 73, 258, 9) && !hl2) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Tag", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 73, 258, 9) && hl2) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Tag", DARKGRAY, BLACK);
			hl2 = FALSE;
		}
		if (MouseInArea(2, MaxY - 63, 258, 9) && !hl3) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, "Special", BLUE, WHITE);
			hl3 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 63, 258, 9) && hl3) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, "Special", DARKGRAY, BLACK);
			hl3 = FALSE;
		}
		if (MouseInArea(2, MaxY - 53, 258, 9) && !hl1a) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, "Vertexes", DARKERGRAY, BLACK);
			hl1a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 53, 258, 9) && hl1a) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, "Vertexes", DARKGRAY, BLACK);
			hl1a = FALSE;
		}
		if (MouseInArea(2, MaxY - 43, 258, 9) && !hl2a) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, "Length", DARKERGRAY, LIGHTGRAY);
			hl2a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 43, 258, 9) && hl2a) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, "Length", DARKGRAY, LIGHTGRAY);
			hl2a = FALSE;
		}
		if (MouseInArea(2, MaxY - 33, 258, 9) && !hl3a) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, "SideDefs", DARKERGRAY, BLACK);
			hl3a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 33, 258, 9) && hl3a) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, "SideDefs", DARKGRAY, BLACK);
			hl3a = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(2, MaxY - 83, 258, 9))
			return FunctionLineDefFlags(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 73, 258, 9))
			return FunctionLineDefTag(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 63, 258, 9))
			return FunctionLineDefSpecial(CurObject, Selected);
	}
	ScreenHighlightArea(2, MaxY - 85, 76, 11, "Flags", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 75, 76, 11, "Tag", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 65, 76, 11, "Special", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 55, 76, 11, "Vertexes", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 45, 76, 11, "Length", DARKGRAY, LIGHTGRAY);
	ScreenHighlightArea(2, MaxY - 35, 76, 11, "SideDefs", DARKGRAY, BLACK);
	return 0;
}
#endif

BCINT OnScreenEditingSideDef1(BCINT CurObject, SelPtr Selected)
{
	BCINT MouseX, MouseY, buttons;
	BCINT MaxX, MaxY, foo;
	BCINT s1 = -1, s2 = -1, sd1 = -1, sd2 = -1;
	Bool hl1, hl2, hl3, hl4, hl5, hl6;
	Bool hl1a;
	char texname[9];

	sd1 = LineDefs[ CurObject].sidedef1;
	sd2 = LineDefs[ CurObject].sidedef2;
	if (sd1 >= 0)
		s1 = SideDefs[ sd1].sector;
	else
		s1 = -1;
	if (sd2 >= 0)
		s2 = SideDefs[ sd2].sector;
	else
		s2 = -1;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl4 = FALSE;
	hl5 = FALSE;
	hl6 = FALSE;
	hl1a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(261, MaxY - 96, 192, 84)) {
		if (bioskey(1))
			return bioskey(1); 
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(261, MaxY - 83, 192, 9) && !hl1) {
			ScreenHighlightArea(261, MaxY - 85, 108, 11, "Norm. Texture", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(261, MaxY - 83, 192, 9) && hl1) {
			ScreenHighlightArea(261, MaxY - 85, 108, 11, "Norm. Texture", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(261, MaxY - 73, 192, 9) && !hl2) {
			ScreenHighlightArea(261, MaxY - 75, 108, 11, "Upper Texture", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(261, MaxY - 73, 192, 9) && hl2) {
			foo = BLACK;
			strncpy( texname, SideDefs[ sd1].tex1, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s1].ceilh > Sectors[ s2].ceilh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
			ScreenHighlightArea(261, MaxY - 75, 108, 11, "Upper Texture", DARKGRAY, foo);
			hl2 = FALSE;
		}
		if (MouseInArea(261, MaxY - 63, 192, 9) && !hl3) {
			ScreenHighlightArea(261, MaxY - 65, 108, 11, "Lower Texture", BLUE, WHITE);
			hl3 = TRUE;
		}
		if (!MouseInArea(261, MaxY - 63, 192, 9) && hl3) {
			foo = BLACK;
			strncpy( texname, SideDefs[ sd1].tex2, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s1].floorh < Sectors[ s2].floorh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
			ScreenHighlightArea(261, MaxY - 65, 108, 11, "Lower Texture", DARKGRAY, foo);
			hl3 = FALSE;
		}
		if (MouseInArea(261, MaxY - 53, 192, 9) && !hl4) {
			ScreenHighlightArea(261, MaxY - 55, 108, 11, "Tex. X Offset", BLUE, WHITE);
			hl4 = TRUE;
		}
		if (!MouseInArea(261, MaxY - 53, 192, 9) && hl4) {
			ScreenHighlightArea(261, MaxY - 55, 108, 11, "Tex. X Offset", DARKGRAY, BLACK);
			hl4 = FALSE;
		}
		if (MouseInArea(261, MaxY - 43, 192, 9) && !hl5) {
			ScreenHighlightArea(261, MaxY - 45, 108, 11, "Tex. Y Offset", BLUE, WHITE);
			hl5 = TRUE;
		}
		if (!MouseInArea(261, MaxY - 43, 192, 9) && hl5) {
			ScreenHighlightArea(261, MaxY - 45, 108, 11, "Tex. Y Offset", DARKGRAY, BLACK);
			hl5 = FALSE;
		}
		if (MouseInArea(261, MaxY - 33, 192, 9) && !hl1a) {
			ScreenHighlightArea(261, MaxY - 35, 108, 11, "Sector Height", DARKERGRAY, LIGHTGRAY);
			hl1a = TRUE;
		}
		if (!MouseInArea(261, MaxY - 33, 192, 9) && hl1a) {
			ScreenHighlightArea(261, MaxY - 35, 108, 11, "Sector Height", DARKGRAY, LIGHTGRAY);
			hl1a = FALSE;
		}
		if (MouseInArea(261, MaxY - 23, 192, 9) && !hl6) {
			ScreenHighlightArea(261, MaxY - 25, 108, 11, "Sector", BLUE, WHITE);
			hl6 = TRUE;
		}
		if (!MouseInArea(261, MaxY - 23, 192, 9) && hl6) {
			ScreenHighlightArea(261, MaxY - 25, 108, 11, "Sector", DARKGRAY, BLACK);
			hl6 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(261, MaxY - 83, 192, 9))
			return FunctionLineDefWallTextures(CurObject, Selected, 1, 3);
		if (buttons == 1 && MouseClickedArea(261, MaxY - 73, 192, 9))
			return FunctionLineDefWallTextures(CurObject, Selected, 1, 1);
		if (buttons == 1 && MouseClickedArea(261, MaxY - 63, 192, 9))
			return FunctionLineDefWallTextures(CurObject, Selected, 1, 2);
		if (buttons == 1 && MouseClickedArea(261, MaxY - 53, 192, 9))
			return FunctionLineDefXoffset(CurObject, Selected, 1);
		if (buttons == 1 && MouseClickedArea(261, MaxY - 43, 192, 9))
			return FunctionLineDefYoffset(CurObject, Selected, 1);
		if (buttons == 1 && MouseClickedArea(261, MaxY - 23, 192, 9))
			return FunctionLineDefSectorRef(CurObject, Selected, 1);
	}
	ScreenHighlightArea(261, MaxY - 85, 108, 11, "Norm. Texture", DARKGRAY, BLACK);
			foo = BLACK;
			strncpy( texname, SideDefs[ sd1].tex1, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s1].ceilh > Sectors[ s2].ceilh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
	ScreenHighlightArea(261, MaxY - 75, 108, 11, "Upper Texture", DARKGRAY, foo);
			foo = BLACK;
			strncpy( texname, SideDefs[ sd1].tex2, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s1].floorh < Sectors[ s2].floorh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
	ScreenHighlightArea(261, MaxY - 65, 108, 11, "Lower Texture", DARKGRAY, foo);
	ScreenHighlightArea(261, MaxY - 55, 108, 11, "Tex. X Offset", DARKGRAY, BLACK);
	ScreenHighlightArea(261, MaxY - 45, 108, 11, "Tex. Y Offset", DARKGRAY, BLACK);
	ScreenHighlightArea(261, MaxY - 35, 108, 11, "Sector Height", DARKGRAY, LIGHTGRAY);
	ScreenHighlightArea(261, MaxY - 25, 108, 11, "Sector", DARKGRAY, BLACK);
	return 0;
}

BCINT OnScreenEditingSideDef2(BCINT CurObject, SelPtr Selected)
{
	BCINT MouseX, MouseY, buttons;
	BCINT MaxX, MaxY, foo;
	BCINT s1 = -1, s2 = -1, sd1 = -1, sd2 = -1;
	Bool hl1, hl2, hl3, hl4, hl5, hl6;
	Bool hl1a;
	char texname[9];

	sd1 = LineDefs[ CurObject].sidedef1;
	sd2 = LineDefs[ CurObject].sidedef2;
	if (sd1 >= 0)
		s1 = SideDefs[ sd1].sector;
	else
		s1 = -1;
	if (sd2 >= 0)
		s2 = SideDefs[ sd2].sector;
	else
		s2 = -1;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl4 = FALSE;
	hl5 = FALSE;
	hl6 = FALSE;
	hl1a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(450, MaxY - 96, 190, 84)) {
		if (bioskey(1))
			return bioskey(1); 
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(450, MaxY - 83, 190, 9) && !hl1) {
			ScreenHighlightArea(452, MaxY - 85, 108, 11, "Norm. Texture", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(450, MaxY - 83, 190, 9) && hl1) {
			ScreenHighlightArea(452, MaxY - 85, 108, 11, "Norm. Texture", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(450, MaxY - 73, 190, 9) && !hl2) {
			ScreenHighlightArea(452, MaxY - 75, 108, 11, "Upper Texture", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(450, MaxY - 73, 190, 9) && hl2) {
			foo = BLACK;
			strncpy( texname, SideDefs[ sd2].tex1, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s2].ceilh > Sectors[ s1].ceilh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
			ScreenHighlightArea(452, MaxY - 75, 108, 11, "Upper Texture", DARKGRAY, foo);
			hl2 = FALSE;
		}
		if (MouseInArea(450, MaxY - 63, 190, 9) && !hl3) {
			ScreenHighlightArea(452, MaxY - 65, 108, 11, "Lower Texture", BLUE, WHITE);
			hl3 = TRUE;
		}
		if (!MouseInArea(450, MaxY - 63, 190, 9) && hl3) {
			foo = BLACK;
			strncpy( texname, SideDefs[ sd2].tex2, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s2].floorh < Sectors[ s1].floorh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
			ScreenHighlightArea(452, MaxY - 65, 108, 11, "Lower Texture", DARKGRAY, foo);
			hl3 = FALSE;
		}
		if (MouseInArea(450, MaxY - 53, 190, 9) && !hl4) {
			ScreenHighlightArea(452, MaxY - 55, 108, 11, "Tex. X Offset", BLUE, WHITE);
			hl4 = TRUE;
		}
		if (!MouseInArea(450, MaxY - 53, 190, 9) && hl4) {
			ScreenHighlightArea(452, MaxY - 55, 108, 11, "Tex. X Offset", DARKGRAY, BLACK);
			hl4 = FALSE;
		}
		if (MouseInArea(450, MaxY - 43, 190, 9) && !hl5) {
			ScreenHighlightArea(452, MaxY - 45, 108, 11, "Tex. Y Offset", BLUE, WHITE);
			hl5 = TRUE;
		}
		if (!MouseInArea(450, MaxY - 43, 190, 9) && hl5) {
			ScreenHighlightArea(452, MaxY - 45, 108, 11, "Tex. Y Offset", DARKGRAY, BLACK);
			hl5 = FALSE;
		}
		if (MouseInArea(450, MaxY - 33, 190, 9) && !hl1a) {
			ScreenHighlightArea(452, MaxY - 35, 108, 11, "Sector Height", DARKERGRAY, LIGHTGRAY);
			hl1a = TRUE;
		}
		if (!MouseInArea(450, MaxY - 33, 190, 9) && hl1a) {
			ScreenHighlightArea(452, MaxY - 35, 108, 11, "Sector Height", DARKGRAY, LIGHTGRAY);
			hl1a = FALSE;
		}
		if (MouseInArea(450, MaxY - 23, 190, 9) && !hl6) {
			ScreenHighlightArea(452, MaxY - 25, 108, 11, "Sector", BLUE, WHITE);
			hl6 = TRUE;
		}
		if (!MouseInArea(450, MaxY - 23, 190, 9) && hl6) {
			ScreenHighlightArea(452, MaxY - 25, 108, 11, "Sector", DARKGRAY, BLACK);
			hl6 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(450, MaxY - 83, 190, 9))
			return FunctionLineDefWallTextures(CurObject, Selected, 2, 3);
		if (buttons == 1 && MouseClickedArea(450, MaxY - 73, 190, 9))
			return FunctionLineDefWallTextures(CurObject, Selected, 2, 1);
		if (buttons == 1 && MouseClickedArea(450, MaxY - 63, 190, 9))
			return FunctionLineDefWallTextures(CurObject, Selected, 2, 2);
		if (buttons == 1 && MouseClickedArea(450, MaxY - 53, 190, 9))
			return FunctionLineDefXoffset(CurObject, Selected, 2);
		if (buttons == 1 && MouseClickedArea(450, MaxY - 43, 190, 9))
			return FunctionLineDefYoffset(CurObject, Selected, 2);
		if (buttons == 1 && MouseClickedArea(450, MaxY - 23, 190, 9))
			return FunctionLineDefSectorRef(CurObject, Selected, 2);
	}
	ScreenHighlightArea(452, MaxY - 85, 108, 11, "Norm. Texture", DARKGRAY, BLACK);
			foo = BLACK;
			strncpy( texname, SideDefs[ sd2].tex1, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s2].ceilh > Sectors[ s1].ceilh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
	ScreenHighlightArea(452, MaxY - 75, 108, 11, "Upper Texture", DARKGRAY, foo);
			foo = BLACK;
			strncpy( texname, SideDefs[ sd2].tex2, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s2].floorh < Sectors[ s1].floorh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					foo = RED;
			}
			else
				foo = GRAY;
	ScreenHighlightArea(452, MaxY - 65, 108, 11, "Lower Texture", DARKGRAY, foo);
	ScreenHighlightArea(452, MaxY - 55, 108, 11, "Tex. X Offset", DARKGRAY, BLACK);
	ScreenHighlightArea(452, MaxY - 45, 108, 11, "Tex. Y Offset", DARKGRAY, BLACK);
	ScreenHighlightArea(452, MaxY - 35, 108, 11, "Sector Height", DARKGRAY, LIGHTGRAY);
	ScreenHighlightArea(452, MaxY - 25, 108, 11, "Sector", DARKGRAY, BLACK);
	return 0;
}

BCINT OnScreenEditingSideDefTextures(BCINT CurObject, SelPtr Selected)
{
	BCINT  MouseX, MouseY, buttons, key;
	BCINT MaxX, MaxY, x0, y0, sd1, sd2;
	Bool hl1, hl2, hl3, hl4, hl5, hl6;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl4 = FALSE;
	hl5 = FALSE;
	hl6 = FALSE;

	sd1 = LineDefs[ CurObject].sidedef1;
	sd2 = LineDefs[ CurObject].sidedef2;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	x0 = MaxX - 140;
	y0 = MaxY - 223;
	if (VideoMode < 3)
		y0 -= 91;

	while (MouseInArea(x0, y0, 136, 210)) {
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(x0 + 4, y0 + 4, 68, 64) && !hl1) {
			ScreenBoxArea( x0 + 3, y0 + 3, 65, 66, BLUE);
			ScreenBoxArea( x0 + 2, y0 + 2, 67, 68, BLUE);
			hl1 = TRUE;
		}
		if (!MouseInArea(x0 + 4, y0 + 4, 68, 64) && hl1) {
			ScreenBoxArea( x0 + 3, y0 + 3, 65, 66, DARKGRAY);
			ScreenBoxArea( x0 + 2, y0 + 2, 67, 68, DARKGRAY);
			hl1 = FALSE;
		}
		if (MouseInArea(x0 + 4, y0 + 73, 68, 64) && !hl2) {
			ScreenBoxArea( x0 + 3, y0 + 72, 65, 66, BLUE);
			ScreenBoxArea( x0 + 2, y0 + 71, 67, 68, BLUE);
			hl2 = TRUE;
		}
		if (!MouseInArea(x0 + 4, y0 + 73, 68, 64) && hl2) {
			ScreenBoxArea( x0 + 3, y0 + 72, 65, 66, DARKGRAY);
			ScreenBoxArea( x0 + 2, y0 + 71, 67, 68, DARKGRAY);
			hl2 = FALSE;
		}
		if (MouseInArea(x0 + 4, y0 + 142, 68, 64) && !hl3) {
			ScreenBoxArea( x0 + 3, y0 + 141, 65, 66, BLUE);
			ScreenBoxArea( x0 + 2, y0 + 140, 67, 68, BLUE);
			hl3 = TRUE;
		}
		if (!MouseInArea(x0 + 4, y0 + 142, 68, 64) && hl3) {
			ScreenBoxArea( x0 + 3, y0 + 141, 65, 66, DARKGRAY);
			ScreenBoxArea( x0 + 2, y0 + 140, 67, 68, DARKGRAY);
			hl3 = FALSE;
		}
		if (MouseInArea(x0 + 74, y0 + 4, 64, 64) && !hl4) {
			ScreenBoxArea( x0 + 71, y0 + 3, 65, 66, BLUE);
			ScreenBoxArea( x0 + 70, y0 + 2, 67, 68, BLUE);
			hl4 = TRUE;
		}
		if (!MouseInArea(x0 + 74, y0 + 4, 64, 64) && hl4) {
			ScreenBoxArea( x0 + 71, y0 + 3, 65, 66, DARKGRAY);
			ScreenBoxArea( x0 + 70, y0 + 2, 67, 68, DARKGRAY);
			hl4 = FALSE;
		}
		if (MouseInArea(x0 + 74, y0 + 73, 64, 64) && !hl5) {
			ScreenBoxArea( x0 + 71, y0 + 72, 65, 66, BLUE);
			ScreenBoxArea( x0 + 70, y0 + 71, 67, 68, BLUE);
			hl5 = TRUE;
		}
		if (!MouseInArea(x0 + 74, y0 + 73, 64, 64) && hl5) {
			ScreenBoxArea( x0 + 71, y0 + 72, 65, 66, DARKGRAY);
			ScreenBoxArea( x0 + 70, y0 + 71, 67, 68, DARKGRAY);
			hl5 = FALSE;
		}
		if (MouseInArea(x0 + 74, y0 + 142, 64, 64) && !hl6) {
			ScreenBoxArea( x0 + 71, y0 + 141, 65, 66, BLUE);
			ScreenBoxArea( x0 + 70, y0 + 140, 67, 68, BLUE);
			hl6 = TRUE;
		}
		if (!MouseInArea(x0 + 74, y0 + 142, 64, 64) && hl6) {
			ScreenBoxArea( x0 + 71, y0 + 141, 65, 66, DARKGRAY);
			ScreenBoxArea( x0 + 70, y0 + 140, 67, 68, DARKGRAY);
			hl6 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(x0 + 4, y0 + 4, 68, 64))
			return FunctionLineDefBrowseWallTextures(CurObject, Selected, 1, 1);
		if (buttons == 1 && MouseClickedArea(x0 + 4, y0 + 73, 68, 64))
			return FunctionLineDefBrowseWallTextures(CurObject, Selected, 1, 3);
		if (buttons == 1 && MouseClickedArea(x0 + 4, y0 + 142, 68, 64))
			return FunctionLineDefBrowseWallTextures(CurObject, Selected, 1, 2);
		if (buttons == 1 && MouseClickedArea(x0 + 74, y0 + 4, 64, 64))
			return FunctionLineDefBrowseWallTextures(CurObject, Selected, 2, 1);
		if (buttons == 1 && MouseClickedArea(x0 + 74, y0 + 73, 64, 64))
			return FunctionLineDefBrowseWallTextures(CurObject, Selected, 2, 3);
		if (buttons == 1 && MouseClickedArea(x0 + 74, y0 + 142, 64, 64))
			return FunctionLineDefBrowseWallTextures(CurObject, Selected, 2, 2);

		if (bioskey(1)) { 
			if (bioskey(1) == MINUS || bioskey(1) == KEY_N || bioskey(1) == KEY_U || bioskey(1) == KEY_L || bioskey(1) == KEY_O) 
				key = bioskey(0);
			else 	
				return bioskey(1);

			if (key == MINUS && MouseInArea(x0 + 4, y0 + 4, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, "-", 1, 1);
			else if (key == KEY_U && MouseInArea(x0 + 4, y0 + 4, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex1, 1, 1);
			else if (key == KEY_N && MouseInArea(x0 + 4, y0 + 4, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex3, 1, 1);
			else if (key == KEY_L && MouseInArea(x0 + 4, y0 + 4, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex2, 1, 1);
			else if (key == KEY_O && MouseInArea(x0 + 4, y0 + 4, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex1, 1, 1);

			else if (key == MINUS && MouseInArea(x0 + 4, y0 + 73, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, "-", 1, 3);
			else if (key == KEY_U && MouseInArea(x0 + 4, y0 + 73, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex1, 1, 3);
			else if (key == KEY_N && MouseInArea(x0 + 4, y0 + 73, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex3, 1, 3);
			else if (key == KEY_L && MouseInArea(x0 + 4, y0 + 73, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex2, 1, 3);
			else if (key == KEY_O && MouseInArea(x0 + 4, y0 + 73, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex3, 1, 3);

			else if (key == MINUS && MouseInArea(x0 + 4, y0 + 142, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, "-", 1, 2);
			else if (key == KEY_U && MouseInArea(x0 + 4, y0 + 142, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex1, 1, 2);
			else if (key == KEY_N && MouseInArea(x0 + 4, y0 + 142, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex3, 1, 2);
			else if (key == KEY_L && MouseInArea(x0 + 4, y0 + 142, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex2, 1, 2);
			else if (key == KEY_O && MouseInArea(x0 + 4, y0 + 142, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex2, 1, 2);

			else if (key == MINUS && MouseInArea(x0 + 74, y0 + 4, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, "-", 2, 1);
			else if (key == KEY_U && MouseInArea(x0 + 74, y0 + 4, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex1, 2, 1);
			else if (key == KEY_N && MouseInArea(x0 + 74, y0 + 4, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex3, 2, 1);
			else if (key == KEY_L && MouseInArea(x0 + 74, y0 + 4, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex2, 2, 1);
			else if (key == KEY_O && MouseInArea(x0 + 74, y0 + 4, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex1, 2, 1);

			else if (key == MINUS && MouseInArea(x0 + 74, y0 + 73, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, "-", 2, 3);
			else if (key == KEY_U && MouseInArea(x0 + 74, y0 + 73, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex1, 2, 3);
			else if (key == KEY_N && MouseInArea(x0 + 74, y0 + 73, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex3, 2, 3);
			else if (key == KEY_L && MouseInArea(x0 + 74, y0 + 73, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex2, 2, 3);
			else if (key == KEY_O && MouseInArea(x0 + 74, y0 + 73, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex3, 2, 3);

			else if (key == MINUS && MouseInArea(x0 + 74, y0 + 142, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, "-", 2, 2);
			else if (key == KEY_U && MouseInArea(x0 + 74, y0 + 142, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex1, 2, 2);
			else if (key == KEY_N && MouseInArea(x0 + 74, y0 + 142, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex3, 2, 2);
			else if (key == KEY_L && MouseInArea(x0 + 74, y0 + 142, 64, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd2].tex2, 2, 2);
			else if (key == KEY_O && MouseInArea(x0 + 74, y0 + 142, 68, 64))
				return FunctionLineDefWallTexture(CurObject, Selected, SideDefs[sd1].tex2, 2, 2);
		}
	}
	ScreenBoxArea( x0 + 3, y0 + 3, 65, 66, DARKGRAY);
	ScreenBoxArea( x0 + 2, y0 + 2, 67, 68, DARKGRAY);
	ScreenBoxArea( x0 + 3, y0 + 72, 65, 66, DARKGRAY);
	ScreenBoxArea( x0 + 2, y0 + 71, 67, 68, DARKGRAY);
	ScreenBoxArea( x0 + 3, y0 + 141, 65, 66, DARKGRAY);
	ScreenBoxArea( x0 + 2, y0 + 140, 67, 68, DARKGRAY);
	ScreenBoxArea( x0 + 71, y0 + 3, 65, 66, DARKGRAY);
	ScreenBoxArea( x0 + 70, y0 + 2, 67, 68, DARKGRAY);
	ScreenBoxArea( x0 + 71, y0 + 72, 65, 66, DARKGRAY);
	ScreenBoxArea( x0 + 70, y0 + 71, 67, 68, DARKGRAY);
	ScreenBoxArea( x0 + 71, y0 + 141, 65, 66, DARKGRAY);
	ScreenBoxArea( x0 + 70, y0 + 140, 67, 68, DARKGRAY);
	return 0;
}

BCINT OnScreenEditingSectors(BCINT CurObject, SelPtr Selected)
{
	BCINT MouseX, MouseY, buttons;
	BCINT MaxX, MaxY;
	Bool hl1, hl2, hl3, hl4, hl5, hl1a;

	hl1 = FALSE;
	hl2 = FALSE;
	hl3 = FALSE;
	hl4 = FALSE;
	hl5 = FALSE;
	hl1a = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(2, MaxY - 92, 280, 76)) {
		if (bioskey(1))
			return bioskey(1); 
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(2, MaxY - 73, 280, 9) && !hl1) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Floor", BLUE, WHITE);
			hl1 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 73, 280, 9) && hl1) {
			ScreenHighlightArea(2, MaxY - 75, 76, 11, "Floor", DARKGRAY, BLACK);
			hl1 = FALSE;
		}
		if (MouseInArea(2, MaxY - 63, 280, 9) && !hl2) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, "Ceiling", BLUE, WHITE);
			hl2 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 63, 280, 9) && hl2) {
			ScreenHighlightArea(2, MaxY - 65, 76, 11, "Ceiling", DARKGRAY, BLACK);
			hl2 = FALSE;
		}
		if (MouseInArea(2, MaxY - 53, 280, 9) && !hl1a) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, "Height", DARKERGRAY, LIGHTGRAY);
			hl1a = TRUE;
		}
		if (!MouseInArea(2, MaxY - 53, 280, 9) && hl1a) {
			ScreenHighlightArea(2, MaxY - 55, 76, 11, "Height", DARKGRAY, LIGHTGRAY);
			hl1a = FALSE;
		}
		if (MouseInArea(2, MaxY - 43, 280, 9) && !hl3) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, "Light", BLUE, WHITE);
			hl3 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 43, 280, 9) && hl3) {
			ScreenHighlightArea(2, MaxY - 45, 76, 11, "Light", DARKGRAY, BLACK);
			hl3 = FALSE;
		}
		if (MouseInArea(2, MaxY - 33, 280, 9) && !hl4) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, "Tag", BLUE, WHITE);
			hl4 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 33, 280, 9) && hl4) {
			ScreenHighlightArea(2, MaxY - 35, 76, 11, "Tag", DARKGRAY, BLACK);
			hl4 = FALSE;
		}
		if (MouseInArea(2, MaxY - 23, 280, 9) && !hl5) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, "Type", BLUE, WHITE);
			hl5 = TRUE;
		}
		if (!MouseInArea(2, MaxY - 23, 280, 9) && hl5) {
			ScreenHighlightArea(2, MaxY - 25, 76, 11, "Type", DARKGRAY, BLACK);
			hl5 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(2, MaxY - 73, 280, 9))
			return FunctionSectorFloorHeight(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 63, 280, 9))
			return FunctionSectorCeilingHeight(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 43, 280, 9))
			return FunctionSectorLightLevel(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 33, 280, 9))
			return FunctionSectorTag(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(2, MaxY - 23, 280, 9))
			return FunctionSectorType(CurObject, Selected);
	}
	ScreenHighlightArea(2, MaxY - 75, 76, 11, "Floor", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 65, 76, 11, "Ceiling", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 55, 76, 11, "Height", DARKGRAY, LIGHTGRAY);
	ScreenHighlightArea(2, MaxY - 45, 76, 11, "Light", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 35, 76, 11, "Tag", DARKGRAY, BLACK);
	ScreenHighlightArea(2, MaxY - 25, 76, 11, "Type", DARKGRAY, BLACK);
	return 0;
}

BCINT OnScreenEditingSectorFlats(BCINT CurObject, SelPtr Selected)
{
	BCINT MouseX, MouseY, buttons, key;
	BCINT MaxX, MaxY;
	Bool hl1, hl2;
	SelPtr cur;

	hl1 = FALSE;
	hl2 = FALSE;

	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	MaxX = ScrMaxX;

	while (MouseInArea(MaxX - 72, MaxY - 148, 72, 135)) {
		if (bioskey(1))
			return bioskey(1); 
		GetMouseCoords(&MouseX, &MouseY, &buttons);

		if (MouseInArea(MaxX - 72, MaxY - 148, 72, 68) && !hl1) {
			ScreenBoxArea( MaxX - 69, MaxY - 149, 65, 65, BLUE);
			ScreenBoxArea( MaxX - 70, MaxY - 150, 67, 67, BLUE);
			hl1 = TRUE;
		}
		if (!MouseInArea(MaxX - 72, MaxY - 148, 72, 68) && hl1) {
			ScreenBoxArea( MaxX - 69, MaxY - 149, 65, 65, DARKGRAY);
			ScreenBoxArea( MaxX - 70, MaxY - 150, 67, 67, DARKGRAY);
			hl1 = FALSE;
		}
		if (MouseInArea(MaxX - 72, MaxY - 80, 72, 68) && !hl2) {
			ScreenBoxArea( MaxX - 69, MaxY - 81, 65, 65, BLUE);
			ScreenBoxArea( MaxX - 70, MaxY - 82, 67, 67, BLUE);
			hl2 = TRUE;
		}
		if (!MouseInArea(MaxX - 72, MaxY - 80, 72, 68) && hl2) {
			ScreenBoxArea( MaxX - 69, MaxY - 81, 65, 65, DARKGRAY);
			ScreenBoxArea( MaxX - 70, MaxY - 82, 67, 67, DARKGRAY);
			hl2 = FALSE;
		}

		if (buttons == 1 && MouseClickedArea(MaxX - 72, MaxY - 148, 72, 68)) 
			return FunctionSectorBrowseCeiling(CurObject, Selected);
		if (buttons == 1 && MouseClickedArea(MaxX - 72, MaxY - 80, 72, 68)) 
			return FunctionSectorBrowseFloor(CurObject, Selected);

		if (bioskey(1)) { 
			if (bioskey(1) == KEY_F || bioskey(1) == KEY_C) 
				key = bioskey(0);
			else 	
				return bioskey(1);

			if (key == KEY_F && MouseInArea(MaxX - 72, MaxY - 148, 72, 68)) {
				for (cur = Selected; cur; cur = cur->next)
					if (cur->objnum != CurObject) 
						strncpy( Sectors[ cur->objnum].ceilt, Sectors[CurObject].floort, 8);
				if (CurObject >= 0)
						strncpy( Sectors[ CurObject].ceilt, Sectors[CurObject].floort, 8);
				return REDRAW_ALL;
			}
			if (key == KEY_F && MouseInArea(MaxX - 72, MaxY - 80, 72, 68)) {
				for (cur = Selected; cur; cur = cur->next)
					if (cur->objnum != CurObject) 
						strncpy( Sectors[ cur->objnum].floort, Sectors[CurObject].floort, 8);
				return REDRAW_ALL;
			}

			if (key == KEY_C && MouseInArea(MaxX - 72, MaxY - 80, 72, 68)) {
				for (cur = Selected; cur; cur = cur->next)
					if (cur->objnum != CurObject) 
						strncpy( Sectors[ cur->objnum].floort, Sectors[CurObject].ceilt, 8);
				if (CurObject >= 0)
						strncpy( Sectors[CurObject].floort, Sectors[CurObject].ceilt, 8);
				return REDRAW_ALL;
			}
			if (key == KEY_C && MouseInArea(MaxX - 72, MaxY - 148, 72, 68)) {
				for (cur = Selected; cur; cur = cur->next)
					if (cur->objnum != CurObject) 
						strncpy( Sectors[ cur->objnum].ceilt, Sectors[CurObject].ceilt, 8);
				return REDRAW_ALL;
			}
		}
	}

	ScreenBoxArea( MaxX - 69, MaxY - 149, 65, 65, DARKGRAY);
	ScreenBoxArea( MaxX - 70, MaxY - 150, 67, 67, DARKGRAY);
	ScreenBoxArea( MaxX - 69, MaxY - 81, 65, 65, DARKGRAY);
	ScreenBoxArea( MaxX - 70, MaxY - 82, 67, 67, DARKGRAY);
	return 0;
}
