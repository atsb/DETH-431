/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   Menu utilities by Rapha‰l Quinet <quinet@montefiore.ulg.ac.be>
   Drop-down menus by Dewi Morgan <d.morgan@bradford.ac.uk>
   
   MENUS.C - Simple menus for DEU (or other programs).
   */

/* the includes */
#include "deu.h"
#include "levels.h"
#include "keys.h"

BCINT BrowseWalls(char *);
BCINT BrowseFlats(char *);
void XAlignmentMenu(SelPtr *);
void YAlignmentMenu(SelPtr *);

/*
   menu of miscellaneous operations
   */



BCINT MiscOperations( BCINT x0, BCINT y0, BCINT objtype, SelPtr *list, BCINT CurObject)
{
    char   msg[ 80];
    BCINT	m;
    BCINT	angle, scale;
    
    sprintf( msg, "Rotate and Scale %s", GetEditModeName( objtype));
    if (objtype == OBJ_VERTEXES) {
		m = DisplayMenu( x0, y0, ((x0 == -1) ? "Misc. Operations" : NULL),
						  "Find First Free Tag Number",
						  "-",
						  msg,
						  "Delete Vertex And Join LineDefs",
						  "Merge Several Vertices Into One",
						  "Add A LineDef And Split Sector",
						  NULL);
    }
    else if (objtype == OBJ_LINEDEFS) {
		m = DisplayMenu( x0, y0, ((x0 == -1) ? "Misc. Operations" : NULL),
						  "Find First Free Tag Number",
						  "-",
						  msg,
						  "Split LineDef And Add A Vertex",
						  "Split LineDefs And Add A Sector",
						  "Delete LineDefs And Join Sectors",
							"Connect Linedefs And Split Donut",
							"Connect Linedefs And Make Hallway",
						  "-",
						  "Flip LineDef",
						  "Swap SideDefs",
						  "-",
						  "Align Textures Intelligently On Y",
						  "Align Textures Intelligently On X",
						  "-",
						  "Search and Replace Textures",
						  "-",
						  "Renumber Selected LineDefs Lowest",
						  NULL);
    }
    else if (objtype == OBJ_SECTORS) {
		m = DisplayMenu( x0, y0, ((x0 == -1) ? "Misc. Operations" : NULL),
						  "Find First Free Tag Number",
						  "-",
						  msg,
						  "Make Door From Sector",
						  "Make Lift From Sector",
						  "-",
						  "Distribute Sector Floor Heights",
						  "Distribute Sector Ceiling Heights",
						  "Distribute Sector Light Levels",
						  "-",
						  "Search and Replace Textures",
						  "-",
						  "Clear all Secret Sectors",
							"-",
							"Renumber Selected Sectors Lowest",
						  NULL);
    }														 
    else {
		m = DisplayMenu( x0, y0, ((x0 == -1) ? "Misc. Operations" : NULL),
						  "Find First Free Tag Number",
#ifdef GAME_HEXEN
						  "Find First Free Tid Number",
#endif
						  "-",
						  msg,
						  NULL);
    }
    if (m <= 0) /*m == -1 || m == -2) */
    	return m;
#ifdef GAME_HEXEN
    if (m > 1 && m != 3 && m != 2 && *list == NULL) {
#else
    if (m > 1 && m != 3 && m != 15 && *list == NULL) {
#endif
		Beep();
		sprintf( msg, "You Must Select At Least One %s", GetObjectTypeName( objtype));
		Notify( -1, -1, msg, NULL);
		return 0;
    }
    if (objtype == OBJ_THINGS) {
		switch(m) {
			case 1:
				sprintf( msg, "First Free Tag Number: %d", FindFreeTag());
				Notify( -1, -1, msg, NULL);
				break;
#ifdef GAME_HEXEN
			case 2:
				sprintf( msg, "First Free Tid Number: %d", FindFreeTid());
				Notify( -1, -1, msg, NULL);
				break;
			case 4:
#else
			case 3:
#endif
			if ((*list)->next == NULL) {
				Beep();
				sprintf( msg, "You Must Select More Than One Thing");
				Notify( -1, -1, msg, NULL);
				return 0;
			}
			angle = 0;
			scale = 100;
			if (Input2Numbers( -1, -1, "Rotation Angle (ø)", "Scale (%)", 360, 1000, &angle, &scale))
				Backup(BTHINGS);
				strcpy(BackupText, "Rotate And Scale Things");
				RotateAndScaleObjects( objtype, *list, (double) angle * 0.0174533, (double) scale * 0.01, 0);
			break;
		}
    }
    else if (objtype == OBJ_LINEDEFS) {
		switch(m) {
			case 1:
				sprintf( msg, "First Free Tag Number: %d", FindFreeTag());
				Notify( -1, -1, msg, NULL);
				break;
			case 3:
				angle = 0;
				scale = 100;
				if (Input2Numbers( -1, -1, "Rotation Angle (ø)", "Scale (%)", 360, 1000, &angle, &scale))
					Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
					strcpy (BackupText, "Rotate And Scale LineDefs");
					RotateAndScaleObjects( objtype, *list, (double) angle * 0.0174533, (double) scale * 0.01, 0);
				break;
			case 4:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Split LineDefs");
				SplitLineDefs( *list);
				break;
			case 5:
				if ((*list)->next == NULL || (*list)->next->next != NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Exactly Two LineDefs", NULL);
				}
				else {
					Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
					strcpy (BackupText, "Split LineDefs And Add Sector");
					SplitLineDefsAndSector( (*list)->objnum, (*list)->next->objnum);
					ForgetSelection( list);
				}
				break;
			case 6:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
				strcpy (BackupText, "Delete LineDefs and Join Sectors");
				DeleteLineDefsJoinSectors( list);
				break;
			case 7:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
				strcpy (BackupText, "Connect LineDefs and Split Donut");
				FunctionConnectLineDefsSplitDonut(-1,*list);
				ForgetSelection( list);
				break;
			case 8:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
				strcpy (BackupText, "Connect LineDefs and Make Hallway");
				FunctionConnectLineDefsMakeHallway(-1,*list);
				ForgetSelection( list);
				break;
			case 10:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Flip SideDefs");
				FlipLineDefs( *list, TRUE);
				break;
			case 11:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Swap SideDefs");
				FlipLineDefs( *list, FALSE);
				break;
			case 13:
				YAlignmentMenu( list);
				break;
			case 14:
				XAlignmentMenu( list);
				break;
			case 16:
				{
					char origtexture[9];
					int newtexture;				//jff add Replace Any texture functions
					int n, count, val;
					int *sdfs=NULL;
					SelPtr cur;

					// Make a table of selected sidedefs

					sdfs = malloc(NumSideDefs*sizeof(int));
					memset(sdfs,0,NumSideDefs*sizeof(int));	// default none selected

					// if something selected other than CurObject
					if (*list && ((*list)->next!=NULL || (*list)->objnum!=CurObject)) 
						for (cur=*list;cur;cur=cur->next)			// restrict list to selected
						{																			// sidedefs
							if (LineDefs[cur->objnum].sidedef1 >=0)
								sdfs[LineDefs[cur->objnum].sidedef1]=1;
							if (LineDefs[cur->objnum].sidedef2 >=0)
								sdfs[LineDefs[cur->objnum].sidedef2]=1;
						}
					else
						memset(sdfs,1,NumSideDefs*sizeof(int));	// default all selected

					val = DisplayMenu( x0, y0, ((x0 == -1) ? "Search For Which Texture?" : NULL),
								  "Normal Texture (Match with 1st SideDef)",
								  "Upper Texture            -- \" --",
								  "Lower Texture            -- \" --",
								  "All Textures from Normal (1st Sidedef)",
								  "All Textures from Upper  -- \" --",
								  "All Textures from Lower  -- \" --",
								  "Normal Texture (Match with 2nd SideDef)",
								  "Upper Texture            -- \" --",
								  "Lower Texture            -- \" --",
								  "All Textures from Normal (2nd Sidedef)",
								  "All Textures from Upper  -- \" --",
								  "All Textures from Lower  -- \" --",
								  NULL);
					switch(val) {
						case 1:
						case 7:
							if (val == 1)
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef1].tex3, 8);
							else
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef2].tex3, 8);
							newtexture = BrowseWalls(origtexture);
							if (newtexture >= 0) {
								Backup (BSIDEDEFS);
								count = 0;
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex3, origtexture, 8) == 0) {
										count ++;
										strncpy(SideDefs[n].tex3, WTexture[newtexture], 8);
									}
								sprintf (BackupText, "Replace %d Normal Wall Textures", count);
							}
							break;
						case 2:
						case 8:
							if (val == 2)
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef1].tex1, 8);
							else
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef2].tex1, 8);
							newtexture = BrowseWalls(origtexture);
							if (newtexture >= 0) {
								Backup (BSIDEDEFS);
								count = 0;
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex1, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex1, WTexture[newtexture], 8);
										count++;
									}
								sprintf (BackupText, "Replace %d Upper Wall Textures", count);
							}
							break;
						case 3:
						case 9:
							if (val == 3)
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef1].tex2, 8);
							else
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef2].tex2, 8);
							newtexture = BrowseWalls(origtexture);
							if (newtexture >= 0) {
								Backup (BSIDEDEFS);
								count = 0;
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex2, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex2, WTexture[newtexture], 8);
										count++;
									}
								sprintf (BackupText, "Replace %d Lower Wall Textures", count);
							}
							break;
						case 4:
						case 10:
							if (val == 4)
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef1].tex3, 8);
							else
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef2].tex3, 8);
							newtexture = BrowseWalls(origtexture);
							if (newtexture >= 0) {
								Backup (BSIDEDEFS);
								count = 0;
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex2, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex2, WTexture[newtexture], 8);
										count++;
									}
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex1, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex1, WTexture[newtexture], 8);
										count++;
									}
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex3, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex3, WTexture[newtexture], 8);
										count++;
									}
								sprintf (BackupText, "Replace %d Wall Textures", count);
							}
							break;
						case 5:
						case 11:
							if (val == 5)
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef1].tex1, 8);
							else
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef2].tex1, 8);
							newtexture = BrowseWalls(origtexture);
							if (newtexture >= 0) {
								Backup (BSIDEDEFS);
								count = 0;
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex2, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex2, WTexture[newtexture], 8);
										count++;
									}
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex1, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex1, WTexture[newtexture], 8);
										count++;
									}
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex3, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex3, WTexture[newtexture], 8);
										count++;
									}
								sprintf (BackupText, "Replace %d Wall Textures", count);
							}
							break;
						case 6:
						case 12:
							if (val == 6)
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef1].tex2, 8);
							else
								strncpy(origtexture, SideDefs[LineDefs[CurObject].sidedef2].tex2, 8);
							newtexture = BrowseWalls(origtexture);
							if (newtexture >= 0) {
								Backup (BSIDEDEFS);
								count = 0;
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex2, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex2, WTexture[newtexture], 8);
										count++;
									}
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex1, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex1, WTexture[newtexture], 8);
										count++;
									}
								for (n = 0; n < NumSideDefs; n++)
									if (sdfs[n] && strncmp(SideDefs[n].tex3, origtexture, 8) == 0) {
										strncpy(SideDefs[n].tex3, WTexture[newtexture], 8);
										count++;
									}
								sprintf (BackupText, "Replace %d Wall Textures", count);
							}
							break;
						}

				free(sdfs);
				break;
				}
			case 18:
				FunctionRenumberLineDefsLowest(CurObject,*list);
				ForgetSelection (list);
				CurObject = -1;
				break;

		}
    }
    else if (objtype == OBJ_VERTEXES) {
		switch(m) {
			case 1:
				sprintf( msg, "First Free Tag Number: %d", FindFreeTag());
				Notify( -1, -1, msg, NULL);
				break;
			case 3:
				if ((*list)->next == NULL) {
					Beep();
					sprintf( msg, "You Must Select More Than One Vertex.");
					Notify( -1, -1, msg, NULL);
					return 0;
				}
				angle = 0;
				scale = 100;
				if (Input2Numbers( -1, -1, "Rotation Angle (ø)", "Scale (%)", 360, 1000, &angle, &scale))
					Backup (BVERTEXES);
					strcpy (BackupText, "Rotate And Scale Vertexes");
					RotateAndScaleObjects( objtype, *list, (double) angle * 0.0174533, (double) scale * 0.01, 0);
				break;
			case 4:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Delete Vertexes And Join LineDefs");
				DeleteVerticesJoinLineDefs( *list);
				ForgetSelection( list);
				break;
			case 5:
				Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS);
				strcpy (BackupText, "Merge Vertexes");
				MergeVertices( list);
				break;
			case 6:
				if ((*list)->next == NULL || (*list)->next->next != NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Exactly Two Vertices", NULL);
				}
				else {
					Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
					strcpy (BackupText, "Split Sector Between 2 Vertexes");
					SplitSector( (*list)->objnum, (*list)->next->objnum);
					ForgetSelection( list);
				}
				break;
		}
    }
    else if (objtype == OBJ_SECTORS) {
		switch(m) {
			case 1:
				sprintf( msg, "First Free Tag Number: %d", FindFreeTag());
				Notify( -1, -1, msg, NULL);
				break;
			case 3:
				angle = 0;
				scale = 100;
				if (Input2Numbers( -1, -1, "Rotation Angle (ø)", "Scale (%)", 360, 1000, &angle, &scale))
					Backup (BSECTORS);
					strcpy (BackupText, "Rotate And Scale Sectors");
					RotateAndScaleObjects( objtype, *list, (double) angle * 0.0174533, (double) scale * 0.01, 0);
				break;
			case 4:
				if ((*list)->next != NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Exactly One Sector", NULL);
				}
				else {
					Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
					strcpy (BackupText, "Make Door From Sector");
					MakeDoorFromSector( (*list)->objnum);
				}
				break;
			case 5:
				if ((*list)->next != NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Exactly One Sector", NULL);
				}
				else {
					Backup (BVERTEXES | BLINEDEFS | BSIDEDEFS | BSECTORS);
					strcpy (BackupText, "Make Lift From Sector");
					MakeLiftFromSector( (*list)->objnum);
				}
				break;
			case 7:
				if ((*list)->next == NULL || (*list)->next->next == NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Three Or More Sectors", NULL);
				}
				else {
					Backup (BSECTORS);
					strcpy (BackupText, "Distribute Sector Floor Heights");
					DistributeSectorFloors( *list);
				}
				break;
			case 8:
				if ((*list)->next == NULL || (*list)->next->next == NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Three Or More Sectors", NULL);
				}
				else {
					Backup (BSECTORS);
					strcpy (BackupText, "Distribute Sector Ceiling Heights");
					DistributeSectorCeilings( *list);
				}
				break;
			case 9:
				if ((*list)->next == NULL || (*list)->next->next == NULL) {
					Beep();
					Notify( -1, -1, "You Must Select Three Or More Sectors", NULL);
				}
				else {
					Backup (BSECTORS);
					strcpy (BackupText, "Distribute Sector Light Levels");
					DistributeLightLevels( *list);
				}
				break;
			case 11:
			{
				char origtexture[9];
				int newtexture;
				int n, count, val;
				val = DisplayMenu( x0, y0, ((x0 == -1) ? "Replace Which Texture?" : NULL),
							  "Floor Texture",
							  "Ceiling Texture",
								"Both From Floor",
								"Both From Ceiling",
							  NULL);
				switch(val) {
					case 1:
						strncpy(origtexture, Sectors[CurObject].floort, 8);
						newtexture = BrowseFlats(origtexture);
						if (newtexture >= 0) {
							Backup (BSECTORS);
							count = 0;
							for (n = 0; n < NumSectors; n++)
								if (strncmp(Sectors[n].floort, origtexture, 8) == 0) {
									strncpy(Sectors[n].floort, FTexture[newtexture], 8);
									count++;
								}
							sprintf (BackupText, "Replace %d Floor Textures", count);
						}
						break;
					case 2:
						strncpy(origtexture, Sectors[CurObject].ceilt, 8);
						newtexture = BrowseFlats(origtexture);
						if (newtexture >= 0) {
							Backup (BSECTORS);
							count = 0;
							for (n = 0; n < NumSectors; n++)
								if (strncmp(Sectors[n].ceilt, origtexture, 8) == 0) {
									strncpy(Sectors[n].ceilt, FTexture[newtexture], 8);
									count++;
								}
							sprintf (BackupText, "Replace %d Ceiling Textures", count);
						}
						break;
					case 3:
					{
						int DoSel;

						DoSel = *list && ((*list)->next!=NULL || (*list)->objnum!=CurObject);
						strncpy(origtexture, Sectors[CurObject].floort, 8);
						newtexture = BrowseFlats(origtexture);
						if (newtexture >= 0) {
							Backup (BSECTORS);
							count = 0;
							for (n = 0; n < NumSectors; n++)
								if (!DoSel || IsSelected(*list,n))
									if (strncmp(Sectors[n].floort, origtexture, 8) == 0)
									{
										strncpy(Sectors[n].floort, FTexture[newtexture], 8);
										count++;
									}
							for (n = 0; n < NumSectors; n++)
								if (!DoSel || IsSelected(*list,n))
									if (strncmp(Sectors[n].ceilt, origtexture, 8) == 0)
									{
										strncpy(Sectors[n].ceilt, FTexture[newtexture], 8);
										count++;
									}
							sprintf (BackupText, "Replace %d Flats", count);
						}
						break;
					}
					case 4:
					{
						int DoSel;

						DoSel = *list && ((*list)->next!=NULL || (*list)->objnum!=CurObject);
						strncpy(origtexture, Sectors[CurObject].ceilt, 8);
						newtexture = BrowseFlats(origtexture);
						if (newtexture >= 0) {
							Backup (BSECTORS);
							count = 0;
							for (n = 0; n < NumSectors; n++)
								if (!DoSel || IsSelected(*list,n))
									if (strncmp(Sectors[n].floort, origtexture, 8) == 0)
									{
										strncpy(Sectors[n].floort, FTexture[newtexture], 8);
										count++;
									}
							for (n = 0; n < NumSectors; n++)
								if (!DoSel || IsSelected(*list,n))
									if (strncmp(Sectors[n].ceilt, origtexture, 8) == 0)
									{
										strncpy(Sectors[n].ceilt, FTexture[newtexture], 8);
										count++;
									}
							sprintf (BackupText, "Replace %d Flats", count);
						}
						break;
					}
				}
				break;
			}
			case 13:
			{
				int n,count;

				Backup (BSECTORS);
				count = 0;
				for (n=0; n< NumSectors; n++)
				{
					if ((Sectors[n].special&15)==9)
					{
						Sectors[n].special -= 9;
						Sectors[n].special &= ~128;
						count++;
					}
					else if (BOOMEnable && (Sectors[n].special&128))
					{
						Sectors[n].special &= ~128;
						count++;
					}
				}
				sprintf (BackupText, "Cleared %d Sector Secrets",count);
				break;
			}
			case 15:
				FunctionRenumberSectorsLowest(CurObject,*list);
				ForgetSelection(list);
				break;
		}
    }
    return 0;
}



