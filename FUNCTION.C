#include "deu.h"
#include "levels.h"
#include "dialog.h"

#define REDRAW_NONE			0
#define REDRAW_QUICK		1
#define REDRAW_ALL			2
#define REDRAW_INFO			3


BCINT MaxY;
BCINT sd1x;
BCINT sd1y;
BCINT sd2x;
BCINT sd2y;

BCINT FunctionLineDefFlags(BCINT, SelPtr);

void GetCoords()
{
	if (InfoShown)
		MaxY = ScrMaxY;
	else
		MaxY = ScrMaxY + 13;
	sd1x = ScrMaxX - 140;
	sd1y = ScrMaxY - 266;
	sd2x = ScrMaxX - 72;
	sd2y = ScrMaxY - 266;
	if (VideoMode < 3) {
		sd1y -= 91;
		sd2y -= 91;
	}
	if (InfoShown) {
		sd1y -= 13;
		sd2y -= 13;
	}
}

/*******************/
/* THING FUNCTIONS */
/*******************/

BCINT FunctionThingFlag(BCINT CurObject, SelPtr Selected, BCINT Flag)
{
	SelPtr cur;

	Backup (BTHINGS);
	strcpy (BackupText, "Change Thing Flags");
	for (cur = Selected; cur; cur = cur->next)
		if(cur->objnum != CurObject)  
			Things[ cur->objnum].flags ^= 0x01 << (Flag - 1);
	if (CurObject >= 0) 
		Things[ CurObject].flags ^= 0x01 << (Flag - 1);
	MadeChanges = TRUE;
	return REDRAW_INFO;
}

