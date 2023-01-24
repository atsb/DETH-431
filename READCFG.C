/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   READCFG.C, by Simon Oke.
   */

#include "deu.h"

#include <assert.h>
#include <stdio.h>

#ifdef NULL
#undef NULL
#define NULL ((char *)0x0L)
#endif


BCINT getcol(char *str);
void readcfg(char *file);
void add_thing(char *line);
void add_thing_class(char *line);
#ifdef GAME_HEXEN
void add_spawn(char *line);
void add_spawn_class(char *line);
#endif
void add_scheme(char *line);
void add_scheme_class(char *line);
void add_linedef(char *line);
void add_linedef_class(char *line);
void add_sector_type(char *line);
void add_sector_class(char *line);
void add_levels(char *line);
void add_texture_sections(char *line);
void add_ftexture_sections(char *line);
void get_palette_colour(char *line);
int read_line(FILE *f, char *buf, int buflen);

/* stuff for manipulating SList s */
SList SList_append(SList l, char *str);
SList SList_find(SList l, char *str);

extern BCINT Palette[];

/* what all the various sections in the cfg file are */
typedef enum {
	NONE = 0,	/* haven't got a section header yet */
	WAD,
	THINGS,
#ifdef GAME_HEXEN
	SPAWNABLE,
#endif
	SPECIALS,
	LEVELS,
	TEXTURES,
	FTEXTURES,
	REGTEST,
	SECTORS,
	COLORS,
	SCHEMES,
	NUM_SECTIONS
} cfg_section;

char *header[NUM_SECTIONS] = {
    /* these go hand in hand with the numbers above */
	"",
	"[wad]",
	"[things]",
#ifdef GAME_HEXEN
	"[spawnable]",
#endif
	"[specials]",
	"[levels]",
	"[textures]",
	"[ftextures]",
	"[regtest]",
	"[sectors]",
	"[colors]",
	"[schemes]"
};

char *colours[32] = {
	"BLACK", "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN", "LIGHTGRAY",
	"DARKGRAY", "LIGHTBLUE", "LIGHTGREEN", "LIGHTCYAN", "LIGHTRED",
	"LIGHTMAGENTA", "YELLOW", "WHITE", "DARKBLUE", "DARKGREEN", "DARKRED",
	"DARKMAGENTA", "GRAY", "DARKERGRAY", "ORANGE", "SECTORTAGGED", "SECTORSECRET",
	"SECTORPAINFUL", "SECTORLIGHT", "LINEDEFTAGGED", "LINEDEFSECRET",
	"LINEDEFNOSOUND", "LINEDEFNOPASS", "LINEDEFNOMAP"
};

ld_class *linedef_class = (ld_class *)NULL;
/* what was the last linedef class we had? */

thing_class *current_thing_class = (thing_class *)NULL;
/* ditto for thing class */

#ifdef GAME_HEXEN
spawn_class *current_spawn_class = (spawn_class *)NULL;
/* ditto for spawn class */
#endif

sector_class *current_sector_class = (sector_class *)NULL;
/* ditto for sectors */

scheme_class *current_scheme_class = (scheme_class *)NULL;
/* ditto for texture schemes */


BCINT getcol(char *str)
{
	int i = -1;
	
	/* first, do the easy bit -- is the colour specified as a number? */
	if(sscanf(str, "%d", &i))
		return (BCINT)i;
	
	/* check it against the colour names */
	for(i = 0; i < 32; i++)
		if(!stricmp(str, colours[i]))
			return (BCINT)i;
	
	return (BCINT)-1;
}


void readcfg(char *file)
{
	static cfg_section what = NONE;
	/* what section are we currently reading? */
	
	FILE *thefile;
	char buf[256], *tok;
	
	int i;
	
	if(!(thefile = fopen(file, "rb")))
		return;
	
	/* OK, we opened the file */
	while(read_line(thefile, buf, 255)) {
		/* first, see whether it's just a comment */
		if(*buf == '#')
			goto nextline;
		
		if(!strnicmp("include", buf, 7)) {
			tok = strtok(buf + 7, "\t ");
			readcfg(tok);
			goto nextline;
		}
		
		for(i = 0; i < NUM_SECTIONS; i++) {
			if(!stricmp(buf, header[i])) {
				what = i;
				goto nextline;
			}
		}
		
		switch(what) {
		case NONE:
		case NUM_SECTIONS:
			break;
			
		case WAD:
			MainWad = strdup(buf);
			break;
			
		case THINGS:
			if(buf[0] == '"' && buf[strlen(buf) - 1] ==  '"')
				add_thing_class(buf);
			else
				add_thing(buf);
			break;
			
		case SCHEMES:
			if(buf[0] == '"' && buf[strlen(buf) - 1] ==  '"')
				add_scheme_class(buf);
			else
				add_scheme(buf);
			break;

#ifdef GAME_HEXEN
		case SPAWNABLE:
			if(buf[0] == '"' && buf[strlen(buf) - 1] ==  '"')
				add_spawn_class(buf);
			else
				add_spawn(buf);
			break;
#endif

		case SPECIALS:
			if(buf[0] == '"' && buf[strlen(buf) - 1] == '"')
				add_linedef_class(buf);
			else	
				add_linedef(buf);
			break;
			
		case SECTORS:
			if(buf[0] == '"' && buf[strlen(buf) - 1] == '"')
			{
				add_sector_class(buf);
			}
			else if (strlen(buf) && buf[0]!='\n' && buf[0]!='\r')
			{
				add_sector_type(buf);
			}
			break;
			
		case REGTEST:
			RegTest = strdup(buf);
			break;
			
		case LEVELS:
			add_levels(buf);
			break;
			
		case TEXTURES:
			printf("Textures\n");
			add_texture_sections(buf);
			break;
			
		case FTEXTURES:
			add_ftexture_sections(buf);
			break;
			
		case COLORS:
			get_palette_colour(buf);
			break;
		}
		
	nextline:
		;
	}
	
	fclose(thefile);
}
			