/*
   display a "Display Options" menu 
*/

void Options( BCINT x0, BCINT y0)
{
    char   *menustr[MENUARRAYMAX];
    BCINT	 dummy[MENUARRAYMAX];
    BCINT	 n, val;
    BCINT	items = 17;
    Bool	done = FALSE;
    
    if (x0 < 0)
		x0 = (ScrMaxX - 50 * 8) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY + items * 10 + 20 ) / 2;
	do {
		for (n = 0; n < items + 1; n++)
			menustr[ n] = (char*)GetMemory((items + 1 ) * 10);
		sprintf( menustr[items], "Display Options");
		sprintf( menustr[ 0], "LineDef Dimensioning   (%s)", (ShowDistance) ? "On" : "Off");
		sprintf( menustr[ 1], "AutoMap Display        (%s)", (AutoMap) ? "On" : "Off");
		sprintf( menustr[ 2], "Graphical Things       (%s)", (ShowThings) ? "On" : "Off");
		sprintf( menustr[ 3], "Information Bar        (%s)", (InfoShown) ? "On" : "Off");
		sprintf( menustr[ 4], "Alternative Info-bar   (%s)", (AltInfo) ? "On" : "Off");
		sprintf( menustr[ 5], "Graphical Information  (%s)", (DispSprite) ? "On" : "Off");
		sprintf( menustr[ 6], "-");
		sprintf( menustr[ 7], "Grid                   (%s)", (GridShown) ? "On" : "Off");
		sprintf( menustr[ 8], "Grid Dashed            (%s)", (GridDashed) ? "On" : "Off");
		sprintf( menustr[ 9], "Min Grid               (%d)", MinGrid);
		sprintf( menustr[10], "Max Grid               (%d)", MaxGrid);
		sprintf( menustr[11], "-");
		sprintf( menustr[12], "Set Video Mode         (%d)", VideoMode);
		sprintf( menustr[13], "Set Gamma Correction   (%d)", Gamma);
		sprintf( menustr[14], "Set Scroll Factor      (%d)", DefaultLargeScroll);
		sprintf( menustr[15], "-");
		sprintf( menustr[16], "Exit Menu");
		val = DisplayMenuArray( -1, -1, menustr[items], items, NULL, menustr, dummy);

		if (val <= 0)
			done = TRUE;
		switch (val) {
		case 1:
			ShowDistance = (ShowDistance ? FALSE : TRUE);
			break;
		case 2:
			AutoMap = (AutoMap ? FALSE : TRUE);
			break;
		case 3:
			ShowThings = (ShowThings ? FALSE : TRUE);
			break;
		case 4:
			InfoShown = (InfoShown ? FALSE : TRUE);
			break;
		case 5:
			AltInfo = (AltInfo ? FALSE : TRUE);
			break;
		case 6:
			DispSprite = (DispSprite ? FALSE : TRUE);
			break;
		case 8:
			GridShown = (GridShown ? FALSE : TRUE);
			break;
		case 9:
			GridDashed = (GridDashed ? FALSE : TRUE);
			break;
		case 10:
			n = InputIntegerValue( -1, -1, 1, 8, MinGrid);
			if (n >= 1)
				MinGrid = n;
			break;
		case 11:
			n = InputIntegerValue( -1, -1, 256, 2048, MaxGrid);
			if (n >= 256)
				MaxGrid = n;
			break;
		case 13:
			n = InputIntegerValue( -1, -1, 2, 5, VideoMode);
			if (n >= 2)
				VideoMode = n;
			break;
		case 14:
			n = InputIntegerValue( -1, -1, 0, 4, Gamma);
			if (n >= 0) {
				Gamma = n;
				SetDoomPalette(0);
			}
			break;
		case 15:
			n = InputIntegerValue( -1, -1, 2, 32, DefaultLargeScroll);
			if (n >= 2) {
				DefaultLargeScroll = n;
				MoveSpeed = DefaultLargeScroll;
			}
			break;
		case 17:	
			done = TRUE;
			break;
		}			   
		MenuDefault = val - 1;
    } while (done == FALSE);
    if (VideoMode != RealVideoMode) {
    	if (Caching) {
			for (n = 0; n < 64; n++) /* JFF Made bigger for more textures, <= to < */
				if (GotImages[n])
					free(WallImages[n]);
			memset(GotImages,0,64*sizeof(int));
		}
		RealVideoMode = VideoMode;
		TermGfx();
		DethFont = 0;
		InitGfx();
		CheckMouseDriver();
	}
    MenuDefault = 0;
}

