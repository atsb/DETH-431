/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   DEU.H - Main game defines.
   */

/* the includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <conio.h>
#include <ctype.h>
#include <unistd.h>
#include <bios.h>
#include <grx20.h>

#include "deu-go32.h"

/*
    Uncomment/comment one of the following lines to compile either DETH or HETH
    */

/*
#define GAME_HEXEN	"Hexen"
*/
#define GAME_DOOM	"Doom"
/*
#define SLIM		"Slim"
*/

#define DEU_VERSION		"5.21"
#define DETH_VERSION	"4.24"

typedef short int			BCINT;
typedef unsigned short int	UBCINT;
typedef unsigned char		BYTE;


/* define some new colors */
#define DARKBLUE		16
#define DARKGREEN		17
#define DARKRED			18
#define DARKMAGENTA		19
#define GRAY			20
#define DARKERGRAY		21
#define ORANGE			22
#define SECTORTAGGED	23
#define SECTORSECRET	24
#define SECTORPAINFUL	25
#define SECTORLIGHT		26
#define LINEDEFTAGGED	27
#define LINEDEFSECRET	28
#define LINEDEFNOSOUND 	29
#define LINEDEFNOPASS	30
#define LINEDEFNOMAP	31

#define MENUARRAYMAX 40

#ifdef GAME_HEXEN
#include "struct_h.h"
#else
#include "struct_d.h"
#endif

/* constants for filtering out things based on difficulty */

#ifdef GAME_HEXEN
#define TF_EASY			1
#define TF_MEDIUM		2
#define TF_HARD			4
#define TF_FIGHTER		32
#define TF_CLERIC   	64
#define TF_MAGE   		128
#define TF_SINGLE   	256
#define TF_COOP   		512
#define TF_DEATHM   	1024
#else
#define TF_EASY			1
#define TF_MEDIUM		2
#define TF_HARD			4
#define TF_DM			16
#define TF_NOT_DM   	32
#endif

/* constants for telling what we have to Undo */
#define BTHINGS        	1
#define BLINEDEFS      	2
#define BSIDEDEFS      	4
#define BVERTEXES      	8
#define BSECTORS       	16


/*
   the directory structure is the structure used by DOOM to order the
   data in it's WAD files
   */

typedef struct Directory huge *DirPtr;
struct Directory
{
    long start;			/* offset to start of data */
    long size;			/* byte size of data */
    char name[ 8];		/* name of data block */
};



/*
   The wad file pointer structure is used for holding the information
   on the wad files in a linked list.
   
   The first wad file is the main wad file. The rest are patches.
   */

typedef struct WadFileInfo huge *WadPtr;
struct WadFileInfo
{
    WadPtr next;		/* next file in linked list */
    char *filename;		/* name of the wad file */
    FILE *fileinfo;		/* C file stream information */
    char type[ 4];		/* type of wad file (IWAD or PWAD) */
    long dirsize;		/* directory size of WAD */
    long dirstart;		/* offset to start of directory */
    DirPtr directory;	/* array of directory information */
};



/*
   the master directory structure is used to build a complete directory
   of all the data blocks from all the various wad files
   */

typedef struct MasterDirectory huge *MDirPtr;
struct MasterDirectory
{
    MDirPtr next;			/* next in list */
    WadPtr wadfile;			/* file of origin */
    struct Directory dir;	/* directory data */
};



/*
   the selection list is used when more than one object is selected
   */

typedef struct SelectionList *SelPtr;
struct SelectionList
{
    SelPtr next;		/* next in list */
    BCINT objnum;		/* object number */
};


/*
   syntactic sugar
   */
typedef BCINT Bool;		/* Boolean data: true or false */


/*
   description of the command line arguments and config file keywords
   */

typedef struct
{
    char *short_name;		/* abbreviated command line argument */
    char *long_name;		/* command line arg. or keyword */
    enum {                  /* type of this option */
	OPT_BOOLEAN,			/* boolean (toggle) */
	OPT_INTEGER,			/* integer number */
	OPT_STRING,				/* character string */
	OPT_STRINGACC,			/* character string, but store in a list */
	OPT_STRINGLIST,         /* list of character strings */
	OPT_END                 /* end of the options description */
    } opt_type;                    
    char *msg_if_true;		/* message printed if option is true */
    char *msg_if_false;		/* message printed if option is false */
    void *data_ptr;         /* pointer to the data */
} OptDesc;