void add_thing(char *line)
{
	BCINT col, radius, type;
	char *tok, *sprite, *name;
	thing_type *p, *new = (thing_type *)NULL;
	
	if(!current_thing_class)
		return;
	
	tok = strtok(line, "\t ,");
	if(tok && (type = atoi(tok))) {
		tok = strtok(NULL, "\t ,");
		if(tok && (col = getcol(tok)) != -1) {
			tok = strtok(NULL, "\t ,");
			if(tok && (radius = atoi(tok))) {

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		name = strdup(tok);
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		sprite = strdup(tok);
		

					/* do the add here */
					for(p = current_thing_class->types;
						(p && p->next); p = p->next);
					
					new = (thing_type *)malloc(sizeof(thing_type));
					assert(new);
					
					new->type = type;
					new->colour = col;
					new->radius = radius;
					new->name = name;
					new->sprite = sprite;
					new->next = (thing_type *)NULL;
					
					if(p)
						p->next = new;
					else
						current_thing_class->types = new;
				}
			}
		}
}

void add_thing_class(char *line)
{
	thing_class *p, *new = (thing_class *)NULL;
	
	line[strlen(line) - 1] = '\0';
	line++;
	/* add it here */
	
	for(p = Thing_classes; (p && p->next); p = p->next)
		if(!strcmp(line, p->name)) {
			current_thing_class = p;
			return;
		}
	
	new = (thing_class *)malloc(sizeof(thing_class));
	assert(new);
	
	new->name = strdup(line);
	new->next = (thing_class *)NULL;
	new->types = (thing_type *)NULL;	/*JFF initialize this pointer*/
	
	if(p)
		p->next = new;
	else
		Thing_classes = new;
	
	current_thing_class = new; 
}




#ifdef GAME_HEXEN
void add_spawn(char *line)
{
	BCINT type;
	char *tok, *sprite, *name;
	spawn_type *p, *new = (spawn_type *)NULL;
	
	if(!current_spawn_class)
		return;

	tok = strtok(line, "\t ,");
	if(tok && (type = atoi(tok)) != -1) {
	
/*	if(tok && (type = atoi(tok))) { */

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		name = strdup(tok);
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		sprite = strdup(tok);
		

					/* do the add here */
					for(p = current_spawn_class->types;
						(p && p->next); p = p->next);
					
					new = (spawn_type *)malloc(sizeof(spawn_type));
					assert(new);
					
					new->type = type;
					new->name = name;
					new->sprite = sprite;
					new->next = (spawn_type *)NULL;
					
					if(p)
						p->next = new;
					else
						current_spawn_class->types = new;
	}
}

void add_spawn_class(char *line)
{
	spawn_class *p, *new = (spawn_class *)NULL;
	
	line[strlen(line) - 1] = '\0';
	line++;
	/* add it here */
	
	for(p = Spawn_classes; (p && p->next); p = p->next)
		if(!strcmp(line, p->name)) {
			current_spawn_class = p;
			return;
		}
	
	new = (spawn_class *)malloc(sizeof(spawn_class));
	assert(new);
	
	new->name = strdup(line);
	new->next = (spawn_class *)NULL;
	new->types = (spawn_type *)NULL;    /*JFF initialize this pointer*/	
	if(p)
		p->next = new;
	else
		Spawn_classes = new;
	
	current_spawn_class = new; 
}