void PrefabPrefs( BCINT x0, BCINT y0)
{
    char   *menustr[MENUARRAYMAX];
    BCINT	 dummy[MENUARRAYMAX];
    BCINT	 n, val;
    char	texname[ 9];
    int		tex;
    
    if (x0 < 0)
		x0 = (ScrMaxX - 50 * 8 - 19) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 5 * 10 - 28) / 2;
	for (n = 0; n < 8; n++)
		menustr[ n] = (char*)GetMemory( 120);
	sprintf( menustr[ 7], "Prefab Preferences");
	sprintf( menustr[ 0], "Load Texture Scheme  (%s)", CurrentScheme == NULL ? "None" : CurrentScheme);
	sprintf( menustr[ 1], "-");
	sprintf( menustr[ 2], "Normal Texture       (%s)", DefaultWallTexture);
	sprintf( menustr[ 3], "Upper Texture        (%s)", DefaultUpperTexture);
	sprintf( menustr[ 4], "Lower Texture        (%s)", DefaultLowerTexture);
	sprintf( menustr[ 5], "Floor Texture        (%s)", DefaultFloorTexture);
	sprintf( menustr[ 6], "Ceiling Texture      (%s)", DefaultCeilingTexture);
	val = DisplayMenuArray( -1, -1, menustr[ 7], 7, NULL, menustr, dummy);
	for (n = 0; n < 8; n++)
		FreeMemory( menustr[ n]);
	switch (val) {
	case 1:
		SelectScheme();
		break;
	case 3:
		strcpy( texname, DefaultWallTexture);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultWallTexture, WTexture[tex]);
		break;
	case 4:
		strcpy( texname, DefaultUpperTexture);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultUpperTexture, WTexture[tex]);
		break;
	case 5:
		strcpy( texname, DefaultLowerTexture);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultLowerTexture, WTexture[tex]);
		break;
	case 6:
		strcpy( texname, DefaultFloorTexture);
		tex = BrowseFlats(texname);
		if (tex >= 0)
			strcpy(DefaultFloorTexture, FTexture[tex]);
		break;
	case 7:
		strcpy( texname, DefaultCeilingTexture);
		tex = BrowseFlats(texname);
		if (tex >= 0)
			strcpy(DefaultCeilingTexture, FTexture[tex]);
		break;
	}
}

void TexturePrefs( BCINT x0, BCINT y0)
{
    char   *menustr[MENUARRAYMAX];
    BCINT	 dummy[MENUARRAYMAX];
    BCINT	 n, val, tex;
    char	texname[ 9];
    
    if (x0 < 0)
		x0 = (ScrMaxX - 50 * 8 - 19) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 5 * 10 - 28) / 2;
    for (n = 0; n < 9; n++)
		menustr[ n] = (char*)GetMemory( 90);
    sprintf( menustr[ 9], "Other Prefrab Preferences");
    sprintf( menustr[ 0], "Change Default Floor Height     (%d)", (int)DefaultFloorHeight & 0xffff);
    sprintf( menustr[ 1], "Change Default Ceiling Height   (%d)", (int)DefaultCeilingHeight & 0xffff);
    sprintf( menustr[ 2], "Change Default Lighting         (%d)", (int)DefaultLighting & 0xffff);
    sprintf( menustr[ 3], "-");	 // jff add default lighting
    sprintf( menustr[ 4], "Change Door Track Texture       (%s)", DefaultDoorTrack);
    sprintf( menustr[ 5], "Change Door Texture             (%s)", DefaultDoorTexture);
    sprintf( menustr[ 6], "Change Upper Step Texture       (%s)", DefaultUpperStepTexture);
    sprintf( menustr[ 7], "Change Lower Step Texture       (%s)", DefaultLowerStepTexture);
    sprintf( menustr[ 8], "Change Teleport Texture         (%s)", DefaultTeleportTexture);
    val = DisplayMenuArray( -1, -1, menustr[ 9], 9, NULL, menustr, dummy);
    for (n = 0; n < 10; n++)
		FreeMemory( menustr[ n]);
    switch (val) {
    case 1:
		val = InputIntegerValue( -1, -1, -512, 511, DefaultFloorHeight);
		if (val >= -512)
			DefaultFloorHeight = val;
		break;
    case 2:
		val = InputIntegerValue( -1, -1, -512, 511, DefaultCeilingHeight);
		if (val >= -512)
			DefaultCeilingHeight = val;
		break;
    case 3: //jff add default lighting
		val = InputIntegerValue( -1, -1, 0, 255, DefaultLighting);
		if (val >= 0)
			DefaultLighting = val;
		break;
    case 5:
		strcpy( texname, DefaultDoorTrack);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultDoorTrack, WTexture[tex]);
		break;
    case 6:
		strcpy( texname, DefaultDoorTexture);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultDoorTexture, WTexture[tex]);
		break;
    case 7:
		strcpy( texname, DefaultUpperStepTexture);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultUpperStepTexture, WTexture[tex]);
		break;
    case 8:
		strcpy( texname, DefaultLowerStepTexture);
		tex = BrowseWalls(texname);
		if (tex >= 0)
			strcpy(DefaultLowerStepTexture, WTexture[tex]);
		break;
    case 9:
		strcpy( texname, DefaultTeleportTexture);
		tex = BrowseFlats(texname);
		if (tex >= 0)
			strcpy(DefaultTeleportTexture, FTexture[tex]);
		break;
    }			   
}

/*
   display a "Preferences" menu (change default textures, etc.)
   */

void MiscOptions( BCINT x0, BCINT y0)
{
    char   *menustr[MENUARRAYMAX];
    BCINT	 dummy[MENUARRAYMAX];
    BCINT	 n, val;
    Bool	done = FALSE;
    
    if (x0 < 0)
		x0 = (ScrMaxX - 50 * 8 - 19) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 6 * 10 - 28) / 2;
	do {
		for (n = 0; n < 21; n++)
			menustr[ n] = (char*)GetMemory(100);
		sprintf( menustr[20], "Other ini File Settings");
		sprintf( menustr[ 0], "Sector Increment              (%d)", (int)Increment);
		sprintf( menustr[ 1], "How Close To Line To Split    (%d)", (int)CloseToLine);
		sprintf( menustr[ 2], "Marquee Stretch Delay         (%d)", (int)StretchDelay);
		sprintf( menustr[ 3], "-");
		sprintf( menustr[ 4], "Graphical Front End           (%s)", (GraphFront) ? "Yes" : "No");
		sprintf( menustr[ 5], "Front End - Prompt Only       (%s)", (UsePromptOnly) ? "Yes" : "No");
		sprintf( menustr[ 6], "Confirmation Of Vertex Join   (%s)", (VertConf) ? "On" : "Off");
		sprintf( menustr[ 7], "Editor Mode                   (%s)", (Expert) ? "Expert" : "Novice");
		sprintf( menustr[ 8], "'Q' Key Is...                 (%s)", (QisQuit) ? "Quit" : "Save");
		sprintf( menustr[ 9], "Additive Selection Box        (%s)", (AdditiveSelBox) ? "Yes" : "No");
		sprintf( menustr[10], "Select Object 0               (%s)", (Select0) ? "Yes" : "No");
		sprintf( menustr[11], "Copy SideDefs                 (%s)", (CopySideDefs) ? "Yes" : "No");
		sprintf( menustr[12], "Swap Mouse Buttons            (%s)", (SwapButtons) ? "Yes" : "No");
		sprintf( menustr[13], "Nest Pull-Down Menus          (%s)", (NestMenus) ? "Yes" : "No");
		sprintf( menustr[14], "Debugging Mode                (%s)", (Debug) ? "On" : "Off");
		sprintf( menustr[15], "Cache Textures                (%s)", (Caching) ? "On" : "Off");
		sprintf( menustr[16], "Automaticaly Save ini File    (%s)", (SaveIniFile) ? "Yes" : "No");
		sprintf( menustr[17], "Choose Game at Start          (%s)", (ChooseGame) ? "Yes" : "No");
		sprintf( menustr[18], "-");
		sprintf( menustr[19], "Exit Menu");
		val = DisplayMenuArray( -1, -1, menustr[ 20], 20, NULL, menustr, dummy);
		if (val <= 0)
			done = TRUE;
		for (n = 0; n < 21; n++)
			FreeMemory( menustr[ n]);
		switch (val) {
		case 1:
			n = InputIntegerValue( -1, -1, 1, 256, Increment);
			if (n >= 1)
				Increment = n;
			break;
		case 2:
			n = InputIntegerValue( -1, -1, 0, 16, CloseToLine);
			if (n >= 0)
				CloseToLine = n;
			break;
		case 3:
			n = InputIntegerValue( -1, -1, 1, 100, StretchDelay);
			if (n >= 0)
				StretchDelay = n;
			break;
		case 5:
			GraphFront = (GraphFront ? FALSE : TRUE);
			break;
		case 6:
			UsePromptOnly = (UsePromptOnly ? FALSE : TRUE);
			break;
		case 7:
			VertConf = (VertConf ? FALSE : TRUE);
			break;
		case 8:
			Expert = (Expert ? FALSE : TRUE);
			break;
		case 9:
			QisQuit = (QisQuit ? FALSE : TRUE);
			break;
		case 10:
			AdditiveSelBox = (AdditiveSelBox ? FALSE : TRUE);
			break;
		case 11:
			Select0 = (Select0 ? FALSE : TRUE);
			break;
		case 12:
			CopySideDefs = (CopySideDefs ? FALSE : TRUE);
			break;
		case 13:
			SwapButtons = (SwapButtons ? FALSE : TRUE);
			break;
		case 14:
			NestMenus = (NestMenus ? FALSE : TRUE);
			break;
		case 15:
			Debug = (Debug ? FALSE : TRUE);
			break;
		case 16:
			Caching = (Caching ? FALSE : TRUE);
			break;
		case 17:
			SaveIniFile = (SaveIniFile ? FALSE : TRUE);
			break;
		case 18:
			ChooseGame = (ChooseGame ? FALSE : TRUE);
			break;
		case 20:
			done = TRUE;
			break;
		}			   
		MenuDefault = val - 1;
    } while (done == FALSE);
    MenuDefault = 0;
}