/* generic string list type  -- used for holding level names,
   ftexture sections, texture sections */
	typedef struct _SList {
	char *string;
	struct _SList *next;
} *SList;

/* sector type type (!) (actually a list type) */
   typedef struct _sector_type {
	char *shortname, *longname;
	BCINT type;
	struct _sector_type *next;
} sector_type;

typedef struct _sector_class {
	char *name;
	sector_type *types;
	struct _sector_class *next;
} sector_class;


/* type for a list of linedef types */
#ifdef GAME_HEXEN
	typedef struct _ld_type {
		char *shortname, *longname, *args, *arg1text, *arg2text, *arg3text, *arg4text, *arg5text;
		BCINT type;
		struct _ld_type *next;
	} ld_type;
#else
	typedef struct _ld_type {
		char *shortname, *longname;
		BCINT type;
		struct _ld_type *next;
	} ld_type;
#endif

/* type for the list of linedef type classes (phew!) */
typedef struct _ld_class {
	char *name;
	ld_type *types;
	struct _ld_class *next;
} ld_class;

/* as above, but for a thing */
typedef struct _thing_type {
    BCINT	type, colour, radius;
    char	*sprite;
    char 	*name;
    struct _thing_type *next;
} thing_type;

/* and a list of thing classes */
typedef struct _thing_class {
	char *name;
	thing_type *types;
	struct _thing_class *next;
} thing_class;

#ifdef GAME_HEXEN
	/* as above, but for a spawnable thing */
	typedef struct _spawn_type {
		BCINT	type;
		char	*sprite;
		char 	*name;
		struct _spawn_type *next;
	} spawn_type;

	/* and a list of spawnable thing classes */
	typedef struct _spawn_class {
		char *name;
		spawn_type *types;
		struct _spawn_class *next;
	} spawn_class;
#endif

/* as above, but for a scheme */
typedef struct _scheme_type {
    BCINT	type;
    char 	*name;
    char	*normal;
    char	*upper;
    char	*lower;
    char	*floor;
    char	*ceiling;
    struct _scheme_type *next;
} scheme_type;

/* and a list of scheme classes */
typedef struct _scheme_class {
	char *name;
	scheme_type *types;
	struct _scheme_class *next;
} scheme_class;



/*
   the macros and constants
   */

/* name of the configuration file */
/* name of the log file (debug mode) */
#ifdef GAME_HEXEN
	#define DEU_CONFIG_FILE		"HETH.INI"
	#define DEU_LOG_FILE		"HETH.LOG"
#else
	#define DEU_CONFIG_FILE		"DETH.INI"
	#define DEU_LOG_FILE		"DETH.LOG"
#endif


/* convert screen coordinates to map coordinates */
#define MAPX(x)			(OrigX + (int)(((x) - ScrCenterX) / Scale))
#define MAPY(y)			(OrigY + (int)((ScrCenterY - (y)) / Scale))

/* convert map coordinates to screen coordinates */
#define SCREENX(x)		(ScrCenterX + (int) (((x) - OrigX) * Scale))
#define SCREENY(y)		(ScrCenterY + (int) ((OrigY - (y)) * Scale))

#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))

/* object types */
#define OBJ_THINGS		1
#define OBJ_LINEDEFS	2
#define OBJ_SIDEDEFS	3
#define OBJ_VERTEXES	4
#define OBJ_SEGS		5
#define OBJ_SSECTORS	6
#define OBJ_NODES		7
#define OBJ_SECTORS		8
#define OBJ_REJECT		9
#define OBJ_BLOCKMAP	10

/* boolean constants */
#ifndef TRUE
#define TRUE			1
#define FALSE			0
#endif

/* half the size of an object (Thing or Vertex) in map coords */
#define OBJSIZE			7


/* the interfile global variables */

