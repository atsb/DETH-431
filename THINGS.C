/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   THINGS.C - Thing name and type routines.
   */

/* the includes */
#include "deu.h"
#include <assert.h>

#define _WHITE	15

thing_class *Thing_classes;
#ifdef GAME_HEXEN
spawn_class *Spawn_classes;
#endif
scheme_class *Scheme_classes;

typedef struct {
	BCINT type;
	thing_type *thing;
} thing_arr;

typedef struct {
	BCINT type;
	scheme_type *scheme;
} scheme_arr;

#ifdef GAME_HEXEN
	typedef struct {
		BCINT type;
		spawn_type *spawn;
	} spawn_arr;
#endif

thing_arr *Thing_type_array;
int maxThing;

scheme_arr *Scheme_type_array;
int maxScheme;

#ifdef GAME_HEXEN
	spawn_arr *Spawn_type_array;
	int maxSpawn;
#endif

int BCINT_lessp(const void *x, const void *y)
{
	return (int)(*((BCINT *)x) - *((BCINT *)y));
}

void index_things()
{
	thing_class *c;
	thing_type *t;
	thing_arr *p;
	int nThingTypes = 0;
	
	for(c = Thing_classes; c; c = c->next)
		for(t = c->types; t; t = t->next)
			nThingTypes++;
	
	p = Thing_type_array = (thing_arr *)malloc(nThingTypes * sizeof(thing_arr));
	assert(Thing_type_array);
	
	for(c = Thing_classes; c; c = c->next)
		for(t = c->types; t; t = t->next) {
			p->type = t->type;
			p->thing = t;
			p++;
		}
	
	qsort(Thing_type_array, nThingTypes, sizeof(thing_arr),
		  BCINT_lessp);
	maxThing = nThingTypes - 1;
}

void index_schemes()
{
	scheme_class *c;
	scheme_type *t;
	scheme_arr *p;
	int nSchemeTypes = 0;
	
	for(c = Scheme_classes; c; c = c->next)
		for(t = c->types; t; t = t->next)
			nSchemeTypes++;
	
	p = Scheme_type_array = (scheme_arr *)malloc(nSchemeTypes * sizeof(scheme_arr));
	assert(Scheme_type_array);
	
	for(c = Scheme_classes; c; c = c->next)
		for(t = c->types; t; t = t->next) {
			p->type = t->type;
			p->scheme = t;
			p++;
		}
	
	qsort(Scheme_type_array, nSchemeTypes, sizeof(scheme_arr),
		  BCINT_lessp);
	maxScheme = nSchemeTypes - 1;
}


#ifdef GAME_HEXEN
void index_spawns()
{
	spawn_class *c;
	spawn_type *t;
	spawn_arr *p;
	int nSpawnTypes = 0;
	
	for(c = Spawn_classes; c; c = c->next)
		for(t = c->types; t; t = t->next)
			nSpawnTypes++;
	
	p = Spawn_type_array = (spawn_arr *)malloc(nSpawnTypes * sizeof(spawn_arr));
	assert(Spawn_type_array);
	
	for(c = Spawn_classes; c; c = c->next)
		for(t = c->types; t; t = t->next) {
			p->type = t->type;
			p->spawn = t;
			p++;
		}
	
	qsort(Spawn_type_array, nSpawnTypes, sizeof(spawn_arr),
		  BCINT_lessp);
	maxSpawn = nSpawnTypes - 1;
}
#endif


/*
   get the colour of a thing
   */

BCINT GetThingColour( BCINT type)
{
	BCINT col = _WHITE, t;
	int lowbound = 0, highbound = maxThing;
	int middle;
	
	do {
		middle = (lowbound + highbound) / 2;
		t = Thing_type_array[middle].type;
		if(t == type) {
			col = (Thing_type_array[middle].thing)->colour;
			goto found_it;
		}
		
		if(t > type)
			highbound = middle - 1;
		else
			lowbound = middle + 1;
	} while(lowbound <= highbound);
    
    
 found_it:
    return((col == _WHITE) ? WHITE : col);
}



/*
   get the name of a thing
   */

char *GetThingName( BCINT type)
{
   	int lowbound = 0, highbound = maxThing, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		t = Thing_type_array[middle].type;
		
		if(t == type)
			return Thing_type_array[middle].thing->name;
		
		if(t > type)
			highbound = middle - 1;
		else
			lowbound = middle + 1;
	} while(lowbound <= highbound);
    
    /* fall through */
    return("Unknown thing type");
}

#ifdef GAME_HEXEN
/*
   get the name of a spawnable thing
   */

char *GetSpawnName( BCINT arg2)
{
   	int lowbound = 0, highbound = maxSpawn, middle;
	BYTE t;
	
	do {
		middle = (lowbound + highbound) / 2;
		t = Spawn_type_array[middle].type;
		
		if(t == arg2)
			return Spawn_type_array[middle].spawn->name;
		
		if(t > arg2)
			highbound = middle - 1;
		else
			lowbound = middle + 1;
	} while(lowbound <= highbound);
    
    if (arg2 <= 0) 
		return("None");
    else
		return("ERROR!");
}
#endif

/*
   get the name of the sprite to show
   */
char *GetThingSprite( BCINT type)
{
   	int lowbound = 0, highbound = maxThing, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		t = Thing_type_array[middle].type;
		
		if(t == type)
			return Thing_type_array[middle].thing->sprite;
		
		if(t > type)
			highbound = middle - 1;
		else
			lowbound = middle + 1;
	} while(lowbound <= highbound);
    
    /* fall through */
    return("");
}