void Preferences( BCINT x0, BCINT y0)
{
    char   *menustr[MENUARRAYMAX];
    BCINT	 dummy[MENUARRAYMAX];
    BCINT	 n, val;
    
    if (x0 < 0)
		x0 = (ScrMaxX - 50 * 8 - 19) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 5 * 10 - 28) / 2;
    for (n = 0; n < 6; n++)
		menustr[ n] = (char*)GetMemory( 60);
    sprintf( menustr[ 5], "Preferences");
    sprintf( menustr[ 0], "Prefab Texture Settings");
    sprintf( menustr[ 1], "Other Prefab Settings");
    sprintf( menustr[ 2], "-");
    sprintf( menustr[ 3], "Display Options");
    sprintf( menustr[ 4], "Other ini File Settings");
    val = DisplayMenuArray( -1, -1, menustr[ 5], 5, NULL, menustr, dummy);
    for (n = 0; n < 6; n++)
		FreeMemory( menustr[ n]);
    switch (val) {
    case 1:
    	PrefabPrefs(-1, -1);
		break;
    case 2:
    	TexturePrefs(-1, -1);
		break;
    case 4:
    	Options(-1, -1);
		break;
    case 5:
    	MiscOptions(-1, -1);
		break;
    }			   
}

BCINT BrowseFlats(char *CurFlat)
{
	int maxnumx, maxnumy, val, n, key;
	int nx, ny, x, y, page;
	int oldval = 0;
	BCINT buttons;
	char foo[9];
	int currentx = 0;
	int currenty = 0;
	Bool NotThisPage = FALSE;

	maxnumx = 8;
	maxnumy = 5;

	if (VideoMode == 3) {
		maxnumx = 11;
		maxnumy = 7;
	}
	else if (VideoMode == 4) {
		maxnumx = 14;
		maxnumy = 9;
	}
	else if (VideoMode == 5) {
		maxnumx = 17;
		maxnumy = 12;
	}

	page = 0;
	strncpy(foo, CurFlat, 8);
	for (n = 0; n < NumFTexture; n++) 
		if (strncmp(FTexture[n], foo, 8) == 0 ) {
			page = (n / (maxnumx * maxnumy));
			break;
		}
	ClearScreen();
	while (1 == 1)
	{
		NotThisPage = TRUE;
		for (ny = 0; ny < maxnumy; ny++)
		{
			for (nx = 0; nx < maxnumx; nx++)
			{
				x = (nx * 72) + 8;
				y = (ny * 80);				
				n = (nx + (ny * maxnumx)) + (maxnumx * maxnumy * page);
				if (n < NumFTexture)
				{
					if (strncmp(FTexture[n], foo, 8) == 0 )
					{
						SetColor(YELLOW);
						DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
						DrawScreenFrame(x - 2, y, x + 65, y + 67);
						SetColor(RED);
						DrawScreenBox(x - 2, y + 68, x + 65, y + 78);
						currentx = nx;
						currenty = ny;
						NotThisPage = FALSE;
					}
					SetColor(WHITE);
					DrawScreenText(x + 0, y + 70, "%s", FTexture[n]);
					DisplayFloorTexture(x, y + 2, x + 64, y + 66, FTexture[n]);
				}
			}
		}
		if (page > 0) {
			DrawScreenButton(ScrMaxX - 140, ScrMaxY - 16, 40, 16, "<", TRUE);
			DrawScreenButton(ScrMaxX - 180, ScrMaxY - 16, 30, 16, "<<", TRUE);
		}
		else {
			DrawScreenButtonGray(ScrMaxX - 140, ScrMaxY - 16, 40, 16, "<", TRUE);
			DrawScreenButtonGray(ScrMaxX - 180, ScrMaxY - 16, 30, 16, "<<", TRUE);
		}
		if (n < NumFTexture) {
			DrawScreenButton(ScrMaxX - 90, ScrMaxY - 16, 40, 16, ">", TRUE);
			DrawScreenButton(ScrMaxX - 40, ScrMaxY - 16, 30, 16, ">>", TRUE);
		}
		else {
			DrawScreenButtonGray(ScrMaxX - 90, ScrMaxY - 16, 40, 16, ">", TRUE);
			DrawScreenButtonGray(ScrMaxX - 40, ScrMaxY - 16, 30, 16, ">>", TRUE);
		}
		DrawScreenButton(ScrMaxX - 250, ScrMaxY - 16, 60, 16, "Cancel", TRUE);
		if (NotThisPage) {
			currentx = 0;
			currenty = 0;
			SetColor(YELLOW);
			x = (currentx * 72) + 8;
			y = (currenty * 80);				
			DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
			DrawScreenFrame(x - 2, y, x + 65, y + 67);
		}
		while (1 == 1) {
			if (bioskey(1)) {
				key = bioskey(0);
				if (key == PAGEUP && n < NumFTexture) {
					page++;
					ClearScreen();
					break;
				}
				if (key == PAGEDOWN && page >= 1) {
					page--;
					ClearScreen();
					break;
				}
				if (key == HOME) {
					page = 0;
					ClearScreen();
					break;
				}
				if (key == END) {
					page = NumFTexture / (maxnumx * maxnumy);
					ClearScreen();
					break;
				}
				if (key == 283)
					return -1;

				if (key == CURS_RIGHT || key == CURS_LEFT || key == CURS_DOWN || key == CURS_UP) { 
					x = (currentx * 72) + 8;
					y = (currenty * 80);				
					SetColor(BLACK);
						DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
						DrawScreenFrame(x - 2, y, x + 65, y + 67);

					if (key == CURS_RIGHT)
						currentx++;

					if (key == CURS_LEFT)
						currentx--;

					if (key == CURS_UP)
						currenty--;

					if (key == CURS_DOWN)
						currenty++;

					if (currentx >= maxnumx) {
						currentx = maxnumx - 1;
					}
					if (currentx < 0) {
						currentx = 0;
					}
					if (currenty >= maxnumy)
						currenty = maxnumy - 1;

					if (currenty < 0)
						currenty = 0;
				}

				val = (currentx + (currenty * maxnumx));
				val = val + (maxnumx * maxnumy * page);

				if (key == ENTER)
					return val;

				if (val >= NumFTexture) {
					currentx = 0;
					currenty = 0;
					val = 0; /*jff*/
				}

				x = (currentx * 72) + 8;
				y = (currenty * 80);				
				SetColor(YELLOW);
				DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
				DrawScreenFrame(x - 2, y, x + 65, y + 67);
				SetColor(BLACK);
				DrawScreenBox(0, ScrMaxY - 12, 100, ScrMaxY);
				SetColor(WHITE);
				DrawScreenText(0, ScrMaxY - 12, "%s", FTexture[val]);

			}
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			val = (((PointerX - 8) / 72) + ((PointerY / 80) * maxnumx));
			val = val + (maxnumx * maxnumy * page);
			if (oldval != val) {
				if (val <= NumFTexture) {
					SetColor(BLACK);
					DrawScreenBox(0, ScrMaxY - 12, 100, ScrMaxY);
					SetColor(WHITE);
					DrawScreenText(0, ScrMaxY - 12, "%s", FTexture[val]);
				}
				else {
					SetColor(BLACK);
					DrawScreenBox(0, ScrMaxY - 12, 100, ScrMaxY);
				}
				oldval = val;
			}
			if (buttons == 1) {
				if (MouseInArea(ScrMaxX - 250, ScrMaxY - 16, 60, 16)) {
					DrawScreenButtonIn(ScrMaxX - 250, ScrMaxY - 16, 60, 16, "Cancel", TRUE);
					return -1;
				}
				else if (MouseInArea(ScrMaxX - 140, ScrMaxY - 16, 40, 16) && page >= 1) {
					DrawScreenButtonIn(ScrMaxX - 140, ScrMaxY - 16, 40, 16, "<", TRUE);
					page--;
					ClearScreen();
					break;
				}
				else if (MouseInArea(ScrMaxX - 90, ScrMaxY - 16, 40, 16) && n < NumFTexture) {
					DrawScreenButtonIn(ScrMaxX - 90, ScrMaxY - 16, 40, 16, ">", TRUE);
					page++;
					ClearScreen();
					break;
				}
				else if (MouseInArea(ScrMaxX - 180, ScrMaxY - 16, 30, 16) && page >= 1) {
					DrawScreenButtonIn(ScrMaxX - 180, ScrMaxY - 16, 30, 16, "<<", TRUE);
					page = 0;
					ClearScreen();
					break;
				}
				else if (MouseInArea(ScrMaxX - 40, ScrMaxY - 16, 30, 16) && page != NumFTexture / (maxnumx * maxnumy)) {
					DrawScreenButtonIn(ScrMaxX - 40, ScrMaxY - 16, 30, 16, ">>", TRUE);
					page = NumFTexture / (maxnumx * maxnumy);
					ClearScreen();
					break;
				}
				else if (PointerY < ScrMaxY - 20) {
					if (val >= NumFTexture)
						val = -1;
					return val;
				}
			}		
		}
	}
}

void WriteLog(char * logstring)
{
	logst = fopen("dethlog.txt","a");
	if (logst)
	{
		fprintf(logst,"%s\n",logstring);
		fclose(logst);
	}
}

