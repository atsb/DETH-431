/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   EDIT.C - Editor routines.
   */

/* the includes */
#include "deu.h"
#include "levels.h"
#include <time.h>
#include <dir.h>

void SortSelection(SelPtr *);

extern Bool InfoShown;		/* should we display the info bar? */
extern Bool BrowseMap;
Bool BMap;
BCINT MoveSpeed;

int last_backup = 0;
struct Thing *Thing_backup = NULL;
struct Linedef *Linedef_backup = NULL;
struct Sidedef *Sidedef_backup = NULL;
struct Vertex *Vertex_backup = NULL;
struct Sector *Sector_backup = NULL;
long nThing_backup = 0, nLinedef_backup = 0, nSidedef_backup = 0,
       nVertex_backup = 0, nSector_backup = 0;


/* use LevelNameFormat & the token list to make a
   valid level name */

void MakeLevelName(SList tokens)
{
	char *format, scratch[10];
	SList flist, tlist;
	int arg;
	
	LevelName[0] = 0;
	
	flist = LevelNameFormat;
	if (!flist)
		return;
	
	format = flist->string;
	
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			/* move pointer on to the width specifier */
			
			flist = flist->next;
			if (!flist)
				ProgError("cannot make level name");
			
			if (flist->string[0] != '%')
				ProgError("garbage in level name format");
			
			sscanf((flist->string) + 1, "%d", &arg);
			/* now we know which of the tokens from the list we want */
			
			tlist = tokens;
			while (arg > 1)
			{
				arg--;
				tlist = tlist->next;
				if(!tlist)
					ProgError("Please enter e [episode] [mission]");
			}
			
			arg = atoi(tlist->string);
			
			sprintf(scratch, "%0*d", (*format - '0'), arg); 
			format++;
			
			/* actually add it to LevelName */
			strcat(LevelName, scratch);
		}
		else
		{
			LevelName[strlen(LevelName) + 1] = 0;
			LevelName[strlen(LevelName)] = *format++;
		}
	}
}


/*
   the driving program
   */

void EditLevel( Bool newlevel)
{
	BMap = FALSE;

	MoveSpeed = DefaultLargeScroll;				/* movement speed */
  ReadWTextureNames();
  ReadFTextureNames();
  if (BOOMEnable) ReadCMapNames();

  InitGfx();
  CheckMouseDriver();
  if	(!(*LevelName))
	{
		BMap = TRUE;
		SelectLevel();
	}
  if(*LevelName)
	{
		ClearMapScreen(17,0);
		ReadLevelData();
		if (newlevel)
		{
			ForgetLevelData();
			MapMinX = -4000;
			MapMinY = -4000;
			MapMaxX = 4000;
			MapMaxY = 4000;
		}
		LogMessage( ": Editing %s...\n", LevelName);
		EditorLoop();
		LogMessage( ": Finished editing %s...\n", LevelName);
		TermGfx();
		if (!Registered) //jff make more understandable
			printf( "Please register DOOM if you want to be able to save your changes.\n");
		ForgetLevelData();
		/* forget the level pointer */
		Level = NULL;
		/* lose the level name */
		LevelName[0] = 0;
  }
  else
		TermGfx();

  ForgetWTextureNames();
  ForgetFTextureNames();
  if (BOOMEnable) ForgetCMapNames();
}



/*
   select a level
   */

void SelectLevel()
{

  MDirPtr dir;
  char *levels[100];
  char name[9];
  BCINT grid;
  int n = 0;
	
	name[8] = 0;
  for (dir = MasterDir; dir; dir = dir->next)
	{
   	strncpy(name, dir->dir.name, 8);
   	if(isalev(name))
   		levels[n++] = dir->dir.name;
  }
  strcpy(name, levels[0]);
	if (BMap == TRUE)
	{
		Scale = 9;
		BrowseMap = TRUE;
		BMap = FALSE;
	}
	InputNameFromList( -1, -1, "Select Level To Save As:", n, levels, name);
	grid = 0;
	BrowseMap = FALSE;
	strcpy(LevelName, name);
}



/*
   get the name of the new WAD file
   */


