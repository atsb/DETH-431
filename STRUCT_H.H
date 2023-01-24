/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   STRUCT_H.H - HEXEN WAD files data structures.
   */


/*
   this data structure contains the information about the THINGS
   */

/* protection against multiple includes */
#ifndef _WSTRUCTS_H
#define _WSTRUCTS_H

struct Thing
{
    BCINT tid;
    BCINT xpos;     /* x position */
    BCINT ypos;     /* y position */
    BCINT zpos;	    /* z position (height) */
    BCINT angle;    /* facing angle */
    BCINT type;     /* thing type */
    BCINT flags;    /* appears when? */
    BYTE special; 	/* special command to perform */
    BYTE arg1;		/* special argument 1 */
    BYTE arg2; 		/* special argument 2 */
    BYTE arg3;		/* special argument 3 */
    BYTE arg4;		/* special argument 4 */
    BYTE arg5;		/* special argument 5 */
};
typedef struct Thing huge *TPtr;



/*
   this data structure contains the information about the LINEDEFS
   */
struct LineDef
{
    BCINT start;     /* from this vertex ... */
    BCINT end;       /* ... to this vertex */
    BCINT flags;     /* see NAMES.C for more info */
    BYTE special;      /* see NAMES.C for more info */
    BYTE arg1;
    BYTE arg2;     
    BYTE arg3;     
    BYTE arg4;     
    BYTE arg5;     
    BCINT sidedef1;  /* sidedef */
    BCINT sidedef2;  /* only if this line adjoins 2 sectors */
};
typedef struct LineDef huge *LDPtr;



/*
   this data structure contains the information about the SIDEDEFS
   */
struct SideDef
{
    BCINT xoff;      /* X offset for texture */
    BCINT yoff;      /* Y offset for texture */
    char tex1[8];  /* texture name for the part above */
    char tex2[8];  /* texture name for the part below */
    char tex3[8];  /* texture name for the regular part */
    BCINT sector;    /* adjacent sector  */
};
typedef struct SideDef huge *SDPtr;



/*
   this data structure contains the information about the VERTEXES
   */
struct Vertex
{
    BCINT x;         /* X coordinate */
    BCINT y;         /* Y coordinate */
};
typedef struct Vertex huge *VPtr;



/*
   this data structure contains the information about the SEGS
   */
typedef struct Seg huge *SEPtr;
struct Seg
{
    SEPtr next;      /* next Seg in list */
    BCINT start;     /* from this vertex ... */
    BCINT end;       /* ... to this vertex */
    UBCINT angle;    /* angle (0 = east, 16384 = north, ...) */
    BCINT linedef;   /* linedef that this seg goes along*/
    BCINT flip;      /* true if not the same direction as linedef */
    UBCINT dist;     /* distance from starting point */
};



/*
   this data structure contains the information about the SSECTORS
   */
typedef struct SSector huge *SSPtr;
struct SSector
{
    SSPtr next;	  /* next Sub-Sector in list */
    BCINT num;       /* number of Segs in this Sub-Sector */
    BCINT first;     /* first Seg */
};



/*
   this data structure contains the information about the NODES
   */
typedef struct Node *NPtr;
struct Node
{
    BCINT x, y;                         /* starting poBCINT */
    BCINT dx, dy;                       /* offset to ending point */
    BCINT miny1, maxy1, minx1, maxx1;   /* bounding rectangle 1 */
    BCINT miny2, maxy2, minx2, maxx2;   /* bounding rectangle 2 */
    BCINT child1, child2;               /* Node or SSector (if high bit is set) */
    NPtr node1, node2;                /* pointer if the child is a Node */
    BCINT num;                          /* number given to this Node */
};



/*
   this data structure contains the information about the SECTORS
   */
struct Sector
{
    BCINT floorh;    /* floor height */
    BCINT ceilh;     /* ceiling height */
    char floort[8];/* floor texture */
    char ceilt[8]; /* ceiling texture */
    BCINT light;     /* light level (0-255) */
    BCINT special;   /* special behaviour (0 = normal, 9 = secret, ...) */
    BCINT tag;       /* sector activated by a linedef with the same tag */
};
typedef struct Sector huge *SPtr;


struct Behavior
{
    BCINT unknown1;   
    BCINT unknown2;   
};
typedef struct Behavior huge *BPtr;



/* SO: added these 2 25/02/95 to speed up displaying wall textures */

typedef struct {
    char name[8]; /* name of this texture */
    BCINT zero1;  /* fields which are always zero */
    BCINT zero2;
    BCINT width;  /* dimensions of this texture */
    BCINT height;
    BCINT zero3;
    BCINT zero4;
    BCINT nPatches; /* number of patch descriptors following */
} Texture;

typedef struct {
    BCINT xOffset,
	yOffset,
	PatchNumber,
	One,
	Zero;
} PatchDesc;

/* these are to make drawing pictures easier */
typedef struct {
	BCINT width, height, xofs, yofs;
} PHeader;

typedef struct {
	BYTE yofs, npixels;
} PColumn;

#endif
/* _WSTRUCTS_H */

/* end of file */