BCINT BrowseWalls(char *CurTexture)
{
	int maxnumx, maxnumy, val, n, key;
	int nx, ny, x, y, page;
	int oldval = 0;
	char foo[9];
	Texture *tinfo;
	BCINT buttons, xsize, ysize;
	int currentx = 0;
	int currenty = 0;
	Bool NotThisPage = FALSE;

	if (VideoMode == 3) {
		maxnumx = 11;
		maxnumy = 7;
	}
	else if (VideoMode == 4) {
		maxnumx = 14;
		maxnumy = 9;
	}
	else if (VideoMode == 5) {
		maxnumx = 17;
		maxnumy = 12;
	}
	else {
		maxnumx = 8;
		maxnumy = 5;
	}

	page = 0;
	strncpy(foo, CurTexture, 8);
	for (n = 0; n < NumWTexture; n++) 
		if (strncmp(WTexture[n], foo, 8) == 0 ) {
			page = (n / (maxnumx * maxnumy));
			break;
		}
	ClearScreen();
	while (1 == 1) {
		NotThisPage = TRUE;
		SetColor(WHITE);
		for (ny = 0; ny < maxnumy; ny++) {
			for (nx = 0; nx < maxnumx; nx++) {
				x = (nx * (64 + 8)) + 8;
				y = (ny * (64 + 14));				
				n = (nx + (ny * maxnumx)) + (maxnumx * maxnumy * page);
				if (n < NumWTexture)
					DrawScreenText(x + 0, y + 64 + 6, "%s", WTexture[n]);
			}
		}
		if (!GotImages[page + 1] || Caching == FALSE) { /*jff GotImages is int */
			HideMousePointer();
			settextjustify(1,2);
			SetColor(YELLOW);
			DrawScreenText(ScrMaxX / 2, ScrMaxY - 12, "Caching textures - please wait");
			settextjustify(0,2);
			for (ny = 0; ny < maxnumy; ny++) {
				y = (ny * (64 + 14));				
				for (nx = 0; nx < maxnumx; nx++) {
					x = (nx * (64 + 8)) + 8;
					n = (nx + (ny * maxnumx)) + (maxnumx * maxnumy * page);
					if (n < NumWTexture)
						DisplayWallQuick(x, y + 2, x + 127, y + 130, WTexture[n], 2);
				}
			}
			if (Caching) {
				WallImages[page + 1] = (unsigned char*)malloc((((maxnumx * 72) + 8) * (maxnumy * 78)) + sizeof(GrContext) +4/*jff*/);
				getimage(0, 0, ((maxnumx * 72) + 8) - 1, (maxnumy * 78) - 1, WallImages[page + 1]);
				GotImages[page + 1] = 1; /*jff GotImages is int */
			}
			SetColor(BLACK);
			DrawScreenBox(0, ScrMaxY - 12, ScrMaxX, ScrMaxY);
			ShowMousePointer();
		}
		else 
			putimage(0, 0, WallImages[page + 1], COPY_PUT);
		for (ny = 0; ny < maxnumy; ny++) {
			for (nx = 0; nx < maxnumx; nx++) {
				x = (nx * (64 + 8)) + 8;
				y = (ny * (64 + 14));				
				n = (nx + (ny * maxnumx)) + (maxnumx * maxnumy * page);
				if (n<NumWTexture /*JFF don't index past end! */ && strncmp(WTexture[n], foo, 8) == 0 ) {
					SetColor(YELLOW);
					DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
					DrawScreenFrame(x - 2, y, x + 65, y + 67);
					SetColor(RED);
					DrawScreenBox(x - 2, y + 68, x + 65, y + 78);
					SetColor(WHITE);
					DrawScreenText(x + 0, y + 64 + 6, "%s", WTexture[n]);
					currentx = nx;
					currenty = ny;
					NotThisPage = FALSE;
					break;
				}
			}
		}
		if (page > 0) {
			DrawScreenButton(ScrMaxX - 140, ScrMaxY - 16, 40, 16, "<", TRUE);
			DrawScreenButton(ScrMaxX - 180, ScrMaxY - 16, 30, 16, "<<", TRUE);
		}
		else {
			DrawScreenButtonGray(ScrMaxX - 140, ScrMaxY - 16, 40, 16, "<", TRUE);
			DrawScreenButtonGray(ScrMaxX - 180, ScrMaxY - 16, 30, 16, "<<", TRUE);
		}
		if (n < NumWTexture) {
			DrawScreenButton(ScrMaxX - 90, ScrMaxY - 16, 40, 16, ">", TRUE);
			DrawScreenButton(ScrMaxX - 40, ScrMaxY - 16, 30, 16, ">>", TRUE);
		}
		else {
			DrawScreenButtonGray(ScrMaxX - 90, ScrMaxY - 16, 40, 16, ">", TRUE);
			DrawScreenButtonGray(ScrMaxX - 40, ScrMaxY - 16, 30, 16, ">>", TRUE);
		}
		DrawScreenButton(ScrMaxX - 250, ScrMaxY - 16, 60, 16, "Cancel", TRUE);
		if (NotThisPage) {
			currentx = 0;
			currenty = 0;
			SetColor(YELLOW);
			x = (currentx * (64 + 8)) + 8;
			y = (currenty * (64 + 14));				
			DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
			DrawScreenFrame(x - 2, y, x + 65, y + 67);
		}
		while (1 == 1) {
			if (bioskey(1)) {
				key = bioskey(0);
				if (key == PAGEUP && n < NumWTexture) {
					page++;
					ClearScreen();
					break;
				}
				if (key == PAGEDOWN && page >= 1) {
					page--;
					ClearScreen();
					break;
				}
				if (key == HOME) {
					page = 0;
					ClearScreen();
					break;
				}
				if (key == END) {
					page = NumWTexture / (maxnumx * maxnumy);
					ClearScreen();
					break;
				}
				if (key == 283)
					return -1;

				if (key == CURS_RIGHT || key == CURS_LEFT || key == CURS_DOWN || key == CURS_UP) { 
					x = (currentx * (64 + 8)) + 8;
					y = (currenty * (64 + 14));				
					SetColor(BLACK);
						DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
						DrawScreenFrame(x - 2, y, x + 65, y + 67);

					if (key == CURS_RIGHT)
						currentx++;

					if (key == CURS_LEFT)
						currentx--;

					if (key == CURS_UP)
						currenty--;

					if (key == CURS_DOWN)
						currenty++;

					if (currentx >= maxnumx) {
						currentx = maxnumx - 1;
					}
					if (currentx < 0) {
						currentx = 0;
					}
					if (currenty >= maxnumy)
						currenty = maxnumy - 1;

					if (currenty < 0)
						currenty = 0;
				}

				val = (currentx + (currenty * maxnumx));
				val = val + (maxnumx * maxnumy * page);

				if (key == ENTER)
					return val;

				if (val >= NumWTexture) {
					currentx = 0;
					currenty = 0;
					val = 0;		/*jff*/
				}

				x = (currentx * (64 + 8)) + 8;
				y = (currenty * (64 + 14));				
				SetColor(YELLOW);
				DrawScreenFrame(x - 1, y + 1, x + 64, y + 66);
				DrawScreenFrame(x - 2, y, x + 65, y + 67);
				SetColor(BLACK);
				DrawScreenBox(0, ScrMaxY - 12, 100, ScrMaxY);
				SetColor(WHITE);
				DrawScreenText(0, ScrMaxY - 12, "%s", WTexture[val]);
			}
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			val = (((PointerX - 8) / (64 + 8)) + ((PointerY / (64 + 14)) * maxnumx));
			val = val + (maxnumx * maxnumy * page);
			if (oldval != val) {
				SetColor(BLACK);
				DrawScreenBox(0, ScrMaxY - 12, 260, ScrMaxY);	/* jff val< not val<= */
				if (val < NumWTexture && (tinfo = FindTexture(WTexture[val]))) {
					xsize = tinfo->width;
					ysize = tinfo->height;
					SetColor(WHITE);
					DrawScreenText(0, ScrMaxY - 12, "%s", WTexture[val]);
					DrawScreenText(86, ScrMaxY - 12, "%dx%d", xsize, ysize);
					if (tinfo->nPatches == 1)
						DrawScreenText(165, ScrMaxY - 12, "Transparent");
					else
						DrawScreenText(165, ScrMaxY - 12, "%d Patches", tinfo->nPatches);
				}
				oldval = val;
			}
			if (buttons == 1) {
				if (MouseInArea(ScrMaxX - 250, ScrMaxY - 16, 60, 16)) 
					return -1;
				else if (MouseInArea(ScrMaxX - 140, ScrMaxY - 16, 40, 16) && page >= 1) {
					page--;
					delay(256);
					ClearScreen();
					break;
				}
				else if (MouseInArea(ScrMaxX - 90, ScrMaxY - 16, 40, 16) && n < NumWTexture) {
					page++;
					delay(256);
					ClearScreen();
					break;
				}
				else if (MouseInArea(ScrMaxX - 180, ScrMaxY - 16, 30, 16) && page >= 1) {
					page = 0;
					delay(256);
					ClearScreen();
					break;
				}
				else if (MouseInArea(ScrMaxX - 40, ScrMaxY - 16, 30, 16) && page != NumWTexture / (maxnumx * maxnumy)) {
					page = NumWTexture / (maxnumx * maxnumy);
					delay(256);
					ClearScreen();
					break;
				}
				else if (PointerY < ScrMaxY - 20) {
					if (val >= NumWTexture)
						val = -1;
					return val;
				}
			}		
		}
	}
}

/*
   draws a line of text in a menu
   */

void DisplayMenuText( BCINT x0, BCINT y0, BCINT line, char *text, BCINT highlightnum, Bool shownumbers)
{
    char h[ 2];
    
    if (UseMouse)
		HideMousePointer(); 
    if (shownumbers) {
		DrawScreenText( x0 + 26, y0 + 8 + line * 10, "- %s", text);
		SetColor( WHITE);
		if (line < 9)
			DrawScreenText( x0 + 10, y0 + 8 + line * 10, "%d", line + 1, text);
		else
			DrawScreenText( x0 + 10, y0 + 8 + line * 10, "%c", line + 56, text);
    }
    else {
		if (highlightnum > 0) {
			DrawScreenText( x0 + 10, y0 + 8 + line * 10, text);
			SetColor( WHITE);
			h[ 0] = text[ highlightnum - 1];
			h[ 1] = '\0';
			DrawScreenText( x0 + 2 + highlightnum * 8, y0 + 8 + line * 10, h);
		}
		else {
			SetColor(BLACK);
			DrawScreenText( x0 + 10, y0 + 8 + line * 10, text);
		}
    }
    if (UseMouse)
		ShowMousePointer();
}

/*
   display and execute a menu
   */

