/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   DEU.C - Main program execution routines.
   */


// the includes
#include "deu.h"
#include "keys.h"
#include <time.h>
#include <dos.h>
#include <signal.h>

#include <stdio.h>
#include <osfcn.h>
#include <dir.h>
#include <sys/stat.h>

// global variables
FILE *logst =                   NULL;   // filepointer to the debug log jff 
FILE  *logfile =								NULL;		// filepointer to the error log 
BCINT InitialScale = 						8;			// initial zoom factor for map 
BCINT VideoMode = 							2;			// default video mode for VESA/SuperVGA
BCINT RealVideoMode = 					3;			// default video mode for VESA/SuperVGA
BCINT DefaultFloorHeight = 			0;			// default floor height
BCINT DefaultCeilingHeight = 		128;		// default ceiling height
BCINT DefaultLighting = 		    255;		// default sector lighting
BCINT DefaultLargeScroll = 			10;			// default large scroll factor
BCINT CloseToLine = 						1;	
BCINT TempCloseToLine = 				1;	
BCINT LastLineDefTag = 					0;	
BCINT LastSectorTag = 					0;	
BCINT Increment = 							16;
BCINT MinGrid = 								4;
BCINT MaxGrid = 								512;
BCINT InitGrid = 								64;
BCINT GridScale = 							0;
BCINT Gamma = 									0;
BCINT StretchDelay =						5;
BCINT Count = 									0;
BCINT Sensitivity;
BCINT MoveSpeed = 							10;
BCINT DethFont =								0;
BCINT MenuDefault =							0;
BCINT NumPatchWads = 						0;
BCINT NumConfigPatchWads = 			0;
BCINT EdMode = 									0;
Bool  ChooseGame = 							TRUE;
Bool  GraphFront = 							TRUE;
Bool  Registered = 							FALSE;	/* registered or shareware game? */
Bool  Debug = 									FALSE;	/* are we debugging? */
Bool  SwapButtons =							FALSE;	/* swap right and middle mouse buttons */
Bool  Quiet = 									FALSE;	/* don't play a sound when an object is selected */
Bool  Quieter =									FALSE;	/* don't play any sound, even when an error occurs */
Bool  Expert = 									FALSE;	/* don't ask for confirmation for some operations */
Bool  FakeCursor = 							FALSE;	/* use a "fake" mouse cursor */
Bool  CirrusCursor = 						FALSE;	/* use hardware cursor on Cirrus Logic VGA cards */
Bool  Colour2 = 								FALSE;	/* use the alternate set for things colors */
Bool  InfoShown = 							TRUE;		/* should we display the info bar? */
Bool  ShowSprite = 							FALSE;	
Bool  DragObject = 							FALSE;
Bool  BrowseMap = 							FALSE;
Bool  ShowDistance = 						FALSE;
Bool  AdditiveSelBox = 					FALSE;	/* additive selection box or select in box only? */
Bool  ThingAngle = 							FALSE;  /* draw things with directional arrow */
Bool  DispSprite = 							TRUE;
Bool  AltInfo = 								TRUE;
Bool  VertConf = 								FALSE;	/* don't ask for confirmation for some operations */
Bool  QisQuit = 								FALSE;		
Bool  Select0 = 								TRUE;		/* select object 0 by default when switching modes */
Bool  Reminder = 								FALSE;	/* display a funny message when HETH starts */
Bool  Failed = 									FALSE;
Bool  CheckReport =							FALSE;  /* just list errors, don't stop on them */
Bool  DoingBehavior =						FALSE;
Bool  KeepInfo = 								FALSE;
Bool  Caching = 								FALSE;
Bool  UsePromptOnly = 					FALSE;
Bool  GridShown = 							TRUE;
Bool  NestMenus = 							TRUE;
Bool  AutoMap = 								FALSE;
Bool  ShowThings = 							FALSE;
Bool  GridDashed = 							FALSE;
Bool  HideGrid = 								TRUE;
Bool  SaveIniFile = 						TRUE;
Bool  CopySideDefs =						FALSE;
Bool  CreatingNewLevel =				FALSE;
Bool  BOOMEnable =							FALSE;	/* allow support for BOOM features */
Bool  SpecialEffects =					FALSE;	/* turn off some error checks */
char  *CfgFile = 								DEU_CONFIG_FILE;	/* name of the configuration file */
char  *BGIDriver = 							"VESA";	/* default extended BGI driver */
#ifdef GAME_HEXEN
char  *DefaultWallTexture = 		"FOREST01";			/* default normal wall texture */
char  *DefaultUpperTexture = 		"FOREST01";			/* default upper wall texture */
char  *DefaultLowerTexture = 		"FOREST01";    	/* default lower wall texture */
char  *DefaultFloorTexture = 		"F_014\0\0\0";	/* default floor texture */
char  *DefaultCeilingTexture = 	"F_089\0\0\0";	/* default ceiling texture */
char  *DefaultDoorTrack = 			"D_END1\0\0";		/* default door track texture */
char  *DefaultDoorTexture = 		"BRASS1\0\0";		/* default door texture */
char  *DefaultUpperStepTexture ="WINN01\0\0";		/* default step texture */
char  *DefaultLowerStepTexture ="WINN01\0\0";		/* default step texture */
char  *DefaultTeleportTexture = "F_084\0\0\0";	/* default teleport texture */
char  *MainWad = 								"HEXEN.WAD"; 		/* name of the main wad file */
#else
char  *DefaultWallTexture = 		"STARTAN3";			/* default normal wall texture */
char  *DefaultUpperTexture = 		"STARTAN3";			/* default upper wall texture */
char  *DefaultLowerTexture = 		"STARTAN3";    	/* default lower wall texture */
char  *DefaultFloorTexture = 		"FLOOR7_1";			/* default floor texture */
char  *DefaultCeilingTexture = 	"RROCK09\0";		/* default ceiling texture */
char  *DefaultDoorTrack = 			"DOORTRAK";			/* default door track texture */
char  *DefaultDoorTexture = 		"DOOR1\0\0\0";	/* default door texture */
char  *DefaultUpperStepTexture ="STEP5\0\0\0";	/* default step texture */
char  *DefaultLowerStepTexture ="STEP5\0\0\0";	/* default step texture */
char  *DefaultTeleportTexture = "GATE1\0\0\0";	/* default teleport texture */
char  *MainWad = 								"DOOM2.WAD"; 		/* name of the main wad file */
#endif
char  **PatchWads = 						NULL;						/* list of patch wad files */
char  **ConfigPatchWads =       NULL;  					/* list of patch wad files */
char  *BackupText = 						"Nothing To Undo";
char *Doom = 										NULL;
char *Doom2 = 									NULL;
char *Heretic = 								NULL;
char *Hexen = 									NULL;
char *Strife = 									NULL;
char *Game = 										"DOOM2";
char LevelName[9] = 						"\0\0\0\0\0\0\0\0";
char *RegTest = 								NULL;
char *CurrentScheme =						NULL;
char *DoomPwads =								".";
char *Doom2Pwads =							".";
char *HereticPwads =						".";
char *StrifePwads =							".";
char *HexenPwads =							".";
char *Pwads =										".";
char *Cwd =											NULL;
int MaxExtendedType = 					0;

int GotImages[64] =
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,/*JFF plan for more textures*/
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
unsigned char *WallImages[64] =
{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};
SList LevelNameFormat = (SList)NULL;