/* from deu.c */
extern FILE *logst;				/* filepointer to the error log */
extern SList LevelNameFormat;	/* format of level name structure */
extern BCINT InitialScale;		/* initial zoom factor for map */
extern BCINT TempCloseToLine;   /* temporary variable */	
extern BCINT CloseToLine;		/* how close to a line does a vertex have to be to merge with line */	
extern BCINT VideoMode;	  		/* default video mode for VESA cards */
extern BCINT RealVideoMode;	  	/* the actual, chosen default video mode for VESA cards, this is used with graphical front-end */
extern BCINT Increment;			/* increment for changing sectors with hot keys */
extern BCINT InitGrid;			/* current grid setting, used for starting deth as we left it */
extern BCINT GridScale;			/* current grid setting */
extern BCINT HideGrid;			/* is the grid visible ? */
extern BCINT BarStyle;			/* button-bar style */
extern BCINT Gamma;				/* gamma level 0-4 */
extern BCINT StretchDelay;		/* how long to hold left mouse down until stretch box started */
extern BCINT Sensitivity;		/* ? */
extern BCINT MoveSpeed;			/* ? */
extern BCINT DefaultLargeScroll;/* amount of pixels to scroll when mouse moves to edge of screen */
extern BCINT DethFont;			/* external font */
extern BCINT MenuDefault;		/* last item used within a menu, used for menus that return i.e thing flags */
extern BCINT NumPatchWads;		/* number of patch wads open */
extern BCINT NumConfigPatchWads;
extern BCINT EdMode;			/* current editing mode - used for starting deth in last mode edited */
extern Bool ChooseGame;			/* should deth ask which game to edit ? */
extern Bool GraphFront;			/* should we use a graphical style front-end ? */
extern Bool CirrusCursor;      	/* use hardware cursor on Cirrus Logic VGA cards */
extern Bool Select0;           	/* select object 0 by default when switching modes */
extern Bool Debug;				/* are we debugging? */
extern Bool SwapButtons;		/* swap right and middle mouse buttons */
extern Bool Quiet;				/* don't play a sound when an object is selected */
extern Bool Quieter;			/* don't play any sound, even when an error occurs */
extern Bool Expert;				/* don't ask for confirmation for some operations */
extern Bool QisQuit;			/* Quit on Key 'Q' or Save on 'Q' */	
extern Bool VertConf;			/* don't ask for confirmation on Vertices merging even if in expert mode */
extern Bool FakeCursor;			/* use a "fake" mouse cursor */
extern Bool AdditiveSelBox;		/* additive selection box or select in box only? */
extern Bool ThingAngle;			/* draw things with an arrow ? */
extern Bool UsePromptOnly;		/* should we start the command at the prompt ? */
extern Bool DispSprite;   		/* display pretty pictures ? */ 
extern Bool AltInfo;   			/* show an alternative information bar ? */ 
extern Bool Registered;			/* registered or shareware WAD file? */
extern Bool DoingBehavior;		/* temp variable */
extern Bool KeepInfo;			/* should we keep the MAPINFO and SNDINFO lumps */
extern Bool CloseOnUse;			/* close the button-bar upon it's use ? */
extern Bool ShowDistance;		/* show the linedefs lengths in vertex mode ? */
extern Bool NestMenus;			/* draw the pulldown menus from the top or center */
extern Bool AutoMap;			/* draw linedef mode in a doom style automap */
extern Bool ShowThings;			/* draw the things with sprites ? */
extern Bool GridDashed;			/* draw the grid with dashed lines ? */
extern Bool SaveIniFile;		/* save the settings to the ini file when you exit deth ? */
extern Bool CopySideDefs;		/* copy sidedef info when copying linedefs ? */
extern Bool CreatingNewLevel;	
extern Bool BOOMEnable;			/* allow BOOM feature support */
extern Bool SpecialEffects;	/* turn off some error checks */
extern char *BackupText;		/* text to show on alternative info-bar */
extern char LevelName[];		/* what level we are editing ? */
extern char *MainWad;			/* name of the main wad file */
extern char *Doom;				/* path to the Doom config file */
extern char *Doom2;				/* path to the Doom2 config file */
extern char *Heretic;			/* path to the Heretic config file */
extern char *Hexen;				/* path to the Hexen config file */
extern char *Strife;			/* path to the Strife config file */
extern char *Game;				/* default game to edit */
extern char *BGIDriver;			/* default extended BGI driver */
extern char *RegTest;			/* level present in registerd version */
extern char *CurrentScheme;		/* current loaded prefab */
extern char *DoomPwads;			/* path to DOOM pwads  */
extern char *Doom2Pwads;		/* path to DOOM2 pwads  */
extern char *HereticPwads;		/* path to HERETIC pwads  */
extern char *StrifePwads;		/* path to STRIFE pwads  */
extern char *HexenPwads;		/* path to HEXEN pwads  */
extern char *Pwads;				/* path to pwads  */
extern char *Cwd;				/* current working directoy */
extern int MaxExtendedType;	/* largest linedef type in COMMON.CFG */
extern char **PatchWads;		/* list of patch wads to load */
extern char **ConfigPatchWads;		/* list of patch wads to load */
extern char *CfgFile;			/* alternative config file */
extern int GotImages[];
extern unsigned char *WallImages[]; /*JFF change for fix in DEU.C */