void add_linedef(char *line)
{
	char *short_desc, *long_desc, *args, *tok, *arg1text, *arg2text, *arg3text, *arg4text, *arg5text;
	BCINT type = -1;
	ld_type *p, *new = (ld_type *)NULL;
	
	if(!linedef_class)
		return;
	/* can't add a linedef if we don't know what class
	   to put it in */
	
	tok = strtok(line, "\t ,");
	if(tok && (type = atoi(tok)) != -1) {
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the short description */
		short_desc = strdup(tok);
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the long description */
		long_desc = strdup(tok);
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the argument types */
		args = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the argument types */
		arg1text = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the argument types */
		arg2text = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the argument types */
		arg3text = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the argument types */
		arg4text = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the argument types */
		arg5text = strdup(tok);

		/* now add it */
		for(p = linedef_class->types; (p && p->next); p = p->next);
		/* make p point to the last item in the list, or be NULL */
		
		new = (ld_type *)malloc(sizeof(ld_type));
		assert(new);
		
		new->longname = long_desc;
		new->shortname = short_desc;
		new->args = args;
		new->arg1text = arg1text;
		new->arg2text = arg2text;
		new->arg3text = arg3text;
		new->arg4text = arg4text;
		new->arg5text = arg5text;
		new->type = type;
		new->next = (ld_type *)NULL;

		if(p)
			p->next = new;
		else
			linedef_class->types = new;
	}
}
#else
void add_linedef(char *line)
{
	char *short_desc, *long_desc, *tok ;
	BCINT type = -1;
	ld_type *p, *new = (ld_type *)NULL;
	
	if(!linedef_class)
		return;
	/* can't add a linedef if we don't know what class
	   to put it in */
	
	tok = strtok(line, "\t ,");
	if(tok && (type = atoi(tok)) != -1) {
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the short description */
		short_desc = strdup(tok);
		if (!BOOMEnable && islower(short_desc[0]))
			return;
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the long description */
		long_desc = strdup(tok);
		
		/* now add it */
		for(p = linedef_class->types; (p && p->next); p = p->next);
		/* make p point to the last item in the list, or be NULL */
		
		new = (ld_type *)malloc(sizeof(ld_type));
		assert(new);
		
		new->longname = long_desc;
		new->shortname = short_desc;
		new->type = type;
		new->next = (ld_type *)NULL;

		if (type>MaxExtendedType)
			MaxExtendedType = type;

		if(p)
			p->next = new;
		else
			linedef_class->types = new;
	}
}

#endif

void add_linedef_class(char *line)
{
	ld_class *p, *new = (ld_class *)NULL;
	
	line++;
	line[strlen(line) - 1] = '\0';
	/* lose the quotes */
	
	for(p = Linedef_classes; (p && p->next); p = p->next)
		if(!strcmp(p->name, line)) {
			linedef_class = p;
			return;
			/* we already had it */
		}
	
	/* p now points to the last item in the list, or is NULL
	   if there is no list. */
	
	new = (ld_class *)malloc(sizeof(ld_class));
	assert(new);
	/* get memory */
	
	new->name = strdup(line);
	new->types = (ld_type *)NULL;
	new->next = (ld_class *)NULL;
	/* fill in the fields */
	
	if(p)
		p->next = new;
	else
		Linedef_classes = new;
	
	linedef_class = new;
}