char *GetWadFileName()
{
	char *dotp;
	static char outfile[80];
	WadPtr wad;
	int x0 = -1, y0 = -1;

	/* get the file name */
	if (!strcmp(Level->wadfile->filename, MainWad))
  	sprintf(outfile, "%s.WAD", LevelName);
	else
		strcpy(outfile, Level->wadfile->filename);

	do
	{
		InputFileName( x0, y0, "Name Of The New WAD File:", 79, outfile);
		/* escape */
		if (outfile[ 0] == '\0')
			return NULL;
	}
	while (!strcmp(outfile, MainWad));

	/* if the WAD file already exists, rename it to "*.BAK" */
	for (wad = WadFileList; wad; wad = wad->next)
	if (!stricmp( outfile, wad->filename))
	{
		dotp = strrchr( wad->filename, '.');
		if (dotp == NULL)
			strcat( wad->filename, ".BAK");
		else
			strcpy( dotp, ".BAK");
	
		/* need to close, then reopen: problems with SHARE.EXE */
		fclose( wad->fileinfo);
	
		CopyFile(outfile, wad->filename);
	
		wad->fileinfo = fopen( wad->filename, "rb");
		if (wad->fileinfo == NULL)
			ProgError( "Could Not Reopen File \"%s\"", wad->filename);
		strcpy(wad->filename, outfile);
		break;
	}
	return outfile;
}

void FindThing(int mode, SelPtr *List)
{
	static int thing_no = 0, last = -1;
	static BCINT type = -1;
	int action = 0;
	Bool mark = FALSE;
	static thing_class *c = NULL;
	thing_type *t = NULL;
	
	if(List)
	{
		action = DisplayMenu(-1, -1, "Find/Mark/Filter",
							 "Find Things",
							 "-",
							 "Mark Things",
							 "Mark Things By Class",
							 "-",
							 "Filter Things"
							 ,NULL);

		if (action == 2)
			mark = TRUE;
		
		if (action == 3)
		{
			c = SelectThingClass();
			if (c)
				for (t = c->types; t; t = t->next)
					for (thing_no = 0; thing_no < NumThings; thing_no++)
						if (Things[thing_no].type == t->type)
							SelectObject(List, thing_no);
			return;
		}

		if (action == 4) {
		 	tff = SetThingFlagFilter(tff);
		 	MenuDefault = 0;
		 	return;
		}

 		if (action == 0)
			return;
	}
	
	if (mode == 0 || type == -1)
	{
		type = SelectThingType();
		thing_no = 0;
	}
	
	if (type == -1)
		return;
	
	if (mark)
	{
		for (thing_no = 0; thing_no < NumThings; thing_no++)
			if (Things[thing_no].type == type)
				SelectObject(List, thing_no);
		
		return;
	}

	thing_no = last + 1;
	if(last == -1)
		last = 0;
		
	do
	{
		if (thing_no == NumThings)
			thing_no = 0;
		else
		{
			if (Things[thing_no].type == type)
			{
				last = thing_no;
				GoToObject(OBJ_THINGS, thing_no);
				return;
			}
			thing_no++;
		}
	}
	while (thing_no != last);
}


void CopyFile(const char *from, const char *to)
{
	FILE *f = NULL, *t = NULL;
	char *buffer;
	long bytes, left;
	
	if  (!((buffer = malloc(65536)) &&
	    (f = fopen(from, "rb")) &&
	    (t = fopen(to, "wb"))))
		ProgError("Unable To Copy File");
	
	fseek(f, 0, SEEK_END);
	left = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	while(left > 0)
	{
		bytes = fread(buffer, 1, 65536, f);
		fwrite(buffer, 1, bytes, t);
		left -= bytes;
	}
	
	fclose(f);
	fclose(t);
	
	free(buffer);
} 

/* stuff for Undo-ing */
void Backup(int what)
{
	if (what & BTHINGS)
	{
		if (NumThings > nThing_backup) 
			Thing_backup = ResizeMemory(Thing_backup, NumThings * sizeof(struct Thing));
		bcopy(Things, Thing_backup, NumThings * sizeof(struct Thing));
		nThing_backup = NumThings; 
	}

	if (what & BLINEDEFS)
	{
		if (NumLineDefs > nLinedef_backup)
			Linedef_backup = ResizeMemory(Linedef_backup, NumLineDefs * sizeof(struct LineDef));
		bcopy(LineDefs, Linedef_backup, NumLineDefs * sizeof(struct LineDef));
		nLinedef_backup = NumLineDefs;
	}

	if (what & BSIDEDEFS)
	{
		if (NumSideDefs > nSidedef_backup)
  		Sidedef_backup = ResizeMemory(Sidedef_backup, NumSideDefs * sizeof(struct SideDef));
		bcopy(SideDefs, Sidedef_backup, NumSideDefs * sizeof(struct SideDef));
		nSidedef_backup = NumSideDefs;
	}

	if (what & BVERTEXES)
	{
		if(NumVertexes > nVertex_backup)
			Vertex_backup = ResizeMemory(Vertex_backup, NumVertexes * sizeof(struct Vertex));
		bcopy(Vertexes, Vertex_backup, NumVertexes * sizeof(struct Vertex));
		nVertex_backup = NumVertexes;
	}

	if (what & BSECTORS)
	{
		if (NumSectors > nSector_backup)
			Sector_backup = ResizeMemory(Sector_backup, NumSectors * sizeof(struct Sector));
		bcopy(Sectors, Sector_backup, NumSectors * sizeof(struct Sector));
		nSector_backup = NumSectors;
	}
	last_backup = what;
}

