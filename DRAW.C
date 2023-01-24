/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   DRAW.C - Editor routines.
   */

/* the includes */
#include "deu.h"
#include "levels.h"

void DrawMapLinesQuick(BCINT);
void DrawInfoBar(BCINT, BCINT);

#ifdef GAME_HEXEN
	int tff = TF_EASY | TF_MEDIUM | TF_HARD | TF_FIGHTER | TF_CLERIC | TF_MAGE | TF_SINGLE | TF_COOP | TF_DEATHM;
#else
	int tff = TF_EASY | TF_MEDIUM | TF_HARD | TF_DM | TF_NOT_DM;
#endif


void DrawMapQuick(BCINT MoveSpeed)
{
  BCINT clip, yoff;
  Bool KeepScrolling = TRUE;
  BCINT PointerX, PointerY, buttons;

	clip = (InfoShown ? 13 : 0);
	yoff = 17;
    
	if (!BrowseMap)
	{
		setviewport(0, yoff, ScrMaxX, ScrMaxY - clip, TRUE);
		clearviewport();
		DrawMapLinesQuick(yoff); 
		setviewport(0, 0, ScrMaxX, ScrMaxY, TRUE);
		return;
	}
	else
	{
		DrawMapLinesQuick(0);
		DrawScreenBox3D( 0, ScrMaxY - 12, ScrMaxX, ScrMaxY);
		SetColor( WHITE);
		DrawScreenText( 5, ScrMaxY - 9, "Viewing: %s", Level->dir.name);
		DrawScreenText( ScrMaxX - 450, ScrMaxY - 9, "Browse: Up,Down,PgUp,PgDn,Home,End  View: Q,T,V,L,S,-,+");
		return;
	}
	setwritemode(XOR_PUT);
	DrawMapLinesQuick(yoff); 
	while (1 == 1)
	{
		if (!KeepScrolling)
			break;
		KeepScrolling = FALSE;
		GetMouseCoords(&PointerX, &PointerY, &buttons);
		if ((bioskey( 2) & 0x10) == 0x00)
		{
			if (PointerY <= (UseMouse ? 0 : 20))
			{
				if (!UseMouse)
					PointerY += MoveSpeed;
				if (MAPY( ScrCenterY) < MapMaxY)
				{								   
					DrawMapLinesQuick(yoff);
					OrigY += (BCINT) (MoveSpeed * 2.0 / Scale);
					DrawMapLinesQuick(yoff);
					KeepScrolling = TRUE;
				}
				else
					break;
			}
			if (PointerY >= ScrMaxY - (UseMouse ? 0 : 20))
			{
				if (!UseMouse)
					PointerY -= MoveSpeed;
				if (MAPY( ScrCenterY) > MapMinY)
				{
					DrawMapLinesQuick(yoff);
					OrigY -= (BCINT) (MoveSpeed * 2.0 / Scale);
					DrawMapLinesQuick(yoff);
					KeepScrolling = TRUE;
				}
				else
					break;
			}
			if (PointerX <= (UseMouse ? 0 : 20))
			{
				if (!UseMouse)   
					PointerX += MoveSpeed;
				if (MAPX( ScrCenterX) > MapMinX)
				{
					DrawMapLinesQuick(yoff);
					OrigX -= (BCINT) (MoveSpeed * 2.0 / Scale);
					DrawMapLinesQuick(yoff);
					KeepScrolling = TRUE;
				}
				else
					break;
			}
			if (PointerX >= ScrMaxX - (UseMouse ? 8 : 20))
			{
				if (!UseMouse)
					PointerX -= MoveSpeed;
				if (MAPX( ScrCenterX) < MapMaxX)
				{
					DrawMapLinesQuick(yoff);
					OrigX += (BCINT) (MoveSpeed * 2.0 / Scale);
					DrawMapLinesQuick(yoff);
					KeepScrolling = TRUE;
				}
				else
					break;
			}
		}
	}
	setviewport(0, 0, ScrMaxX, ScrMaxY, TRUE);
	setwritemode(COPY_PUT);
}