/* from wads.c */
extern WadPtr  WadFileList;		/* list of wad files */
extern MDirPtr MasterDir;		/* the master directory */
extern SList LevelNames;
extern int isalev(char *);
extern int isamark(char *mrk);

/* from edit.c */
extern BCINT LastSectorTag;		/* last sector tag mouse passed over */
extern BCINT LastLineDefTag;	/* last lindef tag mouse passed over */
extern BCINT MinGrid;			/* minimum grid setting */
extern BCINT MaxGrid;			/* maximum grid setting */
extern Bool DragObject;			/* are we dragging ? */
extern Bool ShowSprite;			/* do we show graphical info boxes ? */
extern Bool InfoShown;      	/* is the bottom line displayed? */
extern Bool BrowseMap;      	/* should we browse the map? */
extern Bool Caching;      		/* are we cahching texture information ? */
extern int tff;					/* what things to show, based on difficulty level */


/* from gfx.c */
//extern BCINT GfxMode;			/* current graphics mode, or 0 for text */
//extern float Scale;				/* scale to draw map 20 to 1 */
//extern BCINT OrigX;				/* the X origin */
//extern BCINT OrigY;				/* the Y origin */
extern BCINT PointerX;			/* X position of pointer */
extern BCINT PointerY;			/* Y position of pointer */
//extern BCINT ScrMaxX;			/* maximum X screen coord */
//extern BCINT ScrMaxY;			/* maximum Y screen coord */
//extern BCINT ScrCenterX;		/* X coord of screen center */
//extern BCINT ScrCenterY;		/* Y coord of screen center */
extern BCINT GfxMode;			/* current graphics mode, or 0 for text */
extern float Scale;				/* scale to draw map 20 to 1 */
extern int OrigX;				/* the X origin */
extern int OrigY;				/* the Y origin */
//extern int PointerX;			/* X position of pointer */
//extern int PointerY;			/* Y position of pointer */
extern int ScrMaxX;			/* maximum X screen coord */
extern int ScrMaxY;			/* maximum Y screen coord */
extern int ScrCenterX;		/* X coord of screen center */
extern int ScrCenterY;		/* Y coord of screen center */

/* from mouse.c */
extern Bool UseMouse;			/* is there a mouse driver? */
Bool MouseClickedArea(BCINT, BCINT, BCINT, BCINT);

/* from things.c */
extern thing_class *Thing_classes;
extern scheme_class *Scheme_classes;
#ifdef GAME_HEXEN
extern spawn_class *Spawn_classes;
#endif

/* from names.c */
extern ld_class *Linedef_classes;
extern sector_class *Sector_classes;

/* from levels.c */
extern SList Ftexture_sections;
extern SList CMap_sections;
extern SList Texture_sections;

/*   the function prototypes    */

/* from deu.c */
int main( int, char *[]);
void ParseCommandLineOptions( int, char *[]);
void ParseConfigFileOptions( char *);
void Usage( FILE *);
void Credits( FILE *);
void FunnyMessage( FILE *);
void Beep( void);
void PlaySound( BCINT , BCINT );
void ProgError( char *, ...);
void LogMessage( char *, ...);
void MainLoop( void);
void MainLoop2( void);
void FrontEnd( void);
void ParseInput( char *);

/* from memory.c */
void *GetMemory( size_t);
void *ResizeMemory( void *, size_t);
void FreeMemory( void *);
void huge *GetFarMemory( unsigned long size);
void huge *ResizeFarMemory( void huge *old, unsigned long size);
void FreeFarMemory( void huge *);