void Undo()
{
	if (last_backup & BTHINGS)
	{
		Things = ResizeMemory(Things, nThing_backup * sizeof(struct Thing));
		bcopy(Thing_backup, Things, nThing_backup * sizeof(struct Thing));
		NumThings = nThing_backup;
	}

	if (last_backup & BLINEDEFS)
	{
		LineDefs = ResizeMemory(LineDefs, nLinedef_backup * sizeof(struct LineDef));
		bcopy(Linedef_backup, LineDefs, nLinedef_backup * sizeof(struct LineDef));
		NumLineDefs = nLinedef_backup;
	}

	if (last_backup & BSIDEDEFS)
	{
		SideDefs = ResizeMemory(SideDefs, nSidedef_backup * sizeof(struct SideDef));
		bcopy(Sidedef_backup, SideDefs, nSidedef_backup * sizeof(struct SideDef));
		NumSideDefs = nSidedef_backup;
	}

	if (last_backup & BVERTEXES)
	{
		Vertexes = ResizeMemory(Vertexes, nVertex_backup * sizeof(struct Vertex));
		bcopy(Vertex_backup, Vertexes, nVertex_backup * sizeof(struct Vertex));
		NumVertexes = nVertex_backup;
	}

	if (last_backup & BSECTORS)
	{
		Sectors = ResizeMemory(Sectors, nSector_backup * sizeof(struct Sector));
		bcopy(Sector_backup, Sectors, nSector_backup * sizeof(struct Sector));
		NumSectors = nSector_backup;
	}

	last_backup = 0;
	strcpy (BackupText, "Nothing To Undo");
}

/* SO 23/7/95:
   Sort the Selection list.
   This makes sure that deletions of multiple selected objects
   work properly. */

/* First, a compare function for the sort routine to call */
int greaterp(const void *e1, const void *e2)
{
	return(*((BCINT *)e2) - *((BCINT *)e1));
}
   
void SortSelection(SelPtr *List)
{
  BCINT *array;
  int numelements = 0, i;
  SelPtr p;
  
  /* We are going to copy all the selection.objnums into
      an array, qsort it, and copy back into the list. */
  
  /* count the elements */
  for (p = *List; p; p = p->next)
    numelements++;
  
  if (numelements < 2)
    return;
  /* not enough to sort */
  
  array = (BCINT *)malloc(numelements * sizeof(BCINT));
  
  i = 0;
  for (p = *List; p; p = p->next)
    array[i++] = p->objnum;
  
  qsort(array, numelements, sizeof(BCINT), greaterp);

  i = 0;
  for (p = *List; p; p = p->next)
    p->objnum = array[i++];

  free(array); 
}