OptDesc options[] =				/* description of the command line options */
{
	//  short & long names  type            message if true/changed        message if false    where to store the value
	//--------------------------------------------------------------------------------------------------------------------
  { "f",  "file",		      OPT_STRINGLIST, "Patch WAD file",              NULL,                         &PatchWads },
  { "pw", "pwad",			    OPT_STRINGACC,  "Patch WAD file",              NULL,                         &PatchWads },
  { "b",	"boom",  	      OPT_BOOLEAN,    "BOOM support ON",             "BOOM support OFF",           &BOOMEnable },
  { "",	  "specialeffects",OPT_BOOLEAN,   "Special Effect Checking ON",  "Special Effect Checking OFF",&SpecialEffects},
  { "",   "config",      	OPT_STRING,     "Config file",                 NULL,                         &CfgFile },
  { "",   "main",        	OPT_STRING,     "Main WAD file",               NULL,                         &MainWad },
  { "",   "bgi",         	OPT_STRING,     "Default video driver",        NULL,                         &BGIDriver },
  { "",   "walltexture", 	OPT_STRING,     "Default wall texture",        NULL,                         &DefaultWallTexture },
  { "",   "lowertexture",	OPT_STRING,     "Default lower wall texture",  NULL,                         &DefaultLowerTexture },
  { "",   "uppertexture",	OPT_STRING,     "Default upper wall texture",  NULL,                         &DefaultUpperTexture },
  { "",   "floortexture",	OPT_STRING,     "Default floor texture",       NULL,                         &DefaultFloorTexture },
  { "",   "ceiltexture", 	OPT_STRING,     "Default ceiling texture",     NULL,                         &DefaultCeilingTexture },
  { "",   "doortrack",   	OPT_STRING,     "Default door track texture",  NULL,                         &DefaultDoorTrack },
  { "",   "doortexture", 	OPT_STRING,     "Default door texture",        NULL,                         &DefaultDoorTexture },
  { "",   "upperstep",   	OPT_STRING,     "Default upper step texture",  NULL,                         &DefaultUpperStepTexture },
  { "",   "lowerstep",   	OPT_STRING,     "Default lower step texture",  NULL,                         &DefaultLowerStepTexture },
  { "",   "teletexture", 	OPT_STRING,     "Default teleport texture",    NULL,                         &DefaultTeleportTexture },
  { "",		"doom",	   		  OPT_STRING,     NULL,                					 NULL,                         &Doom },
  { "",		"doom2",	   	  OPT_STRING,     NULL,                					 NULL,                         &Doom2 },
  { "",		"heretic",	   	OPT_STRING,     NULL,                					 NULL,                         &Heretic },
  { "",		"hexen",	  	  OPT_STRING,     NULL,                					 NULL,                         &Hexen },
  { "",		"strife",	  	  OPT_STRING,     NULL,                					 NULL,                         &Strife },
  { "",		"game",	   		  OPT_STRING,     NULL,                					 NULL,                         &Game },
  { "",		"doompwads",	  OPT_STRING,     NULL,                					 NULL,                         &DoomPwads },
  { "",		"doom2pwads",	  OPT_STRING,     NULL,                					 NULL,                         &Doom2Pwads },
  { "",		"hereticpwads",	OPT_STRING,     NULL,                					 NULL,                         &HereticPwads },
  { "",		"strifepwads",	OPT_STRING,     NULL,                					 NULL,                         &StrifePwads },
  { "",		"hexenpwads",	  OPT_STRING,     NULL,                					 NULL,                         &HexenPwads },
  { "",   "pwads",				OPT_STRINGLIST, "Patch WAD files",             NULL,                         &ConfigPatchWads},
  { "z",	"zoom",        	OPT_INTEGER,    "Initial zoom factor",         NULL,                         &InitialScale },
  { "",		"closetoline",  OPT_INTEGER,    "Close to Line to Split",	     NULL,                         &CloseToLine },
  { "v",  "video",       	OPT_INTEGER,    "Default video mode",          NULL,                         &VideoMode },
	{ "",   "increment",   	OPT_INTEGER,    NULL,  												 NULL, 												 &Increment },
	{ "",   "mingrid",     	OPT_INTEGER,    NULL,  												 NULL, 												 &MinGrid },
	{ "",   "maxgrid",     	OPT_INTEGER,    NULL,  												 NULL, 												 &MaxGrid },
	{ "",   "initgrid",   	OPT_INTEGER,    NULL,  												 NULL, 												 &InitGrid },
  { "",   "lighting",   	OPT_INTEGER,    "Default lighting",            NULL,                         &DefaultLighting },
  { "",   "floorheight", 	OPT_INTEGER,    "Default floor height",        NULL,                         &DefaultFloorHeight },
  { "",   "ceilheight",  	OPT_INTEGER,    "Default ceiling height",      NULL,                         &DefaultCeilingHeight },
  { "",   "largescroll", 	OPT_INTEGER,    "Default large scroll factor", NULL, 			                   &DefaultLargeScroll },
	{ "",   "gamma",   		  OPT_INTEGER,    "",  													 "", 												   &Gamma },
  { "",   "stretchdelay",	OPT_INTEGER,    "",							  						 NULL,												 &StretchDelay },
  { "",   "edmode",		    OPT_INTEGER,    "",							  						 NULL,												 &EdMode },
  { "",	  "choosegame",  	OPT_BOOLEAN,    "",							  						 NULL,												 &ChooseGame },
  { "",	  "graphfront",  	OPT_BOOLEAN,    "",							  						 NULL,												 &GraphFront },
  { "d",  "debug",       	OPT_BOOLEAN,    "Debug mode ON",               "Debug mode OFF",             &Debug },
  { "q", 	"quiet",       	OPT_BOOLEAN,    "Quiet mode ON",               "Quiet mode OFF",             &Quiet },
  { "qq",	"quieter",     	OPT_BOOLEAN,    "Quieter mode ON",             "Quieter mode OFF",           &Quieter },
  { "e",	"expert",      	OPT_BOOLEAN,    "Expert mode ON",              "Expert mode OFF",            &Expert },
  { "",		"qisquit",    	OPT_BOOLEAN,    "Quit on Q",                   "Save on Q",                  &QisQuit },
  { "sb",	"swapbuttons", 	OPT_BOOLEAN,    "Mouse buttons swapped",       "Mouse buttons restored",     &SwapButtons },
  { "",		"vertconf",    	OPT_BOOLEAN,    "Confirm Vertices Merge",      NULL,            				     &VertConf },
  { "",		"select0",     	OPT_BOOLEAN,    "Select 0 by default",         "No default selection",       &Select0 },
  { "",	 	"thingangle",  	OPT_BOOLEAN,    "Show Thing angle",            NULL,                         &ThingAngle },
  { "fc",	"fakecursor",  	OPT_BOOLEAN,    "Fake cursor ON",              "Fake cursor OFF",            &FakeCursor },
  { "cc",	"cirruscursor",	OPT_BOOLEAN,    "Cirrus hardware cursor ON",   "Cirrus hardware cursor OFF", &CirrusCursor },
  { "i",	"infobar",     	OPT_BOOLEAN,    "Info bar shown",              "Info bar hidden",            &InfoShown },
  { "a",	"addselbox",   	OPT_BOOLEAN,    "Additive selection box",      "Select objects in box only", &AdditiveSelBox },
	{ "",		"dispsprite",  	OPT_BOOLEAN,    "Show graphics",   		  	     "", 													 &DispSprite },
	{ "",		"altinfo",     	OPT_BOOLEAN,    "Using alternative info bar",  "", 													 &AltInfo },
	{ "",		"keepinfo",		  OPT_BOOLEAN,    "",  													 "", 													 &KeepInfo },
	{ "",		"caching", 		  OPT_BOOLEAN,    "",  													 "", 													 &Caching },
	{ "",		"usepromptonly",OPT_BOOLEAN,    "",  													 "", 													 &UsePromptOnly },
	{ "",		"nestmenus",	  OPT_BOOLEAN,    "",  													 "", 													 &NestMenus },
	{ "",		"automap",		  OPT_BOOLEAN,    "",  													 "", 													 &AutoMap },
	{ "",		"showthings",	  OPT_BOOLEAN,    "",  													 "", 													 &ShowThings },
	{ "",		"griddashed",	  OPT_BOOLEAN,    "",  													 "", 													 &GridDashed },
	{ "",		"hidegrid",		  OPT_BOOLEAN,    "",  													 "", 													 &HideGrid },
	{ "",		"saveinifile",	OPT_BOOLEAN,    "",  													 "", 													 &SaveIniFile },
	{ "",		"showdistance",	OPT_BOOLEAN,    "",  													 "", 													 &ShowDistance },
	{ "",		"copysidedefs",	OPT_BOOLEAN,    "",  													 "", 													 &CopySideDefs },
  { NULL, NULL,          	OPT_END,        NULL,			  									 NULL,                         NULL }
};

//
//  the main program
//

int main( int argc, char *argv[])
{
  BCINT i;
	char input[ 10];
	char **p;
    
  setcbrk(0);		// disable CTRL-BREAK checking
	signal(SIGINT,SIG_IGN);

  argv++;
  argc--;

  /* quick and dirty check for a "-config" option */
  for (i = 0; i < argc - 1; i++)
		if (!strcmp( argv[i], "-config"))
		{
			CfgFile = argv[i + 1];
			break;
		}

  /* read config file and command line options */
  ParseConfigFileOptions(CfgFile);
  ParseCommandLineOptions(argc, argv);

#ifdef GAME_DOOM
	if (ChooseGame)
	{
		strupr(Game);
		ScreenClear();
		Credits( stdout);
		input[ 0] = '\0';
		printf( "\n[Enter game or hit enter]> ");
		gets(input);
		strupr(input);
		if (strstr(input, "2"))
			strcpy(Game, "DOOM2");
		else if (input[0] == 'D' || input[0] == '1')
			strcpy(Game, "DOOM");
		else if (input[0] == 'H' || input[0] == '3')
			strcpy(Game, "HERETIC");
		else if (input[0] == 'S' || input[0] ==  '4')
			strcpy(Game, "STRIFE");
		else if (strstr(input, "Q"))
		{
			printf("\nDeth: Beyond DEU\n");
			return 0; 
		}
	}
#endif

  if (strstr(Game, "DOOM2"))
	{
		readcfg(Doom2);
		strcpy(Pwads, Doom2Pwads);
	}
	else if (strstr(Game, "DOOM"))
	{
		readcfg(Doom);
		strcpy(Pwads, DoomPwads);
	}
	else if (strstr(Game, "HERETIC"))
	{
		readcfg(Heretic);
		strcpy(Pwads, HereticPwads);
	}
	else if (strstr(Game, "HEXEN"))
	{
		readcfg(Hexen);
		strcpy(Pwads, HexenPwads);
	}
	else if (strstr(Game, "STRIFE"))
	{
		readcfg(Strife);
		strcpy(Pwads, StrifePwads);
	}


#ifndef SLIM
	if (!GraphFront)
	{
		ScreenClear();
		Credits(stdout); 
	}
#else
	ScreenClear();
	Credits(stdout); 
#endif

#ifdef GAME_HEXEN
	LogMessage(": Welcome to HETH!\n");
#else
	LogMessage(": Welcome to DETH!\n");
#endif

  if (Quieter)
		Quiet = TRUE;
	Cwd = getcwd(0,255);
	if (Pwads[1] == ':')
		setdisk((Pwads[0] | 32) - 'a');
	chdir(Pwads);

  /* load the wad files */     
  OpenMainWad(MainWad);
  if ((p=ConfigPatchWads))
		while (p[0])
		{
			OpenPatchWad(strupr(p[0]));
			p++;
			NumConfigPatchWads++;
		}
  if ((p=PatchWads))
		while (p[0])
		{
			OpenPatchWad(strupr(p[0]));
			p++;
			NumPatchWads++;
		}

  /* sanity check */
  CloseUnusedWadFiles();
    
  /* SO: here seems like a good place to make the ld_types index */
  /* not so good now that the data structures have changed! */
  index_ld_types();
  index_things();
#ifdef GAME_HEXEN
  index_spawns();
#endif
  index_schemes();

  /* all systems go! */
  RealVideoMode = VideoMode;


#ifndef SLIM
	if (GraphFront)
		MainLoop();
	else
		MainLoop2();
#else
  MainLoop2();
#endif

	if (Cwd[1] == ':')
		setdisk((Cwd[0] | 32) - 'a');
  chdir(Cwd);
  
	/* that's all, folks! */
  CloseWadFiles();
  LogMessage(": The end!\n\n\n");
  if (logfile != NULL)
		fclose(logfile);

	signal(SIGINT,SIG_DFL);
  return 0;
}