/* from wads.c */
int isalev(char *);
void OpenMainWad( char *);
void OpenPatchWad( char *);
void CloseWadFiles( void);
void CloseUnusedWadFiles( void);
WadPtr BasicWadOpen( char *);
void BasicWadRead( WadPtr, void huge *, long);
void BasicWadSeek( WadPtr, long);
MDirPtr FindMasterDir( MDirPtr, char *);
void ListMasterDirectory( FILE *);
void ListFileDirectory( FILE *, WadPtr);
void BuildNewMainWad( char *, Bool);
void WriteBytes( FILE *, void huge *, long);
void CopyBytes( FILE *, FILE *, long);
BCINT  Exists( char *);
void DumpDirectoryEntry( FILE *, char *);
void SaveDirectoryEntry( FILE *, char *);
void SaveEntryToRawFile( FILE *, char *);
void SaveEntryFromRawFile( FILE *, FILE *, char *);

/* from levels.c */
void ReadLevelData();
void ForgetLevelData( void);
void SaveLevelData( char *); 
void ReadWTextureNames( void);
void ForgetWTextureNames( void);
void ReadFTextureNames( void);
void ReadFTextureNamesIn( char *);
void ForgetFTextureNames( void);
void ReadCMapNames( void);
void ReadCMapNamesIn( char *);
void ForgetCMapNames( void);

/* from edit.c */
void MakeLevelName(SList);
void EditLevel(Bool);
void SelectLevel();
void EditorLoop();
void DrawMap( BCINT , BCINT, Bool ); 
void CenterMapAroundCoords( BCINT , BCINT );
void GoToObject( BCINT , BCINT ); 
void FindThing(int, SelPtr *);
void SaveAs(int);
void CopyFile(const char *, const char *);
void Backup(int);
void Undo();
extern BCINT Count;	


/* from gfx.c */
void InitGfx( void);
Bool SwitchToVGA256( void);
Bool SwitchToVGA16( void);
void TermGfx( void);
void ClearScreen( void);
void ClearMapScreen(BCINT, BCINT);
void SetColor( BCINT );
void DrawMapLine( BCINT , BCINT , BCINT , BCINT );
void DrawMapCircle( BCINT , BCINT , BCINT );
void DrawMapVector( BCINT , BCINT , BCINT , BCINT );
void DrawMapArrow( BCINT , BCINT , UBCINT);
void DrawScreenLine( BCINT , BCINT , BCINT , BCINT );
void /*JFF temp*/DrawThickScreenLine( BCINT , BCINT , BCINT , BCINT );
void ScreenBoxArea(int ,int ,int ,int,int );
void ScreenHighlightArea(int ,int ,int ,int, char *,int, int );
void DrawScreenWindow(int ,int ,int ,int, char *, int);
void DrawScreenBox( BCINT , BCINT , BCINT , BCINT );
void DrawScreenBox3D( BCINT , BCINT , BCINT , BCINT );
void DrawScreenBoxHollow( BCINT , BCINT , BCINT , BCINT );
void DrawScreenFrame( BCINT , BCINT , BCINT , BCINT );
void DrawScreenButton( BCINT , BCINT , BCINT , BCINT, char *, Bool );
void DrawScreenButtonIn( BCINT , BCINT , BCINT , BCINT, char *, Bool );
void DrawScreenButtonOut(BCINT, BCINT, BCINT, BCINT, Bool);
void DrawScreenButtonGray(BCINT, BCINT, BCINT, BCINT, char *, Bool);
void DrawScreenMeter( BCINT , BCINT , BCINT , BCINT , float);
void DrawScreenText( BCINT , BCINT , char *, ...);
void DrawScreenTextFonted(BCINT, BCINT, BCINT , BCINT , char *, ...);
void DrawScreen3DText( BCINT , BCINT , BCINT, Bool, char *, ...);
void DrawScreenBar( BCINT , BCINT , BCINT, Bool);
void DrawPointer();
void SetDoomPalette( BCINT );
BCINT TranslateToGameColor( BCINT );
UBCINT ComputeAngle( BCINT , BCINT );
UBCINT ComputeDist( BCINT , BCINT );
void InsertPolygonVertices( BCINT , BCINT , BCINT , BCINT );
//jff change integer io variables to double for better accuracy
void RotateAndScaleCoords( double *, double *, double, double);

/* from things.c */
int BCINT_lessp(const void *, const void *);
void index_things(void);
void index_schemes(void);
BCINT  GetThingColour( BCINT );
char *GetThingFlag( BCINT, BCINT );
char *GetTaggedLineDefFlag( BCINT, BCINT );
char *GetThingName( BCINT );
char *GetSpawnName( BCINT );
char *GetThingSprite( BCINT );
BCINT  GetThingRadius( BCINT );
char *GetAngleName( BCINT );
char *GetWhenName( BCINT );
char *GetUnknown3Name( BCINT );
int SetThingFlagFilter(int);

