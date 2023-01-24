/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   LEVELS.C - Level loading and saving routines.
   */

/* the includes */
#include "deu.h"
#ifdef GAME_HEXEN
#include "struct_h.h"
#else
#include "struct_d.h"
#endif

/* the global data */
MDirPtr Level = NULL;                /* master dictionary entry for the level */
BCINT NumThings = 0;                /* number of things */
TPtr Things = NULL;                   /* things data */
BCINT NumBehavior = 0;                /* number of things */
BPtr Behavior = NULL;                        /* things data */
BCINT NumLineDefs = 0;                /* number of line defs */
LDPtr LineDefs=NULL;                  /* line defs data */
BCINT NumSideDefs = 0;                /* number of side defs */
SDPtr SideDefs=NULL;                   /* side defs data */
BCINT NumVertexes = 0;                /* number of vertexes */
VPtr Vertexes=NULL;                    /* vertex data */
BCINT NumSectors = 0;                /* number of sectors */
SPtr Sectors=NULL;                   /* sectors data */
BCINT NumSegs = 0;                /* number of segments */
SEPtr Segs = NULL;                /* list of segments */
SEPtr LastSeg = NULL;                /* last segment in the list */
BCINT NumSSectors = 0;                /* number of subsectors */
SSPtr SSectors = NULL;                /* list of subsectors */
SSPtr LastSSector = NULL;        /* last subsector in the list */
BCINT NumNodes = 0;                /* number of Nodes */
NPtr Nodes = NULL;                /* nodes tree */
BCINT NumWTexture = 0;                /* number of wall textures */
char **WTexture;	                /* array of wall texture names */
char *_WTexture;
BCINT NumFTexture = 0;                /* number of floor/ceiling textures */
char *FTexture[2048];                /* array of texture names */
BCINT NumCMaps = 0;										/* number of colormaps */
char *CMaps[2048];									  /* array of colormap names */
BCINT MapMaxX = -32767;                /* maximum X value of map */
BCINT MapMaxY = -32767;                /* maximum Y value of map */
BCINT MapMinX = 32767;                /* minimum X value of map */
BCINT MapMinY = 32767;                /* minimum Y value of map */
Bool MadeChanges = FALSE;        /* made changes? */
Bool MadeMapChanges = FALSE;        /* made changes that need rebuilding? */
SelPtr errld = NULL;                /* LineDefs in error (Nodes builder) */

SList Texture_sections = NULL;
SList Ftexture_sections = NULL;
SList CMap_sections = NULL;

/*
   read in the level data
   */

void ReadLevelData()
{
    MDirPtr dir;
    BCINT n, m;
    BCINT val;
    BCINT OldNumVertexes;
    BCINT *VertexUsed = (BCINT *)NULL;

    /* No objects are needed: they may be swapped after they have been read */

/*
    DisplayMessage( -1, -1, "Reading Data For Level %s...", LevelName);
*/
    Level = FindMasterDir( MasterDir, LevelName);		//jff try the name
    if (!Level)
			Level = FindMasterDir( MasterDir, "MAP01");		//jff try DOOM2 default
    if (!Level)                                     
			Level = FindMasterDir( MasterDir, "E1M1");		//jff try DOOM default
    if (!Level)
			ProgError( "level data not found");           //jff if no level, error

    /* get the number of Vertices */
    dir = FindMasterDir( Level, "VERTEXES");
    if (dir != NULL)
        OldNumVertexes = (BCINT) (dir->dir.size / 4L);
    else
        OldNumVertexes = 0;
    if (OldNumVertexes > 0) {
        VertexUsed = (BCINT*)GetMemory( OldNumVertexes * sizeof( BCINT));
        for (n = 0; n < OldNumVertexes; n++)
            VertexUsed[ n] = FALSE;
    }

    /* read in the Things data */
    dir = FindMasterDir( Level, "THINGS");
    if (dir != 0)
#ifdef GAME_HEXEN
        NumThings = (BCINT) (dir->dir.size / 20L);
#else
        NumThings = (BCINT) (dir->dir.size / 10L);
#endif
    else
        NumThings = 0;
    if (NumThings > 0) {
        Things = (TPtr)GetFarMemory( (unsigned long) NumThings * sizeof( struct Thing));
        BasicWadSeek( dir->wadfile, dir->dir.start);
#ifdef GAME_HEXEN
        	for (n = 0; n < NumThings; n++) {
			BasicWadRead( dir->wadfile, &(Things[ n].tid), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].xpos), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].ypos), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].zpos), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].angle), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].type), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].flags), 2);
			BasicWadRead( dir->wadfile, &(Things[ n].special), 1);
			BasicWadRead( dir->wadfile, &(Things[ n].arg1), 1);
			BasicWadRead( dir->wadfile, &(Things[ n].arg2), 1);
			BasicWadRead( dir->wadfile, &(Things[ n].arg3), 1);
			BasicWadRead( dir->wadfile, &(Things[ n].arg4), 1);
			BasicWadRead( dir->wadfile, &(Things[ n].arg5), 1);
			} 