//
//  Append a string to a null-terminated string list
//

void AppendItemToList( char ***list, char *item)
{
  BCINT i;
  
  i = 0;
  if (*list != NULL)
	{
		/* count the number of elements in the list (last = null) */
		while ((*list)[i] != NULL)
			i++;
		/* expand the list */
		*list = (char**)ResizeMemory(*list, (i + 2) * sizeof( char **));
  }
  else
	{
		/* create a new list */
		*list = (char**)GetMemory(2 * sizeof( char **));
  }

  /* append the new element */
  (*list)[i] = item;
  (*list)[i + 1] = NULL;
}


//
// Handle command line options 
//

void ParseCommandLineOptions(int argc, char *argv[])
{
  BCINT optnum;
  
  while (argc > 0)
	{
		if (argv[0][0] != '-' && argv[0][0] != '+')
			ProgError("options must start with '-' or '+'");

		if (!strcmp(argv[0], "-?") || !stricmp(argv[0], "-h") ||
				  !stricmp(argv[0], "-help"))
		{
			Usage(stdout);
			exit(0);
		}

		for (optnum = 0; options[optnum].opt_type != OPT_END; optnum++)
		{
			if (!stricmp(argv[0]+1, options[optnum].short_name) ||
					  !stricmp(argv[0]+1, options[optnum].long_name))
			{
				switch (options[ optnum].opt_type)
				{
					case OPT_BOOLEAN:
						if (argv[0][0] == '-')
							*((Bool *) (options[optnum].data_ptr)) = TRUE;
						else 
							*((Bool *) (options[optnum].data_ptr)) = FALSE;
						break;
					case OPT_INTEGER:
						if (argc <= 1)
							ProgError("missing argument after \"%s\"", argv[0]);
						argv++;
						argc--;
						*((BCINT *) (options[optnum].data_ptr)) = atoi(argv[0]);
						break;
					case OPT_STRING:
						if (argc <= 1)
							ProgError("missing argument after \"%s\"", argv[0]);
						argv++;
						argc--;
						*((char **) (options[optnum].data_ptr)) = argv[0];
						break;
					case OPT_STRINGACC:
						if (argc <= 1)
							ProgError("missing argument after \"%s\"", argv[0]);
						argv++;
						argc--;
						AppendItemToList((char ***) options[optnum].data_ptr, argv[0]);
						break;
					case OPT_STRINGLIST:
						if (argc <= 1)
							ProgError("missing argument after \"%s\"", argv[0]);
						while (argc > 1 && argv[1][0] != '-' && argv[1][0] != '+')
						{
							argv++;
							argc--;
							AppendItemToList((char ***) options[optnum].data_ptr, argv[0]);
						}
						break;
					default:
						ProgError("unknown option type (BUG!)");
				}
				break;
			}
		}
		if (options[optnum].opt_type == OPT_END)
			ProgError("invalid argument: \"%s\"", argv[0]);
		argv++;
		argc--;
  }
}



/* read the config file */

void ParseConfigFileOptions( char *filename)
{
  FILE *cfgfile;
  char  line[ 1024];
  char *value;
  char *option;
  char *p;
  BCINT   optnum;
  
  if ((cfgfile = fopen (filename, "r")) == NULL)
	{
		printf( "Warning: Configuration file not found (%s)\n", filename);
		return;
  }

  while (fgets (line, 1024, cfgfile) != NULL)
	{
		// skip comments and lines less than 2 char long
		if (line[0] == '#' || strlen(line) < 2)
			continue;

		// remove end-of-line terminators
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';

		// skip blanks before the option name
		option = line;
		while (isspace(option[0]))
			option++;

		// skip the option name 
		value = option;
		while (value[0] && value[0] != '=' && !isspace(value[0]))
			value++;
		if (!value[0])
			ProgError("invalid line in %s (ends prematurely)", filename);
		// mark the end of the option name 
		value[0] = '\0';

		if (value[0] != '=')
		{
			value++;
			/* skip blanks after the option name */
			while (isspace(value[0]))
				value++;
			if (value[0] != '=')
				ProgError("invalid line in %s (no '=')", filename);
		}

		value++;
		/* skip blanks after the equal sign */
		while (isspace(value[0]))
			value++;

		for (optnum = 0; options[optnum].opt_type != OPT_END; optnum++)
		{
			if (!stricmp(option, options[optnum].long_name))
			{
				switch (options[optnum].opt_type)
				{
					case OPT_BOOLEAN:
						if
						(
							!stricmp(value, "yes") ||
							!stricmp(value, "true") ||
					    !stricmp(value, "on") ||
							!stricmp(value, "1")
						) 
							*((Bool *) (options[optnum].data_ptr)) = TRUE;
						else if
						(
							!stricmp(value, "no") ||
							!stricmp(value, "false") ||
							!stricmp(value, "off") ||
							!stricmp(value, "0")
						) 
							*((Bool *) (options[optnum].data_ptr)) = FALSE;
						else
							ProgError( "invalid value for option %s: \"%s\"", option, value);
						break;
					case OPT_INTEGER:
						*((BCINT *) (options[optnum].data_ptr)) = atoi(value);
						break;
					case OPT_STRING:
						p = (char*)GetMemory((strlen(value) + 1) * sizeof(char));
						strcpy(p, value);
						*((char **) (options[optnum].data_ptr)) = p;
						break;
					case OPT_STRINGACC:
						p = (char*)GetMemory((strlen( value) + 1) * sizeof(char));
						strcpy(p, value);
						AppendItemToList((char ***) options[optnum].data_ptr, p);
						break;
					case OPT_STRINGLIST:
						while (value[0])
						{
							option = value;
							while (option[0] && !isspace(option[0]))
								option++;
							option[0] = '\0';
							option++;
							while (isspace(option[0]))
								option++;
							p = (char*)GetMemory( (strlen(value) + 1) * sizeof(char));
							strcpy(p, value);
							AppendItemToList((char ***) options[optnum].data_ptr, p);
							value = option;
						}
						break;
					default:
						ProgError("unknown option type (BUG!)");
				}
				break;
			}
		}
		if (options[optnum].opt_type == OPT_END)
			ProgError("Invalid option in %s: \"%s\"", filename, option);
  }
  fclose( cfgfile);

  if (StretchDelay < 1)
    StretchDelay = 1;
  if (StretchDelay > 100)
    StretchDelay = 100;

  if (CloseToLine < 0)
    CloseToLine = 0;
  if (CloseToLine > 16)
    CloseToLine = 16;

  GridScale = InitGrid;
}


//
//   output the program usage to the specified file
//

void Usage( FILE *where)
{
  fprintf( where, "Usage:\n");
  fprintf( where, "DETH [-w <main_wad_file>] [-d] [-sb] [-c] [-q] [-qq] [-sq] [-e] [-a] [-i] [-z <zoom>] [-bgi <driver>] [-v <mode>] [-fc] [-config <ini_file>] [-pw <pwad_file>] [-file <pwad_files>...]\n");
  fprintf( where, "   -w    Gives the name of the main wad file (also -main).  Default is DOOM.WAD\n");
  fprintf( where, "   -d    Enter debug mode (also -debug).\n");
  fprintf( where, "   -q    Suppresses some sounds (also -quiet).\n");
  fprintf( where, "   -qq   Suppresses all sounds (also -quieter).\n");
  fprintf( where, "   -e    Stops prompts for confirmation (also -expert).\n");
  fprintf( where, "   -a    To have an additive selection box (also -addselbox).\n");
  fprintf( where, "   -i    Show the info bar in the editors (also -infobar).\n");
  fprintf( where, "   -z    Set the initial zoom factor for the editors (also -zoom).\n");
  fprintf( where, "   -v    Set the default video mode number (also -video).\n");
  fprintf( where, "   -fc   Use a \"fake\" mouse cursor (also -fakecursor).\n");
  fprintf( where, "   -cc   Turns on cirrus hardware cursor (also -cirruscursor).\n");
  fprintf( where, "   -sb   Swaps the mouse buttons (also -swapbuttons).\n");
  fprintf( where, "   -pw   To add one patch wad file to be loaded; may be repeated (also -pwad).\n");
  fprintf( where, "   -file To add a list of patch wad files to be loaded.\n");
  fprintf( where, "   -config Gives the name of the config file.\n");
  fprintf( where, "Put a '+' instead of a '-' before boolean options to reverse their effect.\n");
}


//
// output the credits of the program to the specified file
//

void Credits( FILE *where)
{
#ifdef GAME_HEXEN

#ifdef SLIM
  fprintf( where, "\nHETH - Hexen Editor for Total Headcases, Version %s SLIM\n\n", DETH_VERSION);
#else
  fprintf( where, "\nHETH - Hexen Editor for Total Headcases, Version %s\n\n", DETH_VERSION);
#endif
#else

#ifdef SLIM
  fprintf( where, "\nDETH - Doom Editor for Total Headcases, Version %s SLIM\n\n", DETH_VERSION);
#else
  fprintf( where, "\nDETH - Doom Editor for Total Headcases, Version %s\n\n", DETH_VERSION);
#endif
#endif
  fprintf( where, "The Professional DEU-GCC Editor of Choice for Discriminating Authors.\n\n");
  fprintf( where, "Authors:-\n");
	fprintf( where, "    Antony J. Burden (100765.3240@compuserve.com)\n");
	fprintf( where, "    and    Simon Oke (loony@demon.co.uk)\n\n");
	fprintf( where, "Hacks by:-\n");
	fprintf( where, "    Jim F. Flynn  (jflynn@pacbell.net)\n");
	fprintf( where, "With Help From:-\n");
	fprintf( where, "    John W. Anderson, Jim F. Flynn.\n");
	fprintf( where, "    Sverre A. Kvernmo, Ty Halderman.\n");
	fprintf( where, "    Ken F. Henderson and Kjell Rilbe.\n\n");
	fprintf( where, "Editing: %s\n\n", Game);
}