void add_scheme(char *line)
{
	BCINT type;
	char *tok, *name, *normal, *upper, *lower, *floor, *ceiling;
	scheme_type *p, *new = (scheme_type *)NULL;
	
	if(!current_scheme_class)
		return;

	tok = strtok(line, "\t ,");
	if(tok && (type = atoi(tok)) != -1) {
	
/*	if(tok && (type = atoi(tok))) { */

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		name = strdup(tok);
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		normal = strdup(tok);
		
		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		upper = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		lower = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		floor = strdup(tok);

		tok = strtok(NULL, "\t,\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		ceiling = strdup(tok);

					/* do the add here */
					for(p = current_scheme_class->types;
						(p && p->next); p = p->next);
					
					new = (scheme_type *)malloc(sizeof(scheme_type));
					assert(new);
					
					new->type = type;
					new->name = name;
					new->normal = normal;
					new->upper = upper;
					new->lower = lower;
					new->floor = floor;
					new->ceiling = ceiling;

					new->next = (scheme_type *)NULL;
					
					if(p)
						p->next = new;
					else
						current_scheme_class->types = new;
	}
}

void add_scheme_class(char *line)
{
	scheme_class *p, *new = (scheme_class *)NULL;
	
	line[strlen(line) - 1] = '\0';
	line++;
	/* add it here */
	
	for(p = Scheme_classes; (p && p->next); p = p->next)
		if(!strcmp(line, p->name)) {
			current_scheme_class = p;
			return;
		}
	
	new = (scheme_class *)malloc(sizeof(scheme_class));
	assert(new);
	
	new->name = strdup(line);
	new->next = (scheme_class *)NULL;
	new->types = (scheme_type *)NULL;	/*JFF initialize this pointer*/
	
	if(p)
		p->next = new;
	else
		Scheme_classes = new;
	
	current_scheme_class = new; 
}





void add_sector_class(char *line)
{
	sector_class *p, *new = (sector_class *)NULL;
	
	line[strlen(line) - 1] = '\0';
	line++;
	
	for(p = Sector_classes; (p && p->next); p = p->next)
		if(!strcmp(line, p->name)) {
			current_sector_class = p;
			return;
		}
	
	new = (sector_class *)malloc(sizeof(sector_class));
	assert(new);
	
	new->name = strdup(line);
	new->next = (sector_class *)NULL;
	new->types = (sector_type *)NULL;	/*JFF initialize this pointer*/
	
	if(p)
		p->next = new;
	else
		Sector_classes = new;
	
	current_sector_class = new; 
}

void add_sector_type(char *line)
{
	char *short_desc;
	char *long_desc;
	BCINT type = -1;
	char *tok;
	sector_type *p, *new = (sector_type *)NULL;
	
	tok = strtok(line, "\t ,");
	if(tok && (type = atoi(tok)) != -1) {
		tok = strtok(NULL, "\t\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the short description */
		short_desc = strdup(tok);
		
		tok = strtok(NULL, "\t\"");
		while(tok && ((tok[0] == ' ') || (tok[0] == ',')))
			tok = strtok(NULL, "\t\"");
		/* should by now have the long description */
		long_desc = strdup(tok);
		
		/* add it now */
		new = (sector_type *)malloc(sizeof(sector_type));
		assert(new);
		new->shortname = short_desc;
		new->longname = long_desc;
		new->type = type;
		new->next = (sector_type *)NULL;
		
		for(p = current_sector_class->types; (p && p->next); p = p->next);
		
		if(p)
			p->next = new;
		else
			current_sector_class->types = new;
	}
}

void add_levels(char *line)
{
	char *tok;
	
	tok = strtok(line, "\t ,");

	if(tok && !stricmp(tok, "format")) {
		tok = strtok(NULL, "\t ,");
		while(tok) {
			LevelNameFormat = SList_append(LevelNameFormat, tok);
			tok = strtok(NULL, "\t ,");
		}
	}
	else {
		while(tok) {
			/* add a level */
			LevelNames = SList_append(LevelNames, tok);
			
			tok = strtok(NULL, "\t ,");
		}
	}
}

void add_texture_sections(char *line)
{
	char *tok;
	
	tok = strtok(line, "\t ,");
	while(tok) {
		/* add it to list */
		Texture_sections = SList_append(Texture_sections, tok);
		
		tok = strtok(NULL, "\t ,");
	}
}

void add_ftexture_sections(char *line)
{
	char *tok;
	
	tok = strtok(line, "\t ,");
	while(tok) {
		/* add it to list */
		Ftexture_sections = SList_append(Ftexture_sections, tok);
		
		tok = strtok(NULL, "\t ,");
	}
}

void get_palette_colour(char *line)
{
	char *tok;
	BCINT color = -1;
	BCINT palette_index;
	
	tok = strtok(line, "\t ,=");
	if(tok) {
		color = getcol(tok);
		if(color == -1)
			return;
		
		tok = strtok(NULL, "\t ,=");
		palette_index = atoi(tok);
		
		Palette[color] = palette_index;
	}
}



/* read a line in from the current file
   return zero if we hit the end of the file */
int read_line(FILE *f, char *buf, int buflen)
{
	int eof = 0, eol = 0;
	int c;
	
	while(((c = fgetc(f)) == '\n') ||
		  (c == '\r') ||
		  (c == ' ') ||
		  (c == '\t'));
	/* ignore leading whitespace */
	
	while(!(eof || eol) && (buflen > 0)) {
		if(c == EOF)
			eof = 1;
		else if((c == '\n') || (c == '\r'))
			eol = 1;
		else {
			*buf++ = (char)c;
			buflen--;
            c = fgetc(f);
		}
	}

	while(((c = *(buf - 1)) == ' ') ||
		  (c == '\t'))
		buf--;
	/* remove trailing whitespace */
	
	*buf = 0; /* bung a terminator on it */
	
	return(1 - eof);
}

SList SList_append(SList l, char *str)
{
	SList nw, head = l;

	
	while(l && l->next)	/*JFF prevent dereference of NULL pointer*/
		l = l->next;
	
	nw = (SList)malloc(sizeof(struct _SList));
	if(head)
		l->next = nw;
	else
		head = nw;
	
	nw->string = strdup(str);
	nw->next = (SList)NULL;
	
	return head;
}

SList SList_find(SList l, char *str)
{
	while(l)
		if(!strcmp(l->string, str))
			return l;
		else
			l = l->next;
	
	return (SList)NULL;
}

