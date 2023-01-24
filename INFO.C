/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   INFO.C - object display routines.
   */

/* the includes */
#include "deu.h"
#include "levels.h"

/*
   display the information about one object
   */
void DisplayObjectInfo( BCINT objtype, BCINT objnum) /* SWAP! */
{
	char texname[ 9];
#ifdef GAME_HEXEN
	char foo[ 40], argtext[ 12];
#endif
	BCINT  tag, n, val;
	BCINT  sd1 = -1, sd2 = -1, s1 = -1, s2 = -1;
	BCINT  x0, y0, x1, y1;
	BCINT  width, height;

	ShowSprite = FALSE;
	switch (objtype)
	{
	case OBJ_THINGS:
		x0 = 0;
#ifdef GAME_HEXEN
		y0 = ScrMaxY - 120;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D( x0, y0, x0 + 360, y0 + 120);
#else
		y0 = ScrMaxY - 50;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D( x0, y0, x0 + 330, y0 + 50);
#endif
		if (objnum < 0) {
			SetColor(LIGHTGRAY);

#ifdef GAME_HEXEN
			DrawScreenText( x0 + 120, y0 + 50, "(No Thing Selected)");
#else
			DrawScreenText( x0 + 70, y0 + 20, "(No Thing Selected)");
#endif
			break;
		}
		if (DispSprite) {
			y0 = ScrMaxY - 120;
			if (InfoShown)
				y0 -= 13;
			SetColor(BLACK);
			HideMousePointer();
			DrawScreenBox3D( ScrMaxX - 120, y0, ScrMaxX, y0 + 120);
			GetPicSize(GetThingSprite(Things[ objnum].type), &width, &height);
			if (Debug) {
				DrawScreenBox(ScrMaxX - 80, y0 - 30, ScrMaxX, y0 - 20);
				SetColor(ORANGE);
				DrawScreenText(ScrMaxX - 80, y0 - 30,"%d,%d", width, height);
				SetColor(BLACK);
			}
			x1 = ((120 - width) / 2);
			y1 = ((120 - height) / 2);
			if (x1 < 0)
				x1 = 4;
			if (y1 < 0)
				y1 = 4;
			DisplayPic( ScrMaxX - 120 + x1, y0 + y1 , ScrMaxX - 4, y0 + 98, GetThingSprite( Things[ objnum].type), 0); 
#ifdef GAME_HEXEN
			if (Things[ objnum].flags & 0x01)
				DrawScreen3DText(ScrMaxX - 112, y0 + 108, RED, FALSE, "E");
			else
				DrawScreen3DText(ScrMaxX - 112, y0 + 108, DARKERGRAY, TRUE, "E");
			if (Things[ objnum].flags & 0x02)
				DrawScreen3DText(ScrMaxX - 100, y0 + 108, RED, FALSE, "M");
			else
				DrawScreen3DText(ScrMaxX - 100, y0 + 108, DARKERGRAY, TRUE, "M");
			if (Things[ objnum].flags & 0x04)
				DrawScreen3DText(ScrMaxX - 88, y0 + 108, RED, FALSE, "H");
			else
				DrawScreen3DText(ScrMaxX - 88, y0 + 108, DARKERGRAY, TRUE, "H");
			if (Things[ objnum].flags & 0x20)
				DrawScreen3DText(ScrMaxX - 74, y0 + 108, BLUE, FALSE, "F");
			else
				DrawScreen3DText(ScrMaxX - 74, y0 + 108, DARKERGRAY, TRUE, "F");
			if (Things[ objnum].flags & 0x40)
				DrawScreen3DText(ScrMaxX - 62, y0 + 108, BLUE, FALSE, "C");
			else
				DrawScreen3DText(ScrMaxX - 62, y0 + 108, DARKERGRAY, TRUE, "C");
			if (Things[ objnum].flags & 0x80)
				DrawScreen3DText(ScrMaxX - 50, y0 + 108, BLUE, FALSE, "M");
			else
				DrawScreen3DText(ScrMaxX - 50, y0 + 108, DARKERGRAY, TRUE, "M");
			if (Things[ objnum].flags & 0x100)
				DrawScreen3DText(ScrMaxX - 36, y0 + 108, GREEN, FALSE, "S");
			else
				DrawScreen3DText(ScrMaxX - 36, y0 + 108, DARKERGRAY, TRUE, "S");
			if (Things[ objnum].flags & 0x200)
				DrawScreen3DText(ScrMaxX - 24, y0 + 108, GREEN, FALSE, "C");
			else
				DrawScreen3DText(ScrMaxX - 24, y0 + 108, DARKERGRAY, TRUE, "C");
			if (Things[ objnum].flags & 0x400)
				DrawScreen3DText(ScrMaxX - 12, y0 + 108, GREEN, FALSE, "D");
			else
				DrawScreen3DText(ScrMaxX - 12, y0 + 108, DARKERGRAY, TRUE, "D");
#else
			if (Things[ objnum].flags & 0x01)
				DrawScreen3DText(ScrMaxX - 112, y0 + 108, RED, FALSE, "E");
			else
				DrawScreen3DText(ScrMaxX - 112, y0 + 108, DARKERGRAY, TRUE, "E");
			if (Things[ objnum].flags & 0x02)
				DrawScreen3DText(ScrMaxX - 100, y0 + 108, RED, FALSE, "M");
			else
				DrawScreen3DText(ScrMaxX - 100, y0 + 108, DARKERGRAY, TRUE, "M");
			if (Things[ objnum].flags & 0x04)
				DrawScreen3DText(ScrMaxX - 88, y0 + 108, RED, FALSE, "H");
			else
				DrawScreen3DText(ScrMaxX - 88, y0 + 108, DARKERGRAY, TRUE, "H");
			if (Things[ objnum].flags & 0x08)
				DrawScreen3DText(ScrMaxX - 74, y0 + 108, BLUE, FALSE, "DEAF");
			else
				DrawScreen3DText(ScrMaxX - 74, y0 + 108, DARKERGRAY, TRUE, "DEAF");
			if (Things[ objnum].flags & 0x10)
				DrawScreen3DText(ScrMaxX - 36, y0 + 108, GREEN, FALSE, "MULT");
			else
				DrawScreen3DText(ScrMaxX - 36, y0 + 108, DARKERGRAY, TRUE, "MULT");
#endif
			ShowMousePointer();
		}
#ifdef GAME_HEXEN
		y0 = ScrMaxY - 120;
		if (InfoShown)
			y0 -= 13;
#else
		y0 = ScrMaxY - 50;
		if (InfoShown)
			y0 -= 13;
#endif
		SetColor( YELLOW);
		DrawScreenText( x0 + 5, y0 + 5, "Selected Thing (#%d),  (%4d)", objnum, Things[ objnum].type);
		SetColor( BLACK);
		DrawScreenText( -1, y0 + 20, "Type     : %s", GetThingName( Things[ objnum].type));
#ifdef GAME_HEXEN
		DrawScreenText( -1, -1, "Position : %d %s (%d, %d, %d)", Things[ objnum].angle, GetAngleName( Things[ objnum].angle), Things[ objnum].xpos, Things[ objnum].ypos, Things[ objnum].zpos);
		DrawScreenText( -1, -1, "                %s", GetThingFlagsName( Things[ objnum].flags));
#else
		SetColor(LIGHTGRAY);
		DrawScreenText( -1, -1, "Position : %d %s (%d, %d)", Things[ objnum].angle, GetAngleName( Things[ objnum].angle), Things[ objnum].xpos, Things[ objnum].ypos);
		SetColor( BLACK);
		DrawScreenText( -1, -1, "              %s", GetThingFlagsName( Things[ objnum].flags));
#endif
		DrawScreenText( -1, y0 + 40, "Flags    : %d", Things[objnum].flags);
#ifdef GAME_HEXEN
		DrawScreenText( -1, -1, "TID      : %d", Things[ objnum].tid);
		DrawScreenText( -1, -1, "Special  : %d %s", Things[ objnum].special ,GetLineDefTypeName( Things[ objnum].special));
		strcpy(argtext, GetLineDefArgs(Things[ objnum].special));
		if (argtext[0] == 'P')
			strcpy(foo, GetPuzzleItemName(Things[ objnum].arg1));
		else if (argtext[0] == 'T')
			strcpy(foo, GetSpawnName( Things[ objnum].arg1));
		else if (argtext[0] == 'S')
			strcpy(foo, GetSectorSoundName( Things[ objnum].arg1));
		else if (argtext[0] == 'A')
			strcpy(foo, GetHexenAngleName( Things[ objnum].arg1));
		else if (argtext[0] == 'N')
			strcpy(foo, GetNegativeName( Things[ objnum].arg1));
		else if (argtext[0] == 'K')
			strcpy(foo, GetKeyTypeName( Things[ objnum].arg1));
		else if ((Things[ objnum].type >= 104 && Things[ objnum].type <= 106) || Things[ objnum].type == 8064)
			strcpy(foo, GetSpawnName( Things[ objnum].arg1));
		else
		    	strcpy( foo, " ");
		if (Things[ objnum].type == 10225) /* Bat Spawner */
		    DrawScreenText( -1, -1, "Frequency: %d %s", Things[ objnum].arg1, foo);
		else if ((Things[ objnum].type >= 10000) && (Things[ objnum].type <= 10003)) /* Fog Spawner */
		    DrawScreenText( -1, -1, "Speed    : %d %s", Things[ objnum].arg1, foo);
		else if (Things[ objnum].type == 10225) /* Dragon Lich */
		    DrawScreenText( -1, -1, "TID      : %d %s", Things[ objnum].arg1, foo);
		else if ((Things[ objnum].type >= 104 && Things[ objnum].type <= 106) || Things[ objnum].type == 8064)
		    DrawScreenText( -1, -1, "Type     : %d %s", Things[ objnum].arg1, foo);
		else DrawScreenText( -1, -1, "%9s: %d %s", GetArg1Text(Things[ objnum].special), Things[ objnum].arg1, foo);
		if (argtext[1] == 'P')
			strcpy(foo, GetPuzzleItemName(Things[ objnum].arg2));
		else if (argtext[1] == 'T' )
			strcpy(foo, GetSpawnName( Things[ objnum].arg2));
		else if (argtext[1] == 'S')
			strcpy(foo, GetSectorSoundName( Things[ objnum].arg2));
		else if (argtext[1] == 'A')
			strcpy(foo, GetHexenAngleName( Things[ objnum].arg2));
		else if (argtext[1] == 'N')
			strcpy(foo, GetNegativeName( Things[ objnum].arg2));
		else if (argtext[1] == 'K')
			strcpy(foo, GetKeyTypeName( Things[ objnum].arg2));
		else
		    	strcpy( foo, " ");
		if (Things[ objnum].type == 10225) /* Bat Spawner */
		    DrawScreenText( -1, -1, "Angle    : %d %s", Things[ objnum].arg2, foo);
		else if (Things[ objnum].type == 10000) /* Fog Spawner */
		    DrawScreenText( -1, -1, "Spread   : %d %s", Things[ objnum].arg2, foo);
		else if ((Things[ objnum].type >= 10001) && (Things[ objnum].type <= 10003)) /* Fog Patches */
		    DrawScreenText( -1, -1, "Arg2(N/A): %d %s", Things[ objnum].arg2, foo);
		else if (Things[ objnum].type == 10225) /* Dragon Lich */
		    DrawScreenText( -1, -1, "TID      : %d %s", Things[ objnum].arg2, foo);
		else DrawScreenText( -1, -1, "%9s: %d %s", GetArg2Text(Things[ objnum].special), Things[ objnum].arg2, foo);
		if (argtext[2] == 'P')
			strcpy(foo, GetPuzzleItemName(Things[ objnum].arg3));
		else if (argtext[2] == 'T')
			strcpy(foo, GetSpawnName( Things[ objnum].arg3));
		else if (argtext[2] == 'S')
			strcpy(foo, GetSectorSoundName( Things[ objnum].arg3));
		else if (argtext[2] == 'A')
			strcpy(foo, GetHexenAngleName( Things[ objnum].arg3));
		else if (argtext[2] == 'N')
			strcpy(foo, GetNegativeName( Things[ objnum].arg3));
		else if (argtext[2] == 'K')
			strcpy(foo, GetKeyTypeName( Things[ objnum].arg3));
		else
		    	strcpy( foo, " ");
		if (Things[ objnum].type == 10225) /* Bat Spawner */
		    DrawScreenText( -1, -1, "Arg3(N/A): %d %s", Things[ objnum].arg3, foo);
		else if (Things[ objnum].type == 10000) /* Fog Spawner */
		    DrawScreenText( -1, -1, "Frequency: %d %s", Things[ objnum].arg3, foo);
		else if ((Things[ objnum].type >= 10001) && (Things[ objnum].type <= 10003)) /* Fog Patches */
		    DrawScreenText( -1, -1, "Arg3(N/A): %d %s", Things[ objnum].arg3, foo);
		else if (Things[ objnum].type == 10225) /* Dragon Lich */
		    DrawScreenText( -1, -1, "TID      : %d %s", Things[ objnum].arg3, foo);
		else DrawScreenText( -1, -1, "%9s: %d %s", GetArg3Text(Things[ objnum].special), Things[ objnum].arg3, foo);
		if (argtext[3] == 'P')
			strcpy(foo, GetPuzzleItemName(Things[ objnum].arg4));
		else if (argtext[3] == 'T')
			strcpy(foo, GetSpawnName( Things[ objnum].arg4));
		else if (argtext[3] == 'S')
			strcpy(foo, GetSectorSoundName( Things[ objnum].arg4));
		else if (argtext[3] == 'A')
			strcpy(foo, GetHexenAngleName( Things[ objnum].arg4));
		else if (argtext[3] == 'N')
			strcpy(foo, GetNegativeName( Things[ objnum].arg4));
		else if (argtext[3] == 'K')
			strcpy(foo, GetKeyTypeName( Things[ objnum].arg4));
		else
		    	strcpy( foo, " ");
		if (Things[ objnum].type == 10225) /* Bat Spawner */
		    DrawScreenText( -1, -1, "Duration : %d %s", Things[ objnum].arg4, foo);
		else if ((Things[ objnum].type >= 10000) && (Things[ objnum].type <= 10003)) /* Fog Spawner */
		    DrawScreenText( -1, -1, "Life Time: %d %s", Things[ objnum].arg4, foo);
		else if (Things[ objnum].type == 10225) /* Dragon Lich */
		    DrawScreenText( -1, -1, "TID      : %d %s", Things[ objnum].arg4, foo);
		else DrawScreenText( -1, -1, "%9s: %d %s", GetArg4Text(Things[ objnum].special), Things[ objnum].arg4, foo);
		if (argtext[4] == 'P')
			strcpy(foo, GetPuzzleItemName(Things[ objnum].arg5));
		else if (argtext[4] == 'T')
			strcpy(foo, GetSpawnName( Things[ objnum].arg5));
		else if (argtext[4] == 'S')
			strcpy(foo, GetSectorSoundName( Things[ objnum].arg5));
		else if (argtext[4] == 'A')
			strcpy(foo, GetHexenAngleName( Things[ objnum].arg5));
		else if (argtext[4] == 'N')
			strcpy(foo, GetNegativeName( Things[ objnum].arg5));
		else if (argtext[4] == 'K')
			strcpy(foo, GetKeyTypeName( Things[ objnum].arg5));
		else
		    	strcpy( foo, " ");
		if (Things[ objnum].type == 10225) /* Bat Spawner */
		    DrawScreenText( -1, -1, "Turn Ang : %d %s", Things[ objnum].arg5, foo);
		else if (Things[ objnum].type == 10000) /* Fog Spawner */
		    DrawScreenText( -1, -1, "Arg5(N/A): %d %s", Things[ objnum].arg5, foo);
		else if ((Things[ objnum].type >= 10001) && (Things[ objnum].type <= 10003)) /* Fog Patches */
		    DrawScreenText( -1, -1, "Moving?  : %d %s", Things[ objnum].arg5, foo);
		else if (Things[ objnum].type == 10225) /* Dragon Lich */
		    DrawScreenText( -1, -1, "TID      : %d %s", Things[ objnum].arg5, foo);
		else DrawScreenText( -1, -1, "%9s: %d %s", GetArg5Text(Things[ objnum].special), Things[ objnum].arg5, foo);
#endif
		break;
	case OBJ_LINEDEFS:
#ifdef GAME_HEXEN
		strcpy( foo, " ");
		x0 = 0;
		y0 = ScrMaxY - 130;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D(   x0, y0, x0 + 258, y0 + 130);
#else
		x0 = 0;
		y0 = ScrMaxY - 90;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D(   x0, y0, x0 + 258, y0 + 90);
#endif
		if (objnum >= 0) {
			SetColor( YELLOW);
			DrawScreenText( x0 + 5, y0 + 5, "Selected LineDef (#%d)", objnum);
			SetColor( BLACK);
			DrawScreenText( -1, y0 + 20, "Flags    : %s", GetLineDefFlagsName( LineDefs[ objnum].flags));
#ifdef GAME_HEXEN
			strcpy( foo, "Walk");
			if (LineDefs[ objnum].flags & 0x0800) 
				strcpy( foo, "Monster");
			if (LineDefs[ objnum].flags & 0x0400) {
				if (LineDefs[ objnum].flags & 0x0800) 
					strcpy( foo, "Hit");
				else
					strcpy( foo, "Switch");
			}
			if (LineDefs[ objnum].flags & 0x1000)  {
				if (LineDefs[ objnum].flags & 0x0400) 
					strcpy( foo, "Projectile");
				else
					strcpy( foo, "Push");
			}
			DrawScreenText( -1, -1, "Trigger  : %s", foo);
			strcpy( foo, " ");
#endif
			sd1 = LineDefs[ objnum].sidedef1;
			sd2 = LineDefs[ objnum].sidedef2;
			s1 = LineDefs[ objnum].start;
			s2 = LineDefs[ objnum].end;
#ifdef GAME_DOOM
			tag = LineDefs[ objnum].tag;
			if (tag > 0) {
				for (n = 0; n < NumSectors; n++)
					if (Sectors[ n].tag == tag)
						break;
			}
			else
				n = NumSectors;
			if (n < NumSectors)
				DrawScreenText( -1, -1, "Tag      : %d (#%d)", tag, n);
			else
				DrawScreenText( -1, -1, "Tag      : %d (none)", tag);
			val = LineDefs[ objnum].special;
			DrawScreenText( -1, -1, "Special  :");
			DrawScreenTextFonted(2, 0, x0 + 95, y0 + 37 , "(%3d) %s", val, GetLineDefTypeName( val));
#endif
			DrawScreenText( -1, -1, "Vertexes : (#%d, #%d)", s1, s2);
			SetColor( LIGHTGRAY);
			n = ComputeDist( Vertexes[ s2].x - Vertexes[ s1].x, Vertexes[ s2].y - Vertexes[ s1].y);
			DrawScreenText( -1, -1, "Length   : %d",n);
			SetColor( BLACK);
			DrawScreenText( -1, -1, "SideDefs : (#%d, #%d)", sd1, sd2);
#ifdef GAME_HEXEN
			val = LineDefs[ objnum].special;
			DrawScreenText( -1, -1, "Special  :");
			DrawScreenTextFonted(2, 0, x0 + 105, y0 + 67 , "(%3d) %s", val, GetLineDefTypeName( val));
		strcpy(argtext, GetLineDefArgs(LineDefs[ objnum].special));
		if (argtext[0] == 'P')
			strcpy(foo, GetPuzzleItemName(LineDefs[ objnum].arg1));
		else if (argtext[0] == 'T')
			strcpy(foo, GetSpawnName( LineDefs[ objnum].arg1));
		else if (argtext[0] == 'S')
			strcpy(foo, GetSectorSoundName( LineDefs[ objnum].arg1));
		else if (argtext[0] == 'A')
			strcpy(foo, GetHexenAngleName( LineDefs[ objnum].arg1));
		else if (argtext[0] == 'N')
			strcpy(foo, GetNegativeName( LineDefs[ objnum].arg1));
		else if (argtext[0] == 'K')
			strcpy(foo, GetKeyTypeName( LineDefs[ objnum].arg1));
		else
		    	strcpy( foo, " ");
		DrawScreenText( -1, -1, "%9s: %d %s", GetArg1Text(LineDefs[ objnum].special), LineDefs[ objnum].arg1, foo);
		if (argtext[1] == 'P')
			strcpy(foo, GetPuzzleItemName(LineDefs[ objnum].arg2));
		else if (argtext[1] == 'T')
			strcpy(foo, GetSpawnName( LineDefs[ objnum].arg2));
		else if (argtext[1] == 'S')
			strcpy(foo, GetSectorSoundName( LineDefs[ objnum].arg2));
		else if (argtext[1] == 'A')
			strcpy(foo, GetHexenAngleName( LineDefs[ objnum].arg2));
		else if (argtext[1] == 'N')
			strcpy(foo, GetNegativeName( LineDefs[ objnum].arg2));
		else if (argtext[1] == 'K')
			strcpy(foo, GetKeyTypeName( LineDefs[ objnum].arg2));
		else
		    	strcpy( foo, " ");
		DrawScreenText( -1, -1, "%9s: %d %s", GetArg2Text(LineDefs[ objnum].special), LineDefs[ objnum].arg2, foo);
		if (argtext[2] == 'P')
			strcpy(foo, GetPuzzleItemName(LineDefs[ objnum].arg3));
		else if (argtext[2] == 'T')
			strcpy(foo, GetSpawnName( LineDefs[ objnum].arg3));
		else if (argtext[2] == 'S')
			strcpy(foo, GetSectorSoundName( LineDefs[ objnum].arg3));
		else if (argtext[2] == 'A')
			strcpy(foo, GetHexenAngleName( LineDefs[ objnum].arg3));
		else if (argtext[2] == 'N')
			strcpy(foo, GetNegativeName( LineDefs[ objnum].arg3));
		else if (argtext[2] == 'K')
			strcpy(foo, GetKeyTypeName( LineDefs[ objnum].arg3));
		else
		    	strcpy( foo, " ");
		DrawScreenText( -1, -1, "%9s: %d %s", GetArg3Text(LineDefs[ objnum].special), LineDefs[ objnum].arg3, foo);
		if (argtext[3] == 'P')
			strcpy(foo, GetPuzzleItemName(LineDefs[ objnum].arg4));
		else if (argtext[3] == 'T')
			strcpy(foo, GetSpawnName( LineDefs[ objnum].arg4));
		else if (argtext[3] == 'S')
			strcpy(foo, GetSectorSoundName( LineDefs[ objnum].arg4));
		else if (argtext[3] == 'A')
			strcpy(foo, GetHexenAngleName( LineDefs[ objnum].arg4));
		else if (argtext[3] == 'N')
			strcpy(foo, GetNegativeName( LineDefs[ objnum].arg4));
		else if (argtext[3] == 'K')
			strcpy(foo, GetKeyTypeName( LineDefs[ objnum].arg4));
		else
		    	strcpy( foo, " ");
		DrawScreenText( -1, -1, "%9s: %d %s", GetArg4Text(LineDefs[ objnum].special), LineDefs[ objnum].arg4, foo);
		if (argtext[4] == 'P')
			strcpy(foo, GetPuzzleItemName(LineDefs[ objnum].arg5));
		else if (argtext[4] == 'T')
			strcpy(foo, GetSpawnName( LineDefs[ objnum].arg5));
		else if (argtext[4] == 'S')
			strcpy(foo, GetSectorSoundName( LineDefs[ objnum].arg5));
		else if (argtext[4] == 'A')
			strcpy(foo, GetHexenAngleName( LineDefs[ objnum].arg5));
		else if (argtext[4] == 'N')
			strcpy(foo, GetNegativeName( LineDefs[ objnum].arg5));
		else if (argtext[4] == 'K')
			strcpy(foo, GetKeyTypeName( LineDefs[ objnum].arg5));
		else
		    	strcpy( foo, " ");
		DrawScreenText( -1, -1, "%9s: %d %s", GetArg5Text(LineDefs[ objnum].special), LineDefs[ objnum].arg5, foo);
#endif
			if (sd1 >= 0)
				s1 = SideDefs[ sd1].sector;
			else
				s1 = -1;
			if (sd2 >= 0)
				s2 = SideDefs[ sd2].sector;
			else
				s2 = -1;
		}
		else {
			SetColor(LIGHTGRAY);
			DrawScreenText( x0 + 45, y0 + 35, "(No LineDef Selected)");
		}
		x0 = 259;
		y0 = ScrMaxY - 90;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D( x0, y0, x0 + 190, y0 + 90);
		if (objnum >= 0 && sd1 >= 0) {
			SetColor( YELLOW);
			DrawScreenText( x0 + 5, y0 + 5, "First SideDef (#%d)", sd1);
			SetColor( BLACK);
			texname[ 8] = '\0';
			strncpy( texname, SideDefs[ sd1].tex3, 8);
			DrawScreenText( -1, y0 + 20, "Norm. Texture: %s", texname);
			strncpy( texname, SideDefs[ sd1].tex1, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s1].ceilh > Sectors[ s2].ceilh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					SetColor( RED);
			}
			else
				SetColor(GRAY);
			DrawScreenText( -1, -1, "Upper Texture: %s", texname);
			SetColor( BLACK);
			strncpy( texname, SideDefs[ sd1].tex2, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s1].floorh < Sectors[ s2].floorh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					SetColor( RED);
			}
			else
				SetColor(GRAY);
			DrawScreenText( -1, -1, "Lower Texture: %s", texname);
			SetColor( BLACK);
			DrawScreenText( -1, -1, "Tex. X Offset: %d", SideDefs[ sd1].xoff);
			DrawScreenText( -1, -1, "Tex. Y Offset: %d", SideDefs[ sd1].yoff);
			SetColor( LIGHTGRAY);
		        DrawScreenText( -1, -1, "Sector Height: %d", Sectors[ s1].ceilh - Sectors[s1].floorh);
			SetColor( BLACK);
            		DrawScreenText( -1, -1, "Sector       : #%d", s1);
			SetColor( DARKGRAY);
			if (DispSprite) {
				HideMousePointer();
				x0 = ScrMaxX - 140;
				y0 = ScrMaxY - 210;
				if (VideoMode < 3)
					y0 -= 91;
				if (InfoShown) 
					y0 -= 13;
				DrawScreenBox3D( x0, y0, ScrMaxX, y0 + 210);
				DisplayWallQuick( x0 + 4, y0 + 4 , x0 + 131, y0 + 4 + 128, SideDefs[ sd1].tex1, 2);
				DisplayWallQuick( x0 + 4, y0 + 73, x0 + 131, y0 + 73 + 128, SideDefs[ sd1].tex3, 2);
				DisplayWallQuick( x0 + 4, y0 + 142, x0 + 131, y0 + 142 + 128, SideDefs[ sd1].tex2, 2);
				ShowMousePointer();
			}
		}
		else {
			SetColor(LIGHTGRAY);
			DrawScreenText( x0 + 24, y0 + 35, "(No First SideDef)");
			if (objnum >= 0)
				DrawScreenButton(x0 + 45, y0 + 60, 100, 16, "Add 1st SideDef", TRUE);
		}
		x0 = 450;
		y0 = ScrMaxY - 90;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D( x0, y0, x0 + 189, y0 + 90);
		if (objnum >= 0 && sd2 >= 0) {
			SetColor( YELLOW);
			DrawScreenText( x0 + 5, y0 + 5, "Second SideDef (#%d)", sd2);
			SetColor( BLACK);
			texname[ 8] = '\0';
			strncpy( texname, SideDefs[ sd2].tex3, 8);
			
			DrawScreenText( -1, y0 + 20, "Norm. Texture: %s", texname);
			strncpy( texname, SideDefs[ sd2].tex1, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s2].ceilh > Sectors[ s1].ceilh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					SetColor( RED);
			}
			else
				SetColor(GRAY);
			DrawScreenText( -1, -1, "Upper Texture: %s", texname);
			SetColor( BLACK);
			strncpy( texname, SideDefs[ sd2].tex2, 8);
			if (s1 >= 0 && s2 >= 0 && Sectors[ s2].floorh < Sectors[ s1].floorh) {
				if (texname[ 0] == '-' && texname[ 1] == '\0')
					SetColor( RED);
			}
			else
				SetColor(GRAY);
			DrawScreenText( -1, -1, "Lower Texture: %s", texname);
			SetColor( BLACK);
			DrawScreenText( -1, -1, "Tex. X Offset: %d", SideDefs[ sd2].xoff);
			DrawScreenText( -1, -1, "Tex. Y Offset: %d", SideDefs[ sd2].yoff);
			SetColor( LIGHTGRAY);
			DrawScreenText( -1, -1, "Sector Height: %d", Sectors[ s2].ceilh - Sectors[s2].floorh);
			SetColor( BLACK);
			DrawScreenText( -1, -1, "Sector       : #%d", s2);
			if (DispSprite) {
				HideMousePointer();
				x0 = ScrMaxX - 72;
				y0 = ScrMaxY - 210;
				if (VideoMode < 3)
					y0 -= 91;
				if (InfoShown) 
					y0 -= 13;
				DisplayWallQuick( x0 + 4, y0 + 4 , x0 + 131, y0 + 4 + 128, SideDefs[ sd2].tex1, 2);
				DisplayWallQuick( x0 + 4, y0 + 73, x0 + 131, y0 + 73 + 128, SideDefs[ sd2].tex3, 2);
				DisplayWallQuick( x0 + 4, y0 + 142, x0 + 131, y0 + 142 + 128, SideDefs[ sd2].tex2, 2);
				ShowMousePointer();
			}
		}
		else {
			SetColor(LIGHTGRAY);
			DrawScreenText( x0 + 20, y0 + 35, "(No Second SideDef)");
			if (sd1 >= 0)
				DrawScreenButton(x0 + 45, y0 + 60, 100, 16, "Add 2nd SideDef", TRUE);
		}
		break;
    case OBJ_VERTEXES:
		x0 = 0;
		y0 = ScrMaxY - 30;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D( x0, y0, x0 + 220, y0 + 30);
		if (objnum < 0) {
			SetColor(LIGHTGRAY);
			DrawScreenText( x0 + 30, y0 + 12, "(No Vertex Selected)");
			break;
		}
		SetColor( YELLOW);
		DrawScreenText( x0 + 5, y0 + 5, "Selected Vertex (#%d)", objnum);
		SetColor( BLACK);
		DrawScreenText( -1, y0 + 20, "Coordinates: (%d, %d)", Vertexes[ objnum].x, Vertexes[ objnum].y);
		break;
    case OBJ_SECTORS:
		x0 = 0;
		y0 = ScrMaxY - 80;
		if (InfoShown)
			y0 -= 13;
		DrawScreenBox3D( x0, y0, x0 + 278, y0 + 80);
		if (objnum < 0) {
			SetColor(LIGHTGRAY);
			DrawScreenText( x0 + 60, y0 + 42, "(No Sector Selected)");
			break;
		}
		SetColor( YELLOW);
		DrawScreenText( x0 + 5, y0 + 5, "Selected Sector (#%d)", objnum);
		SetColor( BLACK);
		texname[ 8] = '\0';
		strncpy( texname, Sectors[ objnum].floort, 8);
    	DrawScreenText( -1, y0 + 20, "Floor    : %d", Sectors[ objnum].floorh);
    	DrawScreenText( x0 + 150, y0 + 20, "%s", texname);
		if (DispSprite) {
			HideMousePointer();
			if (InfoShown) {
				DrawScreenBox3D( ScrMaxX - 72, ScrMaxY - 152, ScrMaxX, ScrMaxY - 13);
				DisplayFloorTexture( ScrMaxX - 68, ScrMaxY - 80 , ScrMaxX - 4, ScrMaxY - 80 + 64, texname);
			}
			else {
				DrawScreenBox3D( ScrMaxX - 72, ScrMaxY - 138, ScrMaxX, ScrMaxY);
				DisplayFloorTexture( ScrMaxX - 68, ScrMaxY - 67 , ScrMaxX - 4, ScrMaxY - 67 + 64, texname);
			}
			ShowMousePointer();
		}
		strncpy( texname, Sectors[ objnum].ceilt, 8);
		DrawScreenText( x0 + 5, y0 + 30, "Ceiling  : %d", Sectors[ objnum].ceilh);
    	DrawScreenText( x0 + 150, y0 + 30, "%s", texname);
		if (DispSprite) {
			HideMousePointer();
			if (InfoShown)
				DisplayFloorTexture( ScrMaxX - 68, ScrMaxY - 148 , ScrMaxX - 4, ScrMaxY - 148 + 64, texname);
			else
				DisplayFloorTexture( ScrMaxX - 68, ScrMaxY - 135 , ScrMaxX - 4, ScrMaxY - 135 + 64, texname);
			ShowMousePointer();
		}
		SetColor(LIGHTGRAY);
		DrawScreenText( x0 + 5, y0 + 40, "Height   : %d", Sectors[ objnum].ceilh - Sectors[objnum].floorh);
		SetColor(BLACK);
		DrawScreenText( -1, -1, "Light    : %d", Sectors[ objnum].light);
		tag = Sectors[ objnum].tag;
		if (tag == 0)
			n = NumLineDefs;
		else
			for (n = 0; n < NumLineDefs; n++)
#ifdef GAME_HEXEN
				if (LineDefs[ n].arg1 == tag)
#else
				if (LineDefs[ n].tag == tag)
#endif
					break;
		if (n < NumLineDefs)
            DrawScreenText( -1, -1, "Tag      : %d (#%d)", tag, n);
		else
            DrawScreenText( -1, -1, "Tag      : %d (none)", tag);
	    DrawScreenText( -1, -1, "Type     : %d %s", Sectors[ objnum].special, GetSectorTypeName( Sectors[ objnum].special));
		break;
    }
}



/* end of file */
