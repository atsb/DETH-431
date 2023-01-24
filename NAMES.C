/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   NAMES.C - Object name and type routines.
   */

/* the includes */
#include "deu.h"
#include <assert.h>
#include "dialog.h"

sector_class *Sector_classes = (sector_class *)NULL;
ld_class *Linedef_classes = (ld_class *)NULL;

typedef struct {
	BCINT type;
	ld_type *linedef;
} ld_arr_type;

ld_arr_type *ld_array;
int nLinedefTypes;

/*
   get the name of an object type
   */

char *GetObjectTypeName( BCINT objtype)
{
    switch (objtype) {
		
    case OBJ_THINGS:
		return "Thing";
    case OBJ_LINEDEFS:
		return "LineDef";
    case OBJ_SIDEDEFS:
		return "SideDef";
    case OBJ_VERTEXES:
		return "Vertex";
    case OBJ_SEGS:
		return "Segment";
    case OBJ_SSECTORS:
		return "SSector";
    case OBJ_NODES:
		return "Node";
    case OBJ_SECTORS:
		return "Sector";
    case OBJ_REJECT:
		return "Reject";
    case OBJ_BLOCKMAP:
		return "Blockmap";
    }
    return "< Bug! >";
}



/*
   what are we editing?
   */

char *GetEditModeName( BCINT objtype)
{
    switch (objtype) {
		
    case OBJ_THINGS:
		return "Things";
    case OBJ_LINEDEFS:
    case OBJ_SIDEDEFS:
		return "LineDefs & SideDefs";
    case OBJ_VERTEXES:
		return "Vertices";
    case OBJ_SEGS:
		return "Segments";
    case OBJ_SSECTORS:
		return "Seg-Sectors";
    case OBJ_NODES:
		return "Nodes";
    case OBJ_SECTORS:
		return "Sectors";
    }
    return "< Bug! >";
}


void index_ld_types()
{
	ld_class *c;
	ld_type *t;
	int n = 0;
	ld_arr_type *p;
	
	for(c = Linedef_classes; c; c = c->next)
		for(t = c->types; t; t = t->next)
			n++;
	/* count linedef types */
	
	nLinedefTypes = n;
	p = ld_array = (ld_arr_type *)malloc(n * sizeof(ld_arr_type));
	assert(ld_array);
	
	for(c = Linedef_classes; c; c = c->next)
		for(t = c->types; t; t = t->next) {
			p->type = t->type;
			p->linedef = t;
			p++;
		}
	
	qsort(ld_array, n, sizeof(ld_arr_type), BCINT_lessp);
}

/*
   get a short (2 + 13 char.) description of the type of a linedef
   */

char *GetLineDefTypeName( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	char *p;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->shortname;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	if (BOOMEnable && (p=ParseGeneralizedLinedef(type)))
		return p;
	return("Unknown LineDef");
}


#ifdef GAME_HEXEN

char *GetLineDefArgs( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->args;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("ERROR");
}


char *GetArg1Text( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->arg1text;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("ERROR");
}

char *GetArg2Text( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->arg2text;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("ERROR");
}

char *GetArg3Text( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->arg3text;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("ERROR");
}
char *GetArg4Text( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->arg4text;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("ERROR");
}
char *GetArg5Text( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->arg5text;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("ERROR");
}
#endif



/*
   get a long description of the type of a linedef
   */

char *GetLineDefTypeLongName( BCINT type)
{
	int lowbound = 0, highbound = nLinedefTypes - 1, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		
		t = ld_array[middle].type;
		if(t == type)
			return ld_array[middle].linedef->longname;
		
		if(t < type)
			lowbound = middle + 1;
		else
			highbound = middle - 1;
	} while(lowbound <= highbound);
	
	return("Unknown LineDef");
}


/*
   get a short description of the flags of a linedef
   */