//
// play a fascinating tune
//

void Beep()
{
  if (!Quieter)
	{
		sound(640);
		delay(100);
		nosound();
  }
}

//
// play a sound
//

void PlaySound( BCINT freq, BCINT msec)
{
  if (!Quiet)
	{
		sound(freq);
		delay(msec);
		nosound();
  }
}

//
// terminate the program reporting an error
//

void ProgError( char *errstr, ...)
{
  va_list args;
  
  Beep();
  Beep();
  if (GfxMode)
	{
		sleep(1);
		TermGfx();
  }
  va_start(args, errstr);
  printf( "\nProgram Error: *** ");
  vprintf(errstr, args);
  printf(" ***\n");
  if (Debug && logfile != NULL)
	{
		fprintf( logfile, "\nProgram Error: *** ");
		vfprintf( logfile, errstr, args);
		fprintf( logfile, " ***\n");
  }
  va_end( args);

  // clean up things
  ForgetLevelData();
  ForgetWTextureNames();
  ForgetFTextureNames();
  CloseWadFiles();
	if (Cwd[1] == ':')
		setdisk((Cwd[0] | 32) - 'a');
  chdir(Cwd);
  exit(5); 
}

//
// write a message in the log file
//
// SO: changed this 13/3/95.
// we now open the logfile before, and close it after, writing each
// line. This way we guarantee that all messages get into the file
// (even if the program crashes).
//

void LogMessage(char *logstr, ...)
{
  va_list  args;
  time_t   tval;
  char    *tstr;
    
  if (Debug)
	{
		logfile = fopen(DEU_LOG_FILE, "a");
		va_start(args, logstr);
		/* if the messsage begins with ":",
		   output the current date & time first */
		if (logstr[0] == ':')
		{
		    time(&tval);
		    tstr = ctime(&tval);
		    tstr[strlen(tstr) - 1] = '\0';
		    fprintf(logfile, "%s", tstr);
		}
		vfprintf(logfile, logstr, args);
		va_end(args);
		fclose(logfile);
  }
}

#ifndef SLIM

//
// Graphical Credits 
//

void GraphCredits()
{
	BCINT height, width;
	InitGfx();
	CheckMouseDriver(); 
	VideoMode = 1;
	DrawScreenBox3D( 0, 0, 639, 149);
	DrawScreenBox3D( 0, 150, 639, 429);
	DrawScreenBox3D( 0, 430, 639, 479);
#ifdef GAME_HEXEN
	GetPicSize("M_HTIC", &width, &height);
	DisplayPic( 630 - width, 10, 630, 10 + height, "M_HTIC",0);
	DrawScreenText(10, 10, "HETH - Hexen Editor for Total Headcases, Version %s", DETH_VERSION);
#else
	GetPicSize("M_HTIC", &width, &height);
	DisplayPic( 630 - width, 10, 630, 10 + height, "M_HTIC",0);
	GetPicSize("M_DOOM", &width, &height);
	DisplayPic( 630 - width, 10, 630, 10 + height, "M_DOOM",0);
	DrawScreenText(10, 10, "DETH - Doom Editor for Total Headcases, Version %s", DETH_VERSION);
#endif
	settextstyle(2,0,0);
	DrawScreen3DText(20, 24, DARKBLUE, TRUE, "The Professional DEU-GCC Editor of Choice for Discriminating Authors.");
	settextstyle(0,0,1);
	SetColor(DARKBLUE);
	DrawScreenText(10, 44, "Authors:-");
	DrawScreenText(-1, -1, "    Antony J. Burden (100765.3240@compuserve.com)");
	DrawScreenText(-1, -1, "    and    Simon Oke (loony@demon.co.uk)");
	SetColor(BLUE);
	DrawScreenTextFonted(2, 0,  10, 84, "With Help From:-");
	DrawScreenTextFonted(2, 0,  -1, -1, "    John W. Anderson, Jim F. Flynn.");
	DrawScreenTextFonted(2, 0,  -1, -1, "    Sverre A. Kvernmo, Ty Halderman.");
	DrawScreenTextFonted(2, 0,  -1, -1, "    Ken F. Henderson and Kjell Rilbe.");
	VideoMode = 2;
	FrontEnd();
}

//
// Graphic Front End
//

void FrontEnd()
{
  int count;
  WadPtr wad;
  int wadcount, x;
	struct ffblk info;
	int done;

	settextstyle(2,0,0);
	DrawScreenBox3D( 0, 150, 639, 429);
	SetColor(BLUE);
	DrawScreenTextFonted(2, 0,  10, 160, "Current Settings");
	DrawScreenLine(10, 171, 101, 171);
	SetColor(DARKBLUE);
	DrawScreenTextFonted(2, 0,  10, 172, " ");
	DrawScreenTextFonted(2, 0,  -1, -1, "Video Mode");
	DrawScreenTextFonted(2, 0,  -1, -1, "Editor Mode");
	DrawScreenTextFonted(2, 0,  -1, -1, "Sounds");
	DrawScreenTextFonted(2, 0,  -1, -1, "Info Bar");
	DrawScreenTextFonted(2, 0,  -1, -1, "Nested Menus");
	DrawScreenTextFonted(2, 0,  -1, -1, "Scroll Factor");
	DrawScreenTextFonted(2, 0,  -1, -1, "Grid Settings");
	DrawScreenTextFonted(2, 0,  -1, -1, "Initial Zoom");
	DrawScreenTextFonted(2, 0,  -1, -1, "Close to Split");
	DrawScreenTextFonted(2, 0,  -1, -1, "Marquee Delay");
	DrawScreenTextFonted(2, 0,  -1, -1, "Debugging");
	DrawScreenTextFonted(2, 0,  100, 172, " ");
	switch (RealVideoMode)
	{
		case 0:
			DrawScreenTextFonted(2, 0,  -1, -1, ": Standard");
			break;
		case 1:
		case 2:
			DrawScreenTextFonted(2, 0,  -1, -1, ": 640x480");
			break;
		case 3:
			DrawScreenTextFonted(2, 0,  -1, -1, ": 800x600");
			break;
		case 4:
			DrawScreenTextFonted(2, 0,  -1, -1, ": 1024x768");
			break;
		case 5:
			DrawScreenTextFonted(2, 0,  -1, -1, ": 1280x1024");
			break;
	}
	DrawScreenTextFonted(2, 0,  -1, -1, ": %s", Expert ? "Expert": "Novice");
	if (Quieter)
		DrawScreenTextFonted(2, 0,  -1, -1, ": Off");
	else if (Quiet)
		DrawScreenTextFonted(2, 0,  -1, -1, ": Partial");
	else 
		DrawScreenTextFonted(2, 0,  -1, -1, ": On");
	if (!InfoShown)
		DrawScreenTextFonted(2, 0,  -1, -1, ": Off");
	else
		DrawScreenTextFonted(2, 0,  -1, -1, ": %s", (InfoShown && AltInfo) ? "On, Alternative": "On, Standard");
	DrawScreenTextFonted(2, 0,  -1, -1, ": %s", NestMenus ? "On": "Off");
	DrawScreenTextFonted(2, 0,  -1, -1, ": %d", DefaultLargeScroll);
	DrawScreenTextFonted(2, 0,  -1, -1, ": %d, %d, %d%s", MinGrid, MaxGrid, InitGrid, GridDashed ? ", Dashed" : " ");
	DrawScreenTextFonted(2, 0,  -1, -1, ": %d", InitialScale);
	DrawScreenTextFonted(2, 0,  -1, -1, ": %d", CloseToLine);
	DrawScreenTextFonted(2, 0,  -1, -1, ": %d", StretchDelay);
	DrawScreenTextFonted(2, 0,  -1, -1, ": %s", Debug ? "On": "Off");
	DrawScreenTextFonted(2, 0,  10, -1, " ");
	if (ThingAngle)
		DrawScreenTextFonted(2, 0,  -1, -1, "Showing Things with Angle");
	if (VertConf)
		DrawScreenTextFonted(2, 0,  -1, -1, "Confirming Merging of Vertices");
	if (DispSprite)
		DrawScreenTextFonted(2, 0,  -1, -1, "Showing Textures/Flats and Sprites");
	if (AdditiveSelBox)
		DrawScreenTextFonted(2, 0,  -1, -1, "Additive Selection Box");
	if (Caching)
		DrawScreenTextFonted(2, 0,  -1, -1, "Caching Textures");
	if (KeepInfo)
		DrawScreenTextFonted(2, 0,  -1, -1, "Keeping INFO Lumps");
	DrawScreenTextFonted(2, 0,  -1, -1, " ");
#ifdef GAME_HEXEN
	SetColor(DARKMAGENTA);
#else
	SetColor(GREEN);
#endif
	DrawScreenTextFonted(2, 0, 10, 360, "Path:");
	DrawScreenTextFonted(2, 0, 50, 360, "%s", strlen(Pwads) <= 1 ? strupr(Cwd) : strupr(Pwads));
	count = 0;
	for (wad = WadFileList->next; wad; wad = wad->next)
	{
		if (count == 0)
		{
			DrawScreenTextFonted(2, 0, 10, 370 + (10 * count), "Added:");
			DrawScreenTextFonted(2, 0, 50, 370 + (10 * count), "%s", strupr(wad->filename));
		}
		else
			DrawScreenTextFonted(2, 0, 50, 370 + (10 * count), "%s", strupr(wad->filename));
		if (count >= 5)
		{
			DrawScreenTextFonted(2, 0, 50, 370 + (10 * count), "More..");
			break;
		}
		count++;
	}
	if (UsePromptOnly)
	{
		SetColor(DARKRED);
		x = 300;
		DrawScreenTextFonted(2, 0, x, 160, "Pwads Present in %s", strlen(Pwads) <= 1 ? strupr(Cwd) : strupr(Pwads));
		SetColor(DARKBLUE);
		wadcount = 0;
		done = findfirst("*.wad", &info, 0);
		while(!done && wadcount <= 50)
		{
			DrawScreenTextFonted(2, 0, x, 180 + (wadcount * 10), info.ff_name);
			DrawScreenTextFonted(2, 0, x + 80, 180 + (wadcount * 10), "%dK", (info.ff_fsize / 1024));
			done = findnext(&info);
			wadcount++;
			if (wadcount == 24)
			{
				x += 130;
				wadcount = 0;
			}
		}
	}
	if (!UsePromptOnly)
	{
		SetColor(BLUE);
		DrawScreenTextFonted(2, 0, 400, 160, "Quick Keys");
		DrawScreenLine(400, 171, 458, 171);
		SetColor(DARKBLUE);
		DrawScreenTextFonted(2, 0, 400, 172, " ");
		DrawScreenTextFonted(2, 0, -1, -1, "Enter");
		DrawScreenTextFonted(2, 0, -1, -1, "E");
		DrawScreenTextFonted(2, 0, -1, -1, "Shift E");
		DrawScreenTextFonted(2, 0, -1, -1, "C");
		DrawScreenTextFonted(2, 0, -1, -1, "Shift C");
		DrawScreenTextFonted(2, 0, -1, -1, "W");
		DrawScreenTextFonted(2, 0, -1, -1, "V");
		DrawScreenTextFonted(2, 0, -1, -1, "Q");
		DrawScreenTextFonted(2, 0, 490, 172, " ");
		DrawScreenTextFonted(2, 0, -1, -1, "Command Line");
		DrawScreenTextFonted(2, 0, -1, -1, "Edit Level 1");
		DrawScreenTextFonted(2, 0, -1, -1, "Browse Levels to Edit");
		DrawScreenTextFonted(2, 0, -1, -1, "Create Level 1");
		DrawScreenTextFonted(2, 0, -1, -1, "Browse Levels to Create");
		DrawScreenTextFonted(2, 0, -1, -1, "List Open Wads");
		DrawScreenTextFonted(2, 0, -1, -1, "View Sprites");
		DrawScreenTextFonted(2, 0, -1, -1, "Quit");
		ShowMousePointer();
		DrawScreenButton( 586, 400, 40, 16, "Help", TRUE);
		DrawScreenButton( 520, 400, 40, 16, "Quit", TRUE);
		DrawScreenButton( 460, 400, 40, 16, "DOS", TRUE);
		DrawScreenButton( 400, 400, 40, 16, "Wads", TRUE);
		DrawScreenButton( 340, 400, 40, 16, "Dir", TRUE);
		DrawScreenButton( 460, 342, 80, 16, "Edit", TRUE);
		DrawScreenButton( 546, 342, 80, 16, "Browse", TRUE);
		DrawScreenButton( 460, 370, 80, 16, "Create", TRUE);
		DrawScreenButton( 546, 370, 80, 16, "Browse", TRUE);
	}
	settextstyle(0,0,1);
	SetColor( WHITE);
	DrawScreenText(  20, ScrMaxY - 26, "Command Line:");
	DrawScreenText(  470, ScrMaxY - 26, "Enter \'?\' for Help");
	DrawScreenButton( 130, ScrMaxY - 30, 325, 13, "", TRUE);   
}

