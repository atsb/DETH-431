/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   EDITOBJ.C - object editing routines.
   */

/* the includes */
#include "deu.h"
#include "levels.h"


/*
   ask for an object number and check for maximum valid number
   (this is just like InputIntegerValue, but with a different prompt)
   */

BCINT InputObjectNumber( BCINT x0, BCINT y0, BCINT objtype, BCINT curobj)
{
    BCINT val, key;
    char prompt[ 80];
    
    if (UseMouse)
		HideMousePointer();
    sprintf( prompt, "Enter A %s Value Between 0 And %d:", GetObjectTypeName( objtype), GetMaxObjectNum( objtype));
    if (x0 < 0)
		x0 = (ScrMaxX - 25 - 8 * strlen( prompt)) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 55) / 2;
    DrawScreenBox3D( x0, y0, x0 + 25 + 8 * strlen( prompt), y0 + 55);
    SetColor( WHITE);
    DrawScreenText( x0 + 10, y0 + 8, prompt);
    val = curobj;
    while (((key = InputInteger( -1, -1, &val, 0, GetMaxObjectNum( objtype))) & 0x00FF) != 0x000D && (key & 0x00FF) != 0x001B)
		Beep();
    if (UseMouse)
		ShowMousePointer();
    return val;
}



/*
   ask for an object number and display a warning message
   */

BCINT InputObjectXRef( BCINT x0, BCINT y0, BCINT objtype, Bool allownone, BCINT curobj)
{
    BCINT val, key;
    char prompt[ 80];
    
    if (UseMouse)
		HideMousePointer();
    sprintf( prompt, "Enter A %s Value Between 0 And %d%c", GetObjectTypeName( objtype), GetMaxObjectNum( objtype), allownone ? ',' : ':');
    val = strlen( prompt);
    if (val < 40)
		val = 40;
    if (x0 < 0)
		x0 = (ScrMaxX - 25 - 8 * val) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - (allownone ? 85 : 75)) / 2;
    DrawScreenBox3D( x0, y0, x0 + 25 + 8 * val, y0 + (allownone ? 85 : 75));
    SetColor( WHITE);
    DrawScreenText( x0 + 10, y0 + 8, prompt);
    if (allownone)
		DrawScreenText( x0 + 10, y0 + 18, "or -1 for none:");
    SetColor( RED);
    DrawScreenText( x0 + 10, y0 + (allownone ? 60 : 50), "Warning: Modifying The Cross-References");
    DrawScreenText( x0 + 10, y0 + (allownone ? 70 : 60), "Between Some Objects May Crash The Game.");
    val = curobj;
    while (((key = InputInteger( x0 + 10, y0 + (allownone ? 38 : 28), &val, allownone ? -1 : 0, GetMaxObjectNum( objtype))) & 0x00FF) != 0x000D && (key & 0x00FF) != 0x001B)
		Beep();
    if (UseMouse)
		ShowMousePointer();
    return val;
}



/*
   ask for two vertex numbers and check for maximum valid number
   */

Bool Input2VertexNumbers( BCINT x0, BCINT y0, char *prompt1, BCINT *v1, BCINT *v2)
{
    BCINT  key;
    BCINT  maxlen, first;
    Bool ok;
    char prompt2[ 80];
    
    if (UseMouse)
		HideMousePointer();
    sprintf( prompt2, "Enter Two Numbers Between 0 And %d:", NumVertexes - 1);
    if (strlen( prompt1) > strlen( prompt2))
		maxlen = strlen( prompt1);
    else
		maxlen = strlen( prompt2);
    if (x0 < 0)
		x0 = (ScrMaxX - 25 - 8 * maxlen) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 75) / 2;
    DrawScreenBox3D( x0, y0, x0 + 25 + 8 * maxlen, y0 + 75);
    DrawScreenText( x0 + 10, y0 + 36, "From This Vertex");
    DrawScreenText( x0 + 180, y0 + 36, "To This Vertex");
    SetColor( WHITE);
    DrawScreenText( x0 + 10, y0 + 8, prompt1);
    DrawScreenText( x0 + 10, y0 + 18, prompt2);
    first = TRUE;
    key = 0;
    for (;;) {
		ok = TRUE;
		DrawScreenBox3D( x0 + 10, y0 + 48, x0 + 71, y0 + 61);
		if (*v1 < 0 || *v1 >= NumVertexes)
			{
				SetColor( DARKGRAY);
				ok = FALSE;
			}
		DrawScreenText( x0 + 14, y0 + 51, "%d", *v1);
		DrawScreenBox3D( x0 + 180, y0 + 48, x0 + 241, y0 + 61);
		if (*v2 < 0 || *v2 >= NumVertexes) {
			SetColor( DARKGRAY);
			ok = FALSE;
		}
		DrawScreenText( x0 + 184, y0 + 51, "%d", *v2);
		if (first)
			key = InputInteger( x0 + 10, y0 + 48, v1, 0, NumVertexes - 1);
		else
			key = InputInteger( x0 + 180, y0 + 48, v2, 0, NumVertexes - 1);
		if ((key & 0xFF00) == 0x4B00 || (key & 0xFF00) == 0x4D00 || (key & 0x00FF) == 0x0009 || (key & 0xFF00) == 0x0F00)
			first = !first;
		else if ((key & 0x00FF) == 0x001B)
			break;
		else if ((key & 0x00FF) == 0x000D) {
			if (first)
				first = FALSE;
			else if (ok)
				break;
			else
				Beep();
		}
		else
			Beep();
    }
    if (UseMouse)
		ShowMousePointer();
    return ((key & 0x00FF) == 0x000D);
}



/*
 */

char *GetTaggedLineDefFlag( BCINT linedefnum, BCINT flagndx)
{
    static char ldstr[ 16][ 50];
    
    if ((LineDefs[ linedefnum].flags & (0x01 << (flagndx - 1))) != 0)
		strcpy( ldstr[ flagndx - 1], "* ");
    else
		strcpy( ldstr[ flagndx - 1], "  ");
    strcat( ldstr[ flagndx - 1], GetLineDefFlagsLongName( 0x01 << (flagndx - 1)));
    return ldstr[ flagndx - 1];
}


