/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   WAD.C - Wad files routines.
   */

/* the includes */
#include "deu.h"

/* global variables */
WadPtr WadFileList = NULL;       /* linked list of wad files */
MDirPtr MasterDir = NULL;        /* the master directory */
SList LevelNames = NULL; /* list of levels for the current game */

/*
   open the main wad file, read in its directory and create the
   master directory
   */

void OpenMainWad( char *filename)
{
    MDirPtr lastp, newp;
    long n;
    WadPtr wad;
    
    /* open the iwad file */
    wad = BasicWadOpen( filename);
    if (strncmp( wad->type, "IWAD", 4))
        ProgError( "\"%s\" is not the main WAD file", filename);
    
    /* create the master directory */
	/* linked list of directory entries and file they came from */
    lastp = NULL;
    for (n = 0; n < wad->dirsize; n++) {
        newp = (MDirPtr) GetMemory( sizeof( struct MasterDirectory));
        newp->next = NULL;
        newp->wadfile = wad;
        memcpy( &(newp->dir), &(wad->directory[ n]), sizeof( struct Directory));
        if (MasterDir)
            lastp->next = newp;
        else
            MasterDir = newp;
        lastp = newp;
    }
    
    /* check if registered version */
    Registered = FALSE; /* If you remove this, bad things will happen to you... */
    if ( RegTest && FindMasterDir( MasterDir, RegTest))
        Registered = TRUE;
    
    if(!Registered) {
        printf( "   *-------------------------------------------------*\n");
        printf( "   |     Warning: this is the shareware version.     |\n");
        printf( "   |   You won't be allowed to save your changes.    |\n");
        printf( "   |     PLEASE REGISTER YOUR COPY OF THE GAME.      |\n");
        printf( "   *-------------------------------------------------*\n");
    }
}

/* short helper function for the next fn */
int isalev(char *s)
{
	SList p;
	
	for(p = LevelNames; p; p = p->next)
		if(!strcmp(p->string, s))
			return 1;
	
	return 0;
}

int isamark(char *mrk)
{
	if (!strchr("FSP",mrk[0]))
		return 0;

	if (mrk[1]!=mrk[0] && !strchr("123_",mrk[1]))
		return 0;

	if (mrk[1]=='_' && (strncmp(mrk+2,"START",5)==0 || strncmp(mrk+2,"END",3)==0))
		return 1;

	if (mrk[2]=='_' && (strncmp(mrk+3,"START",5)==0 || strncmp(mrk+3,"END",3)==0))
		return 1;

	return 0;
}

/*
   open a patch wad file, read in its directory and alter the master
   directory
   */

void OpenPatchWad( char *filename)
{
    WadPtr wad;
    MDirPtr mdir = NULL;
    BCINT n, l;
    char entryname[9];
    /* ignore the file if it doesn't exist */
    if (! Exists( filename)) {
        printf( "Warning: patch WAD file \"%s\" doesn't exist.  Ignored.\n", filename);
        return;
    }
	
    /* if we have loaded the TEXTURE1 & TEXTURE2 resources, now 
       would be a good time to get rid of them, in case the
       wad we are now opening contains its own versions of them */
	
    ForgetAllResources();
    
    /* open the wad file */
    LogMessage( "Loading patch WAD file: %s...\n", filename);
    wad = BasicWadOpen( filename);
    if (strncmp( wad->type, "PWAD", 4))
        ProgError( "\"%s\" is not a patch WAD file", filename);
    
    /* alter the master directory */
    l = 0;
    for (n = 0; n < wad->dirsize; n++) {
        strncpy( entryname, wad->directory[ n].name, 8);
        entryname[8] = '\0';
        if (l == 0) {
            mdir = FindMasterDir( MasterDir, wad->directory[ n].name);
            /* if this entry is not in the master directory, then add it */
            if (mdir == NULL || isamark(wad->directory[n].name)) {
                mdir = MasterDir;
                while (mdir->next)
                    mdir = mdir->next;
                mdir->next = (MDirPtr) GetMemory( sizeof( struct MasterDirectory));
                mdir = mdir->next;
                mdir->next = NULL;
            }
            /* if this is a level, then copy this entry and the next 10 or 11 */
            else if (isalev(wad->directory[ n].name)) {
                LogMessage( "   [Updating level %s] from %s\n", entryname, wad->filename);
#ifdef GAME_HEXEN
                l = 11;
#else
                l = 10;
#endif
				mdir->wadfile = wad;
            }
        }
        else {
            mdir = mdir->next;
            /* the level data should replace an existing level */
			/* the level entries must be in the standard order */
            if (mdir == NULL || strncmp(mdir->dir.name, wad->directory[ n].name, 8))
                ProgError( "\\%s\" is not an understandable PWAD file (error with %s)", filename, entryname);
            l--;
        }
        mdir->wadfile = wad;
        memcpy( &(mdir->dir), &(wad->directory[ n]), sizeof( struct Directory));
    }
}