char *GetLineDefFlagsName( BCINT flags)
{
#ifdef GAME_HEXEN
    static char temp[ 26];
    if (flags & 0x1000)
		strcpy( temp, ""); 
    else
		strcpy( temp, "");
    if (flags & 0x0800)
		strcat( temp, ""); 
    else
		strcat( temp, "");
    if (flags & 0x0400)
		strcat( temp, ""); 
    else
		strcat( temp, "");
    if (flags & 0x0200)
		strcat( temp, "Re"); 
    else
		strcat( temp, "-");
    if (flags & 0x0100)
		strcat( temp, "Ma"); /* Already on the map */
    else
		strcat( temp, "-");
#else
    static char temp[ 22];
    if (flags & 0x0200)
		strcpy( temp, "Ut"); /* Use works through line */ //jff 3/21/98 add flag
    else
		strcpy( temp, "-");
    if (flags & 0x0100)
		strcat( temp, "Ma"); /* Already on the map */
    else
		strcat( temp, "-");
#endif
    if (flags & 0x80)
		strcat( temp, "In"); /* Invisible on the map */
    else
		strcat( temp, "-");
    if (flags & 0x40)
		strcat( temp, "So"); /* Blocks sound */
    else
		strcat( temp, "-");
    if (flags & 0x20)
		strcat( temp, "Se"); /* Secret (normal on the map) */
    else
		strcat( temp, "-");
    if (flags & 0x10)
		strcat( temp, "Lo"); /* Lower texture offset changed */
    else
		strcat( temp, "-");
    if (flags & 0x08)
		strcat( temp, "Up"); /* Upper texture offset changed */
    else
		strcat( temp, "-");
    if (flags & 0x04)
		strcat( temp, "2S"); /* Two-sided */
    else
		strcat( temp, "-");
    if (flags & 0x02)
		strcat( temp, "Mo"); /* Monsters can't cross this line */
    else
		strcat( temp, "-");
    if (flags & 0x01)
		strcat( temp, "Im"); /* Impassible */
    else
		strcat( temp, "-");
    return temp;
}


/*
   get a short description of the flags of a linedef
   */

#ifdef GAME_HEXEN

char *GetThingFlagsName( BCINT flags)
{
    static char temp[ 26];
    if (flags & 0x01)
		strcpy( temp, "E"); 
    else
		strcpy( temp, "-");
    if (flags & 0x02)
		strcat( temp, "M"); 
    else
		strcat( temp, "-");
    if (flags & 0x04)
		strcat( temp, "H|");
    else
		strcat( temp, "-|");
    if (flags & 0x20)
		strcat( temp, "F"); 
    else
		strcat( temp, "-");
    if (flags & 0x40)
		strcat( temp, "C"); 
    else
		strcat( temp, "-");
    if (flags & 0x80)
		strcat( temp, "M|"); 
    else
		strcat( temp, "-|");
    if (flags & 0x0100)
		strcat( temp, "S"); 
    else
		strcat( temp, "-");
    if (flags & 0x0200)
		strcat( temp, "C"); 
    else
		strcat( temp, "-");
    if (flags & 0x0400)
		strcat( temp, "D "); 
    else
		strcat( temp, "- ");
    if (flags & 0x08)
		strcat( temp, "Deaf "); 
    else
		strcat( temp, " ");
    if (flags & 0x10)
		strcat( temp, "Dormant"); 
    else
		strcat( temp, "");
    return temp;
}

#else
char *GetThingFlagsName( BCINT flags)
{
    static char temp[ 12];
    if (flags & 0x01)
			strcpy( temp, "Easy "); 
    else
			strcpy( temp, "");
    if (flags & 0x02)
			strcat( temp, "Med "); 
    else
			strcat( temp, "");
    if (flags & 0x04)
			strcat( temp, "Hard ");
    else
			strcat( temp, "");
    if (flags & 0x08)
			strcat( temp, "Deaf "); 
    else
			strcat( temp, "");
    if (flags & 0x10)
			strcat( temp, "NotS"); 
    else
			strcat( temp, "");
    if (flags & 0x20)
			strcat( temp, "NotD"); 
    else
			strcat( temp, "");
    if (flags & 0x40)
			strcat( temp, "NotC"); 
    else
			strcat( temp, "");
    return temp;
}
#endif


/*
   get a long description of one linedef flag
   */

char *GetLineDefFlagsLongName( BCINT flags)
{
#ifdef GAME_HEXEN
    if (flags & 0x0200)
		return "Repeatable";
#endif
    if (flags & 0x0200)
		return "Use Works Thru the Line";	 //jff 3/21/98 new linedef flag
    if (flags & 0x0100)
		return "Already On The Map At Startup";
    if (flags & 0x80)
		return "Invisible On The Map";
    if (flags & 0x40)
		return "Blocks Sound";
    if (flags & 0x20)
		return "Secret (Normal On Map)";
    if (flags & 0x10)
		return "Lower Texture Is \"Unpegged\"";
    if (flags & 0x08)
		return "Upper Texture Is \"Unpegged\"";
    if (flags & 0x04)
		return "Two-Sided (May Be Transparent)";
    if (flags & 0x02)
		return "Monsters Cannot Cross This Line";
    if (flags & 0x01)
		return "Impassible";
    return "Unknown";
}