char *GetThingFlag( BCINT thingnum, BCINT flagndx)
{
    static char ldstr[16][ 50];
    
    if ((Things[ thingnum].flags & (0x01 << (flagndx - 1))) != 0)
		strcpy( ldstr[ flagndx - 1], "* ");
    else
		strcpy( ldstr[ flagndx - 1], "  ");
    strcat( ldstr[ flagndx - 1], GetThingFlagsLongName( 0x01 << (flagndx - 1)));
    return ldstr[ flagndx - 1];
}



/*
   edit an object or a group of objects
   */

void EditObjectsInfo( BCINT x0, BCINT y0, BCINT objtype, SelPtr obj) /* SWAP! */
{
    char  *menustr[ 30];
#ifdef GAME_HEXEN
    char argtext[5], arg1text[ 12], arg2text[ 12], arg3text[ 12], arg4text[ 12], arg5text[ 12];
#endif
    BCINT	dummy[ 30];
    char   texname[ 9];
    BCINT	n, val, startval, endval;
    SelPtr cur = NULL;
    SelPtr sdlist;
    
    if (obj == NULL)
		return;
    switch (objtype) {

#ifdef GAME_HEXEN
    case OBJ_THINGS:
		Backup (BTHINGS);
		strcpy(argtext, GetLineDefArgs(Things[ obj->objnum].special));
		for (n = 0; n < 14; n++)
			menustr[ n] = (char*)GetMemory( 150);
			sprintf( menustr[ 13], "Edit Thing #%d", obj->objnum);
       		sprintf( menustr[ 0], "Change Type       (Current: %s)", GetThingName( Things[ obj->objnum].type));
    		sprintf( menustr[ 1], "Change Angle      (Current: %s)", GetAngleName( Things[ obj->objnum].angle));
			sprintf( menustr[ 2], "Change Flags      (Current: %s)", GetWhenName( Things[ obj->objnum].flags));
       		sprintf( menustr[ 3], "Change X Pos      (Current: %d)", Things[ obj->objnum].xpos);
       		sprintf( menustr[ 4], "Change Y Pos      (Current: %d)", Things[ obj->objnum].ypos);
       		sprintf( menustr[ 5], "Change Z Pos      (Current: %d)", Things[ obj->objnum].zpos);
			sprintf( menustr[ 6], "Change TID        (Current: %d)", Things[ obj->objnum].tid);
			sprintf( menustr[ 7], "Change Special    (Current: %d, %s)", Things[ obj->objnum].special, GetLineDefTypeName(Things[obj->objnum].special));
			sprintf( menustr[ 8], "Change %9s  (Current: %d)", GetArg1Text(Things[ obj->objnum].special), Things[ obj->objnum].arg1);
			sprintf( menustr[ 9], "Change %9s  (Current: %d)", GetArg2Text(Things[ obj->objnum].special), Things[ obj->objnum].arg2);
			sprintf( menustr[10], "Change %9s  (Current: %d)", GetArg3Text(Things[ obj->objnum].special), Things[ obj->objnum].arg3);
	    	sprintf( menustr[11], "Change %9s  (Current: %d)", GetArg4Text(Things[ obj->objnum].special), Things[ obj->objnum].arg4);                
	    	sprintf( menustr[12], "Change %9s  (Current: %d)", GetArg5Text(Things[ obj->objnum].special), Things[ obj->objnum].arg5);
		val = DisplayMenuArray( -1, -1, menustr[ 13], 13, NULL, menustr, dummy);
		for (n = 0; n < 14; n++)
			FreeMemory( menustr[ n]);
		switch (val) {
		case 1:
			FunctionThingType(-1, obj);
			break;
		case 2:
			FunctionThingAngle(-1, obj);
			break;
		case 3:
			FunctionThingFlags(-1, obj);
			break;
		case 4:
			FunctionThingXPos(-1, obj);
			break;
		case 5:
			FunctionThingYPos(-1, obj);
			break;
		case 6:
			FunctionThingZPos(-1, obj);
			break;
		case 7:
			FunctionThingTID(-1, obj);
			break;
		case 8:
			FunctionThingSpecial(-1, obj);
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			if (argtext[val - 9] == 'P')
				FunctionThingPuzzle(-1, obj, val - 8);
			else if (argtext[val - 9] == 'S')
				FunctionThingSectorSound(-1, obj, val - 8);
			else if (argtext[val - 9] == 'A')
				FunctionThingHexenAngle(-1, obj, val - 8);
			else if (argtext[val - 9] == 'N')
				FunctionThingNegative(-1, obj, val - 8);
			else if (argtext[val - 9] == 'T')
				FunctionThingSpawnType(-1, obj, val - 8);
			else if (argtext[val - 9] == 'K')
				FunctionThingKeyType(-1, obj, val - 8);
			else if ((Things[ obj->objnum].type >= 104 && Things[ obj->objnum].type <= 106) || Things[ obj->objnum].type == 8064) 
				FunctionThingSpawnType(-1, obj, val - 8);
			else {
				FunctionThingArg(-1, obj, val - 8);
			break;
		}
		break;
	}
	break;
#else
    case OBJ_THINGS:
		Backup (BTHINGS);
		for (n = 0; n < 6; n++)
			menustr[ n] = (char*)GetMemory( 60);
			sprintf( menustr[ 5], "Edit Thing #%d", obj->objnum);
       		sprintf( menustr[ 0], "Change Type       (Current: %s)", GetThingName( Things[ obj->objnum].type));
    		sprintf( menustr[ 1], "Change Angle      (Current: %s)", GetAngleName( Things[ obj->objnum].angle));
			sprintf( menustr[ 2], "Change Flags      (Current: %s)", GetWhenName( Things[ obj->objnum].flags));
       		sprintf( menustr[ 3], "Change X Pos      (Current: %d)", Things[ obj->objnum].xpos);
       		sprintf( menustr[ 4], "Change Y Pos      (Current: %d)", Things[ obj->objnum].ypos);
		val = DisplayMenuArray( -1, -1, menustr[ 5], 5, NULL, menustr, dummy);
		for (n = 0; n < 6; n++)
			FreeMemory( menustr[ n]);
		switch (val) {
		case 1:
			FunctionThingType(-1, obj);
			break;
		case 2:
			FunctionThingAngle(-1, obj);
			break;
		case 3:
			FunctionThingFlags(-1, obj);
			break;
		case 4:
			FunctionThingXPos(-1, obj);
			break;
		case 5:
			FunctionThingYPos(-1, obj);
			break;
		}
		break;
#endif


    case OBJ_VERTEXES:
		Backup (BVERTEXES);
		for (n = 0; n < 3; n++)
			menustr[ n] = (char*)GetMemory( 60);
		sprintf( menustr[ 2], "Edit Vertex #%d", obj->objnum);
		sprintf( menustr[ 0], "Change X Position (Current: %d)", Vertexes[ obj->objnum].x);
		sprintf( menustr[ 1], "Change Y Position (Current: %d)", Vertexes[ obj->objnum].y);
		val = DisplayMenuArray( -1, -1, menustr[ 2], 2, NULL, menustr, dummy);
		for (n = 0; n < 3; n++)
			FreeMemory( menustr[ n]);
		switch (val) {
		case 1:
			val = InputIntegerValue( -1, -1, min( MapMinX, -10000), max( MapMaxX, 10000), Vertexes[ obj->objnum].x);
			if (val >= min( MapMinX, -10000)) {
				strcpy (BackupText, "Change Vertex X Position");
				n = val - Vertexes[ obj->objnum].x;
				for (cur = obj; cur; cur = cur->next)
					Vertexes[ cur->objnum].x += n;
				MadeChanges = TRUE;
				MadeMapChanges = TRUE;
			}
			break;
			
		case 2:
			val = InputIntegerValue( -1, -1, min( MapMinY, -10000), max( MapMaxY, 10000), Vertexes[ obj->objnum].y);
			if (val >= min( MapMinY, -10000)) {
				strcpy (BackupText, "Change Vertex Y Position");
				n = val - Vertexes[ obj->objnum].y;
				for (cur = obj; cur; cur = cur->next)
					Vertexes[ cur->objnum].y += n;
				MadeChanges = TRUE;
				MadeMapChanges = TRUE;
			}
			break;
		}		 
		break;
#ifdef GAME_HEXEN
    case OBJ_LINEDEFS:
		Backup (BLINEDEFS | BSIDEDEFS);
		switch (DisplayMenu( -1, -1, "Choose The Object To Edit:",
							"Edit The LineDef",
							(LineDefs[ obj->objnum].sidedef1 >= 0) ? "Edit The 1st SideDef" : "Add A 1st SideDef",
							(LineDefs[ obj->objnum].sidedef2 >= 0) ? "Edit The 2nd SideDef" : "Add A 2nd SideDef",
							NULL)) {
		case 1:
			strcpy(argtext, GetLineDefArgs(LineDefs[ obj->objnum].special));
			for (n = 0; n < 13; n++)
				menustr[ n] = (char*)GetMemory( 120);
				sprintf( menustr[ 12], "Edit LineDef #%d", obj->objnum);
				sprintf( menustr[ 0], "Change Flags            (Current: %d)", LineDefs[ obj->objnum].flags);
				sprintf( menustr[ 1], "Change Trigger          (Current: %d)", LineDefs[ obj->objnum].flags);
				sprintf( menustr[ 2], "Change Starting Vertex  (Current: #%d)", LineDefs[ obj->objnum].start);
				sprintf( menustr[ 3], "Change Ending Vertex    (Current: #%d)", LineDefs[ obj->objnum].end);
				sprintf( menustr[ 4], "Change 1st SideDef Ref. (Current: #%d)", LineDefs[ obj->objnum].sidedef1);
				sprintf( menustr[ 5], "Change 2nd SideDef Ref. (Current: #%d)", LineDefs[ obj->objnum].sidedef2);
				sprintf( menustr[ 6], "Change Special          (Current: %d, %s)", LineDefs[ obj->objnum].special, GetLineDefTypeName(LineDefs[ obj->objnum].special));
				sprintf( menustr[ 7], "Change %9s        (Current: %d)", GetArg1Text(LineDefs[obj->objnum].special), LineDefs[ obj->objnum].arg1);
				sprintf( menustr[ 8], "Change %9s        (Current: %d)", GetArg2Text(LineDefs[obj->objnum].special), LineDefs[ obj->objnum].arg2);
				sprintf( menustr[ 9], "Change %9s        (Current: %d)", GetArg3Text(LineDefs[obj->objnum].special), LineDefs[ obj->objnum].arg3);
				sprintf( menustr[ 10],"Change %9s        (Current: %d)", GetArg4Text(LineDefs[obj->objnum].special), LineDefs[ obj->objnum].arg4);
				sprintf( menustr[ 11],"Change %9s        (Current: %d)", GetArg5Text(LineDefs[obj->objnum].special), LineDefs[ obj->objnum].arg5);
				val = DisplayMenuArray( -1 , -1, menustr[ 12], 12, NULL, menustr, dummy);
			for (n = 0; n < 13; n++)
				FreeMemory( menustr[ n]);
			switch (val) {
			case 1:
				FunctionLineDefFlags(-1, obj);
				break;
			case 2:
				FunctionLineDefTrigger(-1, obj);
				break;
			case 7:
				FunctionLineDefSpecial(-1, obj);
				break;
			case 3:
				val = InputObjectXRef( -1, -1, OBJ_VERTEXES, FALSE, LineDefs[ obj->objnum].start);
				if (val >= 0) {
					strcpy (BackupText, "Change LineDef Start Vertex");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].start = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 4:
				val = InputObjectXRef( -1, -1, OBJ_VERTEXES, FALSE, LineDefs[ obj->objnum].end);
				if (val >= 0) {
					strcpy (BackupText, "Change LineDef End Vertex");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].end = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 5:
				val = InputObjectXRef( -1, -1, OBJ_SIDEDEFS, TRUE, LineDefs[ obj->objnum].sidedef1);
				if (val >= -1) {
					strcpy (BackupText, "Change LineDef 1st SideDef Ref.");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].sidedef1 = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 6:
				val = InputObjectXRef( -1, -1, OBJ_SIDEDEFS, TRUE, LineDefs[ obj->objnum].sidedef2);
				if (val >= -1) {
					strcpy (BackupText, "Change LineDef 2nd SideDef Ref.");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].sidedef2 = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			if (argtext[val - 8] == 'P')
				FunctionLineDefPuzzle(-1, obj, val - 7);
			else if (argtext[val - 8] == 'S')
				FunctionLineDefSectorSound(-1, obj, val - 7);
			else if (argtext[val - 8] == 'A')
				FunctionLineDefHexenAngle(-1, obj, val - 7);
			else if (argtext[val - 8] == 'N')
				FunctionLineDefNegative(-1, obj, val - 7);
			else if (argtext[val - 8] == 'T')
				FunctionLineDefSpawnType(-1, obj, val - 7);
			else if (argtext[val - 8] == 'K')
				FunctionLineDefKeyType(-1, obj, val - 7);
			else
				FunctionLineDefArg(-1, obj, val - 7);
			break;
		}
		break;
#else
    case OBJ_LINEDEFS:
		Backup (BLINEDEFS | BSIDEDEFS);
		switch (DisplayMenu( -1, -1, "Choose The Object To Edit:",
							"Edit The LineDef",
							(LineDefs[ obj->objnum].sidedef1 >= 0) ? "Edit The 1st SideDef" : "Add A 1st SideDef",
							(LineDefs[ obj->objnum].sidedef2 >= 0) ? "Edit The 2nd SideDef" : "Add A 2nd SideDef",
							NULL)) {
		case 1:
			for (n = 0; n < 8; n++)
				menustr[ n] = (char*)GetMemory( 80);
				sprintf( menustr[ 7], "Edit LineDef #%d", obj->objnum);
				sprintf( menustr[ 0], "Change Flags            (Current: %d)", LineDefs[ obj->objnum].flags);
				sprintf( menustr[ 1], "Change Starting Vertex  (Current: #%d)", LineDefs[ obj->objnum].start);
				sprintf( menustr[ 2], "Change Ending Vertex    (Current: #%d)", LineDefs[ obj->objnum].end);
				sprintf( menustr[ 3], "Change 1st SideDef Ref. (Current: #%d)", LineDefs[ obj->objnum].sidedef1);
				sprintf( menustr[ 4], "Change 2nd SideDef Ref. (Current: #%d)", LineDefs[ obj->objnum].sidedef2);
				sprintf( menustr[ 5], "Change Special          (Current: %d, %s)", LineDefs[ obj->objnum].special, GetLineDefTypeName(LineDefs[ obj->objnum].special));
				sprintf( menustr[ 6], "Change Tag              (Current: %d)", LineDefs[ obj->objnum].tag);
				val = DisplayMenuArray( -1 , -1, menustr[ 7], 7, NULL, menustr, dummy);
			for (n = 0; n < 8; n++)
				FreeMemory( menustr[ n]);
			switch (val) {
			case 1:
				FunctionLineDefFlags(-1, obj);
				break;
			case 2:
				val = InputObjectXRef( -1, -1, OBJ_VERTEXES, FALSE, LineDefs[ obj->objnum].start);
				if (val >= 0) {
					strcpy (BackupText, "Change LineDef Start Vertex");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].start = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 3:
				val = InputObjectXRef( -1, -1, OBJ_VERTEXES, FALSE, LineDefs[ obj->objnum].end);
				if (val >= 0) {
					strcpy (BackupText, "Change LineDef End Vertex");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].end = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 4:
				val = InputObjectXRef( -1, -1, OBJ_SIDEDEFS, TRUE, LineDefs[ obj->objnum].sidedef1);
				if (val >= -1) {
					strcpy (BackupText, "Change LineDef 1st SideDef Ref.");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].sidedef1 = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 5:
				val = InputObjectXRef( -1, -1, OBJ_SIDEDEFS, TRUE, LineDefs[ obj->objnum].sidedef2);
				if (val >= -1) {
					strcpy (BackupText, "Change LineDef 2nd SideDef Ref.");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].sidedef2 = val;
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				break;
			case 6:
				FunctionLineDefSpecial(-1, obj);
				break;
			case 7:
				if (LineDefs[ obj->objnum].tag == 0) { /* jff allow line tags up to 32767 */
					val = InputIntegerValue( -1, -1, 0, 32767, FindFreeTag());
				}
				else {
					val = InputIntegerValue( -1, -1, 0, 32767, LineDefs[ obj->objnum].tag);
				}
				if (val >= 0) {
					strcpy (BackupText, "Change LineDef Sector Tag");
					for (cur = obj; cur; cur = cur->next)
						LineDefs[ cur->objnum].tag = val;
					MadeChanges = TRUE;
				}
			break;
		}
		break;
#endif
		/* edit or add the first SideDef */
		case 2:
			if (LineDefs[ obj->objnum].sidedef1 >= 0) {
				/* build a new selection list with the first SideDefs */
				objtype = OBJ_SIDEDEFS;
				sdlist = NULL;
				for (cur = obj; cur; cur = cur->next)
					if (LineDefs[ cur->objnum].sidedef1 >= 0)
						SelectObject( &sdlist, LineDefs[ cur->objnum].sidedef1);
			}
			else {
				/* add a new first SideDef */
				strcpy (BackupText, "Add 1st SideDef");
				for (cur = obj; cur; cur = cur->next)
					if (LineDefs[ cur->objnum].sidedef1 == -1) {
						InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
						LineDefs[ cur->objnum].sidedef1 = NumSideDefs - 1;
					}
				break;
			}
			/* no break here */
			
			/* edit or add the second SideDef */
		case 3:
			if (objtype != OBJ_SIDEDEFS) {
				if (LineDefs[ obj->objnum].sidedef2 >= 0) {
					/* build a new selection list with the second (or first) SideDefs */
					objtype = OBJ_SIDEDEFS;
					sdlist = NULL;
					for (cur = obj; cur; cur = cur->next)
						if (LineDefs[ cur->objnum].sidedef2 >= 0)
							SelectObject( &sdlist, LineDefs[ cur->objnum].sidedef2);
						else if (LineDefs[ cur->objnum].sidedef1 >= 0)
							SelectObject( &sdlist, LineDefs[ cur->objnum].sidedef1);
				}
				else {
					/* add a new second (or first) SideDef */
					for (cur = obj; cur; cur = cur->next)
						if (LineDefs[ cur->objnum].sidedef1 == -1) {
							strcpy (BackupText, "Add 1st SideDef");
							InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
							LineDefs[ cur->objnum].sidedef1 = NumSideDefs - 1;
						}
						else if (LineDefs[ cur->objnum].sidedef2 == -1) {
							strcpy (BackupText, "Add 2nd SideDef");
							n = LineDefs[ cur->objnum].sidedef1;
							InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
							strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
							strncpy( SideDefs[ n].tex3, "-", 8);
							LineDefs[ cur->objnum].sidedef2 = NumSideDefs - 1;
							LineDefs[ cur->objnum].flags = 4;
						}
					break;
				}
			}
			for (n = 0; n < 10; n++)
				menustr[ n] = (char*)GetMemory( 90);
			sprintf( menustr[ 9], "Edit SideDef #%d", sdlist->objnum);
			texname[ 8] = '\0';
			strncpy( texname, SideDefs[ sdlist->objnum].tex3, 8);
			sprintf( menustr[ 0], "Change Normal Texture   (Current: %s)", texname);
			strncpy( texname, SideDefs[ sdlist->objnum].tex1, 8);
	        sprintf( menustr[ 1], "Change Upper Texture    (Current: %s)", texname);
			strncpy( texname, SideDefs[ sdlist->objnum].tex2, 8);
            sprintf( menustr[ 2], "Change Lower Texture    (Current: %s)", texname);
			sprintf( menustr[ 3], "Browse Normal Textures");
			sprintf( menustr[ 4], "Browse Upper Textures");
			sprintf( menustr[ 5], "Browse Lower Textures");
			sprintf( menustr[ 6], "Change Texture X Offset (Current: %d)", SideDefs[ sdlist->objnum].xoff);
			sprintf( menustr[ 7], "Change Texture Y Offset (Current: %d)", SideDefs[ sdlist->objnum].yoff);
	            sprintf( menustr[ 8], "Change Sector Ref.      (Current: #%d)", SideDefs[ sdlist->objnum].sector);
			val = DisplayMenuArray( -1, -1, menustr[ 9], 9, NULL, menustr, dummy);
			for (n = 0; n < 10; n++)
				FreeMemory( menustr[ n]);
			switch (val) {
			case 1:
				strncpy( texname, SideDefs[ sdlist->objnum].tex3, 8);
				ChooseWallTexture( -1 , -1, "Choose A Wall Texture", NumWTexture, WTexture, texname);
				if (strlen(texname) > 0) {
					strcpy (BackupText, "Change Normal Texture");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							strncpy( SideDefs[ cur->objnum].tex3, texname, 8);
					MadeChanges = TRUE;
				}
				break;
			case 4:
				val = BrowseWalls(SideDefs[sdlist->objnum].tex3); /* jff sdlist not cur */
				if (val >= 0) {
					strcpy (BackupText, "Change Normal Texture");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							strncpy( SideDefs[ cur->objnum].tex3, WTexture[val], 8);
					MadeChanges = TRUE;
				}
				break;
			case 2:
				strncpy( texname, SideDefs[ sdlist->objnum].tex1, 8);
				ChooseWallTexture( -1, -1, "Choose A Wall Texture", NumWTexture, WTexture, texname);
				if (strlen(texname) > 0) {
					strcpy (BackupText, "Change Upper Texture");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							strncpy( SideDefs[ cur->objnum].tex1, texname, 8);
					MadeChanges = TRUE;
				}
				break;
			case 5:
				val = BrowseWalls(SideDefs[sdlist->objnum].tex1); /* jff sdlist not cur */
				if (val >= 0) {
					strcpy (BackupText, "Change Upper Texture");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							strncpy( SideDefs[ cur->objnum].tex1, WTexture[val], 8);
					MadeChanges = TRUE;
				}
				break;
			case 3:
				strncpy( texname, SideDefs[ sdlist->objnum].tex2, 8);
				ChooseWallTexture( -1, -1, "Choose A Wall Texture", NumWTexture, WTexture, texname);
				if (strlen(texname) > 0) {
					strcpy (BackupText, "Change Lower Texture");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							strncpy( SideDefs[ cur->objnum].tex2, texname, 8);
					MadeChanges = TRUE;
				}
				break;
			case 6:
				val = BrowseWalls(SideDefs[sdlist->objnum].tex2); /* jff sdlist not cur */
				if (val >= 0) {
					strcpy (BackupText, "Change Lower Texture");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							strncpy( SideDefs[ cur->objnum].tex2, WTexture[val], 8);
					MadeChanges = TRUE;
				}
				break;
			case 7:
				val = InputIntegerValue( -1, -1, -16383, 16383, SideDefs[ sdlist->objnum].xoff);
				/*JFF allow x-offsets between -16383 and +16383*/
				if (val >= -16383) {
					strcpy (BackupText, "Change X Offset");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							SideDefs[ cur->objnum].xoff = val;
					MadeChanges = TRUE;
				}
				break;
			case 8:
				val = InputIntegerValue( -1, -1, -16383, 16383, SideDefs[ sdlist->objnum].yoff);
				/*JFF allow y-offsets between -16383 and +16383*/
				if (val >= -16383) {
					strcpy (BackupText, "Change Y Offset");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							SideDefs[ cur->objnum].yoff = val;
					MadeChanges = TRUE;
				}
				break;
			case 9:
				val = InputObjectXRef( -1, -1, OBJ_SECTORS, FALSE, SideDefs[ sdlist->objnum].sector);
				if (val >= 0) {
					strcpy (BackupText, "Change Sector Reference");
					for (cur = sdlist; cur; cur = cur->next)
						if (cur->objnum >= 0)
							SideDefs[ cur->objnum].sector = val;
					MadeChanges = TRUE;
				}
				break;
			}
			ForgetSelection( &sdlist);
			break;
		}
		break;

    case OBJ_SECTORS:
		Backup (BSECTORS);
		texname[ 8] = '\0';
		for (n = 0; n < 10; n++)
			menustr[ n] = (char*)GetMemory( 100);
		sprintf( menustr[ 9], "Edit Sector #%d", obj->objnum);
        sprintf( menustr[ 0], "Change Floor Height     (Current: %d)", Sectors[ obj->objnum].floorh);
		sprintf( menustr[ 1], "Change Ceiling Height   (Current: %d)", Sectors[ obj->objnum].ceilh);
		strncpy( texname, Sectors[ obj->objnum].floort, 8);
        sprintf( menustr[ 2], "Change Floor Texture    (Current: %s)", texname);
		strncpy( texname, Sectors[ obj->objnum].ceilt, 8);
		sprintf( menustr[ 3], "Change Ceiling Texture  (Current: %s)", texname);
        sprintf( menustr[ 4], "Browse Floor Textures");
        sprintf( menustr[ 5], "Browse Ceiling Textures");
        sprintf( menustr[ 6], "Change Light Level      (Current: %d)", Sectors[ obj->objnum].light);
        sprintf( menustr[ 7], "Change Type             (Current: %d)", Sectors[ obj->objnum].special);
        sprintf( menustr[ 8], "Change Tag              (Current: %d)", Sectors[ obj->objnum].tag);
		val = DisplayMenuArray( -1, -1, menustr[ 9], 9, NULL, menustr, dummy);
		for (n = 0; n < 10; n++)
			FreeMemory( menustr[ n]);
		switch (val) {
		case 1:
			for (n = 0; n < 4; n++)
				menustr[ n] = (char*)GetMemory( 60);
			sprintf( menustr[ 3], "Edit Sector #%d", obj->objnum);
			sprintf( menustr[ 0], "Change Floor Height");
			sprintf( menustr[ 1], "Increment Floor");
			sprintf( menustr[ 2], "Change Floor And Sector Height");
			val = DisplayMenuArray( -1, -1, menustr[ 3], 3, NULL, menustr, dummy);
			for (n = 0; n < 4; n++)
				FreeMemory( menustr[ n]);
			switch (val) {
			case 1:
				FunctionSectorFloorHeight(-1, obj);
				break;
			case 2:
				val = InputIntegerValue( -1, -1, -16384, 16384, 0);
				/*JFF allow floor-ceiling diff to be 16384*/
				if (val >= -16384) {
					strcpy (BackupText, "Increment Floor Height");
					for (cur = obj; cur; cur = cur->next)
						Sectors[ cur->objnum].floorh = Sectors[ cur->objnum].floorh + val;
					MadeChanges = TRUE;
				}
				break;
			case 3:
				startval = Sectors[ obj->objnum].floorh;
				endval = Sectors[ obj->objnum].ceilh - Sectors[ obj->objnum].floorh;
				if (Input2NegativeNumbers( -1, -1, "Floor Height", "Sector Height", 16384, 16384, &startval, &endval)) {
				/*JFF allow floor-ceiling diff to be 16384*/
					strcpy (BackupText, "Change Floor And Sector Height");
					for (cur = obj; cur; cur = cur->next) {
						Sectors[ cur->objnum].floorh = startval;
						Sectors[ cur->objnum].ceilh = startval + endval;
					}
					MadeChanges = TRUE;
				}
				break;
			}
			break;
		case 2:
			for (n = 0; n < 4; n++)
				menustr[ n] = (char*)GetMemory( 60);
			sprintf( menustr[ 3], "Edit Sector #%d", obj->objnum);
			sprintf( menustr[ 0], "Change Ceiling Height");
			sprintf( menustr[ 1], "Increment Ceiling");
			sprintf( menustr[ 2], "Change Ceiling And Sector Height");
			val = DisplayMenuArray( -1, -1, menustr[ 3], 3, NULL, menustr, dummy);
			for (n = 0; n < 4; n++)
				FreeMemory( menustr[ n]);
			switch (val) {
			case 1:
				FunctionSectorCeilingHeight(-1, obj);
				break;
			case 2:
				val = InputIntegerValue( -1, -1, -16384, 16384, 0);
				/*JFF allow floor-ceiling diff to be 16384*/
				if (val >= -16384) {
					strcpy (BackupText, "Increment Ceiling Height");
					for (cur = obj; cur; cur = cur->next)
						Sectors[ cur->objnum].ceilh = Sectors[ cur->objnum].ceilh + val;
					MadeChanges = TRUE;
				}
				break;
			case 3:
				startval = Sectors[ obj->objnum].ceilh;
				endval = Sectors[ obj->objnum].ceilh - Sectors[ obj->objnum].floorh;
				if (Input2NegativeNumbers( -1, -1, "Ceiling Height", "Sector Height", 16384, 16384, &startval, &endval)) {
				/*JFF allow floor-ceiling diff to be 16384*/
					strcpy (BackupText, "Change Ceiling And Sector Height");
					for (cur = obj; cur; cur = cur->next) {
						Sectors[ cur->objnum].ceilh = startval;
						Sectors[ cur->objnum].floorh = startval - endval;
					}
					MadeChanges = TRUE;
				}
				break;
			}
			break;
		case 3:
			strncpy( texname, Sectors[ obj->objnum].floort, 8);
			ChooseFloorTexture( -1, -1, "Choose A Floor Texture", NumFTexture, FTexture, texname);
			if (strlen(texname) > 0) {
				strcpy (BackupText, "Change Floor Texture");
				for (cur = obj; cur; cur = cur->next)
					strncpy( Sectors[ cur->objnum].floort, texname, 8);
				MadeChanges = TRUE;
			}
			break;
		case 5:
			FunctionSectorBrowseFloor(-1, obj);
			break;
		case 4:
			strncpy( texname, Sectors[ obj->objnum].ceilt, 8);
			ChooseFloorTexture( -1, -1, "Choose A Ceiling Texture", NumFTexture, FTexture, texname);
			if (strlen(texname) > 0) {
				strcpy (BackupText, "Change Ceiling Texture");
				for (cur = obj; cur; cur = cur->next)
					strncpy( Sectors[ cur->objnum].ceilt, texname, 8);
				MadeChanges = TRUE;
			}
			break;
		case 6:
			FunctionSectorBrowseCeiling(-1, obj);
			break;
		case 7:
			for (n = 0; n < 3; n++)
				menustr[ n] = (char*)GetMemory( 60);
			sprintf( menustr[ 2], "Edit Sector #%d", obj->objnum);
			sprintf( menustr[ 0], "Change Light Level");
			sprintf( menustr[ 1], "Increment Light Level");
			val = DisplayMenuArray( -1, -1, menustr[ 2], 2, NULL, menustr, dummy);
			for (n = 0; n < 3; n++)
				FreeMemory( menustr[ n]);
			switch (val) {
			case 1:
				FunctionSectorLightLevel(-1, obj);
				break;
			case 2:
				val = InputIntegerValue( -1, -1, -255, 255, 0);
				if (val >= -255) {
					strcpy (BackupText, "Increment Light Level");
					for (cur = obj; cur; cur = cur->next) {
						Sectors[ cur->objnum].light = Sectors[ cur->objnum].light + val;
						if (Sectors[ cur->objnum].light <= -1) {
							Sectors[ cur->objnum].light = 0;
						}
						if (Sectors[ cur->objnum].light >= 256) {
							Sectors[ cur->objnum].light = 255;
						}
					}
					MadeChanges = TRUE;
				}
				break;
			}
			break;
		case 8:
			FunctionSectorType(-1, obj);
			break;
		case 9:
			if (Sectors[ obj->objnum].tag == 0) {
				val = InputIntegerValue( -1, -1, 0, 32767, FindFreeTag());
			}
			else {
				val = InputIntegerValue( -1, -1, 0, 32767, Sectors[ obj->objnum].tag);
			}
			if (val >= 0) {
				strcpy (BackupText, "Change Tag");
				for (cur = obj; cur; cur = cur->next)
					Sectors[ cur->objnum].tag = val;
				MadeChanges = TRUE;
			}
			break;
		}
	break;
	}
}


/*
   Yuck!  Dirty piece of code...
   */

Bool Input2Numbers( BCINT x0, BCINT y0, char *name1, char *name2, BCINT v1max, BCINT v2max, BCINT *v1, BCINT *v2)
{
    BCINT  key;
    BCINT  maxlen, first;
    Bool ok;
    char prompt[ 80];
    
    if (UseMouse)
		HideMousePointer();
    sprintf( prompt, "Give The %s And %s For The Object:", name1, name2);
    maxlen = strlen( prompt);
    if (x0 < 0)
		x0 = (ScrMaxX - 25 - 8 * maxlen) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 75) / 2;
    DrawScreenBox3D( x0, y0, x0 + 25 + 8 * maxlen, y0 + 75);
    DrawScreenText( x0 + 10, y0 + 26, name1);
    DrawScreenText( x0 + 180, y0 + 26, name2);
    DrawScreenText( x0 + 10, y0 + 58, "(0-%d)", v1max);
    DrawScreenText( x0 + 180, y0 + 58, "(0-%d)", v2max);
    SetColor( WHITE);
    DrawScreenText( x0 + 10, y0 + 8, prompt);
    first = TRUE;
    key = 0;
    for (;;) {
		ok = TRUE;
		DrawScreenBox3D( x0 + 10, y0 + 38, x0 + 71, y0 + 51);
		if (*v1 < 0 || *v1 > v1max) {
			SetColor( DARKGRAY);
			ok = FALSE;
		}
		DrawScreenText( x0 + 14, y0 + 41, "%d", *v1);
		DrawScreenBox3D( x0 + 180, y0 + 38, x0 + 241, y0 + 51);
		if (*v2 < 0 || *v2 > v2max) {
			SetColor( DARKGRAY);
			ok = FALSE;
		}
		DrawScreenText( x0 + 184, y0 + 41, "%d", *v2);
		if (first)
			key = InputInteger( x0 + 10, y0 + 38, v1, 0, v1max);
		else
			key = InputInteger( x0 + 180, y0 + 38, v2, 0, v2max);
		if ((key & 0xFF00) == 0x4B00 || (key & 0xFF00) == 0x4D00 || (key & 0x00FF) == 0x0009 || (key & 0xFF00) == 0x0F00)
			first = !first;
		else if ((key & 0x00FF) == 0x001B)
			break;
		else if ((key & 0x00FF) == 0x000D) {
			if (first)
				first = FALSE;
			else if (ok)
				break;
			else
				Beep();
		}
		else
			Beep();
    }
    if (UseMouse)
		ShowMousePointer();
    return ((key & 0x00FF) == 0x000D);
}