#else
        BasicWadRead(dir->wadfile, Things, 10 * NumThings);
#endif
    }

    /* read in the LineDef information */
    dir = FindMasterDir( Level, "LINEDEFS");
    if (dir != NULL)
#ifdef GAME_HEXEN
        NumLineDefs = (BCINT) (dir->dir.size / 16L);
#else
        NumLineDefs = (BCINT) (dir->dir.size / 14L);
#endif
    else
        NumLineDefs = 0;
    if (NumLineDefs > 0) {
        LineDefs = (LDPtr)GetFarMemory( (unsigned long) NumLineDefs * sizeof( struct LineDef));
        BasicWadSeek( dir->wadfile, dir->dir.start);
        for (n = 0; n < NumLineDefs; n++) {
            BasicWadRead( dir->wadfile, &(LineDefs[ n].start), 2);
            VertexUsed[ LineDefs[ n].start] = TRUE;
            BasicWadRead( dir->wadfile, &(LineDefs[ n].end), 2);
            VertexUsed[ LineDefs[ n].end] = TRUE;
            BasicWadRead( dir->wadfile, &(LineDefs[ n].flags), 2);
#ifdef GAME_HEXEN
            BasicWadRead( dir->wadfile, &(LineDefs[ n].special), 1);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].arg1), 1);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].arg2), 1);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].arg3), 1);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].arg4), 1);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].arg5), 1);
#else
            BasicWadRead( dir->wadfile, &(LineDefs[ n].special), 2);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].tag), 2);