//
// the main program menu loop, in graphics
//

void MainLoop()
{
	char input[120];
	int key = 0;
	BCINT buttons;

	VideoMode = 2;
	GraphCredits();
	input[0] = '\0';
	for (;;)
	{
		GetMouseCoords( &PointerX, &PointerY, &buttons);

		if (bioskey( 1))
		    key = bioskey( 0);

		if (UsePromptOnly)
		{
			HideMousePointer();
			key = ENTER;
		}

		if (!UsePromptOnly && buttons == 1)
		{
			if (PointerY > 432)
				key = ENTER;
			else if (MouseClickedArea(586, 400, 40, 16))
			{
				DrawScreenButtonIn( 586, 400, 40, 16, "Help", TRUE);
				ParseInput( "?");
				FrontEnd();
			}
			else if (MouseClickedArea(525, 400, 40, 16))
			{
				key = KEY_Q;
				DrawScreenButtonIn( 520, 400, 40, 16, "Quit", TRUE);
			}
			else if (MouseClickedArea(460, 400, 40, 16))
			{
				DrawScreenButtonIn( 460, 400, 40, 16, "DOS", TRUE);
				key = 545;
			}
			else if (MouseClickedArea(400, 400, 40, 16))
			{
				DrawScreenButtonIn( 400, 400, 40, 16, "Wads", TRUE);
				key = KEY_W;
			}
			else if (MouseClickedArea(340, 400, 40, 16))
			{
				DrawScreenButtonIn( 340, 400, 40, 16, "Dir", TRUE);
				ParseInput( "DIR");
				FrontEnd();
			}
			else if (MouseClickedArea(460, 350, 80, 16))
			{
				DrawScreenButtonIn( 460, 342, 80, 16, "Edit", TRUE);
				key = KEY_E;
			}
			else if (MouseClickedArea(546, 350, 80, 16))
			{
				DrawScreenButtonIn( 546, 342, 80, 16, "Browse", TRUE);
				key = SHIFT_E;
			}
			else if (MouseClickedArea(460, 370, 80, 16))
			{
				DrawScreenButtonIn( 460, 370, 80, 16, "Create", TRUE);
				key = KEY_C;
			}
			else if (MouseClickedArea(546, 370, 80, 16))
			{
				DrawScreenButtonIn( 546, 370, 80, 16, "Browse", TRUE);
				key = SHIFT_C;
			}
		}
/*
		if (key >= 1)
			SetColor (YELLOW);
		    DrawScreenText( 100,180, "%d", key);  */

		if (key == KEY_Q)
		{	
			SaveConfigFileOptions();
			TermGfx();
#ifdef GAME_HEXEN
			printf("Heth: Beyond DEU\n");
#else
			printf("Deth: Beyond DEU\n");
#endif
			break;
		}
		else if (key == SHIFT_E)
			ParseInput("E");
		else if (key == KEY_E)
		{
			if (strstr(Game, "DOOM2") || strstr(Game, "HEXEN"))
				strcpy(input, "E 1");
			else
				strcpy(input, "E 1 1");
				ParseInput( input);
				input[0] = '\0';
		}
		else if (key == SHIFT_C)
			ParseInput("C");
		else if (key == KEY_C)
		{
			if (strstr(Game, "DOOM2") || strstr(Game, "HEXEN"))
				strcpy(input, "C 1");
			else
				strcpy(input, "C 1 1");
				ParseInput(input);
				input[0] = '\0';
		}
		else if (key == 545) 		 /* '!' */
			ParseInput("!");
		else if (key == KEY_V)
		{ 
			if (!UsePromptOnly)
				HideMousePointer();
			ParseInput("V");
			ShowMousePointer();
		}
		else if (key == ENTER)
		{		
			if (!UsePromptOnly)
				HideMousePointer();
			SetColor(DARKGRAY);
			DrawScreenBox(130, ScrMaxY - 42, 639, ScrMaxY - 34);
			strcpy(input, "");
			InputCommand(130, ScrMaxY - 30, "", 40, input);  
			if (input[0] == 'Q')
			{
				SaveConfigFileOptions();
			    TermGfx();
#ifdef GAME_HEXEN
				printf( "Heth: Beyond DEU\n");
#else
				printf( "Deth: Beyond DEU\n");
#endif
			    break;
			} 
			else if (input[0] == 'O')
			{
				DoingBehavior = TRUE;
				input[0] = 'I';
				strcat(input, " BEHAVIOR");
				ParseInput(input);
				DoingBehavior = FALSE;
				ShowMousePointer();
				input[0] = '\0';
				FrontEnd();
			}
			else
			{
				ParseInput(input);
				ShowMousePointer();
				FrontEnd();
			}
		}
		else if (key == KEY_W)
		{  
			ParseInput("W");
			FrontEnd();
		}
		key = 0;
	}
}

//
// Front end command line parser for graphic front end
//