/*
   get a long description of one thing flag
   */

#ifdef GAME_HEXEN
char *GetThingFlagsLongName( BCINT flags)
{
    if (flags & 0x0400)
		return "Deathmatch";
    if (flags & 0x0200)
		return "Cooperative";
    if (flags & 0x0100)
		return "Single-player";
    if (flags & 0x80)
		return "Mage";
    if (flags & 0x40)
		return "Cleric";
    if (flags & 0x20)
		return "Fighter";
    if (flags & 0x10)
		return "Dormant";
    if (flags & 0x08)
		return "Deaf";
    if (flags & 0x04)
		return "Hard";
    if (flags & 0x02)
		return "Medium";
    if (flags & 0x01)
		return "Easy";
    return "Unknown";
}
#else
char *GetThingFlagsLongName( BCINT flags)
{
    if (flags & 0x80)
		return "Reserved";
    if (flags & 0x40)
		return "Not Coop";
    if (flags & 0x20)
		return "Not DM";
    if (flags & 0x10)
		return "Not Single";
    if (flags & 0x08)
		return "Deaf";
    if (flags & 0x04)
		return "Hard";
    if (flags & 0x02)
		return "Medium";
    if (flags & 0x01)
		return "Easy";
    return "Unknown";
}
#endif

/*
   get a short (19 char.) description of the type of a sector
   */

char *GetSectorTypeName( BCINT type)
{
	sector_class *c;
	sector_type *t;
	char *p;
	
	for(c = Sector_classes; c; c = c->next)
		for(t = c->types; t; t = t->next)
			if(type == t->type)
				return t->shortname;
	
	if (BOOMEnable && (p=ParseGeneralizedSectorType(type)))
		return p;
  return "Unknown Sector!";
}



/*
   get a long description of the type of a sector
   */

char *GetSectorTypeLongName( BCINT type)
{
	sector_class *c;
	sector_type *t;
	char *p;
	
	for(c = Sector_classes; c; c = c->next)
		for(t = c->types; t; t = t->next)
			if(type == t->type)
				return t->longname;
	
	if (BOOMEnable && (p=ParseGeneralizedSectorType(type)))
		return p;
  return "Unknown Sector!";
}

#ifdef GAME_HEXEN
BCINT SelectPuzzleItem()
{
	int val;
	val = DisplayMenu( -1, -1, "Select Puzzle Item:",
		"Skull             [  1]",
		"Heart             [  2]",
		"Ruby              [  3]",
		"Emerald           [  4]",
		"Codex             [  5]",
		"Liber             [  6]",
		"Emerald2          [  7]",
		"Sapphire2         [  8]",
		"Flame Mask        [  9]",
		"Glaive Seal       [ 10]",
		"Holy Relic        [ 11]",
		"Sigil             [ 12]",
		"Gear1             [ 13]",
		"Gear2             [ 14]",
		"Gear3             [ 15]",
		"Gear4             [ 16]",
		"-",
		"Enter a Decimal Value",
		NULL);
	if (val == 18)
		val = InputIntegerValue( -1, -1, 0, 255, 0);
	if (val == 19)
		return -1;
	return val;
}

BCINT SelectSectorSound()
{
	int val;
	val = DisplayMenu( -1, -1, "Choose Sector Sound:",
		"Heavy             [  1]",
		"Metal             [  2]",
		"Creak             [  3]",
		"Silence           [  4]",
		"Lava              [  5]",
		"Water             [  6]",
		"Ice               [  7]",
		"Earth             [  8]",
		"Metal2            [  9]",
		"-",
		"Enter a Decimal Value",
		NULL);
	if (val == 10)
		val = InputIntegerValue( -1, -1, 0, 255, 0);
	if (val == 11)
		return -1;
	return val;
}


