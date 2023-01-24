/*
   Doom Editor Utility, by Brendon Wyber and Rapha‰l Quinet.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   LEVELS.H - Level data definitions.
   */

/* the includes */
#ifdef GAME_HEXEN
#include "struct_h.h"
#else
#include "struct_d.h"
#endif

/* the external variables from levels.c */
extern MDirPtr Level;		/* master dictionary entry for the level */

extern BCINT	NumThings;		/* number of things */
extern TPtr  	Things;		    /* things data */
extern BCINT	NumLineDefs;	/* number of line defs */
extern LDPtr	LineDefs;		  /* line defs data */
extern BCINT	NumSideDefs;	/* number of side defs */
extern SDPtr	SideDefs;		  /* side defs data */
extern BCINT	NumVertexes;	/* number of vertexes */
extern VPtr		Vertexes;		  /* vertex data */
extern BCINT	NumSegs;		  /* number of segments */
extern SEPtr	Segs;		      /* list of segments */
extern SEPtr	LastSeg;		  /* last segment in the list */
extern BCINT	NumSSectors;	/* number of subsectors */
extern SSPtr	SSectors;		  /* list of subsectors */
extern SSPtr	LastSSector;	/* last subsector in the list */
extern BCINT	NumSectors;	  /* number of sectors */
extern SPtr		Sectors;		  /* sectors data */
extern BCINT	NumWTexture;	/* number of wall textures */
extern char		**WTexture;	  /* array of wall texture names */
extern BCINT	NumFTexture;	/* number of floor/ceiling textures */
extern char		*FTexture[];	/* array of texture names */
extern BCINT  NumCMaps;			/* number of colormaps */
extern char   *CMaps[];			/* array of colormap names */

extern BCINT	MapMaxX;		/* maximum X value of map */
extern BCINT	MapMaxY;		/* maximum Y value of map */
extern BCINT	MapMinX;		/* minimum X value of map */
extern BCINT	MapMinY;		/* minimum Y value of map */

extern Bool		MadeChanges;	/* made changes? */
extern Bool		MadeMapChanges;	/* made changes that need rebuilding? */


/* from objects.c */
extern char		*DefaultWallTexture;		/* default normal wall texture */
extern char		*DefaultLowerTexture;		/* default lower wall texture */
extern char		*DefaultUpperTexture;		/* default upper wall texture */
extern char		*DefaultFloorTexture;		/* default floor texture */
extern char		*DefaultCeilingTexture;		/* default ceiling texture */
extern char		*DefaultDoorTrack;			/* default door track texture */
extern char		*DefaultDoorTexture;		/* default door texture */
extern char		*DefaultLowerStepTexture;	/* default door texture */
extern char		*DefaultUpperStepTexture;	/* default door texture */
extern char		*DefaultTeleportTexture;	/* default door texture */
extern BCINT	DefaultFloorHeight;			/* default floor height */
extern BCINT	DefaultCeilingHeight;		/* default ceiling height */
extern BCINT	DefaultLighting;				/* default sector lighting */


extern Bool		UseOffset;
extern BCINT	NoChecking;