#endif
            BasicWadRead( dir->wadfile, &(LineDefs[ n].sidedef1), 2);
            BasicWadRead( dir->wadfile, &(LineDefs[ n].sidedef2), 2);
        }
    }

    /* read in the SideDef information */
    dir = FindMasterDir( Level, "SIDEDEFS");
    if (dir != NULL)
        NumSideDefs = (BCINT) (dir->dir.size / 30L);
    else
        NumSideDefs = 0;
    if (NumSideDefs > 0) {
        SideDefs = (SDPtr)GetFarMemory( (unsigned long) NumSideDefs * sizeof( struct SideDef));
        BasicWadSeek( dir->wadfile, dir->dir.start);
        for (n = 0; n < NumSideDefs; n++) {
            BasicWadRead( dir->wadfile, &(SideDefs[ n].xoff), 2);
            BasicWadRead( dir->wadfile, &(SideDefs[ n].yoff), 2);
            BasicWadRead( dir->wadfile, &(SideDefs[ n].tex1), 8);
            BasicWadRead( dir->wadfile, &(SideDefs[ n].tex2), 8);
            BasicWadRead( dir->wadfile, &(SideDefs[ n].tex3), 8);
            BasicWadRead( dir->wadfile, &(SideDefs[ n].sector), 2);
        }
    }

    /* read in the Vertices which are all the corners of the level, but ignore the */
    /* Vertices not used in any LineDef (they usually are at the end of the list). */
    NumVertexes = 0;
    for (n = 0; n < OldNumVertexes; n++)
        if (VertexUsed[ n])
            NumVertexes++;
    if (NumVertexes > 0) {
        Vertexes = (VPtr)GetFarMemory( (unsigned long) NumVertexes * sizeof( struct Vertex));
        dir = FindMasterDir( Level, "VERTEXES");
        BasicWadSeek( dir->wadfile, dir->dir.start);
        MapMaxX = -32767;
        MapMaxY = -32767;
        MapMinX = 32767;
        MapMinY = 32767;
        m = 0;
        for (n = 0; n < OldNumVertexes; n++) {
            BasicWadRead( dir->wadfile, &val, 2);
            if (VertexUsed[ n]) {
                if (val < MapMinX)
                    MapMinX = val;
                if (val > MapMaxX)
                    MapMaxX = val;
                Vertexes[ m].x = val;
            }
            BasicWadRead( dir->wadfile, &val, 2);
            if (VertexUsed[ n]) {
                if (val < MapMinY)
                    MapMinY = val;
                if (val > MapMaxY)
                    MapMaxY = val;
                Vertexes[ m].y = val;
                m++;
            }
        }
        if (m != NumVertexes)
            ProgError("inconsistency in the Vertexes data\n");
    }

    if (OldNumVertexes > 0) {
        /* update the Vertex numbers in the LineDefs (not really necessary, but...) */
        m = 0;
        for (n = 0; n < OldNumVertexes; n++)
            if (VertexUsed[ n])
                VertexUsed[ n] = m++;
        for (n = 0; n < NumLineDefs; n++) {
            LineDefs[ n].start = VertexUsed[ LineDefs[ n].start];
            LineDefs[ n].end = VertexUsed[ LineDefs[ n].end];
        }
        FreeMemory( VertexUsed);
    }

    /* ignore the Segs, SSectors and Nodes */

    /* read in the Sectors information */
    dir = FindMasterDir( Level, "SECTORS");
    if (dir != NULL)
        NumSectors = (BCINT) (dir->dir.size / 26L);
    else
        NumSectors = 0;
    if (NumSectors > 0) {
        Sectors = (SPtr)GetFarMemory( (unsigned long) NumSectors * sizeof( struct Sector));
        BasicWadSeek( dir->wadfile, dir->dir.start);
        for (n = 0; n < NumSectors; n++) {
            BasicWadRead( dir->wadfile, &(Sectors[ n].floorh), 2);
            BasicWadRead( dir->wadfile, &(Sectors[ n].ceilh), 2);
            BasicWadRead( dir->wadfile, &(Sectors[ n].floort), 8);
            BasicWadRead( dir->wadfile, &(Sectors[ n].ceilt), 8);
            BasicWadRead( dir->wadfile, &(Sectors[ n].light), 2);
            BasicWadRead( dir->wadfile, &(Sectors[ n].special), 2);
            BasicWadRead( dir->wadfile, &(Sectors[ n].tag), 2);
        }
    }

#ifdef GAME_HEXEN
    /* read in the Behavior data */
    dir = FindMasterDir( Level, "BEHAVIOR");
    if (dir != 0)
        NumBehavior = (BCINT) (dir->dir.size / 4L);
    else
        NumBehavior = 0;
    if (NumBehavior > 0) {
        Behavior = (BPtr)GetFarMemory( (unsigned long) NumBehavior * sizeof( struct Behavior));
        BasicWadSeek( dir->wadfile, dir->dir.start);
        BasicWadRead(dir->wadfile, Behavior, 4 * NumBehavior);
    }
#endif
}



/*
   forget the level data
   */

void ForgetLevelData() /* SWAP! */
{
    /* forget the Things */
    NumThings = 0;
    if (Things)
        FreeFarMemory( Things);
    Things = NULL;

    /* forget the Vertices */
    NumVertexes = 0;
    if (Vertexes)
        FreeFarMemory( Vertexes);
    Vertexes = NULL;

    /* forget the LineDefs */
    NumLineDefs = 0;
    if (LineDefs)
        FreeFarMemory( LineDefs);
    LineDefs = NULL;

    /* forget the SideDefs */
    NumSideDefs = 0;
    if (SideDefs)
        FreeFarMemory( SideDefs);
    SideDefs = NULL;

    /* forget the Sectors */
    NumSectors = 0;
    if (Sectors)
        FreeFarMemory( Sectors);
    Sectors = NULL;
}