BCINT SelectKeyType()
{
	int val;
	val = DisplayMenu( -1, -1, "Choose Key Type:",
		"Steel Key         [  1]",
		"Cave Key          [  2]",
		"Axe Key           [  3]",
		"Fire Key          [  4]",
		"Emerald Key       [  5]",
		"Dungeon Key       [  6]",
		"Silver Key        [  7]",
		"Rusted Key        [  8]",
		"Waste Key         [  9]",
		"Swamp Key         [ 10]",
		"Castle Key        [ 11]",
		"-",
		"Enter a Decimal Value",
		NULL);
	if (val == 12)
		val = InputIntegerValue( -1, -1, 0, 255, 0);
	if (val == 13)
		return -1;
	return val;
}

BCINT SelectHexenAngle()
{
	int val;
	val = DisplayMenu( -1, -1, "Choose Angle:",
		"North             [ 64]",
		"NorthEast         [ 32]",
		"East              [  0]",
		"SouthEast         [224]",
		"South             [192]",
		"SouthWest         [160]",
		"West              [128]",
		"NorthWest         [ 96]",
		"-",
		"Enter a Decimal Value",
		NULL);
	switch (val) {
	    case 1:
		    val = 64;
		    break;
	    case 2:
		    val = 32;
		    break;
	    case 3:
		    val = 0;
		    break;
	    case 4:
		    val = 224;
		    break;
	    case 5:
		    val = 192;
		    break;
	    case 6:
		    val = 160;
		    break;
	    case 7:
		    val = 128;
		    break;
	    case 8:
		    val = 96;
		    break;
		case 9:
			return -1;
	    case 10:
		    val = InputIntegerValue( -1, -1, 0, 255, 0);
	}
	return val;
}

BCINT SelectNegative()
{
	int val;
	val = DisplayMenu( -1, -1, "Choose if Negative Value:",
		"Yes               [  1]",
		"No                [  0]",
		"-",
		"Enter a Decimal Value",
		NULL);
	switch (val) {
	    case 1:
		    val = 1;
		    break;
	    case 2:
		    val = 0;
		    break;
		case 3:
			return -1;
	    case 4:
		val = InputIntegerValue( -1, -1, 0, 255, 0);
	}
	return val;
}

char *GetPuzzleItemName( BCINT val)
{
	switch (val) {
		case 1:
			return "(Skull)";
		case 2:
			return "(Heart)";
		case 3:
			return "(Ruby)";
		case 4:
			return "(Emerald)";
		case 5:
			return "(Codex)";
		case 6:
			return "(Liber)";
		case 7:
			return "(Emerald2)";
		case 8:
			return "(Sapphire2)";
		case 9:
			return "(Flame Mask)";
		case 10:
			return "(Glaive Seal)";
		case 11:
			return "(Holy Relic)";
		case 12:
			return "(Sigil)";
		case 13:
			return "(Gear1)";
		case 14:
			return "(Gear2)";
		case 15:
			return "(Gear3)";
		case 16:
			return "(Gear4)";
	}
	return "";
}

char *GetSectorSoundName( BCINT val)
{
	switch (val) {
		case 1:
			return "(Heavy)";
		case 2:
			return "(Metal)";
		case 3:
			return "(Creak)";
		case 4:
			return "(Silence)";
		case 5:
			return "(Lava)";
		case 6:
			return "(Water)";
		case 7:
			return "(Ice)";
		case 8:
			return "(Earth)";
		case 9:
			return "(Metal2)";
	}
	return "";
}

char *GetKeyTypeName( BCINT val)
{
	switch (val) {
		case 1:
			return "(Steel)";
		case 2:
			return "(Cave)";
		case 3:
			return "(Axe)";
		case 4:
			return "(Fire)";
		case 5:
			return "(Emerald)";
		case 6:
			return "(Dungeon)";
		case 7:
			return "(Silver)";
		case 8:
			return "(Rusted)";
		case 9:
			return "(Waste)";
		case 10:
			return "(Swamp)";
		case 11:
			return "(Castle)";
	}
	return "";
}

char *GetNegativeName( BCINT val)
{
	switch (val) {
		case 0:
			return "(No)";
		case 1:
			return "(Yes)";
	}
	return "";
}

char *GetHexenAngleName( BCINT val)
{
	switch (val) {
		case 0:
			return "(East)";
		case 32:
			return "(NorthEast)";
		case 64:
			return "(North)";
		case 96:
			return "(NorthWest)";
		case 128:
			return "(West)";
		case 160:
			return "(SouthWest)";
		case 192:
			return "(South)";
		case 224:
			return "(SouthEast)";
	}
	return "";
}
#endif

/* end of file */