/* from names.c */
char *GetObjectTypeName( BCINT );
char *GetEditModeName( BCINT );
char *GetLineDefTypeName( BCINT );
char *GetLineDefArgs( BCINT );
char *GetArg1Text( BCINT );
char *GetArg2Text( BCINT );
char *GetArg3Text( BCINT );
char *GetArg4Text( BCINT );
char *GetArg5Text( BCINT );
char *GetLineDefTypeLongName( BCINT );
char *GetLineDefFlagsName( BCINT );
char *GetThingFlagsName( BCINT );
char *GetThingFlagsLongName( BCINT );
char *GetSectorTypeName( BCINT );
char *GetSectorTypeLongName( BCINT );
char *GetPuzzleItemName( BCINT );
char *GetKeyTypeName( BCINT );
char *GetSectorSoundName( BCINT );
char *GetHexenAngleName( BCINT );
char *GetNegativeName( BCINT );
void index_ld_types();

/* from mouse.c */
void CheckMouseDriver( void);
void ShowMousePointer( void);
void HideMousePointer( void);
void GetMouseCoords( BCINT  *, BCINT  *, BCINT  *);
void SetMouseCoords( BCINT , BCINT );
void SetMouseLimits( BCINT , BCINT , BCINT , BCINT );
void ResetMouseLimits( void);

/* from menus.c */
BCINT DisplayMenuArray( BCINT , BCINT , char *, BCINT , BCINT  *, char *[ 30], BCINT  [30]);
BCINT DisplayMenu( BCINT , BCINT , char *, ...);
BCINT PullDownMenu( BCINT , BCINT , ...);
BCINT InputInteger( BCINT , BCINT , BCINT  *, BCINT , BCINT );
Bool Input2Numbers( BCINT , BCINT , char *, char *, BCINT,  BCINT, BCINT * , BCINT * );
BCINT InputIntegerValue( BCINT , BCINT , BCINT , BCINT , BCINT );
BCINT InputIntegerValuePlain( BCINT , BCINT , BCINT , BCINT , BCINT );
void InputNameFromListWithFunc( BCINT , BCINT , char *, BCINT , char **, BCINT , char *, BCINT , BCINT , void (*hookfunc)(BCINT , BCINT , BCINT , BCINT , char *));
BCINT BrowseWalls(char *);
BCINT BrowseFlats(char *);
	 
void InputNameFromList( BCINT , BCINT , char *, BCINT , char **, char *);
void InputFileName( BCINT , BCINT , char *, BCINT , char *);
void InputCommand( BCINT , BCINT , char *, BCINT , char *);
Bool Confirm( BCINT , BCINT , char *, char *);
void Notify( BCINT , BCINT , char *, char *);
void DisplayMessage( BCINT , BCINT , char *, ...);
void NotImplemented( void);