void DrawMapLinesQuick(BCINT yoff)
{
	BCINT n;

	SetColor(LIGHTGRAY);
	for (n = 0; n < NumLineDefs; n++) 
		DrawMapLineOff
		(
			Vertexes[LineDefs[n].start].x,
			Vertexes[LineDefs[n].start].y,
			Vertexes[LineDefs[n].end].x,
			Vertexes[LineDefs[n].end].y,
			yoff
		);
	
	/* draw in the things */
	SetColor(DARKGRAY);
	setlinestyle(1, 0, 1);
	for (n = 0; n < NumThings; n++)
	{
		DrawMapLineOff
		(
			Things[n].xpos - (OBJSIZE * 2),
			Things[n].ypos,
			Things[n].xpos + (OBJSIZE * 2),
			Things[n].ypos,
			yoff
		);
		DrawMapLineOff
		(
			Things[n].xpos,
			Things[n].ypos - (OBJSIZE * 2),
			Things[n].xpos,
			Things[n].ypos + (OBJSIZE * 2),
			yoff
		);
	}
	setlinestyle(0, 0, 1);
}

/*
   draw the actual game map							
   */

void DrawMap( BCINT editmode, BCINT grid, Bool drawgrid) /* SWAP! */
{
  BCINT val, n, m, o, max, grid2;
  BCINT sd1floor, sd2floor, sd1ceil, sd2ceil;
  BCINT width, height;
  BCINT scale = 1;
  Bool NotDrawn;
  int m2 = 0;
    

	max = (InfoShown ? 13 : 0);

	ClearMapScreen(17, max); 
    
  /* draw the grid */
  if (drawgrid && grid > 0)
	{					   
		BCINT mapx1 = (BCINT)(((int)(MAPX( ScrMaxX) + grid)) & ((int)~(grid - 1)));
		BCINT mapx0 = (BCINT)(((int)MAPX( 0)) & ((int)~(grid - 1)));
		BCINT mapy1 = (BCINT)(((int)MAPY( 0)) & ((int)~(grid - 1)));
		BCINT mapy0 = (BCINT)(((int)(MAPY( ScrMaxY - max) - grid)) & ((int)~(grid - 1)));
	
		/* HideMousePointer(); */


		if
		(
			(
				((SCREENX(mapx0 + grid) - SCREENX(mapx0)) >= 8) &&
				((SCREENY(mapy0) - SCREENY(mapy0 + grid)) >= 9)
			)
			||
			Scale >= 8.0
		)
		{
			SetColor(DARKBLUE);
			if (GridDashed)
				setlinestyle(1, 0, 1);
			for (n = mapx0; n <= mapx1; n += grid)
				DrawMapLine(n, mapy0, n, mapy1);
			for (n = mapy0; n <= mapy1; n += grid)
				DrawMapLine(mapx0, n, mapx1, n);
		}
		else
		{
			grid2 = grid * 2;
			while (SCREENX(mapx0 + grid2) - SCREENX(mapx0) < 8)
				grid2 *= 2;
			SetColor(DARKBLUE);
			if (GridDashed)
				setlinestyle(1, 0, 1);
			for (n = mapx0; n <= mapx1; n += grid2)
				DrawMapLine(n, mapy0, n, mapy1);
			for (n = mapy0; n <= mapy1; n += grid2)
				DrawMapLine(mapx0, n, mapx1, n);
		}
		setlinestyle(0, 0, 1);
  }
    
  /* draw the linedefs to form the map */
  switch (editmode)
	{
  	case OBJ_THINGS:
		for (n = 0; n < NumLineDefs; n++)
		{
			if (LineDefs[ n].flags & 1)
				SetColor(LIGHTGRAY);
			else if (LineDefs[ n].flags & 2)
				SetColor(LINEDEFNOPASS);
			else if (LineDefs[ n].flags & 32)
				SetColor(LINEDEFSECRET);
			else if (LineDefs[ n].flags & 64)
				SetColor(LINEDEFNOSOUND);
			else if (LineDefs[ n].flags & 128)
				SetColor(LINEDEFNOMAP);
			else
				SetColor(DARKGRAY);
			DrawMapLine(Vertexes[ LineDefs[ n].start].x, Vertexes[ LineDefs[ n].start].y,
						Vertexes[ LineDefs[ n].end].x, Vertexes[ LineDefs[ n].end].y);
		}
		break;
  	case OBJ_VERTEXES:
			SetColor(DARKGRAY);
			settextjustify(1,2);
			for (n = 0; n < NumLineDefs; n++)
			{
				DrawMapVector
				(
					Vertexes[LineDefs[n].start].x,
					Vertexes[LineDefs[n].start].y,
		  		Vertexes[LineDefs[n].end].x,
					Vertexes[LineDefs[n].end].y
				);
				if (ShowDistance)
				{
					/* HideMousePointer(); */
					o = (Vertexes[LineDefs[n].start].x + Vertexes[LineDefs[n].end].x) / 2;
					m = (Vertexes[LineDefs[n].start].y + Vertexes[LineDefs[n].end].y) / 2;
					val = ComputeDist
								(
									Vertexes[LineDefs[n].end].x -	Vertexes[LineDefs[n].start].x,
									Vertexes[LineDefs[n].end].y - Vertexes[LineDefs[n].start].y
								);
					SetColor(GRAY);
					if (val < 10)
						width = 10;
					if (val >= 10)
						width = 20;
					if (val >= 1000)
						width = 40;
					DrawScreenBox(SCREENX(o) - (width / 2), SCREENY(m) - 4,SCREENX(o) + (width / 2), SCREENY(m) + 4);
					SetColor(WHITE);
					DrawScreenTextFonted(2 ,0 ,SCREENX(o), SCREENY(m) - 6, "%d", val);
					SetColor(DARKGRAY);
				}
			}
			settextjustify(0,2);
			/* ShowMousePointer(); */
			break;
  	case OBJ_LINEDEFS:
  		if (!AutoMap)
			{
				for (n = 0; n < NumLineDefs; n++)
				{
					if (LineDefs[n].special > 0)
					{
#ifdef GAME_HEXEN
						if (LineDefs[n].arg1 > 0)
#else
						if (LineDefs[n].tag > 0)
#endif
							SetColor(LINEDEFTAGGED);
						else
							SetColor(LIGHTGREEN);
					}
#ifdef GAME_HEXEN
					else if (LineDefs[n].arg1 > 0)
#else
					else if (LineDefs[n].tag > 0)
#endif
						SetColor(LINEDEFTAGGED);
					else if (LineDefs[ n].flags & 1)
						SetColor(LIGHTGRAY); 
					else if (LineDefs[ n].flags & 2)
						SetColor(LINEDEFNOPASS);
					else if (LineDefs[ n].flags & 32)
						SetColor(LINEDEFSECRET);
					else if (LineDefs[ n].flags & 64)
						SetColor(LINEDEFNOSOUND);
					else if (LineDefs[ n].flags & 128)
						SetColor(LINEDEFNOMAP);
					else
						SetColor(DARKGRAY);
					o = (Vertexes[ LineDefs[ n].start].x + Vertexes[ LineDefs[ n].end].x) / 2;
					m = (Vertexes[ LineDefs[ n].start].y + Vertexes[ LineDefs[ n].end].y) / 2;
					DrawMapLine( Vertexes[ LineDefs[ n].start].x, Vertexes[ LineDefs[ n].start].y,
						Vertexes[ LineDefs[ n].end].x, Vertexes[ LineDefs[ n].end].y);
				}
			}
			else
			{
				for (n = 0; n < NumLineDefs; n++)
				{
					sd1floor = Sectors[SideDefs[LineDefs[ n].sidedef1].sector].floorh;
					sd2floor = Sectors[SideDefs[LineDefs[ n].sidedef2].sector].floorh;
					sd1ceil = Sectors[SideDefs[LineDefs[ n].sidedef1].sector].ceilh;
					sd2ceil = Sectors[SideDefs[LineDefs[ n].sidedef2].sector].ceilh;

					SetColor(RED);
					NotDrawn = FALSE;
					if (sd1floor == sd2floor)
						NotDrawn = TRUE;
					if (sd1floor > sd2floor)
					{
						NotDrawn = FALSE;
						SetColor(BROWN);
					}
					if (sd2floor > sd1floor)
					{
						NotDrawn = FALSE;
						SetColor(BROWN);
					}
					if (sd1ceil < sd2ceil)
					{
						NotDrawn = FALSE;
						SetColor(YELLOW);
					}
					if (sd2ceil < sd1ceil)
					{
						NotDrawn = FALSE;
						SetColor(YELLOW);
					}
					if (LineDefs[n].flags & 1)
					{ /* IMPASSIBLE */
						NotDrawn = FALSE;
						SetColor(RED);
					}
					if (LineDefs[n].flags & 32)
					{ /* SECRET */
						NotDrawn = FALSE;
						SetColor(RED);
					}
					if (LineDefs[n].flags & 128)  /* NOT ON MAP */
						NotDrawn = TRUE;
					if (LineDefs[n].flags & 256)
					{ /* ALREADY ON MAP */
						NotDrawn = FALSE;
						SetColor(RED);
					}

					if (!NotDrawn)
					{
						o = (Vertexes[LineDefs[n].start].x + Vertexes[LineDefs[n].end].x) / 2;
						m = (Vertexes[LineDefs[n].start].y + Vertexes[LineDefs[n].end].y) / 2;
						DrawMapLine
						(
							Vertexes[LineDefs[n].start].x,
							Vertexes[LineDefs[n].start].y,
							Vertexes[LineDefs[n].end].x,
							Vertexes[LineDefs[n].end].y
						);
					}
				}
			}
			break;
  	case OBJ_SECTORS:
			for (n = 0; n < NumLineDefs; n++)
			{
				if ((m = LineDefs[ n].sidedef1) < 0 || (m = SideDefs[ m].sector) < 0)
					SetColor( CYAN);
				else {
					if (Sectors[ m].tag > 0)
						SetColor( SECTORTAGGED);
					else if (Sectors[ m].special == 26 || Sectors[ m].special == 27)
				 		SetColor( SECTORPAINFUL);
					else if (Sectors[ m].special == 200)
						SetColor( SECTORSECRET);
					else if (Sectors[ m].special == 1 || Sectors[ m].special == 2 || Sectors[ m].special == 3 || Sectors[ m].special == 4 || Sectors[ m].special == 8 || Sectors[ m].special == 12 || Sectors[ m].special == 17 || Sectors[ m].special == 199 || Sectors[ m].special == 198) 
						SetColor( SECTORLIGHT);
					else if (LineDefs[ n].flags & 1)
						SetColor(LIGHTGRAY);
					else if (LineDefs[ n].flags & 2)
						SetColor(LINEDEFNOPASS);
					else if (LineDefs[ n].flags & 32)
						SetColor(LINEDEFSECRET);
					else if (LineDefs[ n].flags & 64)
						SetColor(LINEDEFNOSOUND);
					else if (LineDefs[ n].flags & 128)
						SetColor(LINEDEFNOMAP);
					else
						SetColor(DARKGRAY);

					if ((m = LineDefs[ n].sidedef2) >= 0)
					{
						if ((m = SideDefs[ m].sector) < 0)
							SetColor( LIGHTBLUE);
						else if (Sectors[ m].tag > 0)
							SetColor( SECTORTAGGED);
						else if (Sectors[ m].special == 7 || Sectors[ m].special == 5 || Sectors[ m].special == 16 || Sectors[ m].special == 4 || Sectors[ m].special == 11 || Sectors[ m].special == 6)
							SetColor( SECTORPAINFUL);
						else if (Sectors[ m].special == 9)
							SetColor( SECTORSECRET);
						else if (Sectors[ m].special == 2 || Sectors[ m].special == 3 || Sectors[ m].special == 13 || Sectors[ m].special == 12 || Sectors[ m].special == 1 || Sectors[ m].special == 8 || Sectors[ m].special == 17) 
							SetColor( SECTORLIGHT);
					}
				}
				DrawMapLine( Vertexes[ LineDefs[ n].start].x, Vertexes[ LineDefs[ n].start].y,
							Vertexes[ LineDefs[ n].end].x, Vertexes[ LineDefs[ n].end].y);
				o = (Vertexes[ LineDefs[ n].start].x + Vertexes[ LineDefs[ n].end].x) / 2;
				m = (Vertexes[ LineDefs[ n].start].y + Vertexes[ LineDefs[ n].end].y) / 2;
			}
			break;
  }
  
  /* draw in the vertices */
  if (editmode == OBJ_VERTEXES)
	{
		SetColor( LIGHTGREEN);
		for (n = 0; n < NumVertexes; n++)
		{
			DrawMapLine( Vertexes[ n].x - ( OBJSIZE / 4 ), Vertexes[ n].y - ( OBJSIZE / 4 ), Vertexes[ n].x + ( OBJSIZE / 4 ), Vertexes[ n].y - ( OBJSIZE / 4 ));
			DrawMapLine( Vertexes[ n].x + ( OBJSIZE / 4 ), Vertexes[ n].y - ( OBJSIZE / 4 ), Vertexes[ n].x + ( OBJSIZE / 4 ), Vertexes[ n].y + ( OBJSIZE / 4 ));
			DrawMapLine( Vertexes[ n].x - ( OBJSIZE / 4 ), Vertexes[ n].y - ( OBJSIZE / 4 ), Vertexes[ n].x - ( OBJSIZE / 4 ), Vertexes[ n].y + ( OBJSIZE / 4 ));
			DrawMapLine( Vertexes[ n].x - ( OBJSIZE / 4 ), Vertexes[ n].y + ( OBJSIZE / 4 ), Vertexes[ n].x + ( OBJSIZE / 4 ), Vertexes[ n].y + ( OBJSIZE / 4 ));
		}
  }
  
  /* draw in the things */
  if (editmode == OBJ_THINGS)
	{
		if (ShowThings && (1.0 / Scale + 0.5) < 14)
		{
			if (Scale < 1.0) 
				scale = (1.0 / Scale + 0.5);
			else 
				scale = 1;
		}

		for (n = 0; n < NumThings; n++)
		{
			if(!(Things[n].flags & tff) && Things[n].flags != 0)
				continue;

#ifdef GAME_DOOM
			if(!((Things[n].flags & TF_DM) ?
				(tff & TF_DM) : (tff & TF_NOT_DM)))
				continue;
#endif

			if (ShowThings && (1.0 / Scale + 0.5) < 14)
			{
				if (bioskey(1))
				{
					setlinestyle(0, 0, 1);
					return;
				}
				setlinestyle(1, 0, 1);
				if (SCREENX(Things[n].xpos) >= 0 && SCREENY(Things[n].ypos) >= 0)
				{
					if (Scale > 1.0)
					{
						SetColor(DARKERGRAY);
						m = GetThingRadius(Things[n].type);
						DrawMapLine( Things[ n].xpos - ( OBJSIZE * 2 ), Things[ n].ypos, Things[ n].xpos + ( OBJSIZE * 2 ), Things[ n].ypos);
						DrawMapLine( Things[ n].xpos, Things[ n].ypos - ( OBJSIZE * 2 ), Things[ n].xpos, Things[ n].ypos + ( OBJSIZE * 2 ));
						switch(Things[n].angle)
						{
							case 0:
								DrawMapVector(Things[n].xpos - m2, Things[n].ypos, Things[n].xpos + m, Things[n].ypos);
								break;
							case 180:
								DrawMapVector(Things[n].xpos + m2, Things[n].ypos, Things[n].xpos - m, Things[n].ypos);
								break;
							case 90:
								DrawMapVector(Things[n].xpos, Things[n].ypos - m2, Things[n].xpos, Things[n].ypos + m);
								break;
							case 270:
								DrawMapVector(Things[n].xpos, Things[n].ypos + m2, Things[n].xpos, Things[n].ypos - m);
								break;
							case 45:
								DrawMapVector(Things[n].xpos - m2 / 1.4, Things[n].ypos - m2 / 1.4, Things[n].xpos + m / 1.4, Things[n].ypos + m / 1.4);
								break;
							case 315:
								DrawMapVector(Things[n].xpos - m2 / 1.4, Things[n].ypos + m2 / 1.4, Things[n].xpos + m / 1.4, Things[n].ypos - m / 1.4);
								break;
							case 225:
								DrawMapVector(Things[n].xpos + m2 / 1.4, Things[n].ypos + m2 / 1.4, Things[n].xpos - m / 1.4, Things[n].ypos - m / 1.4);
								break;
							case 135:
								DrawMapVector(Things[n].xpos + m2 / 1.4, Things[n].ypos - m2 / 1.4, Things[n].xpos - m / 1.4, Things[n].ypos + m / 1.4);
						}
					}
					GetPicSize(GetThingSprite(Things[n].type), &width, &height);
					DisplayPicScaled(SCREENX(Things[n].xpos) - (width / (2 * scale)), SCREENY(Things[n].ypos) - (height / (2 * scale)), ScrMaxX * scale, ScrMaxY * scale, GetThingSprite(Things[n].type), scale, 0); 
				}
			}
			else
			{
				if (bioskey(1))
					return;
				m = GetThingRadius(Things[n].type);
				SetColor( GetThingColour(Things[n].type));
				if (!ThingAngle)
				{
					DrawMapLine( Things[n].xpos - m , Things[n].ypos, Things[n].xpos + m, Things[n].ypos);
					DrawMapLine( Things[n].xpos, Things[n].ypos - m, Things[n].xpos, Things[n].ypos + m);
				}
				else
				{
					switch(Things[n].angle)
					{
						case 0:
							DrawMapVector(Things[n].xpos - m2, Things[n].ypos, Things[n].xpos + m, Things[n].ypos);
							break;
						case 180:
							DrawMapVector(Things[n].xpos + m2, Things[n].ypos, Things[n].xpos - m, Things[n].ypos);
							break;
						case 90:
							DrawMapVector(Things[n].xpos, Things[n].ypos - m2, Things[n].xpos, Things[n].ypos + m);
							break;
						case 270:
							DrawMapVector(Things[n].xpos, Things[n].ypos + m2, Things[n].xpos, Things[n].ypos - m);
							break;
						case 45:
							DrawMapVector(Things[n].xpos - m2 / 1.4, Things[n].ypos - m2 / 1.4, Things[n].xpos + m / 1.4, Things[n].ypos + m / 1.4);
							break;
						case 315:
							DrawMapVector(Things[n].xpos - m2 / 1.4, Things[n].ypos + m2 / 1.4, Things[n].xpos + m / 1.4, Things[n].ypos - m / 1.4);
							break;
						case 225:
							DrawMapVector(Things[n].xpos + m2 / 1.4, Things[n].ypos + m2 / 1.4, Things[n].xpos - m / 1.4, Things[n].ypos - m / 1.4);
							break;
						case 135:
							DrawMapVector(Things[n].xpos + m2 / 1.4, Things[n].ypos - m2 / 1.4, Things[n].xpos - m / 1.4, Things[n].ypos + m / 1.4);
					}
				}
				DrawMapCircle( Things[ n].xpos, Things[ n].ypos, m + 2);
			}
		}
		if (ShowThings && (1.0 / Scale + 0.5) < 14)
			setlinestyle(0, 0, 1);
  }
  else
	{
    if (!AutoMap)
		{
			for (n = 0; n < NumThings; n++)
			{
				SetColor(DARKGRAY);
				m = GetThingRadius(Things[n].type);
				DrawMapLine( Things[ n].xpos - ( OBJSIZE * 2 ), Things[ n].ypos, Things[ n].xpos + ( OBJSIZE * 2 ), Things[ n].ypos);
				DrawMapLine( Things[ n].xpos, Things[ n].ypos - ( OBJSIZE * 2 ), Things[ n].xpos, Things[ n].ypos + ( OBJSIZE * 2 ));
			}
		}
  }

  if (BrowseMap)
	{
		DrawScreenBox3D( 0, ScrMaxY - 12, ScrMaxX, ScrMaxY);
		SetColor( WHITE);
		DrawScreenText( 5, ScrMaxY - 9, "Viewing: %s", Level->dir.name);
		DrawScreenText( ScrMaxX - 450, ScrMaxY - 9, "Browse: Up,Down,PgUp,PgDn,Home,End  View: Q,T,V,L,S,-,+");
	}

  if (!BrowseMap)
	{
		/* draw in the title bar */
		DrawScreenBox3D( 0, 0, ScrMaxX, 16);
		SetColor( WHITE);
		DrawScreenText( 7,  4, "File Edit Search Mode Misc(F8) Objects(F9) Check(F10)");
		DrawScreenText( 7,  6, "_    _    _      _     _       _           _");
#ifndef SLIM
		DrawScreenText( ScrMaxX - 50, 4, "Help");
		DrawScreenText( ScrMaxX - 50, 6, "_"); 
#endif
	
	}
	DrawInfoBar(editmode, grid);
}