#ifdef GAME_HEXEN
BCINT FunctionThingTID(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (CurObject >= 0)
		val = InputIntegerValuePlain(90, MaxY - 86, 0, 255, Things[ CurObject].tid);
	else
		val = InputIntegerValue( -1, -1, 0, 255, Things[ Selected->objnum].tid);
	if (val >= 0) {
		Backup (BTHINGS);
		strcpy (BackupText, "Change Thing TID");
		for(cur = Selected; cur; cur = cur->next) 
			if(cur->objnum != CurObject)  
				Things[cur->objnum].tid = val;
		MadeChanges = TRUE;
		if (CurObject >= 0) {
			Things[ CurObject].tid = val;
			return REDRAW_ALL;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionThingSpecial(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	ld_class *c;
	ld_type *t;
	char *menuarr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	int val, i;
	Bool input_integer = FALSE;
	
	c = Linedef_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}
	menuarr[i++] = "Enter A Decimal Value";
	
	val = DisplayMenuArray( -1, -1, "Choose A Special Class:",
						   i, NULL, menuarr, dummy);
	if(val == 0)
		val = -1;
	
	if(i == val) {
		input_integer = TRUE;
		val = InputIntegerValue(-1, -1, 0, 255, 0);
	}
	
	if(val >= 0) {
		if(!input_integer) {
			c = Linedef_classes;
			while(--val)
				c = c->next;
			/* now c points to the class we are
			   going to display a submenu for */
			
			t = c->types;
			for(i = 0; (t && i < 30); t = t->next) {
				menuarr[i++] = t->longname;
			}
			
			if(i == 1) {
				val = c->types->type; 
				strcpy (BackupText, "Change Special Type");
				Backup (BTHINGS);
				for (cur = Selected; cur; cur = cur->next) {
					if(cur->objnum != CurObject) 
						Things[ cur->objnum].special = val;
				}
				if (CurObject >= 0)
					Things[ CurObject].special = val;
				MadeChanges = TRUE;
			}
			else {
				val = DisplayMenuArray(-1, -1, "Choose A Special Type:",
									   i, NULL, menuarr, dummy);
				if(val == 0)
					val = -1;
			}
		}
		
		if (i >= 2 && val >= 0) {
			strcpy (BackupText, "Change Special Type");
			Backup (BTHINGS);
			if(!input_integer) {
				t = c->types;
				while(--val)
					t = t->next;
				val = t->type;
			}
			
			for (cur = Selected; cur; cur = cur->next) {
				if(cur->objnum != CurObject)  
					Things[ cur->objnum].special = val;
			}
			if (CurObject >= 0)
				Things[ CurObject].special = val;
			MadeChanges = TRUE;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionThingZPos(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	val = InputIntegerValue( -1, -1, -32767, 32767, Things[ Selected->objnum].zpos);
	if (val >= MapMinX) {
		strcpy (BackupText, "Change Thing Z Position");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].zpos = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingPuzzle(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectPuzzleItem();
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Puzzle Item");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingSectorSound(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectSectorSound();
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Sector Sound");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingHexenAngle(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectHexenAngle();
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Angle Argument");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingNegative(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectNegative();
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Negative Argument");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingSpawnType(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	ShowSprite = TRUE;
	val = SelectSpawnType();
	ShowSprite = FALSE;
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Spawn Type");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingKeyType(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectKeyType();
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Key Type");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingArg(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (Arg == 1) 
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 66, 0, 255, Things[ CurObject].arg1);
		else
			val = InputIntegerValue(-1, -1, 0, 255, Things[cur->objnum].arg1);
	else if (Arg == 2)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 56, 0, 255, Things[ CurObject].arg2);
		else
			val = InputIntegerValue(-1, -1, 0, 255, Things[cur->objnum].arg2);
	else if (Arg == 3)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 46, 0, 255, Things[ CurObject].arg3);
		else
			val = InputIntegerValue(-1, -1, 0, 255, Things[cur->objnum].arg3);
	else if (Arg == 4)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 36, 0, 255, Things[ CurObject].arg4);
		else
			val = InputIntegerValue(-1, -1, 0, 255, Things[cur->objnum].arg4);
	else if (Arg == 5)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 26, 0, 255, Things[ CurObject].arg5);
		else
			val = InputIntegerValue(-1, -1, 0, 255, Things[cur->objnum].arg5);
	if (val >= 0) {
		strcpy (BackupText, "Change Thing Arguments");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				Things[cur->objnum].arg1 = val;
			else if (Arg == 2)
				Things[cur->objnum].arg2 = val;
			else if (Arg == 3)
				Things[cur->objnum].arg3 = val;
			else if (Arg == 4)
				Things[cur->objnum].arg4 = val;
			else if (Arg == 5)
				Things[cur->objnum].arg5 = val;
		if (CurObject >= 0) {
			if (Arg == 1)
				Things[CurObject].arg1 = val;
			else if (Arg == 2)
				Things[CurObject].arg2 = val;
			else if (Arg == 3)
				Things[CurObject].arg3 = val;
			else if (Arg == 4)
				Things[CurObject].arg4 = val;
			else if (Arg == 5)
				Things[CurObject].arg5 = val;
			return REDRAW_ALL;
		}
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}
#endif

BCINT FunctionThingType(BCINT CurObject, SelPtr Selected)
{
	int val;

	ShowSprite = TRUE;
	val = SelectThingType();
	ShowSprite = FALSE;
	if(val >= 0) {
		SelPtr cur;
		Backup (BTHINGS);
		strcpy (BackupText, "Change Thing Type");
		for(cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject)  
				Things[ cur->objnum].type = val;
		}
		if (CurObject >= 0)
			Things[ CurObject].type = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingFlags(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val, foo;

	MenuDefault = 0;
	do {
		MadeChanges = FALSE;
		if (CurObject >= 0)
			foo = CurObject;
		else
			foo = Selected->objnum;
		val = DisplayMenu( -1, -1, "Toggle the flags:",
			  GetThingFlag( foo, 1),
			  GetThingFlag( foo, 2),
			  GetThingFlag( foo, 3),
			  GetThingFlag( foo, 4),
			  GetThingFlag( foo, 5),
			  GetThingFlag( foo, 6),
			  GetThingFlag( foo, 7),
#ifdef GAME_HEXEN
			  GetThingFlag( foo, 8),
			  GetThingFlag( foo, 9),
			  GetThingFlag( foo, 10),
			  GetThingFlag( foo, 11),
			  "-",
			  "Enter A Decimal Value",
			  "-",
			  "Exit Menu",
			  NULL);
		if (val >= 1 && val <= 11) {
#else
			  "-",
			  "Enter A Decimal Value",
			  "-",
			  "Exit Menu",
			  NULL);
		if (val >= 1 && val <= 7) {
#endif
			strcpy (BackupText, "Change Thing Flags");
			for (cur = Selected; cur; cur = cur->next) 
				if(cur->objnum != CurObject)  
					Things[ cur->objnum].flags ^= 0x01 << (val - 1);
			if (CurObject >= 0)
				Things[ CurObject].flags ^= 0x01 << (val - 1);
			MadeChanges = TRUE;
			MenuDefault = val - 1;
		}
#ifdef GAME_HEXEN
		else if (val == 13) {										   
			MenuDefault = 12;
			val = InputIntegerValue( -1, -1, 0, 8191, Things[ foo].flags);
#else
		else if (val == 9) {										   
			MenuDefault = 8;
			val = InputIntegerValue( -1, -1, 0, 32767, Things[ foo].flags);
#endif
			if (val >= 0) {
				strcpy (BackupText, "Change Thing Flags");
				for (cur = Selected; cur; cur = cur->next) 
					if(cur->objnum != CurObject)  
						Things[ cur->objnum].flags = val;
				if (CurObject >= 0)
					Things[ CurObject].flags = val;
				MadeChanges = TRUE;
			}
		}
		else {
			MenuDefault = 0;
			return REDRAW_ALL;
		}
	} while (MadeChanges == TRUE);
	MenuDefault = 0;
	return REDRAW_ALL;
}

BCINT FunctionThingAngle(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;
	switch (DisplayMenu(-1, -1, "Select Angle",
						"North",
						"NorthEast",
						"East",
						"SouthEast",
						"South",
						"SouthWest",
						"West",
						"NorthWest",
						"-",
						"Enter A Decimal Value",
						NULL)) {
	case 1:
		strcpy (BackupText, "Change Thing Angle To North");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 90;
		MadeChanges = TRUE;
		break;
	case 2:
		strcpy (BackupText, "Change Thing Angle To NorthEast");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 45;
		MadeChanges = TRUE;
		break;
	case 3:
		strcpy (BackupText, "Change Thing Angle To East");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 0;
		MadeChanges = TRUE;
		break;
	case 4:
		strcpy (BackupText, "Change Thing Angle To SouthEast");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 315;
		MadeChanges = TRUE;
		break;
	case 5:
		strcpy (BackupText, "Change Thing Angle To South");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 270;
		MadeChanges = TRUE;
		break;
	case 6:
		strcpy (BackupText, "Change Thing Angle To SouthWest");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 225;
		MadeChanges = TRUE;
		break;
	case 7:
		strcpy (BackupText, "Change Thing Angle To West");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 180;
		MadeChanges = TRUE;
		break;
	case 8:
		strcpy (BackupText, "Change Thing Angle To NorthWest");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].angle = 135;
		MadeChanges = TRUE;
		break;
	case 10:
		val = InputIntegerValue( -1, -1, 0, 255, Things[ Selected->objnum].angle);
		if (val >= 0) {
			strcpy (BackupText, "Change Thing Angle");
			for (cur = Selected; cur; cur = cur->next)
				Things[ cur->objnum].angle = val;
			MadeChanges = TRUE;
		}
		break;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingXPos(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	val = InputIntegerValue( -1, -1, MapMinX, MapMaxX, Things[ Selected->objnum].xpos);
	if (val >= MapMinX) {
		strcpy (BackupText, "Change Thing X Position");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].xpos = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionThingYPos(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	val = InputIntegerValue( -1, -1, MapMinY, MapMaxY, Things[ Selected->objnum].ypos);
	if (val >= MapMinY) {
		strcpy (BackupText, "Change Thing Y Position");
		for (cur = Selected; cur; cur = cur->next)
			Things[ cur->objnum].ypos = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}



/*********************/
/* LINEDEF FUNCTIONS */
/*********************/

#ifdef GAME_HEXEN
BCINT FunctionLineDefPuzzle(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectPuzzleItem();
	if (val >= 0) {
		strcpy (BackupText, "Change LineDef Puzzle Item");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefSectorSound(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectSectorSound();
	if (val >= 0) {
		strcpy (BackupText, "Change LineDefs Sector Sound");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefHexenAngle(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectHexenAngle();
	if (val >= 0) {
		strcpy (BackupText, "Change LineDef Angle Argument");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefNegative(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectNegative();
	if (val >= 0) {
		strcpy (BackupText, "Change LineDef Negative Argument");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefSpawnType(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	ShowSprite = TRUE;
	val = SelectSpawnType();
	ShowSprite = FALSE;
	if (val >= 0) {
		strcpy (BackupText, "Change LineDef Spawn Type");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefKeyType(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	val = SelectKeyType();
	if (val >= 0) {
		strcpy (BackupText, "Change LineDef Key Type");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}
BCINT FunctionLineDefArg(BCINT CurObject, SelPtr Selected, BCINT Arg)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (Arg == 1) 
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 66, 0, 255, LineDefs[ CurObject].arg1);
		else
			val = InputIntegerValue(-1, -1, 0, 255, LineDefs[cur->objnum].arg1);
	else if (Arg == 2)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 56, 0, 255, LineDefs[ CurObject].arg2);
		else
			val = InputIntegerValue(-1, -1, 0, 255, LineDefs[cur->objnum].arg2);
	else if (Arg == 3)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 46, 0, 255, LineDefs[ CurObject].arg3);
		else
			val = InputIntegerValue(-1, -1, 0, 255, LineDefs[cur->objnum].arg3);
	else if (Arg == 4)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 36, 0, 255, LineDefs[ CurObject].arg4);
		else
			val = InputIntegerValue(-1, -1, 0, 255, LineDefs[cur->objnum].arg4);
	else if (Arg == 5)
		if (CurObject >= 0)
			val = InputIntegerValuePlain(90, MaxY - 26, 0, 255, LineDefs[ CurObject].arg5);
		else
			val = InputIntegerValue(-1, -1, 0, 255, LineDefs[cur->objnum].arg5);
	if (val >= 0) {
		strcpy (BackupText, "Change LineDef Arguments");
		for (cur = Selected; cur; cur = cur->next)
			if (Arg == 1)
				LineDefs[cur->objnum].arg1 = val;
			else if (Arg == 2)
				LineDefs[cur->objnum].arg2 = val;
			else if (Arg == 3)
				LineDefs[cur->objnum].arg3 = val;
			else if (Arg == 4)
				LineDefs[cur->objnum].arg4 = val;
			else if (Arg == 5)
				LineDefs[cur->objnum].arg5 = val;
		if (CurObject >= 0) {
			if (Arg == 1)
				LineDefs[CurObject].arg1 = val;
			else if (Arg == 2)
				LineDefs[CurObject].arg2 = val;
			else if (Arg == 3)
				LineDefs[CurObject].arg3 = val;
			else if (Arg == 4)
				LineDefs[CurObject].arg4 = val;
			else if (Arg == 5)
				LineDefs[CurObject].arg5 = val;
			return REDRAW_ALL;
		}
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefTrigger(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	val = DisplayMenu( -1, -1, "Choose Trigger Type:", "Walk", "Switch", "Monster", "Hit", "Push", "Projectile", "-", "Enter A Decimal Value", NULL);
		if (val > 0) {
			for (cur = Selected; cur; cur = cur->next) {
				if(LineDefs[cur->objnum].flags & 0x0400)
					FunctionLineDefFlag(CurObject, Selected, 10);
				if(LineDefs[cur->objnum].flags & 0x0800)
					FunctionLineDefFlag(CurObject, Selected, 11);
				if(LineDefs[cur->objnum].flags & 0x1000)
					FunctionLineDefFlag(CurObject, Selected, 12);
			}
			if (CurObject >= 0 && LineDefs[CurObject].flags & 0x0400)
				FunctionLineDefFlag(CurObject, Selected, 10);
			if (CurObject >= 0 && LineDefs[CurObject].flags & 0x0800)
				FunctionLineDefFlag(CurObject, Selected, 11);
			if (CurObject >= 0 && LineDefs[CurObject].flags & 0x1000)
				FunctionLineDefFlag(CurObject, Selected, 12);
		}
	switch (val) {
		case 2:
			for (cur = Selected; cur; cur = cur->next)
				if(cur->objnum != CurObject)  
					FunctionLineDefFlag(CurObject, Selected, 10);
			if (CurObject >= 0) 
				FunctionLineDefFlag(CurObject, Selected, 10);
			break;
		case 3:
			for (cur = Selected; cur; cur = cur->next)
				if(cur->objnum != CurObject)  
					FunctionLineDefFlag(CurObject, Selected, 11);
			if (CurObject >= 0) 
				FunctionLineDefFlag(CurObject, Selected, 11);
			break;
		case 4:
			for (cur = Selected; cur; cur = cur->next)
				if(cur->objnum != CurObject) { 
					FunctionLineDefFlag(CurObject, Selected, 11);
					FunctionLineDefFlag(CurObject, Selected, 10);
				}
			if (CurObject >= 0) {
				FunctionLineDefFlag(CurObject, Selected, 11);
				FunctionLineDefFlag(CurObject, Selected, 10);
			}
			break;
		case 5:
			for (cur = Selected; cur; cur = cur->next)
				if(cur->objnum != CurObject) 
					FunctionLineDefFlag(CurObject, Selected, 12);
			if (CurObject >= 0) 
				FunctionLineDefFlag(CurObject, Selected, 12);
			break;
		case 6:
			for (cur = Selected; cur; cur = cur->next) 
				if(cur->objnum != CurObject) { 
					FunctionLineDefFlag(CurObject, Selected, 10);
					FunctionLineDefFlag(CurObject, Selected, 12);
				} 
			if (CurObject >= 0) {
				FunctionLineDefFlag(CurObject, Selected, 10);
				FunctionLineDefFlag(CurObject, Selected, 12);
			}
			break;
		case 8:
			val = InputIntegerValue( -1, -1, 0, 8191, LineDefs[ CurObject].flags);
			if (val >= 0) {
				strcpy (BackupText, "Change Trigger Flags");
				for (cur = Selected; cur; cur = cur->next) 
					if(cur->objnum != CurObject)  
						LineDefs[ cur->objnum].flags = val;
				if (CurObject >= 0)
					LineDefs[ CurObject].flags = val;
			}
			break;
	}
	return REDRAW_ALL;
}
#else

BCINT FunctionLineDefTag(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (CurObject >= 0)							/* jff allow tags up to 32767 */
		val = InputIntegerValuePlain(90, MaxY - 76, 0, 32767, LineDefs[ CurObject].tag);
	else
		val = InputIntegerValue( -1, -1, 0, 32767, LineDefs[ Selected->objnum].tag);
	if (val >= 0) {
		Backup (BLINEDEFS);
		strcpy (BackupText, "Change LineDef Tag");
		for(cur = Selected; cur; cur = cur->next) 
			if(cur->objnum != CurObject)  
				LineDefs[cur->objnum].tag = val;
		MadeChanges = TRUE;
		if (CurObject >= 0) {
			LineDefs[ CurObject].tag = val;
			return REDRAW_ALL;
		}
	}
	return REDRAW_ALL;
}
#endif

BCINT FunctionLineDefXoffset(BCINT CurObject, SelPtr Selected, BCINT SideDef)
{
	SelPtr cur;
	int val;
	int sd;

	GetCoords();
	if (SideDef == 1) {
		sd = LineDefs[ CurObject].sidedef1;
		val = InputIntegerValuePlain(381, MaxY - 56, -2048, 2048, SideDefs[ sd].xoff);
	}
	else {
		sd = LineDefs[ CurObject].sidedef2;
		val = InputIntegerValuePlain(572, MaxY - 56, -2048, 2048, SideDefs[ sd].xoff);
	}

	if (val >= -2048) {
		Backup (BLINEDEFS);
		strcpy (BackupText, "Change X Offset");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject) { 
				if (SideDef == 1)
					sd = LineDefs[ cur->objnum].sidedef1;
				else
					sd = LineDefs[ cur->objnum].sidedef2;
				SideDefs[sd].xoff = val;
			} 
		}
		if (SideDef == 1)
			sd = LineDefs[CurObject].sidedef1;
		else
			sd = LineDefs[CurObject].sidedef2;
		SideDefs[sd].xoff = val;
	}
	MadeChanges = TRUE;
	return REDRAW_ALL;
}

BCINT FunctionLineDefYoffset(BCINT CurObject, SelPtr Selected, BCINT SideDef)
{
	SelPtr cur;
	int val;
	int sd;

	GetCoords();
	if (SideDef == 1) {
		sd = LineDefs[ CurObject].sidedef1;
		val = InputIntegerValuePlain(381, MaxY - 46, -2048, 2048, SideDefs[ sd].yoff);
	}
	else {
		sd = LineDefs[ CurObject].sidedef2;
		val = InputIntegerValuePlain(572, MaxY - 46, -2048, 2048, SideDefs[ sd].yoff);
	}

	if (val >= -2048) {
		Backup (BLINEDEFS);
		strcpy (BackupText, "Change Y Offset");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject) { 
				if (SideDef == 1)
					sd = LineDefs[ cur->objnum].sidedef1;
				else
					sd = LineDefs[ cur->objnum].sidedef2;
				SideDefs[sd].yoff = val;
			} 
		}
		if (SideDef == 1)
			sd = LineDefs[CurObject].sidedef1;
		else
			sd = LineDefs[CurObject].sidedef2;
		SideDefs[sd].yoff = val;
	}
	MadeChanges = TRUE;
	return REDRAW_ALL;
}

BCINT FunctionLineDefSectorRef(BCINT CurObject, SelPtr Selected, BCINT SideDef)
{
	SelPtr cur;
	int val;
	int sd;

	GetCoords();
	if (SideDef == 1) {
		sd = LineDefs[ CurObject].sidedef1;
		val = InputIntegerValuePlain(381, MaxY - 26, -1, NumSectors + 1, SideDefs[ sd].sector);
	}
	else {
		sd = LineDefs[ CurObject].sidedef2;
		val = InputIntegerValuePlain(572, MaxY - 26, -1, NumSectors + 1, SideDefs[ sd].sector);
	}

	if (val >= -1) {
		Backup (BLINEDEFS | BSIDEDEFS);
		strcpy (BackupText, "Change SideDef Sector Ref");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject) { 
				if (SideDef == 1) {
					sd = LineDefs[ cur->objnum].sidedef1;
					if (val == -1)
						LineDefs[cur->objnum].sidedef1 = val;
				}
				else {
					sd = LineDefs[ cur->objnum].sidedef2;
					if (val == -1)
						LineDefs[cur->objnum].sidedef2 = val;
				}
				SideDefs[sd].sector = val;
			} 
		}
		if (SideDef == 1) {
			sd = LineDefs[CurObject].sidedef1;
			if (val == -1)
				LineDefs[CurObject].sidedef1 = val;
		}
		else {
			sd = LineDefs[CurObject].sidedef2;
			if (val == -1)
				LineDefs[CurObject].sidedef2 = val;
		}
		SideDefs[sd].sector = val;
	}
	MadeChanges = TRUE;
	return REDRAW_ALL;
}

BCINT FunctionLineDefFlags(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val, foo;

	MenuDefault = 0;
	do {
		MadeChanges = FALSE;
		if (CurObject >= 0)
			foo = CurObject;
		else
			foo = Selected->objnum;
		val = DisplayMenu( -1, -1, "Toggle the flags:",
			  GetTaggedLineDefFlag( foo, 1),
			  GetTaggedLineDefFlag( foo, 2),
			  GetTaggedLineDefFlag( foo, 3),
			  GetTaggedLineDefFlag( foo, 4),
			  GetTaggedLineDefFlag( foo, 5),
			  GetTaggedLineDefFlag( foo, 6),
			  GetTaggedLineDefFlag( foo, 7),
			  GetTaggedLineDefFlag( foo, 8),
			  GetTaggedLineDefFlag( foo, 9),
			  GetTaggedLineDefFlag( foo, 10), //jff 3/21/98 ML_PASSUSE flag
#ifdef GAME_HEXEN
			  "-",
			  "Enter A Decimal Value",
			  "-",
			  "Exit Menu",
			  NULL);
		if (val >= 1 && val <= 10) {
#else
			  "-",
			  "Enter A Decimal Value",
			  "-",
			  "Exit Menu",
			  NULL);
		if (val <=0) {
			MenuDefault = 0;
			return REDRAW_ALL;
		}
		if (val >= 1 && val <= 10) { //jff 3/21/98 add new multi use linedef flag
#endif
			strcpy (BackupText, "Change LineDef Flags");

			for (cur = Selected; cur; cur = cur->next) 
				if(cur->objnum != CurObject)  
					LineDefs[ cur->objnum].flags ^= 0x01 << (val - 1);
			if (CurObject >= 0)
				LineDefs[ CurObject].flags ^= 0x01 << (val - 1);
			MadeChanges = TRUE;
			MenuDefault = val - 1;
		}
#ifdef GAME_HEXEN
		else if (val == 12) {										   
		MenuDefault = 11;
#else
		else if (val == 12) { //jff 3/21/98 one more linedef flag
		MenuDefault = 11;
#endif																		//jff unlimit numeric entry
			val = InputIntegerValue( -1, -1, 0, 32767, LineDefs[ CurObject].flags);
			if (val >= 0) {
				strcpy (BackupText, "Change LineDef Flags");
				for (cur = Selected; cur; cur = cur->next) 
					if(cur->objnum != CurObject)  
						LineDefs[ cur->objnum].flags = val;
				if (CurObject >= 0)
					LineDefs[ CurObject].flags = val;
				MadeChanges = TRUE;
			}
		}
	} while (MadeChanges == TRUE);
	MenuDefault = 0;
	return REDRAW_ALL;
}

BCINT FunctionLineDefSpecial(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	ld_class *c;
	ld_type *t;
	char *menuarr[MENUARRAYMAX]; // jff expand array size
	BCINT dummy[MENUARRAYMAX];	 // to 30
	int i, val;
	Bool input_integer = FALSE;
	
	c = Linedef_classes;
	for(i = 0; (c && i < 30); i++) { //jff expand array size
		menuarr[i] = c->name;
		c = c->next;
	}
	if (BOOMEnable)
	{
		menuarr[i++] = "Generalized Ceiling Mover";
		menuarr[i++] = "Generalized Floor Mover";
		menuarr[i++] = "Generalized Door";
		menuarr[i++] = "Generalized KeyDoor";
  	menuarr[i++] = "Generalized Lift";
  	menuarr[i++] = "Generalized Stairs";
  	menuarr[i++] = "Generalized Crusher";
		menuarr[i++] = "----";
	}
	menuarr[i++] = "Enter A Decimal Value";
	
	val = DisplayMenuArray( -1, -1, "Choose A Special Class:",
						   i, NULL, menuarr, dummy);
	if(val == 0)
		val = -1;

// jff add code for general floor/ceiling movers
	if (BOOMEnable && i-8<=val && i>val)
	{
		input_integer = TRUE;

		if (i-2==val)
		{
			val=-1;
	 		if (DisplayGeneralCrusherDialog())
				val = ComputeCrusherLinedefValue();
		}
		else if (i-3==val)
		{
			val=-1;
	 		if (DisplayGeneralStairsDialog())
				val = ComputeStairsLinedefValue();
		}
		else if (i-4==val)
		{
			val=-1;
	 		if (DisplayGeneralLiftDialog())
				val = ComputeLiftLinedefValue();
		}
		else if (i-5==val)
		{
			val=-1;
	 		if (DisplayGeneralLockedDialog())
				val = ComputeLockedLinedefValue();
		}
		else if (i-6==val)
		{
			val=-1;
	 		if (DisplayGeneralDoorDialog())
				val = ComputeDoorLinedefValue();
		}
		else if (i-7==val)
		{
			val=-1;
	 		if (DisplayGeneralFloorDialog())
				val = ComputeFloorLinedefValue();
		}
		else if (i-8==val)
		{
			val=-1;
	 		if (DisplayGeneralCeilingDialog())
				val = ComputeCeilingLinedefValue();
		}
	}
	else if (i==val)
	{
		input_integer = TRUE;	// jff allow linedefs up to 32767 (for now)
		val = InputIntegerValue(-1, -1, 0, 32767, 0);
	}
	
	if(val >= 0)
	{
		if(!input_integer)
		{
			c = Linedef_classes;
			while(--val)
				c = c->next;
			/* now c points to the class we are
			   going to display a submenu for */
			
			t = c->types;
			for(i = 0; (t && i < 30); t = t->next)
			{
				menuarr[i++] = t->longname;
			}
			
			if(i == 1)
			{
				val = c->types->type; 
				strcpy (BackupText, "Change Special Type");
				Backup (BLINEDEFS);
				for (cur = Selected; cur; cur = cur->next)
				{
					if(cur->objnum != CurObject)  
						LineDefs[ cur->objnum].special = val;
				}
				if (CurObject >= 0)
					LineDefs[ CurObject].special = val;
				MadeChanges = TRUE;
			}
			else
			{
				val = DisplayMenuArray(-1, -1, "Choose A Special Type:",
									   i, NULL, menuarr, dummy);
				if(val == 0)
					val = -1;
			}
		}
		
		if (i >= 2 && val >= 0)
		{
			strcpy (BackupText, "Change Special Type");
			Backup (BLINEDEFS);
			if(!input_integer)
			{
				t = c->types;
				while(--val)
					t = t->next;
				val = t->type;
			}
			
			for (cur = Selected; cur; cur = cur->next)
			{
				if(cur->objnum != CurObject)  
					LineDefs[ cur->objnum].special = val;
			}
			if (CurObject >= 0)
				LineDefs[ CurObject].special = val;
			MadeChanges = TRUE;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefFlag(BCINT CurObject, SelPtr Selected, BCINT Flag)
{
	SelPtr cur;

	Backup (BTHINGS);
	strcpy (BackupText, "Change LineDef Flags");
	for (cur = Selected; cur; cur = cur->next)
		if(cur->objnum != CurObject) 
			LineDefs[ cur->objnum].flags ^= 0x01 << Flag;
	if (CurObject >= 0)
		LineDefs[ CurObject].flags ^= 0x01 << Flag;
	MadeChanges = TRUE;
	return REDRAW_ALL;
}

BCINT FunctionLineDefDeleteJoin(BCINT CurObject, SelPtr Selected)
{
	Backup (~BTHINGS);
	strcpy (BackupText, "Delete LineDefs and Join Sectors");
	DeleteLineDefsJoinSectors ( &Selected);
	return REDRAW_ALL; 
}

BCINT FunctionLineDefSplitAdd(BCINT CurObject, SelPtr Selected)
{
	if (( Selected)->next == NULL || ( Selected)->next->next != NULL) {
		Beep();
		Notify( -1, -1, "You Must Select Exactly Two LineDefs", NULL);
		return REDRAW_ALL;
	}
	else {
		Backup (~BTHINGS);
		strcpy (BackupText, "Split LineDefs And Add Sector");
		SplitLineDefsAndSector( ( Selected)->objnum, ( Selected)->next->objnum);
		return REDRAW_ALL;
	}
}

//jff Split Donut function added

BCINT FunctionConnectLineDefsSplitDonut(BCINT CurObject, SelPtr Selected)
{
	if (!Selected || ( Selected)->next == NULL || ( Selected)->next->next != NULL) {
		Beep();
		Notify( -1, -1, "You Must Select Exactly Two LineDefs", NULL);
		return REDRAW_ALL;
	}
	else {
		Backup (~BTHINGS);
		strcpy (BackupText, "Connect LineDefs And Split Donut");
		ConnectLineDefsSplitDonut( ( Selected)->objnum, ( Selected)->next->objnum);
		return REDRAW_ALL;
	}
}

//jff Make Hallway function added

BCINT FunctionConnectLineDefsMakeHallway(BCINT CurObject, SelPtr Selected)
{
	if (!Selected || ( Selected)->next == NULL || ( Selected)->next->next != NULL) {
		Beep();
		Notify( -1, -1, "You Must Select Exactly Two LineDefs", NULL);
		return REDRAW_ALL;
	}
	else {
		Backup (~BTHINGS);
		strcpy (BackupText, "Connect LineDefs And Make Hallway");
		ConnectLineDefsMakeHallway( ( Selected)->objnum, ( Selected)->next->objnum);
		return REDRAW_ALL;
	}
}

BCINT FunctionRenumberLineDefsLowest(BCINT CurObject, SelPtr Selected)
{
	SelPtr lp,rp;
 	int inspoint = 0;
	int l,i;
	struct LineDef l0;

	Backup (BLINEDEFS);

	lp = Selected;
	if (!lp && CurObject>=0)
	{
		l = CurObject;
		l0 = LineDefs[l];
		for (i=l-1;i>=0;i--)
			LineDefs[i+1] = LineDefs[i];
		LineDefs[0] = l0;
		CurObject = 0;
	}
	else
	{
		while (lp)
		{
			l = lp->objnum;
			l0 = LineDefs[l];
			for (i=l-1;i>=inspoint;i--)
				LineDefs[i+1] = LineDefs[i];
			LineDefs[inspoint] = l0;
			rp = Selected;
			while (rp)
			{
				if (rp->objnum>=inspoint && rp->objnum<l)
					rp->objnum = rp->objnum+1;
				rp = rp->next;
			}
			lp->objnum = inspoint++;
			lp = lp->next;

		}
	}

	sprintf (BackupText, "Renumber Linedefs Lowest");
	return REDRAW_ALL;
}


BCINT FunctionLineDefFlipSideDefs(BCINT CurObject, SelPtr Selected)
{
	Backup (BLINEDEFS | BSIDEDEFS);
	strcpy (BackupText, "Flip SideDefs");
	if (CurObject && !Selected) {
		struct SelectionList cur;
		cur.objnum = CurObject;
		cur.next = NULL;
		FlipLineDefs(&cur, TRUE);
	}
	FlipLineDefs( Selected, TRUE);
	return REDRAW_ALL;
}

BCINT FunctionLineDefSwapSideDefs(BCINT CurObject, SelPtr Selected)
{
	Backup (BLINEDEFS | BSIDEDEFS);
	strcpy (BackupText, "Swap SideDefs");
	if (CurObject && !Selected) {
		struct SelectionList cur;
		cur.objnum = CurObject;
		cur.next = NULL;
		FlipLineDefs(&cur, FALSE);
	}
	FlipLineDefs( Selected, FALSE);
	return REDRAW_ALL; 
}

BCINT FunctionLineDefBrowseWallTextures(BCINT CurObject, SelPtr Selected, BCINT SideDef, BCINT Texture)
{
	SelPtr cur;
	int val = 0;

	if (SideDef == 1 && Texture == 1)
		val = BrowseWalls(SideDefs[LineDefs[CurObject].sidedef1].tex1);
	if (SideDef == 1 && Texture == 2)
		val = BrowseWalls(SideDefs[LineDefs[CurObject].sidedef1].tex2);
	if (SideDef == 1 && Texture == 3)
		val = BrowseWalls(SideDefs[LineDefs[CurObject].sidedef1].tex3);
	if (SideDef == 2 && Texture == 1)
		val = BrowseWalls(SideDefs[LineDefs[CurObject].sidedef2].tex1);
	if (SideDef == 2 && Texture == 2)
		val = BrowseWalls(SideDefs[LineDefs[CurObject].sidedef2].tex2);
	if (SideDef == 2 && Texture == 3)
		val = BrowseWalls(SideDefs[LineDefs[CurObject].sidedef2].tex3);
	if (val >= 0) {
		Backup (BSIDEDEFS | BLINEDEFS);
		strcpy (BackupText, "Change Wall Texture");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject) 
				if (SideDef == 1 && Texture == 1)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex1, WTexture[val], 8);
				else if (SideDef == 1 && Texture == 2)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex2, WTexture[val], 8);
				else if (SideDef == 1 && Texture == 3)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex3, WTexture[val], 8);
				else if (SideDef == 2 && Texture == 1)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex1, WTexture[val], 8);
				else if (SideDef == 2 && Texture == 2)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex2, WTexture[val], 8);
				else if (SideDef == 2 && Texture == 3)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex3, WTexture[val], 8);
		}
		if(CurObject >= 0) {
			if (SideDef == 1 && Texture == 1)
				strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex1, WTexture[val], 8);
			else if (SideDef == 1 && Texture == 2)
				strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex2, WTexture[val], 8);
			else if (SideDef == 1 && Texture == 3)
				strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex3, WTexture[val], 8);
			else if (SideDef == 2 && Texture == 1)
				strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex1, WTexture[val], 8);
			else if (SideDef == 2 && Texture == 2)
				strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex2, WTexture[val], 8);
			else if (SideDef == 2 && Texture == 3)
				strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex3, WTexture[val], 8);
		}
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefWallTextures(BCINT CurObject, SelPtr Selected, BCINT SideDef, BCINT Texture)
{
	SelPtr cur;
	char texname[9];

	texname[8] = '\0';

	if (SideDef == 1 && Texture == 1)
		strncpy(texname, SideDefs[LineDefs[CurObject].sidedef1].tex1, 8);
	else if (SideDef == 1 && Texture == 2)
		strncpy(texname, SideDefs[LineDefs[CurObject].sidedef1].tex2, 8);
	else if (SideDef == 1 && Texture == 3)
		strncpy(texname, SideDefs[LineDefs[CurObject].sidedef1].tex3, 8);
	else if (SideDef == 2 && Texture == 1)
		strncpy(texname, SideDefs[LineDefs[CurObject].sidedef2].tex1, 8);
	else if (SideDef == 2 && Texture == 2)
		strncpy(texname, SideDefs[LineDefs[CurObject].sidedef2].tex2, 8);
	else if (SideDef == 2 && Texture == 3)
		strncpy(texname, SideDefs[LineDefs[CurObject].sidedef2].tex3, 8);

	ChooseWallTexture( -1, -1, "Choose A Wall Texture", NumWTexture, WTexture, texname);
	if (strlen(texname) > 0) {
		Backup (BSIDEDEFS | BLINEDEFS);
		strcpy (BackupText, "Change Wall Texture");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject) 
				if (SideDef == 1 && Texture == 1)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex1, texname, 8);
				else if (SideDef == 1 && Texture == 2)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex2, texname, 8);
				else if (SideDef == 1 && Texture == 3)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex3, texname, 8);
				else if (SideDef == 2 && Texture == 1)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex1, texname, 8);
				else if (SideDef == 2 && Texture == 2)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex2, texname, 8);
				else if (SideDef == 2 && Texture == 3)
					strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex3, texname, 8);
		}
		if(CurObject >= 0) {
			if (SideDef == 1 && Texture == 1)
				strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex1, texname, 8);
			else if (SideDef == 1 && Texture == 2)
				strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex2, texname, 8);
			else if (SideDef == 1 && Texture == 3)
				strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex3, texname, 8);
			else if (SideDef == 2 && Texture == 1)
				strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex1, texname, 8);
			else if (SideDef == 2 && Texture == 2)
				strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex2, texname, 8);
			else if (SideDef == 2 && Texture == 3)
				strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex3, texname, 8);
		}
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}