void ParseInput(char *input)
{
  char *com, *out;
  FILE *file, *raw;
  WadPtr wad = NULL;
  int key = 0;
  BCINT buttons = 0;
  Bool startcheck = FALSE;
  int x, wadcount;

	struct ffblk info;
	int done;

	/* eat the white space and get the first command word */
	com = strtok( input, " ");
	
	/* user just hit return */
	if (com == NULL)
		Beep();
	/* user inputting for help */
	else if (!strcmp( com, "?"))
	{
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
		SetColor( BLUE);
		DrawScreenText(  10, 160, "Command Line Help");
		DrawScreenText(  10, 162, "_________________");
		DrawScreenText(  -1, -1, " ");
		SetColor(DARKBLUE);
		DrawScreenText(  -1, -1, "B[uild] <WadFile>              -  Build new IWAD file");
		DrawScreenText(  -1, -1, "C[reate] [level]               -  Create and edit a new level");
		DrawScreenText(  -1, -1, "D[ump] <DirEntry> [outfile]    -  Dump a directory entry in hex");
		DrawScreenText(  -1, -1, "E[dit] [level]                 -  Edit a level");
		DrawScreenText(  -1, -1, "G[roup] <WadFile>              -  Group all PWADs into a file");
		DrawScreenText(  -1, -1, "I[nsert] <RawFile> <WadFile>   -  Insert a raw file into a PWAD file");
		DrawScreenText(  -1, -1, "L[ist] <WadFile> [outfile]     -  List the directory of a PWAD file");
		DrawScreenText(  -1, -1, "M[aster] [outfile]             -  List the master directory");
		DrawScreenText(  -1, -1, "R[ead] <WadFile>               -  Read a new PWAD file");
		DrawScreenText(  -1, -1, "S[ave] <DirEntry> <WadFile>    -  Save one entry to a separate file");
		DrawScreenText(  -1, -1, "V[iew] [SpriteName]            -  View the sprites");
		DrawScreenText(  -1, -1, "W[ads]                         -  List the open PWADS");
		DrawScreenText(  -1, -1, "X[tract] <DirEntry> <RawFile>  -  Extract one entry to a raw file");
#ifdef GAME_HEXEN
		DrawScreenText(  -1, -1, "O [myscript.raw]               -  Convert compiled script into BEHAVIOR.WAD");
#endif
		DrawScreenText(  -1, -1, "DIR / LS                       -  List Pwads present in current path");
		DrawScreenText(  -1, -1, "!                              -  Shell to DOS, Type 'exit' to return");
		SetColor( DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (startcheck)
			if (buttons == 1)
				break;
			if (bioskey( 1))
			key = bioskey( 0);
			if ( key > 0)
			  break;
			key = 0;
			if (buttons == 0)
			  startcheck = TRUE;
		}

	}
	else if (!strcmp(com, "!"))
	{
		TermGfx();
		printf("Type Exit to return\n");
		system("");
		/*InitGfx(); */
		GraphCredits();
	}
	/* user asked for list of open WAD files */
	else if (!strcmp( com, "WADS") || !strcmp( com, "W"))
	{
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
#ifdef GAME_HEXEN
    SetColor(LIGHTBLUE);
#else
    SetColor(DARKBLUE);
#endif
		DrawScreenTextFonted(2, 0, 10, 160, "%-30s  IWAD  (Main wad file)", WadFileList->filename);
		for (wad = WadFileList->next; wad; wad = wad->next)
		{
			if
			(
				wad->directory[ 0].name[ 0] == 'E' &&
				wad->directory[ 0].name[ 2] == 'M'
			)
				DrawScreenTextFonted
				(
					2, 0, 10, -1,
					"%-30s  PWAD  (Patch wad file for episode %c level %c)",
					wad->filename,
					wad->directory[ 0].name[ 1],
					wad->directory[ 0].name[ 3]
				);
			else
			{
				/* kluge */
				strncpy( input, wad->directory[ 0].name, 8);
				input[ 8] = '\0';
				DrawScreenTextFonted
				(
					2, 0, 10, -1,
					"%-30s  PWAD  (Patch wad file for %s)",
					wad->filename,
					input
				);
			}
		}
		SetColor( DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (startcheck)
			if (buttons == 1)
				break;
			if (bioskey( 1))
			key = bioskey( 0);
			if ( key > 0)
			  break;
			key = 0;
			if (buttons == 0)
			  startcheck = TRUE;
		}
	}
	/* user asked to edit a level */
	else if (!strcmp( com, "EDIT") || !strcmp( com, "E") ||
				!strcmp( com, "CREATE") || !strcmp( com, "C"))
	{
		SList tokens, t;
		
		Bool new = (strcmp(com, "EDIT") && strcmp(com, "E"));
		/* if com is neither of these, the user wants
			  to start from scratch */
		if (new) 
			CreatingNewLevel = TRUE;
		
		tokens = NULL;
		com = strtok( NULL, " ");
		while(com)
		{
			tokens = SList_append(tokens, com);
			com = strtok(NULL, " ");
		}
		if(tokens)
		{
			MakeLevelName(tokens);
			
			/* now forget the token list */
			while(tokens)
			{
				t = tokens->next;
				FreeMemory(tokens->string);
				FreeMemory(tokens);
				tokens = t;
			}
		}
		
		TermGfx();
		VideoMode = RealVideoMode;
		EditLevel(new);
		VideoMode = 2;
		GraphCredits();
	}
	/* user asked to build a new main WAD file */
	else if (!strcmp( com, "BUILD") || !strcmp( com, "B"))
	{
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Wad file name argument missing.");
			return;
		}
		for (wad = WadFileList; wad; wad = wad->next)
			if (!stricmp( com, wad->filename))
				break;
		if (wad)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText
			(
				130, ScrMaxY - 42,
				"File \"%s\" is opened and cannot be overwritten.\n",
				com
			);
			return;
		}
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
		BuildNewMainWad( com, FALSE);
		SetColor( DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			  GetMouseCoords( &PointerX, &PointerY, &buttons);
			  if (startcheck)
				if (buttons == 1)
					break;
			  if (bioskey( 1))
				key = bioskey( 0);
			  if ( key > 0)
			    break;
			  key = 0;
			  if (buttons == 0)
			    startcheck = TRUE;
		}
	}
	/* user asked to build a compound patch WAD file */
	else if (!strcmp( com, "GROUP") || !strcmp( com, "G"))
	{
		if (WadFileList->next == NULL || WadFileList->next->next == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "You need at least two open PWAD files.");
			return;
		}
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Wad file name argument missing.");
			return;
		}
		for (wad = WadFileList; wad; wad = wad->next)
			if (!stricmp( com, wad->filename))
				break;
		if (wad)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "File \"%s\" is opened and cannot be overwritten.", com);
			return;
		}
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
		BuildNewMainWad( com, TRUE);
		SetColor( DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (startcheck)
			if (buttons == 1)
				break;
			if (bioskey( 1))
			key = bioskey( 0);
			if ( key > 0)
			  break;
			key = 0;
			if (buttons == 0)
			  startcheck = TRUE;
		}
	}
	/* user ask for a listing of a WAD file */
	else if (!strcmp( com, "LIST") || !strcmp( com, "L"))
	{
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Wad file name argument missing.");
			return;
		}
		for (wad = WadFileList; wad; wad = wad->next)
			if (!stricmp( com, wad->filename))
				break;
		if (wad == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Wad file \"%s\" is not open.", com);
			return;
		}
		out = strtok( NULL, " ");
		if (out)
		{
			SetColor(DARKGRAY);
			DrawScreenBox3D( 0, 150, 639, 429);
#ifdef GAME_HEXEN
    	SetColor(LIGHTBLUE);
#else
	    SetColor(DARKBLUE);
#endif
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			DrawScreenText(  10, 160, "Outputting directory of \"%s\" to \"%s\".", wad->filename, out);
			if ((file = fopen( out, "wt")) == NULL)
				ProgError( "error opening output file \"%s\"", com);
			Credits( file);
			ListFileDirectory( file, wad);
			fclose( file);
			SetColor( DARKRED);
			DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
			for (;;)
			{
				GetMouseCoords( &PointerX, &PointerY, &buttons);
				if (startcheck)
					if (buttons == 1)
						break;
				if (bioskey( 1))
					key = bioskey( 0);
				if ( key > 0)
					break;
				key = 0;
				if (buttons == 0)
					startcheck = TRUE;
			}
		}
		else
		{
			TermGfx();
			ListFileDirectory( stdout, wad);
			for (;;)
			{
			  GetMouseCoords( &PointerX, &PointerY, &buttons);
			  if (bioskey( 1))
					key = bioskey( 0);
			  if ( key >= 1 || buttons == 1)
					break;
			}
			GraphCredits();
		}
	}
	/* user asked for the list of the master directory */
	else if (!strcmp( com, "MASTER") || !strcmp( com, "M"))
	{
		out = strtok( NULL, " ");
		if (out) {
			DrawScreenBox3D( 0, 150, 639, 429);
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
#ifdef GAME_HEXEN
    SetColor(LIGHTBLUE);
#else
    SetColor(DARKBLUE);
#endif
			DrawScreenText(  10, 160, "Outputting master directory to \"%s\".", out);
			if ((file = fopen( out, "wt")) == NULL)
				ProgError( "error opening output file \"%s\"", com);
			Credits( file);
			ListMasterDirectory( file);
			fprintf( file, "\nEnd of file.\n");
			fclose( file);
		}
		else
		{
			TermGfx();
			ListMasterDirectory( stdout);
			GraphCredits();
		}
	}
	else if (!strcmp( com, "DIR") || !strcmp( com, "LS"))
	{
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKRED);
		DrawScreenTextFonted(2, 0, 10, 160, "Pwads Present in %s", strlen(Pwads) <= 1 ? strupr(Cwd) : strupr(Pwads));
		SetColor(DARKBLUE);
		wadcount = 0;
		x = 10;
		done = findfirst("*.wad", &info, FA_RDONLY | FA_HIDDEN | FA_ARCH | FA_LABEL);
		while(!done && wadcount <= 100)
		{
			DrawScreenTextFonted(2, 0, x, 180 + (wadcount * 10), info.ff_name);
			DrawScreenTextFonted(2, 0, x + 80, 180 + (wadcount * 10), "%dK", (info.ff_fsize / 1024));
			done = findnext(&info);
			wadcount++;
			if (wadcount == 19)
			{
				x += 130;
				wadcount = 0;
			}
		}
		SetColor(DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (startcheck)
				if (buttons == 1)
					break;
			if (bioskey( 1))
				key = bioskey( 0);
			if ( key > 0)
				break;
			key = 0;
			if (buttons == 0)
				startcheck = TRUE;
		}
		return;
	}
	/* user asked to read a new patch WAD file */
	else if (!strcmp( com, "READ") || !strcmp( com, "R"))
	{
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Wad file name argument missing.");
			return;
		}
		out = strtok( NULL, " ");
		if (out)
			*out = '\0';
		out = (char*)GetMemory( (strlen( com) + 1) * sizeof( char));
		strcpy( out, com);
		OpenPatchWad( out);
		CloseUnusedWadFiles();
	}
	/* user asked to dump the contents of a WAD file */
	else if (!strcmp( com, "DUMP") || !strcmp( com, "D"))
	{
		com = strtok( NULL, " ");
		if (com == NULL) {
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Object name argument missing.");
			return;
		}
		out = strtok( NULL, " ");
		if (out)
		{
			DrawScreenBox3D( 0, 150, 639, 429);
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
#ifdef GAME_HEXEN
    	SetColor(LIGHTBLUE);
#else
	    SetColor(DARKBLUE);
#endif
			DrawScreenText(  10, 160, "Outputting directory entry data to \"%s\".", out);
			if ((file = fopen( out, "wt")) == NULL)
				ProgError( "error opening output file \"%s\"", com);
			Credits( file);
			DumpDirectoryEntry( file, com);
#ifdef GAME_HEXEN
  	  SetColor(LIGHTBLUE);
#else
	    SetColor(DARKBLUE);
#endif
			DrawScreenText(  10, -1, "Complete.", wad->filename, out);
			fclose( file);
		}
		else
		{
		  TermGfx();
		  DumpDirectoryEntry( stdout, com);
		  SetColor( DARKRED);
			DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
			for (;;)
			{
				GetMouseCoords( &PointerX, &PointerY, &buttons);
				if (startcheck)
					if (buttons == 1)
						break;
				if (bioskey( 1))
					key = bioskey( 0);
				if ( key > 0)
					break;
				key = 0;
				if (buttons == 0)
					startcheck = TRUE;
			}
		}
		FrontEnd();
	}
	/* user asked to view the sprites */
	else if (!strcmp( com, "VIEW") || !strcmp( com, "V"))
	{
		com = strtok( NULL, " ");
    DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		HideMousePointer();
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);		
		if (Caching)
		{
			Caching = FALSE;
			ChooseSprite( -1, 180, "Sprite viewer", com);
			Caching = TRUE;
		}
		else
			ChooseSprite( -1, 180, "Sprite viewer", com);
		FrontEnd();
	}
	/* user asked to save an object to a separate PWAD file */
	else if (!strcmp( com, "SAVE") || !strcmp( com, "S"))
	{
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Object name argument missing.");
			return;
		}
		if (strlen( com) > 8 || strchr( com, '.') != NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Invalid object name.");
			return;
		}
		out = strtok( NULL, " ");
		if (out == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Wad file name argument missing.");
			return;
		}
		for (wad = WadFileList; wad; wad = wad->next)
			if (!stricmp( out, wad->filename))
				break;
		if (wad)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "This Wad file is already in use.");
			return;
		}
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
#ifdef GAME_HEXEN
    SetColor(LIGHTBLUE);