/*
   save the level data to a PWAD file
   */

void SaveLevelData( char *outfile)
{
    FILE   *file;
    MDirPtr dir;
    long    counter = 11;  
    BCINT   n; 
    long    size;
    long    dirstart;
    long    blocksize;
    long    rejectsize;
    BCINT   oldNumVertexes;
    Bool usem = UseMouse;
#ifdef GAME_HEXEN
    long    behaviorsize;
    long    mapinfosize;
    long    sndinfosize;

	if (KeepInfo == TRUE)
		counter = 14;
	else
		counter = 12;
#endif

    UseMouse = FALSE;
    if (usem)
        HideMousePointer();




    DisplayMessage( -1, -1, "Saving data to \"%s\"...", outfile);
    LogMessage( ": Saving data to \"%s\"...\n", outfile);
    oldNumVertexes = NumVertexes;
    /* open the file */
    if ((file = fopen( outfile, "wb")) == NULL)
        ProgError( "Unable to open file \"%s\"", outfile);
    WriteBytes( file, "PWAD", 4L);     /* PWAD file */
    WriteBytes( file, &counter, 4L);   /* 11 entries */
    WriteBytes( file, &counter, 4L);   /* fix this up later */
    counter = 12L; 
    dir = Level->next;

    /* output the things data */
    for (n = 0; n < NumThings; n++) {
#ifdef GAME_HEXEN
        WriteBytes( file, &(Things[ n].tid), 2L);
        WriteBytes( file, &(Things[ n].xpos), 2L);
        WriteBytes( file, &(Things[ n].ypos), 2L);
        WriteBytes( file, &(Things[ n].zpos), 2L);
        WriteBytes( file, &(Things[ n].angle), 2L);
        WriteBytes( file, &(Things[ n].type), 2L);
        WriteBytes( file, &(Things[ n].flags), 2L);
        WriteBytes( file, &(Things[ n].special), 1L);
        WriteBytes( file, &(Things[ n].arg1), 1L);
        WriteBytes( file, &(Things[ n].arg2), 1L);
        WriteBytes( file, &(Things[ n].arg3), 1L);
        WriteBytes( file, &(Things[ n].arg4), 1L);
        WriteBytes( file, &(Things[ n].arg5), 1L);
        counter += 20L;
#else
        WriteBytes( file, &(Things[ n].xpos), 2L);
        WriteBytes( file, &(Things[ n].ypos), 2L);
        WriteBytes( file, &(Things[ n].angle), 2L);
        WriteBytes( file, &(Things[ n].type), 2L);
        WriteBytes( file, &(Things[ n].flags), 2L);
        counter += 10L;
#endif
    }
    dir = dir->next;

    /* update MapMinX, MapMinY, MapMaxX, MapMaxY */
    MapMaxX = -32767;
    MapMaxY = -32767;
    MapMinX = 32767;
    MapMinY = 32767;
    for (n = 0; n < NumVertexes; n++) {
        if (Vertexes[ n].x < MapMinX)
            MapMinX = Vertexes[ n].x;
        if (Vertexes[ n].x > MapMaxX)
            MapMaxX = Vertexes[ n].x;
        if (Vertexes[ n].y < MapMinY)
            MapMinY = Vertexes[ n].y;
        if (Vertexes[ n].y > MapMaxY)
            MapMaxY = Vertexes[ n].y;
    }

    /* output the LineDefs */
    for (n = 0; n < NumLineDefs; n++) {
#ifdef GAME_HEXEN
        WriteBytes( file, &(LineDefs[ n].start), 2L);
        WriteBytes( file, &(LineDefs[ n].end), 2L);
        WriteBytes( file, &(LineDefs[ n].flags), 2L);
        WriteBytes( file, &(LineDefs[ n].special), 1L);
        WriteBytes( file, &(LineDefs[ n].arg1), 1L);
        WriteBytes( file, &(LineDefs[ n].arg2), 1L);
        WriteBytes( file, &(LineDefs[ n].arg3), 1L);
        WriteBytes( file, &(LineDefs[ n].arg4), 1L);
        WriteBytes( file, &(LineDefs[ n].arg5), 1L);
        WriteBytes( file, &(LineDefs[ n].sidedef1), 2L);
        WriteBytes( file, &(LineDefs[ n].sidedef2), 2L);
        counter += 16L;
#else
        WriteBytes( file, &(LineDefs[ n].start), 2L);
        WriteBytes( file, &(LineDefs[ n].end), 2L);
        WriteBytes( file, &(LineDefs[ n].flags), 2L);
        WriteBytes( file, &(LineDefs[ n].special), 2L);
        WriteBytes( file, &(LineDefs[ n].tag), 2L);
        WriteBytes( file, &(LineDefs[ n].sidedef1), 2L);
        WriteBytes( file, &(LineDefs[ n].sidedef2), 2L);
        counter += 14L;
#endif
    }
    dir = dir->next;

    /* output the SideDefs */
    for (n = 0; n < NumSideDefs; n++) {
        WriteBytes( file, &(SideDefs[ n].xoff), 2L);
        WriteBytes( file, &(SideDefs[ n].yoff), 2L);
        WriteBytes( file, &(SideDefs[ n].tex1), 8L);
        WriteBytes( file, &(SideDefs[ n].tex2), 8L);
        WriteBytes( file, &(SideDefs[ n].tex3), 8L);
        WriteBytes( file, &(SideDefs[ n].sector), 2L);
        counter += 30L;
    }
    dir = dir->next;

    if (MadeMapChanges) {
        /* output the Vertices */
        for (n = 0; n < NumVertexes; n++) {
            WriteBytes( file, &(Vertexes[ n].x), 2L);
            WriteBytes( file, &(Vertexes[ n].y), 2L);
            counter += 4L;
        }
    }
    else {
        /* copy the Vertices */
        size = dir->dir.size;
        counter += size;
        BasicWadSeek( dir->wadfile, dir->dir.start);
        CopyBytes( file, dir->wadfile->fileinfo, size);
    }
    dir = dir->next;

	/* copy the Segs, SSectors and Nodes */
	for (n = 0; n < 3; n++) {
		size = dir->dir.size;
		counter += size;
		BasicWadSeek( dir->wadfile, dir->dir.start);
		CopyBytes( file, dir->wadfile->fileinfo, size);
		dir = dir->next;
	}

    /* output the Sectors */
    for (n = 0; n < NumSectors; n++) {
        WriteBytes( file, &(Sectors[ n].floorh), 2L);
        WriteBytes( file, &(Sectors[ n].ceilh), 2L);
        WriteBytes( file, &(Sectors[ n].floort), 8L);
        WriteBytes( file, &(Sectors[ n].ceilt), 8L);
        WriteBytes( file, &(Sectors[ n].light), 2L);
        WriteBytes( file, &(Sectors[ n].special), 2L);
        WriteBytes( file, &(Sectors[ n].tag), 2L);
        counter += 26L;
    }
    dir = dir->next;

	/* copy the Reject data */
	rejectsize = dir->dir.size;
	size = rejectsize;
	counter += size;
	BasicWadSeek( dir->wadfile, dir->dir.start);
	CopyBytes( file, dir->wadfile->fileinfo, size);
	dir = dir->next;

	/* copy the blockmap data */
	blocksize = dir->dir.size;
	size = blocksize;
	counter += size;
	BasicWadSeek( dir->wadfile, dir->dir.start);
	CopyBytes( file, dir->wadfile->fileinfo, size);
	dir = dir->next;

#ifdef GAME_HEXEN
	/* copy the behavior data */
	behaviorsize = dir->dir.size;
	size = behaviorsize;
	counter += size;
	BasicWadSeek( dir->wadfile, dir->dir.start);
	CopyBytes( file, dir->wadfile->fileinfo, size);
	dir = dir->next;


	if (KeepInfo == TRUE) {
	/* copy the mapinfo data */
    	dir = FindMasterDir( Level, "MAPINFO");
	mapinfosize = dir->dir.size;
	size = mapinfosize;
	counter += size;
	BasicWadSeek( dir->wadfile, dir->dir.start);
	CopyBytes( file, dir->wadfile->fileinfo, size);
	dir = dir->next;

	/* copy the sndinfo data */
    	dir = FindMasterDir( Level, "SNDINFO");
	sndinfosize = dir->dir.size;
	size = sndinfosize;
	counter += size;
	BasicWadSeek( dir->wadfile, dir->dir.start);
	CopyBytes( file, dir->wadfile->fileinfo, size);
	dir = dir->next;
	}
#endif

    /* output the actual directory */
    dirstart = counter;
    counter = 12L;
    size = 0L; /*12L*/
    dir = Level;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, &(dir->dir.name), 8L);
    /*counter += 12;*/
    dir = dir->next;