/*
   close all the wad files, deallocating the WAD file structures
   */

void CloseWadFiles()
{
    WadPtr curw, nextw;
    MDirPtr curd, nextd;
    
    /* close the wad files */
    curw = WadFileList;
    WadFileList = NULL;
    while (curw) {
        nextw = curw->next;
        fclose( curw->fileinfo);
        FreeMemory( curw->directory);
        FreeMemory( curw);
        curw = nextw;
    }
    
    /* delete the master directory */
    curd = MasterDir;
    MasterDir = NULL;
    while (curd) {
        nextd = curd->next;
        FreeMemory( curd);
        curd = nextd;
    }
}



/*
   forget unused patch wad files
   */

void CloseUnusedWadFiles()
{
    WadPtr curw, prevw;
    MDirPtr mdir;
    
    prevw = NULL;
    curw = WadFileList;
    while (curw) {
        /* check if the wad file is used by a directory entry */
        mdir = MasterDir;
        while (mdir && mdir->wadfile != curw)
            mdir = mdir->next;
        if (mdir)
            prevw = curw;
        else {
            /* if this wad file is never used, close it */
            LogMessage("closing wad file %s\n", curw->filename);
            if (prevw)
                prevw->next = curw->next;
            else
                WadFileList = curw->next;
            fclose( curw->fileinfo);
            FreeMemory( curw->directory);
            FreeMemory( curw);
            free(curw->filename);
        }
        curw = prevw->next;
    }
}



/*
   basic opening of WAD file and creation of node in Wad linked list
   */

WadPtr BasicWadOpen( char *filename)
{
    WadPtr curw, prevw;
    
    /* find the wad file in the wad file list */
    prevw = WadFileList;
    if (prevw) {
        curw = prevw->next;
        while (curw && strcmp( filename, curw->filename)) {
            prevw = curw;
            curw = prevw->next;
        }
    }
    else
        curw = NULL;
    
    /* if this entry doesn't exist, add it to the WadFileList */
    if (curw == NULL) {
        curw = (WadPtr) GetMemory( sizeof( struct WadFileInfo));
        if (prevw == NULL)
            WadFileList = curw;
        else
            prevw->next = curw;
        curw->next = NULL;
        /* SO 26/4/95 */
        curw->filename = strdup(filename);
    }
	else
        fclose( curw->fileinfo);
    
    /* open the file */
    if ((curw->fileinfo = fopen( filename, "rb")) == NULL) {
        if (!prevw)  {                
            WadFileList = NULL;
        }
        else {
            prevw->next = curw->next;
        }
        FreeMemory( curw);
        ProgError( "error opening \"%s\"", filename);
    }
    /* read in the WAD directory info */
    BasicWadRead( curw, curw->type, 4);
    if (strncmp( curw->type, "IWAD", 4) && strncmp( curw->type, "PWAD", 4))
        ProgError( "\"%s\" is not a valid WAD file", filename);
    BasicWadRead( curw, &curw->dirsize, sizeof( curw->dirsize));
    BasicWadRead( curw, &curw->dirstart, sizeof( curw->dirstart));
    
    /* read in the WAD directory itself */
    curw->directory = (DirPtr) GetMemory( sizeof( struct Directory) * curw->dirsize);
    BasicWadSeek( curw, curw->dirstart);
    BasicWadRead( curw, curw->directory, sizeof( struct Directory) * curw->dirsize);
    
    /* all done */
    return curw;
}