#else
    SetColor(DARKBLUE);
#endif
		DrawScreenText(  10, 160, "Saving directory entry data to \"%s\".", out);
		if ((file = fopen( out, "wb")) == NULL)
			ProgError( "error opening output file \"%s\"", out);
		SaveDirectoryEntry( file, com);
		fclose( file);
		SetColor( DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (startcheck)
				if (buttons == 1)
					break;
			if (bioskey( 1))
				key = bioskey( 0);
			if ( key > 0)
				break;
			key = 0;
			if (buttons == 0)
				startcheck = TRUE;
		}
	}
	/* user asked to encapsulate a raw file in a PWAD file */
	else if (!strcmp( com, "INSERT") || !strcmp( com, "I"))
	{
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Raw file name argument missing.");
			return;
		}
		out = strtok( NULL, " ");
		if (out == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Object name argument missing.");
			return;
		}
		if (strlen( out) > 8 || strchr( out, '.') != NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Invalid object name.");
			return;
		}
		if ((raw = fopen( com, "rb")) == NULL)
			ProgError( "error opening input file \"%s\"", com);
		/* kluge */
		strcpy( input, out);
		strcat( input, ".WAD");
		for (wad = WadFileList; wad; wad = wad->next)
			if (!stricmp( input, wad->filename))
				break;
		if (wad)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "This Wad file is already in use.");
			return;
		}
		if (DoingBehavior)
		{
			if ((file = fopen( input, "wb")) == NULL)
				ProgError( "error opening output file \"%s\"", input);
			SaveEntryFromRawFile( file, raw, out);
			fclose( raw);
			fclose( file);
			strcpy(input, "R");
			strcat(input, " BEHAVIOR.WAD");
			ParseInput(input);
			return;
		}
		else
		{
			DrawScreenBox3D( 0, 150, 639, 429);
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
#ifdef GAME_HEXEN
    	SetColor(LIGHTBLUE);
#else
	    SetColor(DARKBLUE);
#endif
			DrawScreenText(  10, 160, "Including new object %s in \"%s\".", out, input);
			if ((file = fopen( input, "wb")) == NULL)
				ProgError( "error opening output file \"%s\"", input);
			SaveEntryFromRawFile( file, raw, out);
			fclose( raw);
			fclose( file);
			SetColor( DARKRED);
			DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
			for (;;)
			{
				GetMouseCoords( &PointerX, &PointerY, &buttons);
				if (startcheck)
					if (buttons == 1)
						break;
				if (bioskey( 1))
					key = bioskey( 0);
				if ( key > 0)
					break;
				key = 0;
				if (buttons == 0)
					startcheck = TRUE;
			}
		}
	}
	/* user asked to extract an object to a raw binary file */
	else if
	(
		!strcmp( com, "XTRACT") ||
		!strcmp( com, "EXTRACT") ||
		!strcmp( com, "X")
	)
	{
		com = strtok( NULL, " ");
		if (com == NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Object name argument missing.");
			return;
		}
		if (strlen( com) > 8 || strchr( com, '.') != NULL)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "Invalid object name.");
			return;
		}
		out = strtok( NULL, " ");
		if (out == NULL) {
			printf( "[Raw file name argument missing.]\n");
			return;
		}
		for (wad = WadFileList; wad; wad = wad->next)
			if (!stricmp( out, wad->filename))
				break;
		if (wad)
		{
			SetColor(DARKGRAY);
			DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
			SetColor(DARKRED);
			DrawScreenText(  130, ScrMaxY - 42, "You may not overwrite an opened Wad file with raw data.");
			return;
		}
		DrawScreenBox3D( 0, 150, 639, 429);
		SetColor(DARKGRAY);
		DrawScreenBox( 130, ScrMaxY - 42, 639, ScrMaxY - 34);
#ifdef GAME_HEXEN
    SetColor(LIGHTBLUE);
#else
    SetColor(DARKBLUE);
#endif
		DrawScreenText(  10, 160, "Saving directory entry data to \"%s\".", out);
		if ((file = fopen( out, "wb")) == NULL)
			ProgError( "error opening output file \"%s\"", out);
		SaveEntryToRawFile( file, com);
		fclose( file);
		SetColor( DARKRED);
		DrawScreenTextFonted(2, 0, 10, 410, "Click Mouse Button or Hit Any Key...");
		for (;;)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			if (startcheck)
				if (buttons == 1)
					break;
			if (bioskey( 1))
				key = bioskey( 0);
			if ( key > 0)
				break;
			key = 0;
			if (buttons == 0)
				startcheck = TRUE;
		}
	}
}
#endif

//
// the main program menu loop, in text
//