#ifdef GAME_HEXEN
    size = (long) NumThings * 20L;
#else
    size = (long) NumThings * 10L;
#endif
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "THINGS\0\0", 8L);
    counter += size;
    dir = dir->next;

#ifdef GAME_HEXEN
    size = (long) NumLineDefs * 16L;
#else
    size = (long) NumLineDefs * 14L;
#endif
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "LINEDEFS", 8L);
    counter += size;
    dir = dir->next;

    size = (long) NumSideDefs * 30L;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "SIDEDEFS", 8L);
    counter += size;
    dir = dir->next;

    if (MadeMapChanges)
        size = (long) NumVertexes * 4L;
    else
        size = dir->dir.size;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "VERTEXES", 8L);
    counter += size;
    dir = dir->next;

	size = dir->dir.size;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "SEGS\0\0\0\0", 8L);
    counter += size;
    dir = dir->next;

	size = dir->dir.size;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "SSECTORS", 8L);
    counter += size;
    dir = dir->next;

	size = dir->dir.size;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "NODES\0\0\0", 8L);
    counter += size;
    dir = dir->next;

    size = (long) NumSectors * 26L;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "SECTORS\0", 8L);
    counter += size;
    dir = dir->next;

    size = rejectsize;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "REJECT\0\0", 8L);
    counter += size;
    dir = dir->next;

    size = blocksize;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "BLOCKMAP", 8L);
    counter += size;
    dir = dir->next;