/*
   read bytes from a file and store it into an address with error checking
   */

void BasicWadRead( WadPtr wadfile, void huge *addr, long size)
{
    if (fread( addr, 1, size, wadfile->fileinfo) != size)
        ProgError( "error reading from \"%s\", size %d", wadfile->filename,
                  size);
}



/*
   go to offset of wad file with error checking
   */

void BasicWadSeek( WadPtr wadfile, long offset)
{
    if (fseek( wadfile->fileinfo, offset, 0))
        ProgError( "error reading from \"%s\"", wadfile->filename);
}



/*
   find an entry in the master directory
   */

MDirPtr FindMasterDir( MDirPtr from, char *name)
{
    while (from) {
        if (!strncmp( from->dir.name, name, 8))
            break;
        from = from->next;
    }
    return from;
}



/*
   list the master directory
   */

void ListMasterDirectory( FILE *file)
{
    char dataname[ 9];
    MDirPtr dir;
    char key;
    BCINT lines = 3;
    
    dataname[ 8] = '\0';
    fprintf( file, "The Master Directory\n");
    fprintf( file, "====================\n\n");
    fprintf( file, "NAME____  FILE________________  SIZE__  START____\n");
    for (dir = MasterDir; dir; dir = dir->next) {
        strncpy( dataname, dir->dir.name, 8);
        fprintf( file, "%-8s  %-20s  %6ld  x%08lx\n", dataname, dir->wadfile->filename, dir->dir.size, dir->dir.start);
        if (file == stdout && lines++ > 21) {
            lines = 0;
            printf( "[Q to abort, any other key to continue]");
            key = bioskey( 0);
            printf( "\r                                       \r");
            if (key == 'Q' || key == 'q')
                break;
        }
    }
}



/*
   list the directory of a file
   */

void ListFileDirectory( FILE *file, WadPtr wad)
{
    char dataname[ 9];
    char key;
    BCINT lines = 5;
    long n;
    
    dataname[ 8] = '\0';
    fprintf( file, "WAD File Directory\n");
    fprintf( file, "==================\n\n");
    fprintf( file, "Wad File: %s\n\n", wad->filename);
    fprintf( file, "NAME____  SIZE__  START____  END______\n");
    for (n = 0; n < wad->dirsize; n++) {
        strncpy( dataname, wad->directory[n].name, 8);
        fprintf( file, "%-8s  %6ld  x%08lx  x%08lx\n", dataname, wad->directory[n].size, wad->directory[n].start, wad->directory[n].size + wad->directory[n].start - 1);
        if (file == stdout && lines++ > 21) {
            lines = 0;
            printf( "[Q to abort, any other key to continue]");
            key = bioskey( 0);
            printf( "\r                                       \r");
            if (key == 'Q' || key == 'q')
                break;
        }
    }
}



/*
   build a new wad file from master dictionary
   */