void DrawInfoBar(BCINT editmode, BCINT grid)
{
	if (InfoShown && !BrowseMap) {
		DrawScreenBox3D(0, ScrMaxY - 12, ScrMaxX, ScrMaxY);
		if (!AltInfo)
		{
			if (MadeMapChanges)
				DrawScreenTextFonted(2, 0, 5, ScrMaxY - 12, "%s on %s #", GetEditModeName( editmode), Level->dir.name);
			else if (MadeChanges)
				DrawScreenTextFonted(2, 0, 5, ScrMaxY - 12, "%s on %s *", GetEditModeName( editmode), Level->dir.name);
			else
				DrawScreenTextFonted(2, 0, 5, ScrMaxY - 12, "%s on %s", GetEditModeName( editmode), Level->dir.name);
		}
		else
		{
			SetColor(BLUE);
			DrawScreenTextFonted(2, 0, 5, ScrMaxY - 12, "Undo: %s", BackupText);
		}
		SetColor(BLACK);
		if (Scale < 1.0)
			DrawScreenTextFonted(2, 0, ScrMaxX - 140, ScrMaxY - 12, "Scale: 1/%d", (BCINT) (1.0 / Scale + 0.5));
		else
			DrawScreenTextFonted(2, 0, ScrMaxX - 140, ScrMaxY - 12, "Scale: %d/1", (BCINT) Scale);
		DrawScreenTextFonted(2, 0, ScrMaxX - 62, ScrMaxY - 12, "Grid: %d", grid);
		DrawScreenTextFonted(2, 0, ScrMaxX - 360, ScrMaxY - 12, "Ctl: %d", CloseToLine);
		if (farcoreleft() < 50000L)
		{
			if (farcoreleft() < 20000L)
				SetColor(CYAN);
			else
				SetColor(RED);
		}
		DrawScreenTextFonted(2, 0, ScrMaxX - 310, ScrMaxY - 12, "Mem: %lu", farcoreleft());
		SetColor(YELLOW);
		DrawScreenTextFonted(2, 0, ScrMaxX - 205, ScrMaxY - 12, "Sel: %d", Count);
	}
}