#ifdef GAME_HEXEN
    size = (long) NumBehavior * 4L;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "BEHAVIOR", 8L);
    counter += size;
    dir = dir->next;


    if (KeepInfo == TRUE) {
    size = mapinfosize;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "MAPINFO", 8L);
    counter += size;
    dir = dir->next;
	
    size = sndinfosize;
    WriteBytes( file, &counter, 4L);
    WriteBytes( file, &size, 4L);
    WriteBytes( file, "SNDINFO", 8L);
    counter += size;
    dir = dir->next;
    }
#endif

    /* fix up the directory start information */
    if (fseek( file, 8L, SEEK_SET))
        ProgError( "error writing to file");
    WriteBytes( file, &dirstart, 4L);

    /* close the file */
    fclose( file);

    NumSegs = 0;
    NumSSectors = 0;
    NumNodes = 0;

    /* delete the vertices added by the Nodes builder */
    if (NumVertexes != oldNumVertexes) {
        NumVertexes = oldNumVertexes;
        ResizeFarMemory( Vertexes, NumVertexes * sizeof( struct Vertex));
    }

    /* the file is now up to date */
    MadeChanges = FALSE;

    /* update pointers in Master Directory */
    OpenPatchWad( outfile);

    /* this should free the old "*.BAK" file */
    CloseUnusedWadFiles();

    UseMouse = usem;
    if (UseMouse)
        ShowMousePointer();
}


Bool dupname;
/* SO 14/4/95:
   SortTextures will set this if a name is repeated in the texture list,
   so that we can look for it and eliminate duplicates */


/*
   function used by qsort to sort the texture names
   */
int SortTextures( const void *a, const void *b)
{
    int i = strcmp( *((char **)a), *((char **)b));
    if (i == 0)
			dupname = TRUE;
    return i;
}