void BuildNewMainWad( char *filename, Bool patchonly)
{
    FILE *file;
    long counter = 12;
    MDirPtr cur;
    long size;
    long dirstart;
    long dirnum;
    
    /* open the file and store signatures */
    SetColor(BLUE);

#ifndef SLIM
    if (patchonly)
        DrawScreenText(10, 170, "Building a compound Patch Wad file \"%s\".", filename);
    else
        DrawScreenText(10, 170, "Building a new Main Wad file \"%s\" (size approx 20 Meg)", filename);
#else
    if (patchonly)
        printf("Building a compound Patch Wad file \"%s\".\n", filename);
    else
        printf("Building a new Main Wad file \"%s\" (size approx 20 Meg)\n", filename);
#endif
    if (!Registered)
        ProgError( "You were warned: you are not allowed to do this.");
    if ((file = fopen( filename, "wb")) == NULL)
        ProgError( "unable to open file \"%s\"", filename);
    if (patchonly)
        WriteBytes( file, "PWAD", 4);
    else
        WriteBytes( file, "IWAD", 4);
    WriteBytes( file, &counter, 4L);      /* put true value in later */
    WriteBytes( file, &counter, 4L);      /* put true value in later */
    
    /* output the directory data chuncks */
    for (cur = MasterDir; cur; cur = cur->next) {
        if (patchonly && cur->wadfile == WadFileList)
            continue;
        size = cur->dir.size;
        counter += size;
        BasicWadSeek( cur->wadfile, cur->dir.start);
        CopyBytes( file, cur->wadfile->fileinfo, size);
#ifndef SLIM
		if (GraphFront) {
			SetColor(DARKGRAY);
			DrawScreenBox( 48, 180, 100, 190);
			SetColor(BLUE);
    	}
		if (GraphFront)
			DrawScreenText( 10, 180, "Size: %ld K", counter / 1024);
		else
			printf("Size: %ld K/r", counter / 1024);
#else
        printf("Size: %ld K/r", counter / 1024);
#endif
    }
    
    /* output the directory */
    dirstart = counter;
    counter = 12;
    dirnum = 0;
#ifndef SLIM
    SetColor(BLUE);
	if (GraphFront)
		DrawScreenText( 10, 190, "Building Directory");
	else
		printf("Building Directory\n");
#else
	printf("Building Directory\n");
#endif
    for (cur = MasterDir; cur; cur = cur->next) {
        if (patchonly && cur->wadfile == WadFileList)
            continue;
        /* if (dirnum % 100 == 0) */
        if (cur->dir.start)
            WriteBytes( file, &counter, 4L);
        else
            WriteBytes( file, &(cur->dir.start), 4L);
        WriteBytes( file, &(cur->dir.size), 4L);
        WriteBytes( file, &(cur->dir.name), 8L);
        counter += cur->dir.size;
        dirnum++;
    }
    
    /* fix up the number of entries and directory start information */
    if (fseek( file, 4L, 0))
        ProgError( "error writing to file");
    WriteBytes( file, &dirnum, 4L);
    WriteBytes( file, &dirstart, 4L);
    
    /* close the file */
    fclose( file);
}



/*
   output bytes to a binary file with error checking
   */

void WriteBytes( FILE *file, void huge *addr, long size)
{
    if (!Registered)
        return;
    while (size > 0x8000) {
        if (fwrite( addr, 1, 0x8000, file) != 0x8000)
            ProgError( "error writing to file");
        addr = (char huge *)addr + 0x8000;
        size -= 0x8000;
    }
    if (fwrite( addr, 1, size, file) != size)
        ProgError( "error writing to file");
}



/*
   copy bytes from a binary file to another with error checking
   */

void CopyBytes( FILE *dest, FILE *source, long size)
{
    void huge *data;
    
    if (!Registered)
        return;
    data = GetFarMemory( 0x8000 + 2);
    while (size > 0x8000) {
        if (fread( data, 1, 0x8000, source) != 0x8000)
            ProgError( "error reading from file");
        if (fwrite( data, 1, 0x8000, dest) != 0x8000)
            ProgError( "error writing to file");
        size -= 0x8000;
    }
    if (fread( data, 1, size, source) != size)
        ProgError( "error reading from file");
    if (fwrite( data, 1, size, dest) != size)
        ProgError( "error writing to file");
    FreeFarMemory( data);
}



/*
   check if a file exists and is readable
   */

Bool Exists( char *filename)
{
    FILE *test;
    
    if ((test = fopen( filename, "rb")) == NULL)
        return FALSE;
    fclose( test);
    return TRUE;
}



/*
   dump a directory entry in hex
   */