/* from objects.c */
BCINT GetMaxObjectNum(BCINT );
void HighlightSelection( BCINT , SelPtr);
Bool IsSelected( SelPtr, BCINT );
SelPtr SelectPointer( SelPtr list, BCINT objnum);
void SelectObject( SelPtr *, BCINT );
void UnSelectObject( SelPtr *, BCINT );
void ForgetSelection( SelPtr *);
BCINT GetMaxObjectNum( BCINT);
BCINT GetCurObject( BCINT , BCINT , BCINT , BCINT , BCINT ); 
Bool CurObjectUnderMouse( BCINT , BCINT , BCINT , BCINT , BCINT, BCINT ); 
SelPtr SelectObjectsInBox( BCINT , BCINT , BCINT , BCINT , BCINT ); 
void HighlightObject( BCINT , BCINT , BCINT ); 
void DeleteObject( BCINT , BCINT ); 
void DeleteObjects( BCINT , SelPtr *); 
void InsertObject( BCINT , BCINT , BCINT , BCINT ); 
BCINT GetOppositeSector( BCINT, Bool); /* SWAP ! */
Bool IsLineDefInside( BCINT , BCINT , BCINT , BCINT , BCINT ); /* SWAP - needs Vertexes & LineDefs */
void CopyObjects( BCINT , SelPtr); 
Bool MoveObjectsToCoords( BCINT , SelPtr, BCINT , BCINT , BCINT ); 
void GetObjectCoords( BCINT , BCINT , BCINT  *, BCINT  *); 
void RotateAndScaleObjects( BCINT , SelPtr, double, double, int); 
BCINT FindFreeTag(void); 
#ifdef GAME_HEXEN
BCINT FindFreeTid(void); 
#endif
BCINT FindFreeSectorTag(void); 
BCINT FindFreeLineDefTag(void); 
void FlipLineDefs( SelPtr, Bool); 
void DeleteVerticesJoinLineDefs( SelPtr ); 
void MergeVertices( SelPtr *); 
Bool AutoMergeVertices( SelPtr *); 
void SplitLineDefs( SelPtr); 
void SplitSector( BCINT , BCINT ); 
void SplitLineDefsAndSector( BCINT , BCINT ); 
void ConnectLineDefsSplitDonut( BCINT linedef1, BCINT linedef2); 
void ConnectLineDefsMakeHallway( BCINT linedef1, BCINT linedef2);
void MergeSectors( SelPtr *); 
void DeleteLineDefsJoinSectors( SelPtr *); 
void MakeDoorFromSector( BCINT ); 
void MakeLiftFromSector( BCINT ); 
void AlignTexturesY( SelPtr *, Bool, Bool); 
void AlignTexturesX( SelPtr *, Bool, Bool); 
void DistributeSectorFloors( SelPtr); 
BCINT CommonVertex(BCINT, BCINT);
SelPtr rev_list(SelPtr);	/* return a new list, which is the reverse
							   of the original */
void delete_list(SelPtr);	/* use this to delete it afterwards */
void DistributeSectorCeilings( SelPtr); 
void DistributeLightLevels( SelPtr); 


/* from editobj.c */
void DisplayObjectInfo( BCINT , BCINT ); 
BCINT InputObjectNumber( BCINT , BCINT , BCINT , BCINT );
BCINT InputObjectXRef( BCINT , BCINT , BCINT , Bool, BCINT );
Bool Input2VertexNumbers( BCINT, BCINT, char *, BCINT *, BCINT *);
Bool Input2NegativeNumbers( BCINT, BCINT, char *, char *, BCINT, BCINT, BCINT *, BCINT *);
void EditObjectsInfo( BCINT , BCINT , BCINT , SelPtr);
BCINT CheckLevel( BCINT , BCINT ); 
void CheckStartingPos( void); 
Bool CheckThingHeights( void);
BCINT InsertStandardObject( BCINT , BCINT , BCINT , BCINT ); 
BCINT MiscOperations( BCINT , BCINT , BCINT , SelPtr *, BCINT); 
void Preferences( BCINT , BCINT );
BCINT SelectThingType(void);
thing_class *SelectThingClass(void);
scheme_class *SelectSchemeClass(void);
Bool Failed;
Bool CheckReport; //jff
char *GetLineDefFlagsLongName(BCINT);
#ifdef GAME_HEXEN
BCINT SelectSpawnType(void);
BCINT SelectKeyType(void);
BCINT SelectSectorSound(void);
BCINT SelectPuzzleItem(void);
BCINT SelectNegative(void);
BCINT SelectHexenAngle(void);
spawn_class *SelectSpawnClass(void);
#endif

/* from draw.c */
extern Bool GridShown;


/* from textures.c */
void ChooseFloorTexture( BCINT , BCINT , char *, BCINT , char **, char *);
void ChooseWallTexture( BCINT , BCINT , char *, BCINT , char **, char *);
void ChooseSprite( BCINT , BCINT , char *, char *);
void GetWallTextureSize( BCINT *, BCINT *, char *);
void *GetResource(char *);
void ForgetResource(char *);
Texture *FindTexture(char *);
Texture *FindTextureIn(char *, void *);
void ForgetAllResources(void);
void DrawMapLineOff(BCINT, BCINT, BCINT, BCINT, BCINT);
void GetPicSize(char *, BCINT *, BCINT *);
void DisplayFloorTexture(BCINT, BCINT, BCINT, BCINT, char *);
void DisplayPic(BCINT, BCINT, BCINT, BCINT, char *, Bool);
void DisplayPicScaled(BCINT, BCINT, BCINT, BCINT, char *, BCINT, Bool);
void DisplaySprite( BCINT, BCINT, BCINT, BCINT, char *);
void DisplayWallQuick(BCINT, BCINT, BCINT, BCINT, char *, BCINT);