BCINT DisplayMenuArray( BCINT x0, BCINT y0, char *menutitle, BCINT numitems, BCINT *okkeys, char *menustr[MENUARRAYMAX], BCINT highlight[MENUARRAYMAX])
{
  BCINT maxlen, line, oldline;
  Bool  ok;
  BCINT key, buttons, oldbuttons;
  static char temp[ 10];
  static char temp2[ 10];
  BCINT x1,y1,width,height;
  Bool menubars = FALSE;
  
  /* compute maxlen */
  if (menutitle)
		maxlen = (strlen(menutitle) * .85) - 2;
  else
		maxlen = 1;
  for (line = 0; line < numitems; line++)
		if (strlen( menustr[ line]) > maxlen)
			maxlen = strlen( menustr[ line]);
    
  /* display the menu */
	HideMousePointer(); 
  if (x0 < 0)
		x0 = (ScrMaxX - maxlen * 8 - (menutitle ? 30 : 0) - (okkeys ? 19 : 19)) / 2;
  if (y0 < 0)
		y0 = (ScrMaxY - numitems * 10 - (menutitle ? 28 : 12)) / 2;
  if (x0 > ScrMaxX - maxlen * 8 - (menutitle ? 30 : 0) - (okkeys ? 19 : 19))
		x0 = ScrMaxX - maxlen * 8 - (menutitle ? 30 : 0) - (okkeys ? 19 : 19);

  DrawScreenBox3D( x0, y0, (menutitle ? 30 : 0) + x0 + maxlen * 8 + (okkeys ? 19 : 19), y0 + numitems * 10 + (menutitle ? 32 : 13));
  DrawScreenButtonIn( x0 + 3, y0 + (menutitle ? 21 : 3) , (menutitle ? 30 : 0) + maxlen * 8 + (okkeys ? 19 : 19) - 6, numitems * 10 + (menutitle ? 8 : 7), "", FALSE);
  if (menutitle)
	{
		SetColor(CYAN);
		DrawScreenBox( x0 + 3, y0 + 3, 30 + x0 + maxlen * 8 + (okkeys ? 19 : 19) - 3, y0 + 17);
		SetColor(BLACK);
		DrawScreenTextFonted(2, 0, x0 + 10, y0 + 4, menutitle);
	}
	ShowMousePointer();
  for (line = 0; line < numitems; line++)
	{
		SetColor( BLACK);
		if (strncmp(menustr[line], "-", 1) == 0)
		{
			menubars = TRUE;
			DrawScreenBar( x0 + 7, y0 + (menutitle ? 29 : 10) + (line * 10), (menutitle ? 30 : 0) + (maxlen * 8) + 6, TRUE);
		}
		else
			DisplayMenuText( x0, y0 + (menutitle ? 19 : 0), line, menustr[ line], 0, menutitle ? TRUE : FALSE);
  }
  oldline = -1;
  line = (MenuDefault>=0 && MenuDefault<numitems)? MenuDefault : 0;
  oldbuttons = 0x0000;
  ok = FALSE;
    
  while (! ok)
	{
		if (UseMouse)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			/* right button = cancel */
			if ((buttons & 0x0002) == 0x0002)
				return 0;
			/* left button = select */
			else if (buttons == 0x0001 && PointerX >= x0 && PointerX <= x0 + maxlen * 8 + 19)
					line = (PointerY - y0 - (menutitle ? 27 : 8)) / 10;
			/* release left button = accept selection */
			else if (buttons == 0x0000 && oldbuttons == 0x0001 && PointerY >= 16)
				ok = TRUE;
			oldbuttons = buttons;
			if (line<0 || line >= numitems) line = MenuDefault;
		}
		if (bioskey( 1))
		{
			key = bioskey( 0);
			
			/* enter key = accept selection */
			if ((key & 0x00FF) == 0x000D)
				ok = TRUE;
			/* escape key = cancel */
			else if ((key & 0x00FF) == 0x001B)
				return 0;
			else if (key == 19712)
				return -1; 
			else if (key == 19200)
				return -2; 
			/* up arrow = select previous line */
			else if ((key & 0xFF00) == 0x4800)
			{
				if (line > 0)
				{
					if (strncmp(menustr[line - 1], "-", 1) == 0)
						line--;
					line--;
				}
				else
					line = numitems - 1;
			}
			/* down arrow = select next line */
			else if ((key & 0xFF00) == 0x5000)
			{
				if (line < numitems - 1)
				{
					if (strncmp(menustr[line + 1], "-", 1) == 0)
						line++;
					line++;
				}
				else
					line = 0;
			}
			/* home = select first line */
			else if ((key & 0xFF00) == 0x4700)
				line = 0;
			/* end = select last line */
			else if ((key & 0xFF00) == 0x4F00)
				line = numitems - 1;
			/* pgup = select line - 5 */
			else if ((key & 0xFF00) == 0x4900)
			{
				if (line >= 5)
					line -= 5;
				else
					line = 0;
			}
			/* pgdn = select line + 5 */
			else if ((key & 0xFF00) == 0x5100)
			{
				if (line < numitems - 5)
					line += 5;
				else
					line = numitems - 1;
			}
			/* number or alphabetic key = enter selection */
			else if ((key & 0x00FF) >= '1' && (key & 0x00FF) <= '9' && ((key & 0x00FF) - '1') < numitems)
			{
				line = (key & 0x00FF) - '1';
				ok = TRUE;
			}
			else if ((key & 0x00FF) >= 'A' && (key & 0x00FF) <= 'Z' && ((key & 0x00FF) - 'A' + 9) < numitems)
			{
				line = (key & 0x00FF) - 'A' + 9;
				ok = TRUE;
			}
			else if ((key & 0x00FF) >= 'a' && (key & 0x00FF) <= 'z' && ((key & 0x00FF) - 'a' + 9) < numitems)
			{
				line = (key & 0x00FF) - 'a' + 9;
				ok = TRUE;
			}
			/* check the list of "hot keys" */
			else if (okkeys)
			{
				for (line = 0; line < numitems; line++)
					if (toupper( key & 0x00FF ) == okkeys[ line])
						break;
				if (line < numitems)
					ok = TRUE;
				else
				{
					line = oldline;
					Beep();
				}
			}
			/* other key */
			else
				Beep();
		}
		if (line != oldline)
		{
			if (oldline >= 0 && oldline < numitems)
			{
				SetColor(DARKGRAY);
				DrawScreenBox (x0 + 6, y0 + (menutitle ? 19 : 0) - 2 + 8 + oldline * 10, (menutitle ? 30 : 0) + x0 + maxlen * 8 + (okkeys ? 19 : 19) - 6, y0 + (menutitle ? 19 : 0) + 17 + oldline * 10);
				SetColor( BLACK);
				if (strncmp(menustr[oldline], "-", 1) == 0)
					DrawScreenBar( x0 + 7, y0 + (menutitle ? 29 : 10) + (oldline * 10), (menutitle ? 30 : 0) + (maxlen * 8) + 6, TRUE);
				else
					DisplayMenuText( x0, y0 + (menutitle ? 19 : 0), oldline, menustr[ oldline], 0, menutitle ? TRUE : FALSE);
			}
			if (line >= 0 && line < numitems)
			{
				SetColor(BLUE);
				DrawScreenButtonOut(x0 + 6, y0 + (menutitle ? 19 : 0) - 2 + 8 + line * 10, (menutitle ? 30 : 0) + maxlen * 8 + (okkeys ? 19 : 19) - 12, 10, TRUE);
				SetColor(WHITE);
				if (strncmp(menustr[line], "-", 1) == 0)
					DrawScreenBar( x0 + 7, y0 + (menutitle ? 29 : 10) + (line * 10), (menutitle ? 30 : 0) + (maxlen * 8) + 6, TRUE);
				else
					DisplayMenuText( x0, y0 + (menutitle ? 19 : 0), line, menustr[ line], 1, menutitle ? TRUE : FALSE);

				if (ShowSprite)
				{
					if (strcmp( menutitle, "Select Thing Type:") >= -1)
					{
						temp[ 0] = '\0';
						if (UseMouse)
							HideMousePointer();
						if (InfoShown)
						{
							DrawScreenBox3D( ScrMaxX - 120, ScrMaxY - 133, ScrMaxX, ScrMaxY - 13);
							strncpy( temp, menustr[ line], 6);
							strncpy( temp2, menustr[ line], 8);
							GetPicSize(temp, &width, &height);
							x1 = ((120 - width) / 2);
							y1 = ((120 - height) / 2);
							if (x1 < 0)
								x1 = 4;
							if (y1 < 0)
								y1 = 4;
							DisplayPic( ScrMaxX - 120 + x1, ScrMaxY - 133 + y1 , ScrMaxX - 4, ScrMaxY - 17, temp, 0); 
							DisplayPic( ScrMaxX - 120 + x1, ScrMaxY - 133 + y1 , ScrMaxX - 4, ScrMaxY - 17, temp2, 0); 
						}
						else
						{
							DrawScreenBox3D( ScrMaxX - 120, ScrMaxY - 120, ScrMaxX, ScrMaxY);
							strncpy( temp, menustr[ line], 6);
							strncpy( temp2, menustr[ line], 8);
							GetPicSize(temp, &width, &height);
							x1 = ((120 - width) / 2);
							y1 = ((120 - height) / 2);
							if (x1 < 0)
								x1 = 0;
							if (y1 < 0)
								y1 = 0;
							DisplayPic( ScrMaxX - 120 + x1, ScrMaxY - 120 + y1 , ScrMaxX - 4, ScrMaxY - 4, temp, 0); 
							DisplayPic( ScrMaxX - 120 + x1, ScrMaxY - 120 + y1 , ScrMaxX - 4, ScrMaxY - 4, temp2, 0); 
						}
						if (UseMouse)
							ShowMousePointer();
						if (Debug)
						{
							SetColor(BLACK);
							DrawScreenBox(ScrMaxX - 80, ScrMaxY - 150, ScrMaxX, ScrMaxY -140);
							SetColor(ORANGE);
							DrawScreenText(ScrMaxX - 80, ScrMaxY - 150,"%d,%d", width, height);
						}
					}
				}
			}
			if (line<numitems)
				oldline = line;
		}
  }
  if (menubars)
	if (line < numitems && strncmp(menustr[line], "-", 1) == 0)
		return 0; 

  if (line >= 0 && line < numitems)
		return (line + 1);
  else
		return 0;
}




/*
   display and execute a menu defined with variable arguments
   */

BCINT DisplayMenu( BCINT x0, BCINT y0, char *menutitle, ...)
{
    va_list args;
    BCINT num, val;
    char *menustr[MENUARRAYMAX];
    BCINT dummy[MENUARRAYMAX];
    
    /* put the va_args in the menustr table */
    num = 0;
    va_start( args, menutitle);
    while ((num < 30) && ((menustr[ num] = va_arg( args, char *)) != NULL))
		num++;
    va_end( args);
    
    /* display the menu */
    val = DisplayMenuArray( x0, y0, menutitle, num, NULL, menustr, dummy);
    if (val == -1 || val == -2)
    	return val;
    if (val >= 1)
		return val;
    else
		return 0; 
}



/* display and execute a dropdown menu (returns a key code) */

BCINT PullDownMenu( BCINT x0, BCINT y0, ...)
{
    va_list args;
    BCINT num;
    char *menustr[MENUARRAYMAX];
    BCINT retnkeys[MENUARRAYMAX];
    BCINT permkeys[MENUARRAYMAX];
    BCINT highlight[MENUARRAYMAX];
    
    /* put the va_args in the menustr table and the two strings */
    num = 0;
    va_start( args, y0);
    while ((num < 30) && ((menustr[ num] = va_arg( args, char *)) != NULL))
		{
			if (!(retnkeys[ num] = va_arg( args, BCINT)))
				ProgError( "BUG: PullDownMenu() called with invalid arguments");

			if (!(permkeys[ num] = va_arg( args, BCINT)))
				ProgError( "BUG: PullDownMenu() called with invalid arguments");

			if (!(highlight[ num] = va_arg( args, BCINT)))
				ProgError( "BUG: PullDownMenu() called with invalid arguments");
			num++;
    }
    va_end( args);
    
    /* display the menu */
    num = DisplayMenuArray( x0, y0, NULL, num, permkeys, menustr, highlight);
    if (num == -1 || num == -2)
    	return num;
    if (num >= 1)
			return retnkeys[num - 1]; /* return a valid key */
    else
			return 0; /* return an invalid key */
}



/* display the integer input box */

BCINT InputInteger( BCINT x0, BCINT y0, BCINT *valp, BCINT minv, BCINT maxv)
{
    BCINT  key, val;
    Bool neg, ok, firstkey;
   	if (x0 < 0)
   		x0 = (ScrMaxX - 74) / 2;
   	if (y0 < 0)
   		y0 = (ScrMaxY) / 2;
    DrawScreenBoxHollow( x0, y0, x0 + 61, y0 + 13);
    neg = (*valp < 0);
    val = neg ? -(*valp) : *valp;
    firstkey = TRUE;
    for (;;) {
		ok = (neg ? -val : val) >= minv && (neg ? -val : val) <= maxv;
		SetColor( BLACK);
		DrawScreenBox( x0 + 1, y0 + 1, x0 + 60, y0 + 12);
		if (ok)
			SetColor(WHITE);
		else                                          
			SetColor(DARKGRAY);
		if (neg)
			DrawScreenText( x0 + 3, y0 + 3, "-%d", val);
		else
			DrawScreenText( x0 + 3, y0 + 3, "%d", val);
		key = bioskey( 0);
		if (firstkey && (key & 0x00FF) > ' ') {
			val = 0;
			neg = FALSE;
		}
		firstkey = FALSE;
		if (val < 3277 && (key & 0x00FF) >= '0' && (key & 0x00FF) <= '9')
			val = val * 10 + (key & 0x00FF) - '0';
		else if (val > 0 && (key & 0x00FF) == 0x0008)
			val = val / 10;
		else if (neg && (key & 0x00FF) == 0x0008)
			neg = FALSE;
		else if ((key & 0x00FF) == '-')
			neg = !neg;
		else if (ok && (key & 0x00FF) == 0x000D)
			break; 
		else if ((key & 0xFF00) == 0x4800 || (key & 0xFF00) == 0x5000 ||
				 (key & 0xFF00) == 0x4B00 || (key & 0xFF00) == 0x4D00 ||
				 (key & 0x00FF) == 0x0009 || (key & 0xFF00) == 0x0F00)
			break; 
		else if ((key & 0x00FF) == 0x001B) {
			neg = FALSE;
			val = -16385; /*THIS WAS 0*/ 
			break;
		}
		else
			Beep();
    }
    if (neg)
		*valp = -val;
    else
		*valp = val;
    return key;
}



/* ask for an integer value and check for minimum and maximum */