Bool Input2NegativeNumbers( BCINT x0, BCINT y0, char *name1, char *name2, BCINT v1max, BCINT v2max, BCINT *v1, BCINT *v2)
{
    BCINT  key;
    BCINT  maxlen, first, v1min, v2min;
    Bool ok;
    char prompt[ 80];
    
    v1min = v1max - (2 * v1max);
    v2min = v2max - (2 * v2max);
    if (UseMouse)
		HideMousePointer();
    sprintf( prompt, "Give The %s And %s For The Object:", name1, name2);
    maxlen = strlen( prompt);
    if (x0 < 0)
		x0 = (ScrMaxX - 25 - 8 * maxlen) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 75) / 2;
    DrawScreenBox3D( x0, y0, x0 + 25 + 8 * maxlen, y0 + 75);
    DrawScreenText( x0 + 10, y0 + 26, name1);
    DrawScreenText( x0 + 180, y0 + 26, name2);
    DrawScreenText( x0 + 10, y0 + 58, "(%d to %d)", v1min, v1max);
    DrawScreenText( x0 + 180, y0 + 58, "(%d to %d)", v2min, v2max);
    SetColor( WHITE);
    DrawScreenText( x0 + 10, y0 + 8, prompt);
    first = TRUE;
    key = 0;
    for (;;) {
		ok = TRUE;
		DrawScreenBox3D( x0 + 10, y0 + 38, x0 + 71, y0 + 51);
		if (*v1 < v1min || *v1 > v1max) {
			SetColor( DARKGRAY);
			ok = FALSE;
		} 
		DrawScreenText( x0 + 14, y0 + 41, "%d", *v1);
		DrawScreenBox3D( x0 + 180, y0 + 38, x0 + 241, y0 + 51);
		if (*v2 < v2min || *v2 > v2max) {
			SetColor( DARKGRAY);
			ok = FALSE;
		} 
		DrawScreenText( x0 + 184, y0 + 41, "%d", *v2);
		if (first)
			key = InputInteger( x0 + 10, y0 + 38, v1, v1min, v1max);
		else
			key = InputInteger( x0 + 180, y0 + 38, v2, v2min, v2max);
		if ((key & 0xFF00) == 0x4B00 || (key & 0xFF00) == 0x4D00 || (key & 0x00FF) == 0x0009 || (key & 0xFF00) == 0x0F00)
			first = !first;
		else if ((key & 0x00FF) == 0x001B)
			break;
		else if ((key & 0x00FF) == 0x000D) {
			if (first)
				first = FALSE;
			else if (ok)
				break;
			else
				Beep();
		}
		else
			Beep();
    }
    if (UseMouse)
		ShowMousePointer();
    return ((key & 0x00FF) == 0x000D);
}