/*
   read in the wall texture names
   */

void ReadWTextureNames()
{
  BCINT i, j;
  void *resource;
  long nTexture = 0, n;
  Texture *p;
  SList l;


  for (l = Texture_sections; l; l = l->next)
	{
    resource = GetResource(l->string);
	  n = *((long *)resource);
	  nTexture += n;
	}
	
	NumWTexture = nTexture + 1;
	/* + 1 to allow for the `-' texture */
	
	WTexture = (char **)GetMemory(sizeof(char *) * NumWTexture);
	_WTexture = (char *)GetMemory(9 * sizeof(char) * NumWTexture);
	
	WTexture[0] = _WTexture;
	strcpy(WTexture[0], "-");
	
	j = 1;
	for (l = Texture_sections; l; l = l->next)
	{
	  resource = GetResource(l->string);
	  n = *((long *)resource);
		for (i = 1; i <= n; i++, j++)
		{
			WTexture[j] = _WTexture + 9 * j;
			p = (Texture *)(((long *)resource)[i]);
			p = (Texture *)((long)p + (long)resource); 
			strncpy(WTexture[j], p->name, 8);
			WTexture[j][8] = '\0';
		}
	}
	
  /* sort the names */
  /* SO 14/4/95 */
  dupname = FALSE;
  qsort(WTexture, NumWTexture, sizeof( char *), SortTextures);

  if (dupname)
	{
    /* Grrr. We now have to fool about with the array. */
    i = 0; j = 1;

		while (j<nTexture)		// jff fix logic for eliminating duplicates
		{
			int j0=j;
			while (j<nTexture && !strcmp(WTexture[i],WTexture[j++]));
			nTexture -= j-j0;
		  if (j<nTexture)
				WTexture[++i] = WTexture[j++];
		}
//    while (j < nTexture)
//		{
//    	if (!strcmp(WTexture[j++], WTexture[i++]))
//			{
//    		NumWTexture--;
//    		j++;
//    	}
//    	WTexture[i] = WTexture[j];
//    }
  }
}



/*
   forget the wall texture names
   */

void ForgetWTextureNames()
{
	FreeMemory( _WTexture);
  NumWTexture = 0;
  FreeMemory( WTexture);
}

/*
   read in the floor/ceiling texture names
   */

void ReadFTextureNames()
{
  BCINT i, j;
	SList p;
	
  for(p = Ftexture_sections; p; p = p->next)
    ReadFTextureNamesIn(p->string);

	dupname = FALSE;
  qsort( FTexture, NumFTexture, sizeof( char *), SortTextures);

  if (dupname)
	{
    /* Grrr. We now have to fool about with the array. */
    i = 0; j = 1;

		while (j<NumFTexture)		// jff fix logic for eliminating duplicates
		{
			int j0=j;
			while (j<NumFTexture && !strcmp(FTexture[i],FTexture[j++]));
			NumFTexture -= j-j0;
		  if (j<NumFTexture)
				FTexture[++i] = FTexture[j++];
		}
//    while(j < NumFTexture)
//		{
//    	if(!strcmp(FTexture[j++], FTexture[i++]))
//			{
//    		NumFTexture--;
//    		j++;
//    	}
//    	FTexture[i] = FTexture[j];
//    }
  }
}


void ReadFTextureNamesIn(char *section)
{
  MDirPtr dir,cdir;/*jff*/
  BCINT n, m;
  char start[16];
  char end[16];
	
	strcpy(start, section); strcat(start, "_START");
	strcpy(end, section); strcat(end, "_END");
	
    /* while blocks delimited by start and end exist */

	cdir = MasterDir; 	/*JFF use a variable start pointer to support loop */
  while (cdir && (dir = FindMasterDir( cdir, start)))
	{ 												/*JFF keep looking for more */
		cdir = dir = dir->next;
		/*	count the names */

		if (dir && !isamark(dir->dir.name))
		{
    	for (n = 0; dir && !isamark(dir->dir.name); n++)
       	dir = dir->next;		/* keep moving until points to end tag */

			/* if end tag is found, and non-empty section */
			if (dir && n>0)	
			{
    		/* add the names from master dir. */
    		dir = cdir;
    		for (m = 0; dir &&  m < n; m++)
				{	/* add each new flat name */
       		FTexture[ NumFTexture + m] = (char*)GetMemory( 9 * sizeof( char));
       		strncpy( FTexture[ NumFTexture + m], dir->dir.name, 8);
       		FTexture[ NumFTexture + m][ 8] = '\0';
					dir = dir->next;
    		}		
    		NumFTexture += m;
				cdir = dir; /*JFF continue search where last it left off */
			}
		}
	}
}