BCINT FunctionLineDefWallTexture(BCINT CurObject, SelPtr Selected, char *Name, BCINT SideDef, BCINT Texture)
{
	SelPtr cur;
	char texname[9];

	texname[8] = '\0';
	strncpy(texname, Name, 8);

	Backup (BSIDEDEFS);
	strcpy (BackupText, "Change Wall Texture");
	for (cur = Selected; cur; cur = cur->next) {
		if(cur->objnum != CurObject) 
			if (SideDef == 1 && Texture == 1)
				strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex1, texname, 8);
			else if (SideDef == 1 && Texture == 2)
				strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex2, texname, 8);
			else if (SideDef == 1 && Texture == 3)
				strncpy(SideDefs[LineDefs[cur->objnum].sidedef1].tex3, texname, 8);
			else if (SideDef == 2 && Texture == 1)
				strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex1, texname, 8);
			else if (SideDef == 2 && Texture == 2)
				strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex2, texname, 8);
			else if (SideDef == 2 && Texture == 3)
				strncpy(SideDefs[LineDefs[cur->objnum].sidedef2].tex3, texname, 8);
	}
	if(CurObject >= 0) {
		if (SideDef == 1 && Texture == 1)
			strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex1, texname, 8);
		else if (SideDef == 1 && Texture == 2)
			strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex2, texname, 8);
		else if (SideDef == 1 && Texture == 3)
			strncpy(SideDefs[LineDefs[CurObject].sidedef1].tex3, texname, 8);
		else if (SideDef == 2 && Texture == 1)
			strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex1, texname, 8);
		else if (SideDef == 2 && Texture == 2)
			strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex2, texname, 8);
		else if (SideDef == 2 && Texture == 3)
			strncpy(SideDefs[LineDefs[CurObject].sidedef2].tex3, texname, 8);
	}
	MadeChanges = TRUE;
	return REDRAW_ALL;
}