BCINT SelectThingType()
{
	int i;
	thing_class *c;
	thing_type *t;
	char *menuarr[MENUARRAYMAX];
	char *spritestr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	BCINT val;
	Bool input_integer = FALSE;

	c = Thing_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}
	menuarr[i++] = "Enter A Decimal Value";

	val = DisplayMenuArray( -1, -1, "Choose Thing Class:",
						   i, NULL, menuarr, dummy);

	if(val == 0)
		val = -1; 

	if(val == i) {
		input_integer = TRUE;
		val = InputIntegerValue(-1, -1, 0, 32767, 0);
	}

	if(val > 0) {
		if(!input_integer) {
			c = Thing_classes;
			while(--val)
				c = c->next;
			/* now c points to the class we are
			   going to display a submenu for */
			
			t = c->types;
			for(i = 0; (t && i < 30); i++) {
				spritestr[ i] = (char*)GetMemory( 40);
				spritestr[ i][0] = '\0';
				strcat(spritestr[ i], t->sprite);
				strcat(spritestr[ i], " : ");
				strcat(spritestr[ i], t->name);
				menuarr[i] = spritestr[ i];
				t = t->next;
			}
			
			if(i == 1) {
				val = c->types->type;
			}
			else {
				val = -1;
				val = DisplayMenuArray(-1, -1, "Select Thing Type:",
									   i, NULL, menuarr, dummy);
			}
			for(i = 0; (t && i < 30); i++) {
				FreeMemory( spritestr[ i]);
			}
		}
		
		if (val > 0) {
			if(!input_integer) {
				t = c->types;
				while(--val)
					t = t->next;
				val = t->type;
			}
		}
		else
			val = -1;
	}
	return val;
}