void DumpDirectoryEntry( FILE *file, char *entryname)
{
    MDirPtr entry;
    char dataname[ 9];
    char key;
    BCINT lines = 5;
    long n, c, i;
    unsigned char buf[16];
    
    
    c = 0;
    entry = MasterDir;
    while (entry) {
        if (!strnicmp( entry->dir.name, entryname, 8)) {
            strncpy( dataname, entry->dir.name, 8);
            dataname[ 8] = '\0';
            fprintf( file, "Contents of entry %s (size = %ld bytes):\n", dataname, entry->dir.size);
            BasicWadSeek( entry->wadfile, entry->dir.start);
            n = 0;
            i = -1;
            for (c = 0; c < entry->dir.size; c += i) {
                fprintf( file, "%04lX: ", n);
                for (i = 0; i < 16; i++) {
                    BasicWadRead( entry->wadfile, &(buf[ i]), 1);
                    fprintf( file, " %02X", buf[ i]);
                    n++;
                }
                fprintf( file, "   ");
                for (i = 0; i < 16; i++) {
                    if (buf[ i] >= 32)
                        fprintf( file, "%c", buf[ i]);
                    else
                        fprintf( file, " ");
                }
                fprintf( file, "\n");
                if (file == stdout && lines++ > 21) {
                    lines = 0;
                    printf( "[%ld%% - Q to abort, S to skip this entry, any other key to continue]", n * 100 / entry->dir.size);
                    key = bioskey( 0);
                    printf( "\r                                                                    \r");
                    if (key == 'S' || key == 's')
                        break;
                    if (key == 'Q' || key == 'q')
                        return;
                }
            }
        }
        entry = entry->next;
    }
    if (! c) {
        printf( "[Entry not in master directory]\n");
        return;
    }
}



/*
   save a directory entry to disk
   */

void SaveDirectoryEntry( FILE *file, char *entryname)
{
    MDirPtr entry;
    long    counter;
    long    size;
    
    for (entry = MasterDir; entry; entry = entry->next)
        if (!strnicmp( entry->dir.name, entryname, 8))
            break;
    if (entry) {
        WriteBytes( file, "PWAD", 4L);     /* PWAD file */
        counter = 1L;
        WriteBytes( file, &counter, 4L);   /* 1 entry */
        counter = 12L;
        WriteBytes( file, &counter, 4L);
        counter = 28L;
        WriteBytes( file, &counter, 4L);
        size = entry->dir.size;
        WriteBytes( file, &size, 4L);
        WriteBytes( file, &(entry->dir.name), 8L);
        BasicWadSeek( entry->wadfile, entry->dir.start);
        CopyBytes( file, entry->wadfile->fileinfo, size);
    }
    else {
        printf( "[Entry not in master directory]\n");
        return;
    }
}



/*
   save a directory entry to disk, without a PWAD header
   */

void SaveEntryToRawFile( FILE *file, char *entryname)
{
    MDirPtr entry;
    
    for (entry = MasterDir; entry; entry = entry->next)
        if (!strnicmp( entry->dir.name, entryname, 8))
            break;
    if (entry) {
        BasicWadSeek( entry->wadfile, entry->dir.start);
        CopyBytes( file, entry->wadfile->fileinfo, entry->dir.size);
    }
    else {
        printf( "[Entry not in master directory]\n");
        return;
    }
}



/*
   encapsulate a raw file in a PWAD file
   */

void SaveEntryFromRawFile( FILE *file, FILE *raw, char *entryname)
{
    long    counter;
    long    size;
    char    name8[ 8];
    
    for (counter = 0L; counter < 8L; counter++)
        name8[ counter] = '\0';
		strncpy( name8, entryname, 8);
    WriteBytes( file, "PWAD", 4L);     /* PWAD file */
    counter = 1L;
    WriteBytes( file, &counter, 4L);   /* 1 entry */
    counter = 12L;
    WriteBytes( file, &counter, 4L);
    counter = 28L;
    WriteBytes( file, &counter, 4L);
    if (fseek( raw, 0L, SEEK_END) != 0)
        ProgError( "error reading from raw file");
    size = ftell( raw);
    if (size < 0)
        ProgError( "error reading from raw file");
    if (fseek( raw, 0L, SEEK_SET) != 0)
        ProgError( "error reading from raw file");
    WriteBytes( file, &size, 4L);
    WriteBytes( file, name8, 8L);
    CopyBytes( file, raw, size);
}



/* end of file */