/********************/
/* SECTOR FUNCTIONS */
/********************/

BCINT FunctionSectorType(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	sector_class *c;
	sector_type *t;
	char *menuarr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	int i, val;
	Bool input_integer = FALSE;
	
	c = Sector_classes;
	for(i = 0; (c && i < 29); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}
	if (BOOMEnable)
	{
		menuarr[i++] = "Generalized Sector Type";
		menuarr[i++] = "----";
	}
	menuarr[i++] = "Enter A Decimal Value";
	
	val = -1;
	val = DisplayMenuArray( -1 , -1, "Choose A Sector Class:",
						   i, NULL, menuarr, dummy);
	
	if(val == 0)
		val = -1;
	
// jff add code for general sector types
	if (BOOMEnable && i-2==val)
	{
		input_integer = TRUE;

	  val=-1;
 		if (DisplayGeneralSectorTypeDialog())
			val = ComputeGeneralizedSectorTypeValue();
	}
	else	if(i == val) {
		input_integer = TRUE;
		val = InputIntegerValue(-1, -1, 0, 32767, 0);
	}
	
	if(val >= 0) {
		if(!input_integer) {
			c = Sector_classes;
			while(--val)
				c = c->next;
			
			t = c->types;
			for(i = 0; (t && i < 30); i++) {
				menuarr[i] = t->longname;
				t = t->next;
			}
			
			if(i == 1) {
					val = c->types->type; 
					strcpy (BackupText, "Change Sector Type");
					Backup (BSECTORS);
					for (cur = Selected; cur; cur = cur->next) {
						if(cur->objnum != CurObject)  
							Sectors[ cur->objnum].special = val;
					}
					if (CurObject >= 0)
						Sectors[ CurObject].special = val;
					MadeChanges = TRUE;
			}
			else {
				val = -1;
				val = DisplayMenuArray(-1, -1, "Choose A Sector Type:",
									   i, NULL, menuarr, dummy);
				if(val == 0)
					val = -1;
			}
		}
		
		if (i >= 2 && val >= 0) {
			Backup (BSECTORS);
			strcpy (BackupText, "Change Sector Type");
			if(!input_integer) {
				t = c->types;
				while(--val)
					t = t->next;
				val = t->type;
			}
			
			for(cur = Selected; cur; cur = cur->next) {
				if(cur->objnum != CurObject)  
					Sectors[ cur->objnum].special = val;
				MadeChanges = TRUE;
			}
			if (CurObject >= 0)
				Sectors[ CurObject].special = val;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionSectorFloorHeight(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (CurObject >= 0)
		val = InputIntegerValuePlain(90, MaxY - 76, -16384, 16384, Sectors[ CurObject].floorh);
	else
		val = InputIntegerValue( -1, -1, -16384, 16384, Sectors[Selected->objnum].floorh);

	if (val >= -16384) {
		Backup (BSECTORS);
		strcpy (BackupText, "Change Floor Height");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject)  
				Sectors[ cur->objnum].floorh = val;
		}
		MadeChanges = TRUE;
		if (CurObject >= 0) {
			Sectors[ CurObject].floorh = val;
			return REDRAW_ALL;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionSectorTag(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	GetCoords();
																					/* jff allow sector tags to 32767 */
	val = InputIntegerValuePlain(90, MaxY - 36, 0, 32767, Sectors[ CurObject].tag);
	if (val >= 0) {
		Backup (BSECTORS);
		strcpy (BackupText, "Change Sector Tag");
		for (cur = Selected; cur; cur = cur->next)
			if(cur->objnum != CurObject)  
				Sectors[ cur->objnum].tag = val;
		Sectors[ CurObject].tag = val;
		MadeChanges = TRUE;
	}
	return REDRAW_ALL;
}


BCINT FunctionSectorCeilingHeight(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (CurObject >= 0)
		val = InputIntegerValuePlain(90, MaxY - 66, -16384, 16384, Sectors[ CurObject].ceilh);
	else
		val = InputIntegerValue( -1, -1, -16384, 16384, Sectors[ Selected->objnum].ceilh);

	if (val >= -16384) {
		Backup (BSECTORS);
		strcpy (BackupText, "Change Ceiling Height");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject)  
				Sectors[ cur->objnum].ceilh = val;
		}
		MadeChanges = TRUE;
		if (CurObject >= 0) {
			Sectors[ CurObject].ceilh = val;
			return REDRAW_ALL;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionSectorLightLevel(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	GetCoords();
	if (CurObject >= 0)
		val = InputIntegerValuePlain(90, MaxY - 46, 0, 255, Sectors[ CurObject].light);
	else
		val = InputIntegerValue( -1, -1, 0, 255, Sectors[ Selected->objnum].light);

	if (val >= 0) {
		Backup (BSECTORS);
		strcpy (BackupText, "Change Light Level");
		for (cur = Selected; cur; cur = cur->next) {
			if(cur->objnum != CurObject)  
				Sectors[ cur->objnum].light = val;
		}
		MadeChanges = TRUE;
		if (CurObject >= 0) {
			Sectors[ CurObject].light = val;
			return REDRAW_ALL;
		}
	}
	return REDRAW_ALL;
}

BCINT FunctionSectorBrowseFloor(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	val = BrowseFlats(Sectors[CurObject].floort);
	if (val >= 0) {
		Backup (BSECTORS);
		strcpy (BackupText, "Change Floor Texture");
		for (cur = Selected; cur; cur = cur->next)
			if(cur->objnum != CurObject) 
				strncpy(Sectors[ cur->objnum].floort, FTexture[val], 8);
		MadeChanges = TRUE;
		if (CurObject >= 0)
			strncpy(Sectors[ CurObject].floort, FTexture[val], 8);
	}
	return REDRAW_ALL;
}

BCINT FunctionSectorBrowseCeiling(BCINT CurObject, SelPtr Selected)
{
	SelPtr cur;
	int val;

	val = BrowseFlats(Sectors[CurObject].ceilt);
	if (val >= 0) {
		Backup (BSECTORS);
		strcpy (BackupText, "Change Ceiling Texture");
		for (cur = Selected; cur; cur = cur->next)
			 if(cur->objnum != CurObject)  
				strncpy(Sectors[ cur->objnum].ceilt, FTexture[val], 8);
		MadeChanges = TRUE;
		if (CurObject >= 0)
			strncpy(Sectors[ CurObject].ceilt, FTexture[val], 8);
	}
	return REDRAW_ALL;
}

BCINT FunctionRenumberSectorsLowest(BCINT CurObject, SelPtr Selected)
{
	SelPtr lp,rp;
 	int inspoint = 0;
	int l,i;
	struct Sector l0;

	Backup (BSECTORS | BSIDEDEFS);

	lp = Selected;
	if (!lp && CurObject>=0)
	{
		l = CurObject;
		l0 = Sectors[l];
		for (i=l-1;i>=0;i--)
			Sectors[i+1] = Sectors[i];
		Sectors[0] = l0;
		for (i=0;i<NumSideDefs;i++)
			if (SideDefs[i].sector>=0 && SideDefs[i].sector<l)
				SideDefs[i].sector++;
			else if (SideDefs[i].sector==l)
				SideDefs[i].sector=0;
		CurObject = 0;
	}
	else
	{
		while (lp)
		{
			l = lp->objnum;
			l0 = Sectors[l];
			for (i=l-1;i>=inspoint;i--)
				Sectors[i+1] = Sectors[i];
			Sectors[inspoint] = l0;
			rp = Selected;
			while (rp)
			{
				if (rp->objnum>=inspoint && rp->objnum<l)
					rp->objnum = rp->objnum+1;
				rp = rp->next;
			}
			for (i=0;i<NumSideDefs;i++)
				if (SideDefs[i].sector>=inspoint && SideDefs[i].sector<l)
					SideDefs[i].sector++;
				else if (SideDefs[i].sector==l)
					SideDefs[i].sector=inspoint;

			lp->objnum = inspoint++;
			lp = lp->next;

		}
	}

	sprintf (BackupText, "Renumber Sectors Lowest");
	return REDRAW_ALL;
}



/********************/
/* VERTEX FUNCTIONS */
/********************/

BCINT FunctionVertexDeleteJoin(BCINT CurObject, SelPtr Selected)
{
	Backup (~BTHINGS);
	strcpy (BackupText, "Delete Vertexes and Join LineDefs");
	if (CurObject && !Selected) {
		struct SelectionList cur;
		cur.objnum = CurObject;
		cur.next = NULL;
		DeleteVerticesJoinLineDefs( &cur);
	}
	DeleteVerticesJoinLineDefs( Selected);
	return REDRAW_ALL;
}

BCINT FunctionVertexSplitLineDef(BCINT CurObject, SelPtr Selected)
{
	Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
	strcpy (BackupText, "Split LineDefs");
	if (CurObject>=0 && !Selected) {   /*JFF >=0 so Linedef 0 will split*/
		struct SelectionList cur;
		cur.objnum = CurObject;
		cur.next = NULL;
		SplitLineDefs(&cur);
	}
	SplitLineDefs( Selected);
	return REDRAW_ALL;
}

BCINT FunctionVertexMerge(BCINT CurObject, SelPtr Selected)
{
	Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
	strcpy (BackupText, "Merge Vertexes");
	MergeVertices( &Selected);
	return REDRAW_ALL;
}

BCINT FunctionVertexSplitSector(BCINT CurObject, SelPtr Selected)
{
	if (( Selected)->next == NULL || ( Selected)->next->next != NULL) {
		Beep();
		Notify( -1, -1, "You Must Select Exactly Two Vertices", NULL);
		return REDRAW_ALL;
	}
	else {
		Backup (~BTHINGS);
		strcpy (BackupText, "Split Sector Between 2 Vertexes");
		SplitSector( ( Selected)->objnum, ( Selected)->next->objnum);
		return REDRAW_ALL;
	}
}