void MainLoop2()
{
	char input[120];
  char *com, *out;
  FILE *file, *raw;
  WadPtr wad;
  int wadcount;
	struct ffblk info;
	int done;
	char key;
    
  for (;;)
	{
		/* get the input */
		printf( "\n[? for help]> ");
		gets( input);
		printf( "\n");
		strupr( input);

		/* eat the white space and get the first command word */
		com = strtok( input, " ");
		
		/* user just hit return  */
		if (com == NULL)
			printf( "[Please enter a command.]\n"); 
		
		else if (!strcmp( com, "?"))
		{
			printf( "?                                 -- to display this text\n");
			printf( "B[uild] <WadFile>                 -- to build a new main WAD file\n");
			printf( "C[reate] [level]                  -- to create and edit a new (empty) level\n");
			printf( "D[ump] <DirEntry> [outfile]       -- to dump a directory entry in hex\n");
			printf( "E[dit] [level]                    -- to edit a game level\n");
			printf( "G[roup] <WadFile>                 -- to group all patch wads in a file\n");
			printf( "I[nsert] <RawFile> <DirEntry>     -- to insert a raw file in a patch wad file\n");
			printf( "L[ist] <WadFile> [outfile]        -- to list the directory of a wadfile\n");
			printf( "M[aster] [outfile]                -- to list the master directory\n");
			printf( "Q[uit]                            -- to quit\n");
			printf( "R[ead] <WadFile>                  -- to read a new wad patch file\n");
			printf( "S[ave] <DirEntry> <WadFile>       -- to save one object to a separate file\n");
			printf( "W[ads]                            -- to display the open wads\n");
			printf( "X[tract] <DirEntry> <RawFile>     -- to save (extract) one object to a raw file\n");
			printf( "!                                 -- drop to dos\n");
			printf( "DIR / LS                          -- list wads in current directory\n");
		}
		/* user asked for list of open WAD files */
		else if (!strcmp( com, "WADS") || !strcmp( com, "W"))
		{
			printf( "%-20s  IWAD  (Main wad file)\n", WadFileList->filename);
			for (wad = WadFileList->next; wad; wad = wad->next)
			{
				if (wad->directory[ 0].name[ 0] == 'E' && wad->directory[ 0].name[ 2] == 'M')
					printf( "%-20s  PWAD  (Patch wad file for episode %c level %c)\n", wad->filename, wad->directory[ 0].name[ 1], wad->directory[ 0].name[ 3]);
				else
				{
					/* kluge */
					strncpy( input, wad->directory[ 0].name, 8);
					input[ 8] = '\0';
					printf( "%-20s  PWAD  (Patch wad file for %s)\n", wad->filename, input);
				}
			}
		}
		
		/* user asked to quit */
		else if (!strcmp( com, "QUIT") || !strcmp( com, "Q"))
		{
			SaveConfigFileOptions();
			if (!Registered)
				printf("Remember to register your copy of DOOM!\n");
#ifdef GAME_HEXEN
			printf("Heth: Beyond DEU\n");
#else
			printf("Deth: Beyond DEU\n");
#endif
			break;
		}
		
		
		else if (!strcmp( com, "DIR") || !strcmp( com, "LS"))
		{
			printf("Pwads Present in %s\n", strlen(Pwads) <= 1 ? strupr(Cwd) : strupr(Pwads));
			wadcount = 0;
			done = findfirst("*.wad", &info, FA_RDONLY | FA_HIDDEN | FA_ARCH | FA_LABEL);
			while(!done)
			{
				printf("%-12s  %ldK\n", info.ff_name, info.ff_fsize /1024);
				done = findnext(&info);
				wadcount++;
				if (wadcount == 21)
				{
					key = bioskey( 0);
					if (key == 'Q' || key == 'q')
						break;
					wadcount = 0;
				}
			}
		}

		else if (!strcmp( com, "!"))
		{
			printf("Type Exit to return\n");
			system("");
			Credits(stdout);
		}


		/* user asked to edit a level */
		else if
		(
			!strcmp( com, "EDIT") ||
			!strcmp( com, "E") ||
			!strcmp( com, "CREATE") ||
			!strcmp( com, "C")
		)
		{
			SList tokens, t;
			
			Bool new = (strcmp(com, "EDIT") && strcmp(com, "E"));
			/* if com is neither of these, the user wants
			   to start from scratch */

			if (new)
				CreatingNewLevel = TRUE;
			
			tokens = NULL;
			com = strtok( NULL, " ");
			while(com)
			{
				tokens = SList_append(tokens, com);
				com = strtok(NULL, " ");
			}
			if(tokens)
			{
				MakeLevelName(tokens);
				
				/* now forget the token list */
				while(tokens)
				{
					t = tokens->next;
					FreeMemory(tokens->string);
					FreeMemory(tokens);
					tokens = t;
				}
			}
			
			EditLevel(new);
			Credits(stdout);
		}
		
		/* user asked to build a new main WAD file */
		else if (!strcmp( com, "BUILD") || !strcmp( com, "B"))
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Wad file name argument missing.]\n");
				continue;
			}
			for (wad = WadFileList; wad; wad = wad->next)
				if (!stricmp( com, wad->filename))
					break;
			if (wad)
			{
				printf( "[File \"%s\" is opened and cannot be overwritten.]\n", com);
				continue;
			}
			BuildNewMainWad( com, FALSE);
		}
		
		/* user asked to build a compound patch WAD file */
		else if (!strcmp( com, "GROUP") || !strcmp( com, "G"))
		{
			if (WadFileList->next == NULL || WadFileList->next->next == NULL)
			{
				printf( "[You need at least two open wad files if you want to group them.]\n");
				continue;
			}
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Wad file name argument missing.]\n");
				continue;
			}
			for (wad = WadFileList; wad; wad = wad->next)
				if (!stricmp( com, wad->filename))
					break;
			if (wad)
			{
				printf( "[File \"%s\" is opened and cannot be overwritten.]\n", com);
				continue;
			}
			BuildNewMainWad( com, TRUE);
		}
		
		/* user ask for a listing of a WAD file */
		else if (!strcmp( com, "LIST") || !strcmp( com, "L"))
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Wad file name argument missing.]\n");
				continue;
			}
			for (wad = WadFileList; wad; wad = wad->next)
				if (!stricmp( com, wad->filename))
					break;
			if (wad == NULL)
			{
				printf( "[Wad file \"%s\" is not open.]\n", com);
				continue;
			}
			out = strtok( NULL, " ");
			if (out)
			{
				printf( "Outputting directory of \"%s\" to \"%s\".\n", wad->filename, out);
				if ((file = fopen( out, "wt")) == NULL)
					ProgError( "error opening output file \"%s\"", com);
				Credits( file);
				ListFileDirectory( file, wad);
				fprintf( file, "\nEnd of file.\n");
				fclose( file);
			}
			else
				ListFileDirectory( stdout, wad);
		}
		
		/* user asked for the list of the master directory */
		else if (!strcmp( com, "MASTER") || !strcmp( com, "M"))
		{
			out = strtok( NULL, " ");
			if (out) {
				printf( "Outputting master directory to \"%s\".\n", out);
				if ((file = fopen( out, "wt")) == NULL)
					ProgError( "error opening output file \"%s\"", com);
				Credits( file);
				ListMasterDirectory( file);
				fprintf( file, "\nEnd of file.\n");
				fclose( file);
			}
			else
				ListMasterDirectory( stdout);
		}
		
		/* user asked to read a new patch WAD file */
		else if (!strcmp( com, "READ") || !strcmp( com, "R"))
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Wad file name argument missing.]\n");
				continue;
			}
			out = strtok( NULL, " ");
			if (out)
				*out = '\0';
			out = (char*)GetMemory( (strlen( com) + 1) * sizeof( char));
			strcpy( out, com);
			OpenPatchWad( out);
			CloseUnusedWadFiles();
		}
		
		/* user asked to dump the contents of a WAD file */
		else if (!strcmp( com, "DUMP") || !strcmp( com, "D"))
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Object name argument missing.]\n");
				continue;
			}
			out = strtok( NULL, " ");
			if (out)
			{
				printf( "Outputting directory entry data to \"%s\".\n", out);
				if ((file = fopen( out, "wt")) == NULL)
					ProgError( "error opening output file \"%s\"", com);
				Credits( file);
				DumpDirectoryEntry( file, com);
				fprintf( file, "\nEnd of file.\n");
				fclose( file);
			}
			else
				DumpDirectoryEntry( stdout, com);
		}
		
		/* user asked to save an object to a separate PWAD file */
		else if (!strcmp( com, "SAVE") || !strcmp( com, "S"))
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Object name argument missing.]\n");
				continue;
			}
			if (strlen( com) > 8 || strchr( com, '.') != NULL)
			{
				printf( "[Invalid object name.]\n");
				continue;
			}
			out = strtok( NULL, " ");
			if (out == NULL)
			{
				printf( "[Wad file name argument missing.]\n");
				continue;
			}
			for (wad = WadFileList; wad; wad = wad->next)
				if (!stricmp( out, wad->filename))
					break;
			if (wad)
			{
				printf( "[This Wad file is already in use.  You may not overwrite it.]\n");
				continue;
			}
			printf( "Saving directory entry data to \"%s\".\n", out);
			if ((file = fopen( out, "wb")) == NULL)
				ProgError( "error opening output file \"%s\"", out);
			SaveDirectoryEntry( file, com);
			fclose( file);
		}
		
		/* user asked to encapsulate a raw file in a PWAD file */
		else if (!strcmp( com, "INSERT") || !strcmp( com, "I"))
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Raw file name argument missing.]\n");
				continue;
			}
			out = strtok( NULL, " ");
			if (out == NULL)
			{
				printf( "[Object name argument missing.]\n");
				continue;
			}
			if (strlen( out) > 8 || strchr( out, '.') != NULL)
			{
				printf( "[Invalid object name.]\n");
				continue;
			}
			if ((raw = fopen( com, "rb")) == NULL)
				ProgError( "error opening input file \"%s\"", com);
			/* kluge */
			strcpy( input, out);
			strcat( input, ".WAD");
			for (wad = WadFileList; wad; wad = wad->next)
				if (!stricmp( input, wad->filename))
					break;
			if (wad)
			{
				printf( "[This Wad file is already in use (%s).  You may not overwrite it.]\n", input);
				continue;
			}
			printf( "Including new object %s in \"%s\".\n", out, input);
			if ((file = fopen( input, "wb")) == NULL)
				ProgError( "error opening output file \"%s\"", input);
			SaveEntryFromRawFile( file, raw, out);
			fclose( raw);
			fclose( file);
		}
		
		/* user asked to extract an object to a raw binary file */
		else if
		(
			!strcmp( com, "XTRACT") ||
			!strcmp( com, "EXTRACT") ||
			!strcmp( com, "X")
		)
		{
			com = strtok( NULL, " ");
			if (com == NULL)
			{
				printf( "[Object name argument missing.]\n");
				continue;
			}
			if (strlen( com) > 8 || strchr( com, '.') != NULL)
			{
				printf( "[Invalid object name.]\n");
				continue;
			}
			out = strtok( NULL, " ");
			if (out == NULL)
			{
				printf( "[Raw file name argument missing.]\n");
				continue;
			}
			for (wad = WadFileList; wad; wad = wad->next)
				if (!stricmp( out, wad->filename))
					break;
			if (wad)
			{
				printf( "[You may not overwrite an opened Wad file with raw data.]\n");
				continue;
			}
			printf( "Saving directory entry data to \"%s\".\n", out);
			if ((file = fopen( out, "wb")) == NULL)
				ProgError( "error opening output file \"%s\"", out);
			SaveEntryToRawFile( file, com);
			fclose( file);
		}
		
		/* unknown command */
		else 
			printf( "[Unknown command \"%s\"!]\n", com);  
  }
}

/* end of file */