void SaveConfigFileOptions()
{
	time_t  tval;
	char   *timestr;
	FILE   *cfgfile;

	if (SaveIniFile == FALSE)
		return;

	if (Cwd[1] == ':') 
		setdisk((Cwd[0] | 32) - 'a');
	chdir(Cwd);

#ifdef GAME_HEXEN
	rename(CfgFile, "HETH.BAK");
#else
	rename(CfgFile, "DETH.BAK");
#endif

	cfgfile = fopen(CfgFile, "a");

	time(&tval);
	timestr = ctime(&tval);
	timestr[strlen(timestr) - 1] = '\0';

#ifdef GAME_HEXEN
	fprintf(cfgfile, "# HETH Initialization File.   [Updated on %s]\n", timestr);
#else
	fprintf(cfgfile, "# DETH Initialization File.   [Updated on %s]\n", timestr);
#endif



	fprintf(cfgfile, "\n# STARTUP OPTIONS");
	fprintf(cfgfile, "\n# ---------------");
	fprintf(cfgfile, "\n\n# Path to config files.");
#ifdef GAME_HEXEN
	fprintf(cfgfile, "\nHexen = %s", Hexen);
#else
	fprintf(cfgfile, "\nDoom = %s", Doom);
	fprintf(cfgfile, "\nDoom2 = %s", Doom2);
	fprintf(cfgfile, "\nHeretic = %s", Heretic);
	fprintf(cfgfile, "\nStrife = %s", Strife);
#endif
#ifdef GAME_DOOM
	fprintf(cfgfile,"\n\n# Ask which game to edit?");
	fprintf(cfgfile, "\nChooseGame = %s", (ChooseGame) ? "True" : "False");
#endif
	fprintf(cfgfile,"\n\n# Game To Edit."); 
	fprintf(cfgfile, "\nGame = %s", Game);

#ifdef GAME_DOOM
	fprintf(cfgfile,"\n\n# Paths to your patch wads.");
	fprintf(cfgfile, "\nDoomPwads = %s", DoomPwads);
	fprintf(cfgfile, "\nDoom2Pwads = %s", Doom2Pwads);
	fprintf(cfgfile, "\nHereticPwads = %s", HereticPwads);
	fprintf(cfgfile, "\nStrifePwads = %s", StrifePwads);
#else
	fprintf(cfgfile,"\n\n# Paths to your patch wads.");
	fprintf(cfgfile, "\nHexenPwads = %s", HexenPwads);
#endif


	fprintf(cfgfile,"\n\n# Name of any patch wads you want to load.");
	if (NumConfigPatchWads>0)
	{
		int i;
		fprintf(cfgfile, "\nPWads = %s",ConfigPatchWads[0]);
		for (i=1;i<NumConfigPatchWads && ConfigPatchWads[i];i++)
			fprintf(cfgfile, " %s", ConfigPatchWads[i]);
	}
	else
		fprintf(cfgfile,"\n#File = MAP01.WAD");

	fprintf(cfgfile,"\n\n# MISC. PREFERENCES");
	fprintf(cfgfile,"\n# -----------------");
	fprintf(cfgfile,"\n\n# Enable support for TNT's BOOM??"); //jff
	fprintf(cfgfile, "\nBoom = %s", (BOOMEnable) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Use a graphical style front end?");
	fprintf(cfgfile, "\nGraphFront = %s", (GraphFront) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Input style of graphical front end.");
	fprintf(cfgfile, "\nUsePromptOnly = %s", (UsePromptOnly) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Are you an expert?");
	fprintf(cfgfile, "\nExpert = %s", (Expert) ? "True" : "False");
	fprintf(cfgfile,"\n\n# How much noice do you like?");
	fprintf(cfgfile, "\nQuiet = %s", (Quiet) ? "True" : "False");
	fprintf(cfgfile, "\nQuieter = %s", (Quieter) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Does the 'Q' key Quit(True) or Save and Quit(False)?");
	fprintf(cfgfile, "\nQisQuit = %s", (QisQuit) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Use an additive style selection box?");
	fprintf(cfgfile, "\nAddSelBox = %s", (AdditiveSelBox) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Confirm mergeing of vertices in expert mode?");
	fprintf(cfgfile, "\nVertConf = %s", (VertConf) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Select Object 0 when you switch between modes?");
	fprintf(cfgfile, "\nSelect0 = %s", (Select0) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Copy SideDefs when copying LineDefs?");
	fprintf(cfgfile, "\nCopySideDefs = %s", (CopySideDefs) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Swap mouse buttons?");
	fprintf(cfgfile, "\nSwapButtons = %s", (SwapButtons) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Delay for automatic marquee to startup.");
	fprintf(cfgfile, "\nStretchDelay = %d", StretchDelay);
	fprintf(cfgfile,"\n\n# Default textures and prefab preferences.");
	fprintf(cfgfile, "\nWallTexture = %s", DefaultWallTexture);
	fprintf(cfgfile, "\nUpperTexture = %s", DefaultUpperTexture);
	fprintf(cfgfile, "\nLowerTexture = %s", DefaultLowerTexture);
	fprintf(cfgfile, "\nCeilTexture = %s", DefaultCeilingTexture);
	fprintf(cfgfile, "\nFloorTexture = %s", DefaultFloorTexture);
	fprintf(cfgfile, "\nDoorTrack = %s", DefaultDoorTrack);
	fprintf(cfgfile, "\nDoorTexture = %s", DefaultDoorTexture);
	fprintf(cfgfile, "\nUpperStep = %s", DefaultUpperStepTexture);
	fprintf(cfgfile, "\nLowerStep = %s", DefaultLowerStepTexture);
	fprintf(cfgfile, "\nTeleTexture = %s", DefaultTeleportTexture);
	fprintf(cfgfile, "\nCeilHeight = %d", DefaultCeilingHeight);
	fprintf(cfgfile, "\nFloorHeight = %d", DefaultFloorHeight);
	fprintf(cfgfile, "\nLighting = %d", DefaultLighting); //jff add default lighting
	fprintf(cfgfile,"\n\n# Value of sector increment when using the hotkeys.");
	fprintf(cfgfile, "\nIncrement = %d", Increment);
	fprintf(cfgfile,"\n\n# How close a vertex needs to be, before it splits a line.");
	fprintf(cfgfile, "\nCloseToLine = %d", CloseToLine);

	fprintf(cfgfile,"\n\n# DISPLAY OPTIONS");
	fprintf(cfgfile,"\n# ---------------");

	fprintf(cfgfile,"\n\n# Video mode.");
	fprintf(cfgfile,"\n# 2-640x480x256, 3-800x600x256, 4-1024x768x256, 5-1280x1024x256");
	fprintf(cfgfile, "\nVideo = %d", RealVideoMode);
	fprintf(cfgfile,"\n\n# Nest the pulldown menus?");
	fprintf(cfgfile, "\nNestMenus = %s", (NestMenus) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Gamma correction level (0 - 4).");
	fprintf(cfgfile, "\nGamma = %d", Gamma);
	fprintf(cfgfile,"\n\n# Show the info bar?");
	fprintf(cfgfile, "\nInfoBar = %s", (InfoShown) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Show alternative text on info bar?");
	fprintf(cfgfile, "\nAltInfo = %s", (AltInfo) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Show things with direction indicator?");
	fprintf(cfgfile, "\nThingAngle = %s", (ThingAngle) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Show thing sprites and wall textures?");
	fprintf(cfgfile, "\nDispSprite = %s", (DispSprite) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Large scroll factor.");
	fprintf(cfgfile, "\nLargeScroll = %d", DefaultLargeScroll);
	fprintf(cfgfile,"\n\n# Initial Zoom factor.");
	fprintf(cfgfile, "\nZoom = %d", InitialScale);
	fprintf(cfgfile,"\n\n# Use a fake mouse pointer?");
	fprintf(cfgfile, "\nFakeCursor = %s", (FakeCursor) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Minimum grid scale.");
	fprintf(cfgfile, "\nMinGrid = %d", MinGrid);
	fprintf(cfgfile,"\n\n# Maximum grid scale.");
	fprintf(cfgfile, "\nMaxGrid = %d", MaxGrid);
	fprintf(cfgfile,"\n\n# Current grid scale.");
	fprintf(cfgfile, "\nInitGrid = %d", GridScale);
	fprintf(cfgfile,"\n\n# Grid hidden(True) or shown(False)?");
	fprintf(cfgfile, "\nHideGrid = %s", (GridShown) ? "False" : "True");
	fprintf(cfgfile,"\n\n# Draw grid with dashed lines?");
	fprintf(cfgfile, "\nGridDashed = %s", (GridDashed) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Draw linedef mode in automap style?");
	fprintf(cfgfile, "\nAutoMap = %s", (AutoMap) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Draw thing mode with sprites?");
	fprintf(cfgfile, "\nShowThings = %s", (ShowThings) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Draw vertex mode with linedef dimensioning?");
	fprintf(cfgfile, "\nShowDistance = %s", (ShowDistance) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Current editing mode (1 - 4).");
	fprintf(cfgfile, "\nEdMode = %d", EdMode);

	fprintf(cfgfile,"\n\n# MISC. OPTIONS");
	fprintf(cfgfile,"\n# -------------");
#ifdef GAME_HEXEN
	fprintf(cfgfile,"\n\n# Keep the MAPINFO and SNDINFO lumps when saving?");
	fprintf(cfgfile, "\nKeepInfo = %s", (KeepInfo) ? "True" : "False");
#endif
	fprintf(cfgfile,"\n\n# Debugging mode on?");
	fprintf(cfgfile, "\nDebug = %s", (Debug) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Texture caching on?");
	fprintf(cfgfile, "\nCaching = %s", (Caching) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Automatically save settings?");
	fprintf(cfgfile, "\nSaveIniFile = %s", (SaveIniFile) ? "True" : "False");
	fprintf(cfgfile,"\n\n# Disable Facing Sector, Missing Texture, Ceil<Floor Checks?"); //jff
	fprintf(cfgfile, "\nSpecialEffects = %s", (SpecialEffects) ? "True" : "False");
	fprintf(cfgfile,"\n\n\n# EOF");

	fclose(cfgfile);

	if (Pwads[1] == ':')
		setdisk((Pwads[0] | 32) - 'a');
	chdir(Pwads); 
}