thing_class *SelectThingClass()
{
	int i;
	thing_class *c;
	char *menuarr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	BCINT val;

	c = Thing_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}

	val = DisplayMenuArray( -1, -1, "Choose Thing Class:",
						   i, NULL, menuarr, dummy);

	if(val == 0)
		return(thing_class *)NULL;  

	c = Thing_classes;
	while(--val)
		c = c->next;

	return c;
}
#ifdef GAME_HEXEN
BCINT SelectSpawnType()
{
	int i;
	spawn_class *c;
	spawn_type *t;
	char *menuarr[MENUARRAYMAX];
	char *spritestr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	BCINT val;
	Bool input_integer = FALSE;

	c = Spawn_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}
	menuarr[i++] = "Enter A Decimal Value";

	val = DisplayMenuArray( -1, -1, "Choose Thing Class:",
						   i, NULL, menuarr, dummy);

	if(val == 0)
		val = -1; 

	if(val == i) {
		input_integer = TRUE;
		val = InputIntegerValue(-1, -1, 0, 10503, 0);
	}

	if(val > 0) {
		if(!input_integer) {
			c = Spawn_classes;
			while(--val)
				c = c->next;
			/* now c points to the class we are
			   going to display a submenu for */
			
			t = c->types;
			for(i = 0; (t && i < 30); i++) {
				spritestr[ i] = (char*)GetMemory( 40);
				spritestr[ i][0] = '\0';
				strcat(spritestr[ i], t->sprite);
				strcat(spritestr[ i], " : ");
				strcat(spritestr[ i], t->name);
				menuarr[i] = spritestr[ i];
				t = t->next;
			}
			
			if(i == 1) {
				val = c->types->type;
			}
			else {
				val = -1;
				val = DisplayMenuArray(-1, -1, "Select Thing Type:",
									   i, NULL, menuarr, dummy);
			}
			for(i = 0; (t && i < 30); i++) {
				FreeMemory( spritestr[ i]);
			}
		}
		
		if (val > 0) {
			if(!input_integer) {
				t = c->types;
				while(--val)
					t = t->next;
				val = t->type;
			}
		}
		else
			val = -1;
	}
	return val;
}