/*
   center the map around the given coords
   */

void CenterMapAroundCoords( BCINT xpos, BCINT ypos)
{
  OrigX = xpos;
  OrigY = ypos;
  PointerX = ScrCenterX;
  PointerY = ScrCenterY;
}



/*
   center the map around the object and zoom in if necessary
   */

void GoToObject(BCINT objtype, BCINT objnum) /* SWAP! */
{
  BCINT xpos, ypos;
  BCINT xpos2, ypos2;
  BCINT n;
  BCINT sd1, sd2;
  float oldscale;
  
  GetObjectCoords( objtype, objnum, &xpos, &ypos);
  CenterMapAroundCoords( xpos, ypos);
  oldscale = Scale;
  
  /* zoom in until the object can be selected */
  while (Scale < 4.0 && GetCurObject( objtype, MAPX( PointerX - 4), MAPY( PointerY - 4), MAPX( PointerX + 4), MAPY( PointerY + 4)) != objnum)
	{
		if (Scale < 1.0)
			Scale = 1.0 / ((1.0 / Scale) - 1.0);
		else
			Scale = Scale * 2.0;
 	}

  /* Special case for Sectors: if several Sectors are one inside another, then	 */
  /* zooming in on the center won't help.  So I choose a LineDef that borders the  */
  /* the Sector and move a few pixels towards the inside of the Sector.			*/
  if (objtype == OBJ_SECTORS && GetCurObject( OBJ_SECTORS, OrigX, OrigY, OrigX, OrigY) != objnum)
	{
		/* restore the Scale */
		Scale = oldscale;
		for (n = 0; n < NumLineDefs; n++)
		{
			sd1 = LineDefs[ n].sidedef1;
			sd2 = LineDefs[ n].sidedef2;
			if (sd1 >= 0 && SideDefs[ sd1].sector == objnum)
				break;
			if (sd2 >= 0 && SideDefs[ sd2].sector == objnum)
				break;
		}
		if (n < NumLineDefs)
		{
			GetObjectCoords( OBJ_LINEDEFS, n, &xpos2, &ypos2);
			n = ComputeDist( abs( xpos - xpos2), abs( ypos - ypos2)) / 7;
			if (n <= 1)
				n = 2;
			xpos = xpos2 + (xpos - xpos2) / n;
			ypos = ypos2 + (ypos - ypos2) / n;
			CenterMapAroundCoords( xpos, ypos);
			/* zoom in until the sector can be selected */
			while (Scale > 4.0 && GetCurObject( OBJ_SECTORS, OrigX, OrigY, OrigX, OrigY) != objnum)
			{
				if (Scale < 1.0)
					Scale = 1.0 / ((1.0 / Scale) - 1.0);
				else
					Scale = Scale / 2.0;
			}
		}
  }
  if (UseMouse)
		SetMouseCoords( PointerX, PointerY);
}
/* end of file */