/*
   forget the floor/ceiling texture names
   */

void ForgetFTextureNames()
{
    BCINT n;

    /* forget all names */
    for (n = 0; n < NumFTexture; n++)
        FreeMemory( FTexture[ n]);
	NumFTexture = 0;
}

/*
   read in the colormap names
   */

void ReadCMapNames()
{
  BCINT i, j;
	
	NumCMaps=0;		 // Add COLORMAP, WATERMAP to the list of valid colormap names
	CMaps[NumCMaps] = (char*)GetMemory( 9 * sizeof( char));
	strncpy(CMaps[NumCMaps], "COLORMAP", 8);
	CMaps[NumCMaps++][8] = '\0';
	CMaps[NumCMaps] = (char*)GetMemory( 9 * sizeof( char));
	strncpy(CMaps[NumCMaps], "WATERMAP", 8);
	CMaps[NumCMaps++][8] = '\0';

  ReadCMapNamesIn("C"); // Append any added by PWADs C_START/C_END blocked

	dupname = FALSE;
  qsort( CMaps, NumCMaps, sizeof( char *), SortTextures);

  if (dupname)
	{
    /* Grrr. We now have to fool about with the array. */
    i = 0; j = 1;

		while (j<NumCMaps)		// jff fix logic for eliminating duplicates
		{
			int j0=j;
			while (j<NumCMaps && !strcmp(CMaps[i],CMaps[j++]));
			NumCMaps -= j-j0;
		  if (j<NumCMaps)
				CMaps[++i] = CMaps[j++];
		}
//    while(j < NumCMaps)
//		{
//    	if(!strcmp(CMaps[j++], CMaps[i++]))
//			{
//    		NumCMaps--;
//    		j++;
//    	}
//    	CMaps[i] = CMaps[j];
//    }
  }
}


void ReadCMapNamesIn(char *section)
{
  MDirPtr dir,cdir;/*jff*/
  BCINT n, m;
  char start[16];
  char end[16];
	
	strcpy(start, section); strcat(start, "_START");
	strcpy(end, section); strcat(end, "_END");
	
    /* while blocks delimited by start and end exist */

	cdir = MasterDir; 	/*JFF use a variable start pointer to support loop */
  while (cdir && (dir = FindMasterDir( cdir, start)))
	{ 												/*JFF keep looking for more */
		cdir = dir = dir->next;
		/*	count the names */

		if (dir && !isamark(dir->dir.name))
		{
    	for (n = 0; dir && !isamark(dir->dir.name); n++)
       	dir = dir->next;		/* keep moving until points to end tag */

			/* if end tag is found, and non-empty section */
			if (dir && n>0)	
			{
    		/* add the names from master dir. */
    		dir = cdir;
    		for (m = 0; dir &&  m < n; m++)
				{	/* add each new colormap name */
       		CMaps[NumCMaps + m] = (char*)GetMemory( 9 * sizeof( char));
       		strncpy(CMaps[NumCMaps + m], dir->dir.name, 8);
       		CMaps[NumCMaps + m][ 8] = '\0';
					dir = dir->next;
    		}		
    		NumCMaps += m;
				cdir = dir; /*JFF continue search where last it left off */
			}
		}
	}
}



/*
   forget the floor/ceiling texture names
   */

void ForgetCMapNames()
{
    BCINT n;

  /* forget all names */
  for (n = 0; n < NumCMaps; n++)
    FreeMemory(CMaps[n]);
	NumCMaps = 0;
}



/* end of file */