BCINT InputIntegerValue( BCINT x0, BCINT y0, BCINT minv, BCINT maxv, BCINT defv)
{
    BCINT  val, key;
    char prompt[ 80];
    
    if (UseMouse)
		HideMousePointer();
    sprintf( prompt, "Enter A Decimal Value Between %d And %d:", minv, maxv);
 	if (x0 < 0)
   		x0 = (ScrMaxX - 25 - 8 * strlen( prompt)) / 2;
   	if (y0 < 0)
   		y0 = (ScrMaxY - 55) / 2;
    DrawScreenBox3D( x0, y0, x0 + 25 + 8 * strlen( prompt), y0 + 55);
    SetColor( WHITE);
    DrawScreenText( x0 + 10, y0 + 8, prompt);
    val = defv;
    while (((key = InputInteger( -1, -1, &val, minv, maxv)) & 0x00FF) != 0x000D && (key & 0x00FF) != 0x001B)
		Beep();
    if (UseMouse)
		ShowMousePointer();
    return val;
}

BCINT InputIntegerValuePlain( BCINT x0, BCINT y0, BCINT minv, BCINT maxv, BCINT defv)
{
    BCINT  val, key;
    
    if (UseMouse)
		HideMousePointer();
    val = defv;
    while (((((key = InputInteger( x0, y0, &val, minv, maxv)) & 0x00FF) != 0x000D && (key & 0x00FF) != 0x001B)  && (key & 0x00FF) != 0x0009) && (key & 0xFF00) != 0x0F00)
     
    /*if ((key & 0xFF00) == 0x4B00 || (key & 0xFF00) == 0x4D00 || (key & 0x00FF) == 0x0009 || (key & 0xFF00) == 0x0F00)*/
		Beep();
    if (UseMouse)
		ShowMousePointer();
    return val;
}

/* ask for a name in a given list and call a function (for displaying objects, etc.)
   
   Arguments:
   x0, y0  : where to draw the box.
   prompt  : text to be displayed.
   listsize: number of elements in the list.
   list    : list of names (picture names, level names, etc.).
   listdisp: number of lines that should be displayed.
   name    : what we are editing...
   width   : \ width and height of an optional window where a picture
   height  : / can be displayed (used to display textures, sprites, etc.).
   hookfunc: function that should be called to display a picture.
   (x1, y1, x2, y2 = coordinates of the window in which the
   picture must be drawn, name = name of the picture).
   */