/* from readcfg.c */
void readcfg(char *);
SList SList_append(SList, char *);
BCINT SelectScheme(void);


/* from mainloop.c */
char  *GetWadFileName();
void  SaveConfigFileOptions();
void  DrawMapQuick(BCINT);
void  Options(BCINT, BCINT);
void  SetMouseColors(BCINT, BCINT);
void  DrawInfoBar(BCINT, BCINT);

Bool  MouseInArea(BCINT, BCINT, BCINT, BCINT);

BCINT FunctionSectorLightLevel(BCINT, SelPtr);
BCINT FunctionSectorCeilingHeight(BCINT, SelPtr);
BCINT FunctionSectorFloorHeight(BCINT, SelPtr);
BCINT FunctionSectorType(BCINT, SelPtr);
BCINT FunctionSectorBrowseFloor(BCINT, SelPtr);
BCINT FunctionSectorBrowseCeiling(BCINT, SelPtr);
BCINT FunctionSectorRef(BCINT, SelPtr);
BCINT FunctionSectorTag(BCINT, SelPtr);
BCINT FunctionRenumberSectorsLowest(BCINT CurObject, SelPtr Selected);

BCINT FunctionVertexSplitSector(BCINT, SelPtr);
BCINT FunctionVertexMerge(BCINT, SelPtr);
BCINT FunctionVertexSplitLineDef(BCINT, SelPtr);
BCINT FunctionVertexDeleteJoin(BCINT, SelPtr);

BCINT FunctionThingFlag(BCINT, SelPtr, BCINT);
BCINT FunctionThingFlags(BCINT, SelPtr);
BCINT FunctionThingType(BCINT, SelPtr);
BCINT FunctionThingSpecial(BCINT, SelPtr);
BCINT FunctionThingAngle(BCINT, SelPtr);
BCINT FunctionThingXPos(BCINT, SelPtr);
BCINT FunctionThingYPos(BCINT, SelPtr);

BCINT FunctionLineDefFlag(BCINT, SelPtr, BCINT);
BCINT FunctionLineDefFlags(BCINT, SelPtr);
BCINT FunctionLineDefSplitAdd(BCINT, SelPtr);
BCINT FunctionConnectLineDefsSplitDonut(BCINT CurObject, SelPtr Selected);
BCINT FunctionConnectLineDefsMakeHallway(BCINT CurObject, SelPtr Selected);
BCINT FunctionRenumberLineDefsLowest(BCINT CurObject, SelPtr Selected);
BCINT FunctionLineDefSwapSideDefs(BCINT, SelPtr);
BCINT FunctionLineDefSpecial(BCINT, SelPtr);
BCINT FunctionLineDefDeleteJoin(BCINT, SelPtr);
BCINT FunctionLineDefFlipSideDefs(BCINT, SelPtr);
BCINT FunctionLineDefBrosweWallTextures(BCINT, SelPtr);
BCINT FunctionLineDefTag(BCINT, SelPtr);
BCINT FunctionLineDefWallTexture(BCINT, SelPtr, char *, BCINT, BCINT);
BCINT FunctionLineDefWallTextures(BCINT, SelPtr, BCINT, BCINT);
BCINT FunctionLineDefBrowseWallTextures(BCINT, SelPtr, BCINT, BCINT);
BCINT FunctionLineDefXoffset(BCINT, SelPtr, BCINT);
BCINT FunctionLineDefYoffset(BCINT, SelPtr, BCINT);
BCINT FunctionLineDefSectorRef(BCINT, SelPtr, BCINT);

BCINT OnScreenEditingSideDefTextures(BCINT, SelPtr);
BCINT OnScreenEditingSideDef1(BCINT, SelPtr);
BCINT OnScreenEditingSideDef2(BCINT, SelPtr);
BCINT OnScreenEditingSectors(BCINT, SelPtr);
BCINT OnScreenEditingSectorFlats(BCINT, SelPtr);
BCINT OnScreenEditingLineDefs(BCINT, SelPtr);
BCINT OnScreenEditingThings(BCINT, SelPtr);

char *BroweFlats(BCINT);
void DisplayAbout();
void DisplayHelp(BCINT, BCINT);
void HelpThingBox();
void HelpSectorBox();
void HelpTextureBox();
void DisplayHotKeys(BCINT);
/* end of file */