spawn_class *SelectSpawnClass()
{
	int i;
	spawn_class *c;
	char *menuarr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	BCINT val;

	c = Spawn_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}

	val = DisplayMenuArray( -1, -1, "Choose Thing Class:",
						   i, NULL, menuarr, dummy);

	if(val > 0)
		return(spawn_class *)NULL; 

	c = Spawn_classes;
	while(--val)
		c = c->next;

	return c;
}
#endif

BCINT SelectScheme()
{
	int i;
	scheme_class *c;
	scheme_type *t;
	char *menuarr[MENUARRAYMAX];
	char *spritestr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	BCINT val, count = 0;

	c = Scheme_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
		count++;
	}
	if (count >= 2)
		val = DisplayMenuArray( -1, -1, "Choose Scheme Class:",
						   i, NULL, menuarr, dummy);
	else  
		val = 1;

	if(val == 0)
		val = -1; 

	if(val > 0) {
		c = Scheme_classes;
		while(--val)
			c = c->next;
		/* now c points to the class we are
		   going to display a submenu for */
		
		t = c->types;
		for(i = 0; (t && i < 30); i++) {
			spritestr[ i] = (char*)GetMemory( 40);
			spritestr[ i][0] = '\0';
			strcat(spritestr[ i], t->name);
			menuarr[i] = spritestr[ i];
			t = t->next;
		}
		
		if(i == 1) {
			val = c->types->type;
		}
		else {
			val = -1;
			val = DisplayMenuArray(-1, -1, "Select Scheme:",
								   i, NULL, menuarr, dummy);
		}
		for(i = 0; (t && i < 30); i++)
			FreeMemory( spritestr[ i]);
	}
		
	if (val > 0) {
		t = c->types;
		while(--val)
			t = t->next;
		CurrentScheme = t->name;
		DefaultWallTexture = t->normal;
		DefaultUpperTexture = t->upper;
		DefaultLowerTexture = t->lower;
		DefaultFloorTexture = t->floor;
		DefaultCeilingTexture = t->ceiling;
	}
	return 0;
}

scheme_class *SelectSchemeClass()
{
	int i;
	scheme_class *c;
	char *menuarr[MENUARRAYMAX];
	BCINT dummy[MENUARRAYMAX];
	BCINT val;

	c = Scheme_classes;
	for(i = 0; (c && i < 30); i++) {
		menuarr[i] = c->name;
		c = c->next;
	}

	val = DisplayMenuArray( -1, -1, "Choose Scheme Class:",
						   i, NULL, menuarr, dummy);

	if(val == 0)
		return(scheme_class *)NULL;  

	c = Scheme_classes;
	while(--val)
		c = c->next;

	return c;
}

/* end of file */