void InputNameFromListWithFunc
(
	BCINT x0,
	BCINT y0,
	char *prompt,
	BCINT listsize,
	char **list,
	BCINT listdisp,
	char *name,
	BCINT width,
	BCINT height,
	void (*hookfunc)(BCINT px1, BCINT py1, BCINT px2, BCINT py2, char *name)
)
{
  BCINT  key, n, l = 0;
  BCINT  x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  BCINT  maxlen = 0;
  BCINT  EditMode = OBJ_LINEDEFS;
  Bool ok, firstkey;

  if (BrowseMap)
	{
		EditMode = 99;
		InitialScale = 9;
	}

  /* compute maxlen */
	if (!BrowseMap)
	{
		maxlen = 1;
		for (n = 0; n < listsize; n++)
			if (strlen( list[ n]) > maxlen)
				maxlen = strlen( list[ n]);
		for (n = strlen(name) + 1; n <= maxlen; n++)
			name[ n] = '\0';
		/* compute the minimum width of the dialog box */
		l = maxlen;
		if (strlen( prompt) > l + 12)
			l = strlen( prompt) - 12;
		l = l * 8 + 110;
		x1 = l + 3;
		y1 = 10 + 1;
		if (width > 0)
			l += 10 + width;
		if (height > 65)
			n = height + 20;
		else
			n = 85;
		if (x0 < 0)
			x0 = (ScrMaxX - l) / 2;
		if (y0 < 0)
			y0 = (ScrMaxY - n) / 2;
		x1 += x0;
		y1 += y0;
		if (x1 + width - 1 < ScrMaxX)
			x2 = x1 + width - 1;
		else
			x2 = ScrMaxX;
		if (y1 + height - 1 < ScrMaxY)
			y2 = y1 + height - 1;
		else
			y2 = ScrMaxY;
		/* draw the dialog box */
		DrawScreenBox3D( x0, y0, x0 + l, y0 + n);
		DrawScreenBoxHollow( x0 + 10, y0 + 28, x0 + 101, y0 + 41);
		DrawScreenText( x0 + 10, y0 + 8, prompt);
		if (width > 0)
		{
			DrawScreenBox( x1, y1, x2 + 1, y2 + 1);
			SetColor(DARKERGRAY);
			DrawScreenBox( x1 - 1, y1 - 1, x2, y2);
		}
  }
  firstkey = TRUE;
  for (;;)
	{
		/* test if "name" is in the list */
		for (n = 0; n < listsize; n++)
			if (strcmp( name, list[ n]) <= 0)
				break;
		ok = n < listsize ? ! strcmp( name, list[ n]) : FALSE;
		if (n > listsize - 1)
			n = listsize - 1;
		/* display the "listdisp" next items in the list */
		if (!BrowseMap)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( x0 + 110, y0 + 30, x0 + 110 + 8 * maxlen, y0 + 30 + 10 * listdisp);
			SetColor( BLACK);
			for (l = 0; l < listdisp && n + l < listsize; l++)
				DrawScreenText( x0 + 110, y0 + 30 + l * 10, list[ n + l]);
			l = strlen( name);
			DrawScreenBox( x0 + 11, y0 + 29, x0 + 100, y0 + 40);
			if (ok)
				SetColor(WHITE);
			else
				SetColor(DARKGRAY);
			DrawScreenText( x0 + 13, y0 + 31, name);
		}
		/* call the function to display the picture, if any */
		if (BrowseMap)
		{
			if(ok)
			{
				strcpy(LevelName, name);
				ReadLevelData();
				HideMousePointer();
				Scale = (float) (1.0 / InitialScale); 
				CenterMapAroundCoords( (MapMinX + MapMaxX) / 2, (MapMinY + MapMaxY) / 2);
				ClearMapScreen(0,13);
				if (EditMode == 99)
					DrawMapQuick(0); 
				else
					DrawMap(EditMode,0,0); 
				HideMousePointer();
				ForgetLevelData(); 
			} 
		}
		if (hookfunc)
		{
			/* clear the window */
			SetColor( BLACK);
			DrawScreenBox( x1, y1, x2, y2);
			/* display the picture "name" */
			hookfunc( x1, y1, x2, y2, name);
		}
		/* process user input */
		key = bioskey( 0);
		if (!BrowseMap)
		{
			if (firstkey && (key & 0x00FF) > ' ')
			{
				for (l = 0; l <= maxlen; l++)
					name[ l] = '\0';
				l = 0;
			}
			firstkey = FALSE;
		}
		if ((((key & 0x00FF) == '+' || (key & 0x00FF) == '=') && InitialScale > 1.0) && (BrowseMap))
		{ 
			InitialScale = InitialScale - 1.0;
			continue;
		}
		else if ((((key & 0x00FF) == '-' || (key & 0x00FF) == '_') && InitialScale < 16.0) && (BrowseMap))
		{ 
			InitialScale = InitialScale + 1.0;
			continue;
		}
		else if (((key & 0x00FF) == 's' || (key & 0x00FF) == 'S') && (BrowseMap))
		{ 
			EditMode = OBJ_SECTORS;
			continue;
		}
		else if (((key & 0x00FF) == 't' || (key & 0x00FF) == 'T') && (BrowseMap))
		{ 
			EditMode = OBJ_THINGS;
			continue;
		}
		else if (((key & 0x00FF) == 'v' || (key & 0x00FF) == 'V') && (BrowseMap))
		{ 
			EditMode = OBJ_VERTEXES;
			continue;
		}
		else if (((key & 0x00FF) == 'l' || (key & 0x00FF) == 'L') && (BrowseMap))
		{ 
			EditMode = OBJ_LINEDEFS;
			continue;
		}
		else if (((key & 0x00FF) == 'q' || (key & 0x00FF) == 'Q') && (BrowseMap))
		{ 
			EditMode = 99;
			continue;
		}
		else if (l < maxlen && (key & 0x00FF) >= 'a' && (key & 0x00FF) <= 'z')
		{
			name[ l] = (key & 0x00FF) + 'A' - 'a';
			name[ l + 1] = '\0';
		}
		else if (l < maxlen && (key & 0x00FF) > ' ')
		{
			name[ l] = key & 0x00FF;
			name[ l + 1] = '\0';
		}
		else if (l > 0 && (key & 0x00FF) == 0x0008)
			name[ l - 1] = '\0';
		else if (n < listsize - 1 && (key & 0xFF00) == 0x5000)
		{
			strcpy(name, list[ n + 1]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if (n > 0 && (key & 0xFF00) == 0x4800) {
			strcpy(name, list[ n - 1]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if (n < listsize - listdisp && (key & 0xFF00) == 0x5100) {
			strcpy(name, list[ n + listdisp]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if (n > 0 && (key & 0xFF00) == 0x4900)
		{
			if (n > listdisp)
				strcpy(name, list[ n - listdisp]);
			else
				strcpy(name, list[ 0]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if ((key & 0xFF00) == 0x4F00)
		{
			strcpy(name, list[ listsize - 1]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if ((key & 0xFF00) == 0x4700)
		{
			strcpy(name, list[ 0]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if ((key & 0x00FF) == 0x0009) {
			strcpy(name, list[ n]);
			if (BrowseMap)
				InitialScale = 9;
		}
		else if (/*jff allow non-known texture names: ok &&*/ (key & 0x00FF) == 0x000D)
		{
			if (BrowseMap)
			{
				PointerX = 0;
				PointerY = 0;
				SetMouseCoords(0,0);
			}
			break;						 /* return "name" */
		}
		else if ((key & 0x00FF) == 0x001B)
		{
			name[ 0] = '\0'; /* return an empty string */
			if (BrowseMap)
			{
				PointerX = 0;
				PointerY = 0;
				SetMouseCoords(0,0);
			}
			break;
		}
		else
			if (!BrowseMap) 
				Beep();
  }
}

/* ask for a name in a given list */

void InputNameFromList( BCINT x0, BCINT y0, char *prompt, BCINT listsize, char **list, char *name)
{
  HideMousePointer();
  InputNameFromListWithFunc( x0, y0, prompt, listsize, list, 5, name, 0, 0, NULL);
  if (UseMouse)
		ShowMousePointer();
}

void InputCommand( BCINT x0, BCINT y0, char *prompt, BCINT maxlen, char *command)
{
  BCINT   key, l, boxlen;
  Bool  ok, firstkey;
  
  /*if (UseMouse)
	HideMousePointer(); */
  for (l = strlen(command) + 1; l <= maxlen; l++)
	command[ l] = '\0';
  if (maxlen > 20)
	boxlen = 20;
  else
	boxlen = maxlen;
  boxlen = maxlen;
  /* compute the width of the dialog box */
  if (strlen( prompt) > boxlen)
	l = strlen( prompt);
  else
	l = boxlen;
  if (x0 < 0)
	x0 = (ScrMaxX - 26 - 8 * l) / 2;
  if (y0 < 0)
	y0 = (ScrMaxY - 50) / 2;
  /* draw the dialog box */
  DrawScreenBoxHollow( x0, y0, x0 + 5 + 8 * boxlen, y0 + 13);
  firstkey = TRUE;
  for (;;) {
	ok = TRUE;
	l = strlen( command);
	SetColor( BLACK);
	DrawScreenBox( x0 + 1, y0 + 1, x0 + 4 + 8 * boxlen, y0 + 12);
	if (ok)
		SetColor( WHITE);
	else
		SetColor(DARKGRAY);
	if (l > boxlen) {
		DrawScreenText( x0 + 1, y0 + 3, "<");
		DrawScreenText( x0 + 3, y0 + 3, "<%s", command + (l - boxlen + 1));
	}
	else
		DrawScreenText( x0 + 3, y0 + 3, command);
	key = bioskey( 0);
	if (firstkey && (key & 0x00FF) > ' ') {
		for (l = 0; l <= maxlen; l++)
			command[ l] = '\0';
		l = 0;
	}
	firstkey = FALSE;
	if (l < maxlen && (key & 0x00FF) >= 'a' && (key & 0x00FF) <= 'z') {
		command[ l] = (key & 0x00FF) + 'A' - 'a';
		command[ l + 1] = '\0';
	}
	else if (l < maxlen && (key & 0x00FF) >= ' ') {
		command[ l] = key & 0x00FF;
		command[ l + 1] = '\0';
	}
	else if (l > 0 && (key & 0x00FF) == 0x0008)
		command[ l - 1] = '\0';
	else if (ok && (key & 0x00FF) == 0x000D)
		break; /* return "filename" */
	else if ((key & 0x00FF) == 0x001B) {
		command[ 0] = '\0'; /* return an empty string */
		break;
	}
	else
		Beep();
  }
  if (UseMouse)
	ShowMousePointer();
}



/* ask for a filename */

void InputFileName( BCINT x0, BCINT y0, char *prompt, BCINT maxlen, char *filename)
{
    BCINT   key, l, boxlen;
    Bool  ok, firstkey;
    char *p;
    
    if (UseMouse)
		HideMousePointer();
    for (l = strlen(filename) + 1; l <= maxlen; l++)
		filename[ l] = '\0';
    /* compute the width of the input box */
    if (maxlen > 20)
		boxlen = 20;
    else
		boxlen = maxlen;
    /* compute the width of the dialog box */
    if (strlen( prompt) > boxlen)
		l = strlen( prompt);
    else
		l = boxlen;
    if (x0 < 0)
		x0 = (ScrMaxX - 26 - 8 * l) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 50) / 2;
    /* draw the dialog box */
    DrawScreenBox3D( x0, y0, x0 + 26 + 8 * l, y0 + 50);
    DrawScreenBoxHollow( x0 + 10, y0 + 28, x0 + 15 + 8 * boxlen, y0 + 41);
    DrawScreenText( x0 + 10, y0 + 8, prompt);
    firstkey = TRUE;
    for (;;) {
		/* check that "filename" looks like a valid file name */
		ok = TRUE;
		if (filename[ 1] == ':')
			p = filename + 2;
		else
			p = filename;
		for (l = 8; *p; p++) {
			if (*p == '.')
				l = 3;
			else if (*p == '\\')
				l = 8;
			else
				l--;
			if (l < 0) {
				ok = FALSE;
				break;
			}
		}
		l = strlen( filename);
		SetColor( BLACK);
		DrawScreenBox( x0 + 11, y0 + 29, x0 + 14 + 8 * boxlen, y0 + 40);
		if (ok)
			SetColor( WHITE);
		else
			SetColor(DARKGRAY);
		if (l > boxlen) {
			DrawScreenText( x0 + 11, y0 + 31, "<");
			DrawScreenText( x0 + 13, y0 + 31, "<%s", filename + (l - boxlen + 1));
		}
		else
			DrawScreenText( x0 + 13, y0 + 31, filename);
		key = bioskey( 0);
		if (firstkey && (key & 0x00FF) > ' ') {
			for (l = 0; l <= maxlen; l++)
				filename[ l] = '\0';
			l = 0;
		}
		firstkey = FALSE;
		if (l < maxlen && (key & 0x00FF) >= 'a' && (key & 0x00FF) <= 'z') {
			filename[ l] = (key & 0x00FF) + 'A' - 'a';
			filename[ l + 1] = '\0';
		}
		else if (l < maxlen && (key & 0x00FF) > ' ') {
			filename[ l] = key & 0x00FF;
			filename[ l + 1] = '\0';
		}
		else if (l > 0 && (key & 0x00FF) == 0x0008)
			filename[ l - 1] = '\0';
		else if (ok && (key & 0x00FF) == 0x000D)
			break; /* return "filename" */
		else if ((key & 0x00FF) == 0x001B) {
			filename[ 0] = '\0'; /* return an empty string */
			break;
		}
		else
			Beep();
    }
    if (UseMouse)
		ShowMousePointer();
}



/* ask for confirmation (prompt2 may be NULL) */

Bool Confirm( BCINT x0, BCINT y0, char *prompt1, char *prompt2)
{
  BCINT key;
  BCINT maxlen = 46;

  if (UseMouse)
		HideMousePointer();
  if (strlen( prompt1) > maxlen)
		maxlen = strlen( prompt1);
  if (prompt2 != NULL && strlen( prompt2) > maxlen)
		maxlen = strlen( prompt2);
  if (x0 < 0)
		x0 = (ScrMaxX - 22 - 8 * maxlen) / 2;
  if (y0 < 0)
		y0 = (ScrMaxY - (prompt2 ? 53 : 43)) / 2;
  DrawScreenBox3D( x0, y0, x0 + 22 + 8 * maxlen, y0 + (prompt2 ? 53 : 43));
  SetColor( WHITE);
  DrawScreenText( x0 + 10, y0 + 8, prompt1);
  if (prompt2 != NULL)
		DrawScreenText( x0 + 10, y0 + 18, prompt2);
  SetColor( YELLOW);
  DrawScreenText( x0 + 10, y0 + (prompt2 ? 38 : 28), "Press Y To Confirm, Or Any Other Key To Cancel");
  key = bioskey(0);
  if (UseMouse)
		ShowMousePointer();
  return ((key & 0x00FF) == 'Y' || (key & 0x00FF) == 'y');
}



/* display a notification and wait for a key (prompt2 may be NULL) */

void Notify( BCINT x0, BCINT y0, char *prompt1, char *prompt2)
{
  BCINT maxlen = 30;
  
  if (UseMouse)
		HideMousePointer();
  if (strlen( prompt1) > maxlen)
		maxlen = strlen( prompt1);
  if (prompt2 != NULL && strlen( prompt2) > maxlen)
		maxlen = strlen( prompt2);
  if (x0 < 0)
		x0 = (ScrMaxX - 22 - 8 * maxlen) / 2;
  if (y0 < 0)
		y0 = (ScrMaxY - (prompt2 ? 73 : 63)) / 2;
  DrawScreenBox3D(x0, y0, x0 + 22 + 8 * maxlen, y0 + (prompt2 ? 91 : 81));
  DrawScreenButtonIn(x0 + 3, y0 + 21, 22 + (8 * maxlen) - 6, (prompt2 ? 73 : 63) - 6, "", FALSE);
	SetColor(ORANGE);
  DrawScreenBox(x0 + 3, y0 + 3, x0 + 22 + (8 * maxlen) - 3, y0 + 17);
	SetColor(BLACK);
	DrawScreenTextFonted(2, 0, x0 + 10, y0 + 4, "Message:");
  SetColor(WHITE);
  DrawScreenText(x0 + 10, y0 + 30, prompt1);
  if (prompt2 != NULL)
		DrawScreenText(x0 + 10, y0 + 40, prompt2);
  SetColor(YELLOW);
  DrawScreenText(x0 + 10, y0 + (prompt2 ? 75 : 65), "Press Any Key To Continue...");
  bioskey(0);
  if (UseMouse)
		ShowMousePointer();
}



/* clear the screen and display a message */

void DisplayMessage( BCINT x0, BCINT y0, char *msg, ...)
{
    char prompt[ 120];
    va_list args;
    
    va_start( args, msg);
    vsprintf( prompt, msg, args);
    va_end( args);
    
    if (UseMouse)
		HideMousePointer();
    ClearScreen();
    if (x0 < 0)
		x0 = (ScrMaxX - 40 - 8 * strlen( prompt)) / 2;
    if (y0 < 0)
		y0 = (ScrMaxY - 40) / 2;
    DrawScreenBox3D( x0, y0, x0 + 40 + 8 * strlen( prompt), y0 + 40);
    DrawScreenText( x0 + 20, y0 + 17, prompt);
    if (UseMouse)
		ShowMousePointer();
}

/* let's make the user angry... */

void NotImplemented()
{
    if (UseMouse)
		HideMousePointer();
    DrawScreenBox3D( 140, 220, 500, 260);
    SetColor( RED);
    DrawScreenText( 150, 230, "This Function Is Not Implemented... Yet!");
    SetColor( YELLOW);
    DrawScreenText( 150, 245, "Press Any Key To Return To The Editor.");
    Beep();
    bioskey( 0);
    if (UseMouse)
		ShowMousePointer();
}

void XAlignmentMenu(SelPtr *list)
{
	char	*menustr[MENUARRAYMAX];
	BCINT	dummy[MENUARRAYMAX];
	BCINT	n, val;
	int x0, y0;
	
	x0 = (ScrMaxX - 29 - 8 * 44) / 2;
	y0 = (ScrMaxY - 24 - 10 * 5) / 2;
	for (n = 0; n < 5; n++)
		menustr[ n] = (char*)GetMemory( 80);
	strcpy( menustr[ 4], "X Texture Alignment");
	strcpy( menustr[ 0], "Align X With Texture Checking");
	strcpy( menustr[ 1], "Align X With Texture Checking And Initial Offset");
	strcpy( menustr[ 2], "Align X Without Texture Checking");
	strcpy( menustr[ 3], "Align X Without Texture Checking And Initial Offset");
	val = DisplayMenuArray( -1, -1, menustr[ 4], 4, NULL, menustr, dummy);
	for (n = 0; n < 5; n++)
		FreeMemory( menustr[ n]);
	
	if(val >= 1 && val <= 4) {
		Backup (BLINEDEFS | BSIDEDEFS);
		strcpy (BackupText, "Align Textures on X");
		val--;
		AlignTexturesX(list, 
					   (val & 2) ? TRUE : FALSE, 	
					   (val & 1) ? TRUE : FALSE);	
	}
}


void YAlignmentMenu(SelPtr *list)
{
	char	*menustr[MENUARRAYMAX];
	BCINT	dummy[MENUARRAYMAX];
	BCINT	n, val;
	int x0, y0;
	
	x0 = (ScrMaxX - 29 - 8 * 44) / 2;
	y0 = (ScrMaxY - 24 - 10 * 5) / 2;
	for (n = 0; n < 5; n++)
		menustr[ n] = (char*)GetMemory( 80);
	strcpy( menustr[ 4], "Y Texture Alignment");
	strcpy( menustr[ 0], "Align Y With Texture Checking");
	strcpy( menustr[ 1], "Align Y With Texture Checking And Initial Offset");
	strcpy( menustr[ 2], "Align Y Without Texture Checking");
	strcpy( menustr[ 3], "Align Y Without Texture Checking And Initial Offset");
	val = DisplayMenuArray( -1, -1, menustr[ 4], 4, NULL, menustr, dummy);
	for (n = 0; n < 5; n++)
		FreeMemory( menustr[ n]);
	
	if(val >= 1 && val <= 4) {
		Backup (BLINEDEFS | BSIDEDEFS);
		strcpy (BackupText, "Align Textures on Y");
		val--;
		AlignTexturesY(list, 
					   (val & 2) ? TRUE : FALSE, 	
					   (val & 1) ? TRUE : FALSE);	
	}
}

/* end of file */