#ifdef GAME_HEXEN
/*
   get the name of the spawn sprite to show
   */


char *GetSpawnSprite( BCINT type)
{
   	int lowbound = 0, highbound = maxSpawn, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		t = Spawn_type_array[middle].type;
		
		if(t == type)
			return Spawn_type_array[middle].spawn->sprite;
		
		if(t > type)
			highbound = middle - 1;
		else
			lowbound = middle + 1;
	} while(lowbound <= highbound);
    
    /* fall through */
    return("");
}

#endif

/*
   get the size of a thing
   */

BCINT GetThingRadius( BCINT type)
{
   	int lowbound = 0, highbound = maxThing, middle;
	BCINT t;
	
	do {
		middle = (lowbound + highbound) / 2;
		t = Thing_type_array[middle].type;
		
		if(t == type)
			return Thing_type_array[middle].thing->radius;
		
		if(t > type)
			highbound = middle - 1;
		else
			lowbound = middle + 1;
	} while(lowbound <= highbound);
    
    /* fall through */
    return(16);
}


/*
   get the name of the angle
   */

char *GetAngleName( BCINT angle)
{
    switch (angle) {
    case 0:
		return "(East)";
    case 45:
		return "(NorthEast)";
    case 90:
		return "(North)";
    case 135:
		return "(NorthWest)";
    case 180:
		return "(West)";
    case 225:
		return "(SouthWest)";
    case 270:
		return "(South)";
    case 315:
		return "(SouthEast)";
    }
    return "Unknown";
}


#ifdef GAME_HEXEN

/*
   get string of when something will appear
   */

char *GetWhenName( BCINT when)
{
    static char temp[ 60];
    
    temp[ 0] = '\0';
    if (when & 0x01)
		strcat( temp, "E");
    if (when & 0x02)
		strcat( temp, "M");
    if (when & 0x04)
		strcat( temp, "H ");
    if (when & 0x08)
		strcat( temp, "Deaf ");
    if (when & 0x10)
		strcat( temp, "Dormant ");
    if (when & 0x20)
		strcat( temp, "F");
    if (when & 0x40)
		strcat( temp, "C");
    if (when & 0x80)
		strcat( temp, "M ");
    if (when & 0x100)
		strcat( temp, "S");
    if (when & 0x200)
		strcat( temp, "C");
    if (when & 0x400)
		strcat( temp, "D ");
    return temp;
}

int SetThingFlagFilter(int old)
{
	int new = old;
	/* war == peace */
	
	int selection;

	MenuDefault = 0;
	do {
		selection = DisplayMenu(-1, -1, "Set Filter for Things",
								(new & TF_EASY) ? "* Easy" : "  Easy",
								(new & TF_MEDIUM) ? "* Medium" : "  Medium",
								(new & TF_HARD) ? "* Hard" : "  Hard",
								(new & TF_FIGHTER) ? "* Fighter" : "  Fighter",
								(new & TF_CLERIC) ? "* Cleric" : "  Cleric",
								(new & TF_MAGE) ? "* Mage" : "  Mage",
								(new & TF_SINGLE) ? "* Single" : "  Single",
								(new & TF_COOP) ? "* Cooperative" : "  Cooperative",
								(new & TF_DEATHM) ? "* Death Match" : "  Death Match",
								"  Cancel", NULL);

		switch(selection) {
		case 0: return new;
		case 1:	new ^= TF_EASY; break;
		case 2:	new ^= TF_MEDIUM; break;
		case 3:	new ^= TF_HARD; break;
		case 4:	new ^= TF_FIGHTER; break;
		case 5:	new ^= TF_CLERIC; break;
		case 6:	new ^= TF_MAGE; break;
		case 7:	new ^= TF_SINGLE; break;
		case 8:	new ^= TF_COOP; break;
		case 9:	new ^= TF_DEATHM; break;
		case 10:	return old;
		}
		MenuDefault = selection - 1;
	} while(1);
}
#else
/*
   get string of when something will appear
   */

char *GetWhenName( BCINT when)
{
    static char temp[ 20];
    
    temp[ 0] = '\0';
    if (when & 0x01)
		strcat( temp, "E");
    if (when & 0x02)
		strcat( temp, "M");
    if (when & 0x04)
		strcat( temp, "H ");
    if (when & 0x08)
		strcat( temp, "Deaf ");
    if (when & 0x10)
		strcat( temp, "Multi");
    return temp;
}

int SetThingFlagFilter(int old)
{
	int new = old;
	/* war == peace */
	
	int selection;

	MenuDefault = 0;
	do {
		selection = DisplayMenu(-1, -1, "Set Filter for Things",
								(new & TF_EASY) ? "* Easy" : "  Easy",
								(new & TF_MEDIUM) ? "* Medium" : "  Medium",
								(new & TF_HARD) ? "* Hard" : "  Hard",
								(new & TF_DM) ? "* Show Multi Player Things"
								: "  Show Multi Player Things",
								(new & TF_NOT_DM) ? "* Show non Multi Player Things"
								: "  Show non Multi Player Things",
								"  Cancel", NULL);

		switch(selection) {
		case 0: return new;
		case 1:	new ^= TF_EASY; break;
		case 2:	new ^= TF_MEDIUM; break;
		case 3:	new ^= TF_HARD; break;
		case 4: new ^= TF_DM; break;
		case 5: new ^= TF_NOT_DM; break;
		case 6:	return old;
		}
		MenuDefault = selection - 1;
	} while(1);
}
#endif

/* end of file */

