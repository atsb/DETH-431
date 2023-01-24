/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   OBJECTS.C - object handling routines.
   */

/* the includes */
#include "deu.h"
#include "levels.h"

BCINT GetSectorForThing(BCINT);

/*
   highlight the selected objects
   */

void HighlightSelection( BCINT objtype, SelPtr list) /* SWAP! */
{
  SelPtr cur;
    
  if (list == NULL)
		return;
  for (cur = list; cur; cur = cur->next)
		HighlightObject( objtype, cur->objnum, GREEN);
}



/*
   test if an object is in the selection list
   */

Bool IsSelected( SelPtr list, BCINT objnum)
{
  SelPtr cur;
    
  if (list == NULL)
		return FALSE;
  for (cur = list; cur; cur = cur->next)
		if (cur->objnum == objnum)
			return TRUE;
  return FALSE;
}

/*
   test if an object is in the selection list
	 return NULL if not selected SelPtr to the object otherwise
   */

SelPtr SelectPointer( SelPtr list, BCINT objnum)
{
  SelPtr cur;
    
  if (list == NULL)
		return NULL;
  for (cur = list; cur; cur = cur->next)
		if (cur->objnum == objnum)
			return cur;
  return NULL;
}


/*
   add an object to the selection list
   */

void SelectObject( SelPtr *list, BCINT objnum)
{
    SelPtr cur;
    
    if (objnum < 0)
			ProgError( "BUG: SelectObject called with %d", objnum);
    cur = (SelPtr) GetMemory( sizeof( struct SelectionList));
    cur->next = *list;
    cur->objnum = objnum;
    *list = cur;
    Count++;
}



/*
   remove an object from the selection list
   */

void UnSelectObject( SelPtr *list, BCINT objnum)
{
    SelPtr cur, prev;
    
    if (objnum < 0)
		ProgError( "BUG: UnSelectObject called with %d", objnum);
    prev = NULL;
    cur = *list;
    while (cur) {
		if (cur->objnum == objnum) {
			if (prev)
				prev->next = cur->next;
			else
				*list = cur->next;
			FreeMemory( cur);
			if (prev)
				cur = prev->next;
			else
				cur = *list;
		}
		else {
			prev = cur;
			cur = cur->next;
		}
    }
    Count--;
}



/*
   forget the selection list
   */

void ForgetSelection( SelPtr *list)
{
    SelPtr cur, prev;
    
    cur = *list;
    while (cur) {
		prev = cur;
		cur = cur->next;
		FreeMemory( prev);
    }
    *list = NULL;
}



/*
   get the number of objets of a given type minus one
   */
BCINT GetMaxObjectNum( BCINT objtype)
{
    switch (objtype) {
    case OBJ_THINGS:
		return NumThings - 1;
    case OBJ_LINEDEFS:
		return NumLineDefs - 1;
    case OBJ_SIDEDEFS:
		return NumSideDefs - 1;
    case OBJ_VERTEXES:
		return NumVertexes - 1;
    case OBJ_SEGS:
		return NumSegs - 1;
    case OBJ_SSECTORS:
		return NumSSectors - 1;
    case OBJ_SECTORS:
		return NumSectors - 1;
    }
    return -1;
}


/*
   check if there is something of interest inside the given box
   */

BCINT GetCurObject( BCINT objtype, BCINT x0, BCINT y0, BCINT x1, BCINT y1) /* SWAP! */
{
    BCINT n, m, cur, curx;
    BCINT lx0, ly0, lx1, ly1;
    BCINT midx, midy;
    
    cur = -1;
    if (x1 < x0) {
		n = x0;
		x0 = x1;
		x1 = n;
    }
    if (y1 < y0) {
		n = y0;
		y0 = y1;
		y1 = n;
    }
    
    switch (objtype) {
    case OBJ_THINGS:
		for (n = 0; n < NumThings; n++)
			if (Things[ n].xpos >= x0 && Things[ n].xpos <= x1 && Things[ n].ypos >= y0 && Things[ n].ypos <= y1)
			{
				cur = n;
				break;
			}
		break;
    case OBJ_VERTEXES:
		for (n = 0; n < NumVertexes; n++)
			if (Vertexes[ n].x >= x0 && Vertexes[ n].x <= x1 && Vertexes[ n].y >= y0 && Vertexes[ n].y <= y1) {
				cur = n;
				break;
			}
		break;
    case OBJ_LINEDEFS:
		for (n = 0; n < NumLineDefs; n++) {
			if (IsLineDefInside( n, x0, y0, x1, y1)) {
				cur = n;
				break;
			}
		}
		break;
    case OBJ_SECTORS:
		/* hack, hack...  I look for the first LineDef crossing a horizontal half-line drawn from the cursor */
		curx = MapMaxX + 1;
		cur = -1;
		midx = (x0 + x1) / 2;
		midy = (y0 + y1) / 2;
		for (n = 0; n < NumLineDefs; n++)
			if ((Vertexes[ LineDefs[ n].start].y > midy) != (Vertexes[ LineDefs[ n].end].y > midy)) {
				lx0 = Vertexes[ LineDefs[ n].start].x;
				ly0 = Vertexes[ LineDefs[ n].start].y;
				lx1 = Vertexes[ LineDefs[ n].end].x;
				ly1 = Vertexes[ LineDefs[ n].end].y;
				m = lx0 + (BCINT) ((long) (midy - ly0) * (long) (lx1 - lx0) / (long) (ly1 - ly0));
				if (m >= midx && m < curx) {
					curx = m;
					cur = n;
				}
			}
		/* now look if this LineDef has a SideDef bound to one sector */
		if (cur >= 0) {
			if (Vertexes[ LineDefs[ cur].start].y > Vertexes[ LineDefs[ cur].end].y)
				cur = LineDefs[ cur].sidedef1;
			else
				cur = LineDefs[ cur].sidedef2;
			if (cur >= 0) {
				cur = SideDefs[ cur].sector;
			}
			else
				cur = -1;
		}
		else
			cur = -1;
		break;
    }
    return cur;
}

Bool CurObjectUnderMouse( BCINT objtype, BCINT x0, BCINT y0, BCINT x1, BCINT y1, BCINT curobject)
{
    BCINT n, m, cur, curx;
    BCINT lx0, ly0, lx1, ly1;
    BCINT midx, midy;
    
    cur = -1;
    if (x1 < x0) {
		n = x0;
		x0 = x1;
		x1 = n;
    }
    if (y1 < y0) {
		n = y0;
		y0 = y1;
		y1 = n;
    }
    
    switch (objtype) {
    case OBJ_THINGS:
		for (n = 0; n < NumThings; n++)
			if (Things[ n].xpos >= x0 && Things[ n].xpos <= x1 && Things[ n].ypos >= y0 && Things[ n].ypos <= y1) 
				if (n == curobject)
					return TRUE;
		break;
    case OBJ_VERTEXES:
		for (n = 0; n < NumVertexes; n++)
			if (Vertexes[ n].x >= x0 && Vertexes[ n].x <= x1 && Vertexes[ n].y >= y0 && Vertexes[ n].y <= y1)
				if (n == curobject)
					return TRUE;
		break;
    case OBJ_LINEDEFS:
		for (n = 0; n < NumLineDefs; n++) {
			if (IsLineDefInside( n, x0, y0, x1, y1))
				if (n == curobject)
					return TRUE;
		}
		break;
    case OBJ_SECTORS:
		/* hack, hack...  I look for the first LineDef crossing a horizontal half-line drawn from the cursor */
		curx = MapMaxX + 1;
		cur = -1;
		midx = (x0 + x1) / 2;
		midy = (y0 + y1) / 2;
		for (n = 0; n < NumLineDefs; n++)
			if ((Vertexes[ LineDefs[ n].start].y > midy) != (Vertexes[ LineDefs[ n].end].y > midy)) {
				lx0 = Vertexes[ LineDefs[ n].start].x;
				ly0 = Vertexes[ LineDefs[ n].start].y;
				lx1 = Vertexes[ LineDefs[ n].end].x;
				ly1 = Vertexes[ LineDefs[ n].end].y;
				m = lx0 + (BCINT) ((long) (midy - ly0) * (long) (lx1 - lx0) / (long) (ly1 - ly0));
				if (m >= midx && m < curx) {
					curx = m;
					cur = n;
				}
			}
		/* now look if this LineDef has a SideDef bound to one sector */
		if (cur >= 0) {
			if (Vertexes[ LineDefs[ cur].start].y > Vertexes[ LineDefs[ cur].end].y)
				cur = LineDefs[ cur].sidedef1;
			else
				cur = LineDefs[ cur].sidedef2;
			if (cur >= 0) {
				cur = SideDefs[ cur].sector;
			}
			else
				cur = -1;
		}
		else
			cur = -1;
		break;
    }
	return FALSE;
}
/*
   select all objects inside a given box
   */

SelPtr SelectObjectsInBox( BCINT objtype, BCINT x0, BCINT y0, BCINT x1, BCINT y1) /* SWAP! */
{
    BCINT n, m;
    SelPtr list;
    
    list = NULL;
    if (x1 < x0) {
		n = x0;
		x0 = x1;
		x1 = n;
    }
    if (y1 < y0) {
		n = y0;
		y0 = y1;
		y1 = n;
    }
    
    switch (objtype) {
    case OBJ_THINGS:
		for (n = 0; n < NumThings; n++)
			if (Things[ n].xpos >= x0 && Things[ n].xpos <= x1 && Things[ n].ypos >= y0 && Things[ n].ypos <= y1)
				SelectObject( &list, n);
		break;
    case OBJ_VERTEXES:
		for (n = 0; n < NumVertexes; n++)
			if (Vertexes[ n].x >= x0 && Vertexes[ n].x <= x1 && Vertexes[ n].y >= y0 && Vertexes[ n].y <= y1)
				SelectObject( &list, n);
		break;
    case OBJ_LINEDEFS:
		for (n = 0; n < NumLineDefs; n++) {
			/* the two ends of the line must be in the box */
			m = LineDefs[ n].start;
			if (Vertexes[ m].x < x0 || Vertexes[ m].x > x1 || Vertexes[ m].y < y0 || Vertexes[ m].y > y1)
				continue;
			m = LineDefs[ n].end;
			if (Vertexes[ m].x < x0 || Vertexes[ m].x > x1 || Vertexes[ m].y < y0 || Vertexes[ m].y > y1)
				continue;
			SelectObject( &list, n);
		}
		break;
    case OBJ_SECTORS:
		/* hack: select all sectors... */
		for (n = 0; n < NumSectors; n++)
			SelectObject( &list, n);
		/* ... then remove the unwanted ones from the list */
		for (n = 0; n < NumLineDefs; n++) {
			m = LineDefs[ n].start;
			if (Vertexes[ m].x < x0 || Vertexes[ m].x > x1 || Vertexes[ m].y < y0 || Vertexes[ m].y > y1) {
				m = LineDefs[ n].sidedef1;
				if (m >= 0 && SideDefs[ m].sector >= 0)
					UnSelectObject( &list, SideDefs[ m].sector);
				m = LineDefs[ n].sidedef2;
				if (m >= 0 && SideDefs[ m].sector >= 0)
					UnSelectObject( &list, SideDefs[ m].sector);
				continue;
			}
			m = LineDefs[ n].end;
			if (Vertexes[ m].x < x0 || Vertexes[ m].x > x1 || Vertexes[ m].y < y0 || Vertexes[ m].y > y1) {
				m = LineDefs[ n].sidedef1;
				if (m >= 0 && SideDefs[ m].sector >= 0)
					UnSelectObject( &list, SideDefs[ m].sector);
				m = LineDefs[ n].sidedef2;
				if (m >= 0 && SideDefs[ m].sector >= 0)
					UnSelectObject( &list, SideDefs[ m].sector);
				continue;
			}
		}
		break;
    }
    return list;
}



/*
   highlight the selected object
   */

void HighlightObject( BCINT objtype, BCINT objnum, BCINT color) /* SWAP! */
{
    BCINT  n, m, val;
#ifdef GAME_HEXEN
    char argtext[ 6];
#endif
    
    /* use XOR mode : drawing any line twice erases it */
    setwritemode( XOR_PUT); 
    SetColor( color);
    switch ( objtype)
		{
    	case OBJ_THINGS:
				m = (GetThingRadius( Things[ objnum].type) * 2.5) / 2;
				DrawMapLine( Things[ objnum].xpos - m, Things[ objnum].ypos - m, Things[ objnum].xpos - m, Things[ objnum].ypos + m);
				DrawMapLine( Things[ objnum].xpos - m, Things[ objnum].ypos + m, Things[ objnum].xpos + m, Things[ objnum].ypos + m);
				DrawMapLine( Things[ objnum].xpos + m, Things[ objnum].ypos + m, Things[ objnum].xpos + m, Things[ objnum].ypos - m);
				DrawMapLine( Things[ objnum].xpos + m, Things[ objnum].ypos - m, Things[ objnum].xpos - m, Things[ objnum].ypos - m);
				if	(ThingAngle == FALSE)
					DrawMapArrow( Things[ objnum].xpos, Things[ objnum].ypos, Things[ objnum].angle * 182);
#ifdef GAME_HEXEN
				if (color != LINEDEFTAGGED && (Things[ objnum].arg1 > 0 || Things[ objnum].arg5 > 0 ))
				{
					strcpy (argtext, GetLineDefArgs(Things[ objnum].special));
					if (argtext[0] == '$') {
					for (m = 0; m < NumSectors; m++)
						if (Sectors[ m].tag == Things[ objnum].arg1) 
			    		HighlightObject( OBJ_SECTORS, m, LINEDEFTAGGED);
					for (m = 0; m < NumLineDefs; m++)
						if (LineDefs[ m].arg1 == Things[ objnum].arg1) 
			    		HighlightObject( OBJ_LINEDEFS, m, LINEDEFTAGGED);
					}
					for (m = 0; m < NumThings; m++)
					{
						if (argtext[0] == '*') 
							if (Things[ m].tid == Things[ objnum].arg1) 
			    			HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
						if (argtext[4] == '*' && Things[ objnum].arg5 > 0) 
							if (Things[ m].tid == Things[ objnum].arg5) 
			    			HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
					}
				}
				if (color != LINEDEFTAGGED && Things[ objnum].tid > 0)
				{
					for (m = 0; m < NumLineDefs; m++)
					{
						strcpy (argtext, GetLineDefArgs(LineDefs[ m].special));
						if (argtext[0] == '*') 
							if (LineDefs[ m].arg1 == Things[ objnum].tid) 
						    		HighlightObject( OBJ_LINEDEFS, m, LINEDEFTAGGED);
						if (argtext[4] == '*') 
							if (LineDefs[ m].arg5 == Things[ objnum].tid) 
						    		HighlightObject( OBJ_LINEDEFS, m, LINEDEFTAGGED);
					}
					for (m = 0; m < NumThings; m++)
					{
						strcpy (argtext, GetLineDefArgs(Things[ m].special));
						if (argtext[0] == '*') 
							if (Things[ m].arg1 == Things[ objnum].tid) 
						    		HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
						if (argtext[4] == '*') 
							if (Things[ m].arg5 == Things[ objnum].tid) 
						    		HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
					}
				}
				setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
#endif
				break;

	    case OBJ_LINEDEFS:
				n = (Vertexes[ LineDefs[ objnum].start].x
			   		+ Vertexes[ LineDefs[ objnum].end].x) / 2;
				m = (Vertexes[ LineDefs[ objnum].start].y
			   		+ Vertexes[ LineDefs[ objnum].end].y) / 2;
				DrawMapLine
				(
					n,
					m,
					n + (Vertexes[LineDefs[ objnum].end].y
					- Vertexes[LineDefs[ objnum].start].y) / 3,
					m + (Vertexes[ LineDefs[ objnum].start].x
			  		- Vertexes[ LineDefs[ objnum].end].x) / 3
				);
				setlinestyle(SOLID_LINE, 0, THICK_WIDTH ); /* JFF use constant */
				DrawMapVector
				(
					Vertexes[ LineDefs[ objnum].start].x,
					Vertexes[ LineDefs[ objnum].start].y,
					Vertexes[ LineDefs[ objnum].end].x,
					Vertexes[ LineDefs[ objnum].end].y
				);
#ifdef GAME_DOOM
				switch(BOOMEnable? LineDefs[objnum].special : -1)
				{
					case 0:   //typeless
					case 244:	//jff line-line teleports
					case 243:
					case 263:
					case 262:
					case 267:
					case 266:
					case 265:
					case 264:
					case 260: // translucency
					case 218:	// wall scrollers
					case 249:
					case 254: //jff show lines tagged the same for certain linedef types
						if (color != LINEDEFTAGGED && LineDefs[ objnum].tag > 0)
							for (m = 0; m < NumLineDefs; m++)
								if (LineDefs[m].tag == LineDefs[objnum].tag) 
				   				HighlightObject(OBJ_LINEDEFS, m, LINEDEFTAGGED);
						break;
					default:	//jff normal just show sectors with same tag
						if (color != LINEDEFTAGGED && LineDefs[ objnum].tag > 0)
							for (m = 0; m < NumSectors; m++)
								if (Sectors[ m].tag == LineDefs[ objnum].tag) 
				   				HighlightObject( OBJ_SECTORS, m, LINEDEFTAGGED);
						break;
				}
#else
				if (color != LINEDEFTAGGED && (LineDefs[ objnum].arg1 > 0 || LineDefs[ objnum].arg5 > 0 ))
				{
					strcpy (argtext, GetLineDefArgs(LineDefs[ objnum].special));
					if (argtext[0] == '$')
					{
						for (m = 0; m < NumSectors; m++)
							if (Sectors[ m].tag == LineDefs[ objnum].arg1) 
					    			HighlightObject( OBJ_SECTORS, m, LINEDEFTAGGED);
					}
					else if (argtext[0] == '*')
					{
						for (m = 0; m < NumThings; m++)
							if (Things[ m].tid == LineDefs[ objnum].arg1)
							{
					    			setlinestyle(SOLID_LINE, 0, THICK_WIDTH ); /* JFF use constant */ 
					    			HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
							}
					}
					else if (argtext[4] == '*')
					{
						for (m = 0; m < NumThings; m++)
							if (Things[ m].tid == LineDefs[ objnum].arg5)
							{
					    			setlinestyle(SOLID_LINE, 0, THICK_WIDTH ); /* JFF use constant */ 
					    			HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
							}
					}
				}
#endif
				setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
				break;

	    case OBJ_VERTEXES:
				/*DrawMapLine( Vertexes[ objnum].x - OBJSIZE * 1.8, Vertexes[ objnum].y - OBJSIZE * 1.8, Vertexes[ objnum].x - OBJSIZE * 1.8, Vertexes[ objnum].y + OBJSIZE * 1.8);
				DrawMapLine( Vertexes[ objnum].x - OBJSIZE * 1.8, Vertexes[ objnum].y + OBJSIZE * 1.8, Vertexes[ objnum].x + OBJSIZE * 1.8, Vertexes[ objnum].y + OBJSIZE * 1.8);
				DrawMapLine( Vertexes[ objnum].x + OBJSIZE * 1.8, Vertexes[ objnum].y + OBJSIZE * 1.8, Vertexes[ objnum].x + OBJSIZE * 1.8, Vertexes[ objnum].y - OBJSIZE * 1.8);
				DrawMapLine( Vertexes[ objnum].x + OBJSIZE * 1.8, Vertexes[ objnum].y - OBJSIZE * 1.8, Vertexes[ objnum].x - OBJSIZE * 1.8, Vertexes[ objnum].y - OBJSIZE * 1.8); */

				val = OBJSIZE * ((1 / Scale) * 1.8);
				if (val < 2)
					val = 2;
				if (val > 32)
					val = 32;

				DrawMapLine( Vertexes[ objnum].x - val, Vertexes[ objnum].y - val, Vertexes[ objnum].x - val, Vertexes[ objnum].y + val);
				DrawMapLine( Vertexes[ objnum].x - val, Vertexes[ objnum].y + val, Vertexes[ objnum].x + val, Vertexes[ objnum].y + val);
				DrawMapLine( Vertexes[ objnum].x + val, Vertexes[ objnum].y + val, Vertexes[ objnum].x + val, Vertexes[ objnum].y - val);
				DrawMapLine( Vertexes[ objnum].x + val, Vertexes[ objnum].y - val, Vertexes[ objnum].x - val, Vertexes[ objnum].y - val);
				break;

  	 	case OBJ_SECTORS:
				setlinestyle(SOLID_LINE, 0, THICK_WIDTH ); /* JFF use constant */  
				for (n = 0; n < NumLineDefs; n++)
					if ( (LineDefs[n].sidedef1>=0 && SideDefs[LineDefs[n].sidedef1].sector == objnum ) ||
						(LineDefs[n].sidedef2>=0 && SideDefs[LineDefs[n].sidedef2].sector == objnum ))
						DrawMapLine( Vertexes[ LineDefs[ n].start].x, Vertexes[ LineDefs[ n].start].y,
									Vertexes[ LineDefs[ n].end].x, Vertexes[ LineDefs[ n].end].y);
#ifdef GAME_HEXEN
				if (color != LINEDEFTAGGED && Sectors[ objnum].tag > 0)
				{
					strcpy (argtext, GetLineDefArgs(LineDefs[ objnum].special));
					for (m = 0; m < NumLineDefs; m++) 
						if (LineDefs[ m].arg1 == Sectors[ objnum].tag)
						{
							setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
							HighlightObject( OBJ_LINEDEFS, m, LINEDEFTAGGED);
						}
					for (m = 0; m < NumThings; m++) 
						if (Things[ m].arg1 == Sectors[ objnum].tag)
						{
							setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
							HighlightObject( OBJ_THINGS, m, LINEDEFTAGGED);
						}
#else
				if (color != LINEDEFTAGGED && Sectors[ objnum].tag > 0)
				{
					for (m = 0; m < NumLineDefs; m++) 
						if (LineDefs[ m].tag == Sectors[ objnum].tag)
						{
							setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
							HighlightObject( OBJ_LINEDEFS, m, LINEDEFTAGGED);
						}
#endif
			} 
			setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
			break;
    }
  	/* restore normal write mode */
	  setwritemode( COPY_PUT);
}



/*
   delete an object
   */

void DeleteObject( BCINT objtype, BCINT objnum) /* SWAP! */
{
    SelPtr list;
    
    list = NULL;
    SelectObject( &list, objnum);
    DeleteObjects( objtype, &list);
}



/*
   delete a group of objects (*recursive*)
   */

void DeleteObjects( BCINT objtype, SelPtr *list) /* SWAP! */
{
    BCINT    n, objnum;
    SelPtr cur;
    
    MadeChanges = TRUE;
    switch (objtype) {
    case OBJ_THINGS:
		while (*list) {
			objnum = (*list)->objnum;
			/* delete the Thing */
			NumThings--;
			if (NumThings > 0) {
				for (n = objnum; n < NumThings; n++)
					Things[ n] = Things[ n + 1];
				Things = (TPtr) ResizeFarMemory( Things, NumThings * sizeof( struct Thing));
			}
			else {
				FreeFarMemory( Things);
				Things = NULL;
			}
			for (cur = (*list)->next; cur; cur = cur->next)
				if (cur->objnum > objnum)
					cur->objnum--;
			UnSelectObject( list, objnum);
		}
		break;
    case OBJ_VERTEXES:
		while (*list) {
			objnum = (*list)->objnum;
			/* delete the LineDefs bound to this Vertex and change the references */
			for (n = 0; n < NumLineDefs; n++) {
				if (LineDefs[ n].start == objnum || LineDefs[ n].end == objnum)
					DeleteObject( OBJ_LINEDEFS, n--);
				else {
					if (LineDefs[ n].start >= objnum)
						LineDefs[ n].start--;
					if (LineDefs[ n].end >= objnum)
						LineDefs[ n].end--;
				}
			}
			/* delete the Vertex */
			NumVertexes--;
			if (NumVertexes > 0) {
				for (n = objnum; n < NumVertexes; n++)
					Vertexes[ n] = Vertexes[ n + 1];
				Vertexes = (VPtr) ResizeFarMemory( Vertexes, NumVertexes * sizeof( struct Vertex));
			}
			else {
				FreeFarMemory( Vertexes);
				Vertexes = NULL;
			}
			for (cur = (*list)->next; cur; cur = cur->next)
				if (cur->objnum > objnum)
					cur->objnum--;
			UnSelectObject( list, objnum);
		}
		break;
    case OBJ_LINEDEFS:
		while (*list) {
			objnum = (*list)->objnum;
			/* delete the two SideDefs bound to this LineDef */
			if (LineDefs[ objnum].sidedef1 >= 0) {
				DeleteObject( OBJ_SIDEDEFS, LineDefs[ objnum].sidedef1);
			}
			if (LineDefs[ objnum].sidedef2 >= 0) {
				DeleteObject( OBJ_SIDEDEFS, LineDefs[ objnum].sidedef2);
			}
			/* delete the LineDef */
			NumLineDefs--;
			if (NumLineDefs > 0) {
				for (n = objnum; n < NumLineDefs; n++)
					LineDefs[ n] = LineDefs[ n + 1];
				LineDefs = (LDPtr) ResizeFarMemory( LineDefs, NumLineDefs * sizeof( struct LineDef));
			}
			else {
				FreeFarMemory( LineDefs);
				LineDefs = NULL;
			}
			for (cur = (*list)->next; cur; cur = cur->next)
				if (cur->objnum > objnum)
					cur->objnum--;
			UnSelectObject( list, objnum);
		}
		break;
    case OBJ_SIDEDEFS:
		while (*list) {
			objnum = (*list)->objnum;
			/* change the LineDefs references */
			for (n = 0; n < NumLineDefs; n++) {
				if (LineDefs[ n].sidedef1 == objnum)
					LineDefs[ n].sidedef1 = -1;
				else if (LineDefs[ n].sidedef1 >= objnum)
					LineDefs[ n].sidedef1--;
				if (LineDefs[ n].sidedef2 == objnum)
					LineDefs[ n].sidedef2 = -1;
				else if (LineDefs[ n].sidedef2 >= objnum)
					LineDefs[ n].sidedef2--;
			}
			/* delete the SideDef */
			NumSideDefs--;
			if (NumSideDefs > 0) {
				for (n = objnum; n < NumSideDefs; n++)
					SideDefs[ n] = SideDefs[ n + 1];
				SideDefs = (SDPtr) ResizeFarMemory( SideDefs, NumSideDefs * sizeof( struct SideDef));
			}
			else {
				FreeFarMemory( SideDefs);
				SideDefs = NULL;
			}
			for (cur = (*list)->next; cur; cur = cur->next)
				if (cur->objnum > objnum)
					cur->objnum--;
			UnSelectObject( list, objnum);
		}
		MadeMapChanges = TRUE;
		break;
    case OBJ_SECTORS:
		while (*list) {
			objnum = (*list)->objnum;
			/* delete the SideDefs bound to this Sector and change the references */
			for (n = 0; n < NumSideDefs; n++)
				if (SideDefs[ n].sector == objnum)
					DeleteObject( OBJ_SIDEDEFS, n--);
				else if (SideDefs[ n].sector >= objnum)
					SideDefs[ n].sector--;
			/* delete the Sector */
			NumSectors--;
			if (NumSectors > 0) {
				for (n = objnum; n < NumSectors; n++)
					Sectors[ n] = Sectors[ n + 1];
				Sectors = (SPtr) ResizeFarMemory( Sectors, NumSectors * sizeof( struct Sector));
			}
			else {
				FreeFarMemory( Sectors);
				Sectors = NULL;
			}
			for (cur = (*list)->next; cur; cur = cur->next)
				if (cur->objnum > objnum)
					cur->objnum--;
			UnSelectObject( list, objnum);
		}
		break;
    default:
		Beep();
    }
}



/*
   insert a new object
   */


void InsertObject(BCINT objtype, BCINT copyfrom, BCINT xpos, BCINT ypos) /* SWAP! */
{
  BCINT last;
  
  MadeChanges = TRUE;
  switch (objtype)
	{
  	case OBJ_THINGS:
			last = NumThings++;
			if (last > 0)
				Things = (TPtr) ResizeFarMemory( Things, (unsigned long) NumThings * sizeof( struct Thing));
			else
				Things = (TPtr) GetFarMemory( sizeof( struct Thing));
			Things[ last].xpos = xpos;
			Things[ last].ypos = ypos;
			if (copyfrom >= 0)
			{
				Things[ last].type  = Things[ copyfrom].type;
				Things[ last].angle = Things[ copyfrom].angle;
				Things[ last].flags  = Things[ copyfrom].flags;
#ifdef GAME_HEXEN
				Things[ last].special  = Things[ copyfrom].special;
				Things[ last].tid  = Things[ copyfrom].tid;
				Things[ last].zpos  = Things[ copyfrom].zpos;
				Things[ last].arg1  = Things[ copyfrom].arg1;
				Things[ last].arg2  = Things[ copyfrom].arg2;
				Things[ last].arg3  = Things[ copyfrom].arg3;
				Things[ last].arg4  = Things[ copyfrom].arg4;
				Things[ last].arg5  = Things[ copyfrom].arg5;
#endif
			}
			else
			{
#ifdef GAME_HEXEN
				Things[ last].type  = 1;
				Things[ last].angle = 0;
				Things[ last].flags  = 0x7E7;
				Things[ last].special  = 0;
				Things[ last].tid  = 0;
				Things[ last].zpos  = 0;
				Things[ last].arg1  = 0;
				Things[ last].arg2  = 0;
				Things[ last].arg3  = 0;
				Things[ last].arg4  = 0;
				Things[ last].arg5  = 0;
#else
				Things[ last].type  = 1;
				Things[ last].angle = 0;
				Things[ last].flags  = 0x07;
#endif
			}
			break;
  case OBJ_VERTEXES:
		last = NumVertexes++;
		if (last > 0)
			Vertexes = (VPtr) ResizeFarMemory( Vertexes, (unsigned long) NumVertexes * sizeof( struct Vertex));
		else
			Vertexes = (VPtr) GetFarMemory( sizeof( struct Vertex));
		/* kluge: the Nodes builder will put -2 in copyfrom */
		if (copyfrom == -2)
		{
			Vertexes[ last].x = xpos;
			Vertexes[ last].y = ypos;
		}
		else
		{
			/*Vertexes[ last].x = xpos & ~7;
			Vertexes[ last].y = ypos & ~7; */
			Vertexes[ last].x = xpos ;
			Vertexes[ last].y = ypos ;
			if (Vertexes[ last].x < MapMinX)
				MapMinX = Vertexes[ last].x;
			if (Vertexes[ last].x > MapMaxX)
				MapMaxX = Vertexes[ last].x;
			if (Vertexes[ last].y < MapMinY)
				MapMinY = Vertexes[ last].y;
			if (Vertexes[ last].y > MapMaxY)
				MapMaxY = Vertexes[ last].y;
			MadeMapChanges = TRUE;
		}
		break;
  case OBJ_LINEDEFS:
		last = NumLineDefs++;
		if (last > 0)
			LineDefs = (LDPtr) ResizeFarMemory( LineDefs, (unsigned long) NumLineDefs * sizeof( struct LineDef));
		else
			LineDefs = (LDPtr) GetFarMemory( sizeof( struct LineDef));
		if (copyfrom >= 0)
		{
			LineDefs[ last].start = LineDefs[ copyfrom].start;
			LineDefs[ last].end = LineDefs[ copyfrom].end;
			LineDefs[ last].flags = LineDefs[ copyfrom].flags;
			LineDefs[ last].special = LineDefs[ copyfrom].special;
#ifdef GAME_HEXEN
			LineDefs[ last].arg1 = LineDefs[ copyfrom].arg1;
			LineDefs[ last].arg2 = LineDefs[ copyfrom].arg2;
			LineDefs[ last].arg3 = LineDefs[ copyfrom].arg3;
			LineDefs[ last].arg4 = LineDefs[ copyfrom].arg4;
			LineDefs[ last].arg5 = LineDefs[ copyfrom].arg5;
#else
			LineDefs[ last].tag = LineDefs[ copyfrom].tag;
#endif
		}
		else
		{
			LineDefs[ last].start = 0;
			LineDefs[ last].end = NumVertexes - 1;
			LineDefs[ last].flags = 1;
			LineDefs[ last].special = 0;
			LineDefs[ last].sidedef1 = -1;
			LineDefs[ last].sidedef2 = -1;
#ifdef GAME_HEXEN
			LineDefs[ last].arg1 = 0;
			LineDefs[ last].arg2 = 0;
			LineDefs[ last].arg3 = 0;
			LineDefs[ last].arg4 = 0;
			LineDefs[ last].arg5 = 0;
#else
			LineDefs[ last].tag = 0;
#endif
		}
		LineDefs[ last].sidedef1 = -1;
		LineDefs[ last].sidedef2 = -1;
		break;
  case OBJ_SIDEDEFS:
		/* SideDefs are added from the LineDefs menu, so "copyfrom" should always be -1.  But I test it anyway. */
		last = NumSideDefs++;
		if (last > 0)
			SideDefs = (SDPtr) ResizeFarMemory( SideDefs, (unsigned long) NumSideDefs * sizeof( struct SideDef));
		else
			SideDefs = (SDPtr) GetFarMemory( sizeof( struct SideDef));
		if (copyfrom >= 0)
		{
			SideDefs[ last].xoff = SideDefs[ copyfrom].xoff;
			SideDefs[ last].yoff = SideDefs[ copyfrom].yoff;
			strncpy( SideDefs[ last].tex1, SideDefs[ copyfrom].tex1, 8);
			strncpy( SideDefs[ last].tex2, SideDefs[ copyfrom].tex2, 8);
			strncpy( SideDefs[ last].tex3, SideDefs[ copyfrom].tex3, 8);
			SideDefs[ last].sector = SideDefs[ copyfrom].sector;
		}
		else
		{
			SideDefs[ last].xoff = 0;
			SideDefs[ last].yoff = 0;
			strcpy( SideDefs[ last].tex1, "-");
			strcpy( SideDefs[ last].tex2, "-");
			strcpy( SideDefs[ last].tex3, DefaultWallTexture);
			SideDefs[ last].sector = NumSectors - 1;
		}
		MadeMapChanges = TRUE;
		break;
  case OBJ_SECTORS:
		last = NumSectors++;
		if (last > 0)
			Sectors = (SPtr) ResizeFarMemory( Sectors, (unsigned long) NumSectors * sizeof( struct Sector));
		else
			Sectors = (SPtr) GetFarMemory( sizeof( struct Sector));
		if (copyfrom >= 0)
		{
			Sectors[ last].floorh = Sectors[ copyfrom].floorh;
			Sectors[ last].ceilh = Sectors[ copyfrom].ceilh;
			strncpy( Sectors[ last].floort, Sectors[ copyfrom].floort, 8);
			strncpy( Sectors[ last].ceilt, Sectors[ copyfrom].ceilt, 8);
			Sectors[ last].light = Sectors[ copyfrom].light;
			Sectors[ last].special = Sectors[ copyfrom].special;
			Sectors[ last].tag = Sectors[ copyfrom].tag;
		}
		else
		{
			Sectors[ last].floorh = DefaultFloorHeight;
			Sectors[ last].ceilh = DefaultCeilingHeight;
			strncpy( Sectors[ last].floort, DefaultFloorTexture, 8);
			strncpy( Sectors[ last].ceilt, DefaultCeilingTexture, 8);
			Sectors[ last].light = DefaultLighting; //jff add default lighting
			Sectors[ last].special = 0;
			Sectors[ last].tag = 0;
		}
		break;
  default:
		Beep();
  }
}



/*
   check if a (part of a) LineDef is inside a given block
   */

Bool IsLineDefInside( BCINT ldnum, BCINT x0, BCINT y0, BCINT x1, BCINT y1) /* SWAP - needs Vertexes & LineDefs */
{
    BCINT lx0 = Vertexes[ LineDefs[ ldnum].start].x;
    BCINT ly0 = Vertexes[ LineDefs[ ldnum].start].y;
    BCINT lx1 = Vertexes[ LineDefs[ ldnum].end].x;
    BCINT ly1 = Vertexes[ LineDefs[ ldnum].end].y;
    BCINT i;
    
    /* do you like mathematics? */
    if (lx0 >= x0 && lx0 <= x1 && ly0 >= y0 && ly0 <= y1)
		return TRUE; /* the LineDef start is entirely inside the square */
    if (lx1 >= x0 && lx1 <= x1 && ly1 >= y0 && ly1 <= y1)
		return TRUE; /* the LineDef end is entirely inside the square */
    if ((ly0 > y0) != (ly1 > y0)) {
		i = lx0 + (BCINT) ( (long) (y0 - ly0) * (long) (lx1 - lx0) / (long) (ly1 - ly0));
		if (i >= x0 && i <= x1)
			return TRUE; /* the LineDef crosses the y0 side (left) */
    }
    if ((ly0 > y1) != (ly1 > y1)) {
		i = lx0 + (BCINT) ( (long) (y1 - ly0) * (long) (lx1 - lx0) / (long) (ly1 - ly0));
		if (i >= x0 && i <= x1)
			return TRUE; /* the LineDef crosses the y1 side (right) */
    }
    if ((lx0 > x0) != (lx1 > x0)) {
		i = ly0 + (BCINT) ( (long) (x0 - lx0) * (long) (ly1 - ly0) / (long) (lx1 - lx0));
		if (i >= y0 && i <= y1)
			return TRUE; /* the LineDef crosses the x0 side (down) */
    }
    if ((lx0 > x1) != (lx1 > x1)) {
		i = ly0 + (BCINT) ( (long) (x1 - lx0) * (long) (ly1 - ly0) / (long) (lx1 - lx0));
		if (i >= y0 && i <= y1)
			return TRUE; /* the LineDef crosses the x1 side (up) */
    }
    return FALSE;        
}



/*
   get the Sector number of the SideDef opposite to this SideDef
   (returns -1 if it cannot be found)
   */

BCINT GetOppositeSector( BCINT ld1, Bool firstside) /* SWAP! */
{
    BCINT x0, y0, dx0, dy0;
    BCINT x1, y1, dx1, dy1;
    BCINT x2, y2, dx2, dy2;
    BCINT ld2, dist;
    BCINT bestld, bestdist, bestmdist;
    
    /* get the coords for this LineDef */
    x0  = Vertexes[ LineDefs[ ld1].start].x;
    y0  = Vertexes[ LineDefs[ ld1].start].y;
    dx0 = Vertexes[ LineDefs[ ld1].end].x - x0;
    dy0 = Vertexes[ LineDefs[ ld1].end].y - y0;
    
    /* find the normal vector for this LineDef */
    x1  = (dx0 + x0 + x0) / 2;
    y1  = (dy0 + y0 + y0) / 2;
    if (firstside == TRUE) {
		dx1 = dy0;
		dy1 = -dx0;
    }
    else {
		dx1 = -dy0;
		dy1 = dx0;
    }
    
    bestld = -1;
    /* use a parallel to an axis instead of the normal vector (faster method) */
    if (abs( dy1) > abs( dx1)) {
		if (dy1 > 0) {
			/* get the nearest LineDef in that direction (increasing Y's: North) */
			bestdist = 32767;
			bestmdist = 32767;
			for (ld2 = 0; ld2 < NumLineDefs; ld2++)
				if (ld2 != ld1 && ((Vertexes[ LineDefs[ ld2].start].x > x1) != (Vertexes[ LineDefs[ ld2].end].x > x1))) {
					x2  = Vertexes[ LineDefs[ ld2].start].x;
					y2  = Vertexes[ LineDefs[ ld2].start].y;
					dx2 = Vertexes[ LineDefs[ ld2].end].x - x2;
					dy2 = Vertexes[ LineDefs[ ld2].end].y - y2;
					dist = y2 + (BCINT) ((long) (x1 - x2) * (long) dy2 / (long) dx2);
					if (dist > y1 && (dist < bestdist || (dist == bestdist && (y2 + dy2 / 2) < bestmdist))) {
						bestld = ld2;
						bestdist = dist;
						bestmdist = y2 + dy2 / 2;
					}
				}
		}
		else {
			/* get the nearest LineDef in that direction (decreasing Y's: South) */
			bestdist = -32767;
			bestmdist = -32767;
			for (ld2 = 0; ld2 < NumLineDefs; ld2++)
				if (ld2 != ld1 && ((Vertexes[ LineDefs[ ld2].start].x > x1) != (Vertexes[ LineDefs[ ld2].end].x > x1))) {
					x2  = Vertexes[ LineDefs[ ld2].start].x;
					y2  = Vertexes[ LineDefs[ ld2].start].y;
					dx2 = Vertexes[ LineDefs[ ld2].end].x - x2;
					dy2 = Vertexes[ LineDefs[ ld2].end].y - y2;
					dist = y2 + (BCINT) ((long) (x1 - x2) * (long) dy2 / (long) dx2);
					if (dist < y1 && (dist > bestdist || (dist == bestdist && (y2 + dy2 / 2) > bestmdist))) {
						bestld = ld2;
						bestdist = dist;
						bestmdist = y2 + dy2 / 2;
					}
				}
		}
    }
    else {
		if (dx1 > 0) {
			/* get the nearest LineDef in that direction (increasing X's: East) */
			bestdist = 32767;
			bestmdist = 32767;
			for (ld2 = 0; ld2 < NumLineDefs; ld2++)
				if (ld2 != ld1 && ((Vertexes[ LineDefs[ ld2].start].y > y1) != (Vertexes[ LineDefs[ ld2].end].y > y1))) {
					x2  = Vertexes[ LineDefs[ ld2].start].x;
					y2  = Vertexes[ LineDefs[ ld2].start].y;
					dx2 = Vertexes[ LineDefs[ ld2].end].x - x2;
					dy2 = Vertexes[ LineDefs[ ld2].end].y - y2;
					dist = x2 + (BCINT) ((long) (y1 - y2) * (long) dx2 / (long) dy2);
					if (dist > x1 && (dist < bestdist || (dist == bestdist && (x2 + dx2 / 2) < bestmdist))) {
						bestld = ld2;
						bestdist = dist;
						bestmdist = x2 + dx2 / 2;
					}
				}
		}
		else {
			/* get the nearest LineDef in that direction (decreasing X's: West) */
			bestdist = -32767;
			bestmdist = -32767;
			for (ld2 = 0; ld2 < NumLineDefs; ld2++)
				if (ld2 != ld1 && ((Vertexes[ LineDefs[ ld2].start].y > y1) != (Vertexes[ LineDefs[ ld2].end].y > y1))) {
					x2  = Vertexes[ LineDefs[ ld2].start].x;
					y2  = Vertexes[ LineDefs[ ld2].start].y;
					dx2 = Vertexes[ LineDefs[ ld2].end].x - x2;
					dy2 = Vertexes[ LineDefs[ ld2].end].y - y2;
					dist = x2 + (BCINT) ((long) (y1 - y2) * (long) dx2 / (long) dy2);
					if (dist < x1 && (dist > bestdist || (dist == bestdist && (x2 + dx2 / 2) > bestmdist))) {
						bestld = ld2;
						bestdist = dist;
						bestmdist = x2 + dx2 / 2;
					}
				}
		}
    }
    
    /* no intersection: the LineDef was pointing outwards! */
    if (bestld < 0)
		return -1;
    
    /* now look if this LineDef has a SideDef bound to one sector */
    if (abs( dy1) > abs( dx1)) {
		if ((Vertexes[ LineDefs[ bestld].start].x < Vertexes[ LineDefs[ bestld].end].x) == (dy1 > 0))
			x0 = LineDefs[ bestld].sidedef1;
		else
			x0 = LineDefs[ bestld].sidedef2;
    }
    else {
		if ((Vertexes[ LineDefs[ bestld].start].y < Vertexes[ LineDefs[ bestld].end].y) != (dx1 > 0))
			x0 = LineDefs[ bestld].sidedef1;
		else
			x0 = LineDefs[ bestld].sidedef2;
    }
    
    /* there is no SideDef on this side of the LineDef! */
    if (x0 < 0)
		return -1;
    
    /* OK, we got it -- return the Sector number */
    return SideDefs[ x0].sector;
}



/*
   copy a group of objects to a new position
   */

void CopyObjects( BCINT objtype, SelPtr obj) /* SWAP! */
{
    BCINT        n, m;
    SelPtr     cur;
    SelPtr     list1, list2;
    SelPtr     ref1, ref2;
    
    if (obj == NULL)
		return;
    /* copy the object(s) */
    switch (objtype) {
    case OBJ_THINGS:
		for (cur = obj; cur; cur = cur->next) {
			InsertObject( OBJ_THINGS, cur->objnum, Things[ cur->objnum].xpos, Things[ cur->objnum].ypos);
			cur->objnum = NumThings - 1;
		}
		MadeChanges = TRUE;
		break;
		
    case OBJ_VERTEXES:
		for (cur = obj; cur; cur = cur->next) {
			InsertObject( OBJ_VERTEXES, cur->objnum, Vertexes[ cur->objnum].x, Vertexes[ cur->objnum].y);
			cur->objnum = NumVertexes - 1;
		}
		MadeChanges = TRUE;
		MadeMapChanges = TRUE;
		break;
		
    case OBJ_LINEDEFS:
		list1 = NULL;
		list2 = NULL;
		/* create the LineDefs */
		for (cur = obj; cur; cur = cur->next) {
			InsertObject( OBJ_LINEDEFS, cur->objnum, 0, 0);

			if (CopySideDefs) {
				if ((n = LineDefs[ cur->objnum].sidedef1) >= 0) {
					InsertObject( OBJ_SIDEDEFS, n, 0, 0);
					n = NumSideDefs - 1;
					LineDefs[NumLineDefs - 1].sidedef1 = n;
				}
				if ((m = LineDefs[ cur->objnum].sidedef2) >= 0) {
					InsertObject( OBJ_SIDEDEFS, m, 0, 0);
					m = NumSideDefs - 1;
					LineDefs[NumLineDefs - 1].sidedef2 = m;
				}
			}

			cur->objnum = NumLineDefs - 1;
			if (!IsSelected( list1, LineDefs[ cur->objnum].start)) {
				SelectObject( &list1, LineDefs[ cur->objnum].start);
				SelectObject( &list2, LineDefs[ cur->objnum].start);
			}
			if (!IsSelected( list1, LineDefs[ cur->objnum].end)) {
				SelectObject( &list1, LineDefs[ cur->objnum].end);
				SelectObject( &list2, LineDefs[ cur->objnum].end);
			}

		}
		/* create the Vertices */
		CopyObjects( OBJ_VERTEXES, list2);
		/* update the references to the Vertexes */
		for (ref1 = list1, ref2 = list2; ref1 && ref2; ref1 = ref1->next, ref2 = ref2->next) {
			for (cur = obj; cur; cur = cur->next) {
				if (ref1->objnum == LineDefs[ cur->objnum].start)
					LineDefs[ cur->objnum].start = ref2->objnum;
				if (ref1->objnum == LineDefs[ cur->objnum].end)
					LineDefs[ cur->objnum].end = ref2->objnum;
			}
		}



		ForgetSelection( &list1);
		ForgetSelection( &list2);
		break;
		
    case OBJ_SECTORS:
		list1 = NULL;
		list2 = NULL;
		/* create the LineDefs (and Vertices) */
		for (cur = obj; cur; cur = cur->next) {
			for (n = 0; n < NumLineDefs; n++)
				if ( (((m = LineDefs[ n].sidedef1) >= 0 && SideDefs[ m].sector == cur->objnum)
					  || ((m = LineDefs[ n].sidedef2) >= 0 && SideDefs[ m].sector == cur->objnum))
					&& ! IsSelected( list1, n)) {
					SelectObject( &list1, n);
					SelectObject( &list2, n);
				}
		}
		CopyObjects( OBJ_LINEDEFS, list2);
		/* create the SideDefs */
		for (ref1 = list1, ref2 = list2; ref1 && ref2; ref1 = ref1->next, ref2 = ref2->next) {
			if ((n = LineDefs[ ref1->objnum].sidedef1) >= 0) {
				InsertObject( OBJ_SIDEDEFS, n, 0, 0);
				n = NumSideDefs - 1;
				LineDefs[ ref2->objnum].sidedef1 = n;
			}
			if ((m = LineDefs[ ref1->objnum].sidedef2) >= 0) {
				InsertObject( OBJ_SIDEDEFS, m, 0, 0);
				m = NumSideDefs - 1;
				LineDefs[ ref2->objnum].sidedef2 = m;
			}
			ref1->objnum = n;
			ref2->objnum = m;
		}
		/* create the Sectors */
		for (cur = obj; cur; cur = cur->next) {
			InsertObject( OBJ_SECTORS, cur->objnum, 0, 0);
			for (ref1 = list1, ref2 = list2; ref1 && ref2; ref1 = ref1->next, ref2 = ref2->next) {
				if (ref1->objnum >= 0 && SideDefs[ ref1->objnum].sector == cur->objnum)
					SideDefs[ ref1->objnum].sector = NumSectors - 1;
				if (ref2->objnum >= 0 && SideDefs[ ref2->objnum].sector == cur->objnum)
					SideDefs[ ref2->objnum].sector = NumSectors - 1;
			}
			cur->objnum = NumSectors - 1;
		}
		ForgetSelection( &list1);
		ForgetSelection( &list2);
		break;
    }
}



/*
   move a group of objects to a new position
   (must be called with obj = NULL before moving the objects)
   */

Bool MoveObjectsToCoords( BCINT objtype, SelPtr obj, BCINT newx, BCINT newy, BCINT grid) /* SWAP! */
{
    BCINT   n, m;
    BCINT   dx, dy;
    SelPtr	cur, vertices;
    static BCINT refx, refy; /* previous position */
    
    if (grid > 0) {
		newx = (newx + grid / 2) & ~(grid - 1);
		newy = (newy + grid / 2) & ~(grid - 1);
    }
    
    /* only update the reference point? */
    if (obj == NULL) {
		refx = newx;
		refy = newy;
		return TRUE;
    }
    /* compute the displacement */
    dx = newx - refx;
    dy = newy - refy;
    /* nothing to do? */
    if (dx == 0 && dy == 0)
		return FALSE;
    
    /* move the object(s) */
    switch (objtype) {
    case OBJ_THINGS:
		for (cur = obj; cur; cur = cur->next) {
			Things[ cur->objnum].xpos += dx;
			Things[ cur->objnum].ypos += dy;
		}
		refx = newx;
		refy = newy;
		MadeChanges = TRUE;
		break;
    case OBJ_VERTEXES:
		for (cur = obj; cur; cur = cur->next) {
			Vertexes[ cur->objnum].x += dx;
			Vertexes[ cur->objnum].y += dy;
		}
		refx = newx;
		refy = newy;
		MadeChanges = TRUE;
		MadeMapChanges = TRUE;
		break;
    case OBJ_LINEDEFS:
		vertices = NULL;
		for (cur = obj; cur; cur = cur->next) {
			if (!IsSelected( vertices, LineDefs[ cur->objnum].start))
				SelectObject( &vertices, LineDefs[ cur->objnum].start);
			if (!IsSelected( vertices, LineDefs[ cur->objnum].end))
				SelectObject( &vertices, LineDefs[ cur->objnum].end);
		}
		MoveObjectsToCoords( OBJ_VERTEXES, vertices, newx, newy, grid);
		ForgetSelection( &vertices);
		break;
    case OBJ_SECTORS:
		vertices = NULL;
		for (cur = obj; cur; cur = cur->next) {
			for (n = 0; n < NumLineDefs; n++)
				if (((m = LineDefs[ n].sidedef1) >= 0 && SideDefs[ m].sector == cur->objnum)
					|| ((m = LineDefs[ n].sidedef2) >= 0 && SideDefs[ m].sector == cur->objnum)) {
					if (!IsSelected( vertices, LineDefs[ n].start))
						SelectObject( &vertices, LineDefs[ n].start);
					if (!IsSelected( vertices, LineDefs[ n].end))
						SelectObject( &vertices, LineDefs[ n].end);
				}
		}
		/*
		if (vertices != NULL) {
			BCINT minx, maxx, miny, maxy;
		  	minx = 32767;
		  	maxx = -32767;
		  	miny = 32767;
		  	maxy = -32767;
            for (cur = vertices; cur; cur = cur->next) {
            	if (Vertexes[cur->objnum].x < minx)
                	minx = Vertexes[cur->objnum].x;
                if (Vertexes[cur->objnum].x > maxx)
                    maxx = Vertexes[cur->objnum].x;
                if (Vertexes[cur->objnum].y < miny)
                    miny = Vertexes[cur->objnum].y;
                if (Vertexes[cur->objnum].y > maxy)
                    maxy = Vertexes[cur->objnum].y;
			}
            for (n = 0; n < NumThings; n++) {
            	if (Things[n].xpos < minx || Things[n].xpos > maxx || Things[n].ypos < miny || Things[n].ypos > maxy)
               		continue;
                m = GetSectorForThing(n);
				if (IsSelected(obj, m))
					SelectObject(&things, n);
            }

			MoveObjectsToCoords( OBJ_VERTEXES, vertices, newx, newy, grid);
			ForgetSelection( &vertices);
			for (cur = things; cur; cur = cur->next) {
				Things[cur->objnum].xpos += dx;
				Things[cur->objnum].ypos += dy;
			}
			ForgetSelection( &things);
		}
		*/
		MoveObjectsToCoords( OBJ_VERTEXES, vertices, newx, newy, grid);
		ForgetSelection( &vertices);
		break;
    }
    return TRUE;
}



/*
   get the coordinates (approx.) of an object
   */

void GetObjectCoords( BCINT objtype, BCINT objnum, BCINT *xpos, BCINT *ypos) /* SWAP! */
{
    BCINT  n, v1, v2, sd1, sd2;
    long accx, accy, num;
    
    switch (objtype) {
    case OBJ_THINGS:
		*xpos = Things[ objnum].xpos;
		*ypos = Things[ objnum].ypos;
		break;
    case OBJ_VERTEXES:
		*xpos = Vertexes[ objnum].x;
		*ypos = Vertexes[ objnum].y;
		break;
    case OBJ_LINEDEFS:
		v1 = LineDefs[ objnum].start;
		v2 = LineDefs[ objnum].end;
		*xpos = (Vertexes[ v1].x + Vertexes[ v2].x) / 2;
		*ypos = (Vertexes[ v1].y + Vertexes[ v2].y) / 2;
		break;
    case OBJ_SIDEDEFS:
		for (n = 0; n < NumLineDefs; n++)
			if (LineDefs[ n].sidedef1 == objnum || LineDefs[ n].sidedef2 == objnum) {
				v1 = LineDefs[ n].start;
				v2 = LineDefs[ n].end;
				*xpos = (Vertexes[ v1].x + Vertexes[ v2].x) / 2;
				*ypos = (Vertexes[ v1].y + Vertexes[ v2].y) / 2;
				return;
			}
		*xpos = (MapMinX + MapMaxX) / 2;
		*ypos = (MapMinY + MapMaxY) / 2;
    case OBJ_SECTORS:
		accx = 0L;
		accy = 0L;
		num = 0L;
		for (n = 0; n < NumLineDefs; n++) {
			sd1 = LineDefs[ n].sidedef1;
			sd2 = LineDefs[ n].sidedef2;
			v1 = LineDefs[ n].start;
			v2 = LineDefs[ n].end;
			if ((sd1 >= 0 && SideDefs[ sd1].sector == objnum) || (sd2 >= 0 && SideDefs[ sd2].sector == objnum)) {
				/* if the Sector is closed, all Vertices will be counted twice */
				accx += (long) Vertexes[ v1].x;
				accy += (long) Vertexes[ v1].y;
				num++;
				accx += (long) Vertexes[ v2].x;
				accy += (long) Vertexes[ v2].y;
				num++;
			}
		}
		if (num > 0) {
			*xpos = (BCINT) ((accx + num / 2L) / num);
			*ypos = (BCINT) ((accy + num / 2L) / num);
		}
		else {
			*xpos = (MapMinX + MapMaxX) / 2;
			*ypos = (MapMinY + MapMaxY) / 2;
		}
		break;
    }
}



/*
   rotate and scale a group of objects around the center of gravity
	 jff compute center, scale, angle in double to improve accuracy
   */

void RotateAndScaleObjects( BCINT objtype, SelPtr obj, double angle, double scale, int flipmirr)
{
  BCINT    n, m;
  double   dx, dy;
  double   centerx, centery;
  long   accx, accy, num;
  SelPtr cur, vertices;
  
  if (obj == NULL)
		return;
  
  switch (objtype)
	{
  	case OBJ_THINGS:
    	if (obj->next == NULL)
    		break;
			accx = 0L;
			accy = 0L;
			num = 0L;
			for (cur = obj; cur; cur = cur->next)
			{
				accx += (long) Things[ cur->objnum].xpos;
				accy += (long) Things[ cur->objnum].ypos;
				num++;
			}
			centerx = accx; centerx /= num;
			centery = accy;	centery /= num;
			for (cur = obj; cur; cur = cur->next)
			{
				dx = Things[ cur->objnum].xpos - centerx;
				dy = Things[ cur->objnum].ypos - centery;
				if (!flipmirr)
					RotateAndScaleCoords( &dx, &dy, angle, scale);
				else if (flipmirr==1)
					dy = -dy;
				else if (flipmirr==2)
					dx = -dx;

				Things[ cur->objnum].xpos = (BCINT) (centerx + dx + 0.5);
				Things[ cur->objnum].ypos = (BCINT) (centery + dy + 0.5);
			}
			MadeChanges = TRUE;
			break;

  	case OBJ_VERTEXES:
			accx = 0L;
			accy = 0L;
			num = 0L;
			for (cur = obj; cur; cur = cur->next)
			{
				accx += (long) Vertexes[ cur->objnum].x;
				accy += (long) Vertexes[ cur->objnum].y;
				num++;
			}
			centerx = accx; centerx /= num;
			centery = accy;	centery /= num;
			for (cur = obj; cur; cur = cur->next)
			{
				dx = Vertexes[ cur->objnum].x - centerx;
				dy = Vertexes[ cur->objnum].y - centery;
				if (!flipmirr)
					RotateAndScaleCoords( &dx, &dy, angle, scale);
				else if (flipmirr==1)
					dy = -dy;
				else if (flipmirr==2)
					dx = -dx;
				Vertexes[ cur->objnum].x = (BCINT) (centerx + dx + 0.5);
				Vertexes[ cur->objnum].y = (BCINT) (centery + dy + 0.5);
			}
			MadeChanges = TRUE;
			MadeMapChanges = TRUE;
			break;

  	case OBJ_LINEDEFS:
			vertices = NULL;
			for (cur = obj; cur; cur = cur->next)
			{
				if (!IsSelected( vertices, LineDefs[ cur->objnum].start))
					SelectObject( &vertices, LineDefs[ cur->objnum].start);
				if (!IsSelected( vertices, LineDefs[ cur->objnum].end))
					SelectObject( &vertices, LineDefs[ cur->objnum].end);
			}
			RotateAndScaleObjects( OBJ_VERTEXES, vertices, angle, scale, flipmirr);
			ForgetSelection( &vertices);
			break;

 		case OBJ_SECTORS:
			vertices = NULL;
			for (cur = obj; cur; cur = cur->next)
			{
				for (n = 0; n < NumLineDefs; n++)
					if (((m = LineDefs[ n].sidedef1) >= 0 && SideDefs[ m].sector == cur->objnum)
						|| ((m = LineDefs[ n].sidedef2) >= 0 && SideDefs[ m].sector == cur->objnum)) {
						if (!IsSelected( vertices, LineDefs[ n].start))
							SelectObject( &vertices, LineDefs[ n].start);
						if (!IsSelected( vertices, LineDefs[ n].end))
							SelectObject( &vertices, LineDefs[ n].end);
					}
			}
			RotateAndScaleObjects( OBJ_VERTEXES, vertices, angle, scale, flipmirr);
			ForgetSelection( &vertices);
			break;
  }
}



/*
   find a free tag number
   */

BCINT FindFreeTag() /* SWAP! */
{
    BCINT  tag, n;
    Bool ok;
#ifdef GAME_HEXEN
	char argtext[ 5];
#endif
    
    tag = 1;
    ok = FALSE;
    while (! ok) {
		ok = TRUE;
		for (n = 0; n < NumLineDefs; n++) {
#ifdef GAME_HEXEN
			strcpy (argtext, GetLineDefArgs(LineDefs[ n].special));
			if (argtext[0] == '$') {
				if (LineDefs[ n].arg1 == tag) {
#else
				if (LineDefs[ n].tag == tag) {
#endif
					ok = FALSE;
					break;
				}
#ifdef GAME_HEXEN
			}
#endif
		}
		if (ok)
			for (n = 0; n < NumSectors; n++)
				if (Sectors[ n].tag == tag) {
					ok = FALSE;
					break;
				}
		tag++;
    }
    return tag - 1;
}

#ifdef GAME_HEXEN

/*
   find a free tid number
   */

BCINT FindFreeTid() /* SWAP! */
{
    BCINT  tid, n;
    Bool ok;
    
	tid = 1;
    ok = FALSE;
    while (! ok) {
    	ok = TRUE;
		for (n = 0; n < NumThings; n++)
			if (Things[ n].tid == tid) {
				ok = FALSE;
				break;
			}
			tid++;
	}
    return tid - 1;
}

#endif


/*
   flip one or several LineDefs
   */

void FlipLineDefs( SelPtr obj, Bool swapvertices) /* SWAP! */
{
    SelPtr cur;
    BCINT    tmp;

    
    for (cur = obj; cur; cur = cur->next) {
		if (LineDefs[cur->objnum].sidedef2 == -1 && !swapvertices)
			continue;
		if (swapvertices) {
			/* swap starting and ending Vertices */
			tmp = LineDefs[ cur->objnum].end;
			LineDefs[ cur->objnum].end = LineDefs[ cur->objnum].start;
			LineDefs[ cur->objnum].start = tmp;
		}
		/* swap first and second SideDefs */
		tmp = LineDefs[ cur->objnum].sidedef1;
		LineDefs[ cur->objnum].sidedef1 = LineDefs[ cur->objnum].sidedef2;
		LineDefs[ cur->objnum].sidedef2 = tmp;
    }
    MadeChanges = TRUE;
    MadeMapChanges = TRUE;
}



/*
   delete a Vertex and join the two Linedefs
   */

void DeleteVerticesJoinLineDefs( SelPtr obj) /* SWAP! */
{
    BCINT    lstart, lend, l;
    SelPtr cur;
    char   msg[ 80];
    
    while (obj != NULL) {
		cur = obj;
		obj = obj->next;
		lstart = -1;
		lend = -1;
		for (l = 0; l < NumLineDefs; l++) {
			if (LineDefs[ l].start == cur->objnum) {
				if (lstart == -1)
					lstart = l;
				else
					lstart = -2;
			}
			if (LineDefs[ l].end == cur->objnum) {
				if (lend == -1)
					lend = l;
				else
					lend = -2;
			}
		}
		if (lstart < 0 || lend < 0) {
			Beep();
			sprintf(msg, "Cannot Delete Vertex #%d And Join The LineDefs", cur->objnum);
			Notify( -1, -1, msg, "The Vertex Must Be The Start Of One LineDef And The End Of Another One");
			continue;
		}
		LineDefs[ lend].end = LineDefs[ lstart].end;
		DeleteObject( OBJ_LINEDEFS, lstart);
		DeleteObject( OBJ_VERTEXES, cur->objnum);
		MadeChanges = TRUE;
		MadeMapChanges = TRUE;
    }
}



/*
   merge several vertices into one
   */

void MergeVertices( SelPtr *list) /* SWAP! */
{
    BCINT    v, l;
    
    v = (*list)->objnum;
    UnSelectObject( list, v);
    if (*list == NULL) {
		Beep();
		Notify( -1, -1, "You Must Select At Least Two Vertices", NULL);
		return;
    }
    /* change the LineDefs starts & ends */
    for (l = 0; l < NumLineDefs; l++) {
		if (IsSelected( *list, LineDefs[ l].start)) {      
			/* don't change a LineDef that has both ends on the same spot */
			if (!IsSelected( *list, LineDefs[ l].end) && LineDefs[ l].end != v)
				LineDefs[ l].start = v;
		}
		else if (IsSelected( *list, LineDefs[ l].end)) {
			/* idem */
			if (LineDefs[ l].start != v)
				LineDefs[ l].end = v;
		}
    }
    /* delete the Vertices (and some LineDefs too) */
    DeleteObjects( OBJ_VERTEXES, list);
    MadeChanges = TRUE;
    MadeMapChanges = TRUE;
}



/*
   check if some vertices should be merged into one
   */

Bool AutoMergeVertices( SelPtr *list) /* SWAP! */
{
    SelPtr ref, cur;
    Bool   confirmed, redraw, flipped, mergedone, isldend;
    BCINT  v, refv, ld, sd, oldnumld;
    
    confirmed = FALSE;
    redraw = FALSE;
    mergedone = FALSE;
    isldend = FALSE;
    ref = *list;
    while (ref) {
		refv = ref->objnum;
		ref = ref->next;
		/* check if there is a Vertex at the same position (same X and Y) */
		for (v = 0; v < NumVertexes; v++)
			if (v != refv && Vertexes[ refv].x == Vertexes[ v].x && Vertexes[ refv].y == Vertexes[ v].y) {
				redraw = TRUE;
				if (confirmed || (Expert && !VertConf) || Confirm( -1, -1, "Some Vertices Occupy The Same Position", "Do You Want To Merge Them Into One?")) {
					/* don't ask for confirmation twice */
					confirmed = TRUE;
					/* merge the two vertices */
					mergedone = TRUE;
					cur = NULL;
					SelectObject( &cur, refv);
					SelectObject( &cur, v);
					MergeVertices( &cur);
					/* not useful but safer... */
					/* update the references in the selection list */
					for (cur = *list; cur; cur = cur->next)
						if (cur->objnum > refv)
							cur->objnum = cur->objnum - 1;
					if (v > refv)
						v--;
					/* the old Vertex has been deleted */
					UnSelectObject( list, refv);
					/* select the new Vertex instead */
					if (!IsSelected( *list, v))
						SelectObject( list, v);
					break;
				}
				else
					return redraw;
			}
    }
    confirmed = FALSE;
    ref = *list;
    while (ref) {
		refv = ref->objnum;
		ref = ref->next;
		oldnumld = NumLineDefs;
		/* check if this Vertex is on a LineDef */
		for (ld = 0; ld < oldnumld; ld++) {
			if (LineDefs[ ld].start == refv || LineDefs[ ld].end == refv) {
				/* one Vertex had a LineDef bound to it -- check it later */
				isldend = TRUE;
			}
			else if (IsLineDefInside( ld, Vertexes[ refv].x - CloseToLine, Vertexes[ refv].y - CloseToLine, Vertexes[ refv].x + CloseToLine, Vertexes[ refv].y + CloseToLine)) {
				redraw = TRUE;
				if (confirmed || (Expert && !VertConf) || Confirm( -1, -1, "Some Vertices Are On A LineDef", "Do You Want To Split The LineDef There?")) {
					/* don't ask for confirmation twice */
					confirmed = TRUE;
					/* split the LineDef */
					mergedone = TRUE;
					InsertObject( OBJ_LINEDEFS, ld, 0, 0);
					LineDefs[ ld].end = refv;
					LineDefs[ NumLineDefs - 1].start = refv;
					sd = LineDefs[ ld].sidedef1;
					if (sd >= 0) {
						InsertObject( OBJ_SIDEDEFS, sd, 0, 0);
						LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
					}
					sd = LineDefs[ ld].sidedef2;
					if (sd >= 0) {
						InsertObject( OBJ_SIDEDEFS, sd, 0, 0);
						LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 1;
					}
					MadeChanges = TRUE;
					MadeMapChanges = TRUE;
				}
				else
					return redraw;
			}
		}
    }
    /* don't continue if this isn't necessary */
    if (isldend == FALSE || mergedone == FALSE)
		return redraw;
    
    confirmed = FALSE;
    /* test if two LineDefs are at between the same pair of Vertices */
    for (v = 0; v < NumLineDefs - 1; v++)
		for (ld = v + 1; ld < NumLineDefs; ld++)
			if ((LineDefs[ v].start == LineDefs[ ld].start && LineDefs[ v].end == LineDefs[ ld].end)
				|| (LineDefs[ v].start == LineDefs[ ld].end && LineDefs[ v].end == LineDefs[ ld].start)) {
				redraw = TRUE;
				if (confirmed || (Expert && !VertConf) || Confirm( -1, -1, "Some LineDefs Are Superimposed", "Do You Want To Merge Them Into One?")) {
					/* don't ask for confirmation twice */
					confirmed = TRUE;
					/* test if the LineDefs have the same orientation */
					if (LineDefs[ v].start == LineDefs[ ld].end)
						flipped = TRUE;
					else
						flipped = FALSE;
					/* merge the two LineDefs */
					if (LineDefs[ v].sidedef1 < 0) {
						if (flipped) {
							LineDefs[ v].sidedef1 = LineDefs[ ld].sidedef2;
							LineDefs[ ld].sidedef2 = -1;
						}
						else {
							LineDefs[ v].sidedef1 = LineDefs[ ld].sidedef1;
							LineDefs[ ld].sidedef1 = -1;
						}
					}
					if (LineDefs[ v].sidedef2 < 0) {
						if (flipped) {
							LineDefs[ v].sidedef2 = LineDefs[ ld].sidedef1;
							LineDefs[ ld].sidedef1 = -1;
						}
						else {
							LineDefs[ v].sidedef2 = LineDefs[ ld].sidedef2;
							LineDefs[ ld].sidedef2 = -1;
						}
					}
					if (LineDefs[ v].sidedef1 >= 0 && LineDefs[ v].sidedef2 >= 0 && (LineDefs[ v].flags & 0x04) == 0)
						LineDefs[ v].flags = 0x04;
					DeleteObject( OBJ_LINEDEFS, ld);
				}
			}
    return redraw;
}



/*
   split one or more LineDefs in two, adding new Vertices in the middle
   */

void SplitLineDefs( SelPtr obj) /* SWAP! */
{
    SelPtr cur;
    BCINT    vstart, vend, sd;
    
    for (cur = obj; cur; cur = cur->next) {
		vstart = LineDefs[ cur->objnum].start;
		vend = LineDefs[ cur->objnum].end;
		InsertObject( OBJ_VERTEXES, -1, (Vertexes[ vstart].x + Vertexes[ vend].x) / 2, (Vertexes[ vstart].y + Vertexes[ vend].y) / 2);
		InsertObject( OBJ_LINEDEFS, cur->objnum, 0, 0);
		LineDefs[ cur->objnum].end = NumVertexes - 1;
		LineDefs[ NumLineDefs - 1].start = NumVertexes - 1;
		sd = LineDefs[ cur->objnum].sidedef1;
		if (sd >= 0) {
			InsertObject( OBJ_SIDEDEFS, sd, 0, 0);
			LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
		}
		sd = LineDefs[ cur->objnum].sidedef2;
		if (sd >= 0) {
			InsertObject( OBJ_SIDEDEFS, sd, 0, 0);
			LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 1;
		}                
    }
    MadeChanges = TRUE;
    MadeMapChanges = TRUE;
}



/*
   split a Sector in two, adding a new LineDef between the two Vertices
   */

void SplitSector( BCINT vertex1, BCINT vertex2) /* SWAP! */
{
    SelPtr llist;
    BCINT    curv, s, l, sd, sd1, sd2; //jff
    char   msg1[ 80], msg2[ 80];
    
    /* check if there is a Sector between the two Vertices (in the middle) */
    s = GetCurObject( OBJ_SECTORS, Vertexes[ vertex1].x, Vertexes[ vertex1].y, Vertexes[ vertex2].x, Vertexes[ vertex2].y);
    if (s < 0)
		{
			Beep();
			sprintf( msg1, "There Is No Sector Between Vertex #%d And Vertex #%d", vertex1, vertex2);
			Notify( -1, -1, msg1, NULL);
			return;
    }
    /* check if there is a closed path from vertex1 to vertex2, along the edge of the Sector s */
    llist = NULL;
    curv = vertex1;
    while (curv != vertex2)
		{
			for (l = 0; l < NumLineDefs; l++)
			{
				sd1 = LineDefs[ l].sidedef1;// jff prevent looping forever thru
				sd2 = LineDefs[ l].sidedef2;// intrasector lines
				if (!IsSelected(llist,l) /*sd1<0 || sd2<0  || SideDefs[sd1].sector != SideDefs[sd2].sector*/)
				{
					if (sd1 >= 0 && SideDefs[ sd1].sector == s && LineDefs[ l].start == curv)
					{
						curv = LineDefs[ l].end;
						SelectObject( &llist, l);
						break;
					}
					if (sd2 >= 0 && SideDefs[ sd2].sector == s && LineDefs[ l].end == curv)
					{
						curv = LineDefs[ l].start;
						SelectObject( &llist, l);
						break;
					}
				}
			}
			if (l >= NumLineDefs)
			{
				Beep();
				sprintf( msg1, "Cannot Find A Closed Path From Vertex #%d To Vertex #%d", vertex1, vertex2);
				if (curv == vertex1)
					sprintf( msg2, "There is no SideDef starting from Vertex #%d on Sector #%d", vertex1, s);
				else
					sprintf( msg2, "Check If Sector #%d Is Closed (Cannot Go Past Vertex #%d)", s, curv);
				Notify( -1, -1, msg1, msg2);
				ForgetSelection( &llist);
				return;
			}
			if (curv == vertex1)
			{
				Beep();    
				sprintf( msg1, "Vertex #%d Is Not On The Same Sector (#%d) As Vertex #%d", vertex2, s, vertex1);
				Notify( -1, -1, msg1, NULL);
				ForgetSelection( &llist);
				return;
			}
    }
    /* now, the list of LineDefs for the new Sector is in llist */
    
    /* add the new Sector, LineDef and SideDefs */
    InsertObject( OBJ_SECTORS, s, 0, 0);
    InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    LineDefs[ NumLineDefs - 1].start = vertex1;
    LineDefs[ NumLineDefs - 1].end = vertex2;
    LineDefs[ NumLineDefs - 1].flags = 4;
    InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    SideDefs[ NumSideDefs - 1].sector = s;
    strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 2;
    LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 1;
    
    /* bind all LineDefs in llist to the new Sector */
    while (llist)
		{
			sd = LineDefs[ llist->objnum].sidedef1;
			if (sd < 0 || SideDefs[ sd].sector != s)
				sd = LineDefs[ llist->objnum].sidedef2;
			SideDefs[ sd].sector = NumSectors - 1;
			UnSelectObject( &llist, llist->objnum);
    }
    
    
    /* second check... uselful for Sectors within Sectors */
    for (l = 0; l < NumLineDefs; l++)
		{
			sd = LineDefs[ l].sidedef1;
			if (sd >= 0 && SideDefs[ sd].sector == s)
			{
				curv = GetOppositeSector( l, TRUE);
				if (curv == NumSectors - 1)
					SideDefs[ sd].sector = NumSectors - 1;
			}
			sd = LineDefs[ l].sidedef2;
			if (sd >= 0 && SideDefs[ sd].sector == s)
			{
				curv = GetOppositeSector( l, FALSE);
				if (curv == NumSectors - 1)
					SideDefs[ sd].sector = NumSectors - 1;
			}
    }
    
    MadeChanges = TRUE;
    MadeMapChanges = TRUE;
}



/*
   split two LineDefs, then split the Sector and add a new LineDef between the new Vertices
   */

void SplitLineDefsAndSector( BCINT linedef1, BCINT linedef2) /* SWAP! */
{
    SelPtr llist;
    BCINT    s1, s2, s3, s4;
    char   msg[ 80];
    
    /* check if the two LineDefs are adjacent to the same Sector */
    s1 = LineDefs[ linedef1].sidedef1;
    s2 = LineDefs[ linedef1].sidedef2;
    s3 = LineDefs[ linedef2].sidedef1;
    s4 = LineDefs[ linedef2].sidedef2;
    if (s1 >= 0)
			s1 = SideDefs[ s1].sector;
    if (s2 >= 0)
			s2 = SideDefs[ s2].sector;
    if (s3 >= 0)
			s3 = SideDefs[ s3].sector;
    if (s4 >= 0)
			s4 = SideDefs[ s4].sector;

  	if
		(
			(s1 < 0 || (s1 != s3 && s1 != s4)) &&
			(s2 < 0 || (s2 != s3 && s2 != s4))
		)
		{
			Beep();
			sprintf( msg, "LineDefs #%d And #%d Are Not Adjacent To The Same Sector", linedef1, linedef2);
			Notify( -1, -1, msg, NULL);
			return;
    }

    /* split the two LineDefs and create two new Vertices */
    llist = NULL;
    SelectObject( &llist, linedef1);
    SelectObject( &llist, linedef2);
    SplitLineDefs( llist);
    ForgetSelection( &llist);
    /* split the Sector and create a LineDef between the two Vertices */
    SplitSector( NumVertexes - 1, NumVertexes - 2);
}

/*
   connect two LineDefs, forming a new sector that splits a donut
	 jff - needs better error checking
   */

void ConnectLineDefsSplitDonut( BCINT linedef1, BCINT linedef2) /* SWAP! */
{
    BCINT    s1, s2, s3, s4;
    char   msg[ 80];
    
    /* check if the two LineDefs are adjacent to the same Sector */
    s1 = LineDefs[ linedef1].sidedef1;
    s2 = LineDefs[ linedef1].sidedef2;
    s3 = LineDefs[ linedef2].sidedef1;
    s4 = LineDefs[ linedef2].sidedef2;
    if (s1 >= 0)
			s1 = SideDefs[ s1].sector;
    if (s2 >= 0)
			s2 = SideDefs[ s2].sector;
    if (s3 >= 0)
			s3 = SideDefs[ s3].sector;
    if (s4 >= 0)
			s4 = SideDefs[ s4].sector;

    if
		(
			(s1 >= 0  && s2 >= 0 && s1 == s2) ||
			(s3 >= 0  && s4 >= 0 && s3 == s4)
		)
		{
			Beep();
			sprintf( msg, "Cannot Split Donut Using Line With Same Sector On Both Sides");
			Notify( -1, -1, msg, NULL);
			return;
    }

		if (s1 > 0 && s1 == s3)
		{
			InsertObject(OBJ_SECTORS,s1,0,0);

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef1].end;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef2].start;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s1;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef2].end;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef1].start;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s1;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

			SideDefs[ LineDefs[linedef1].sidedef1].sector = NumSectors - 1;
			SideDefs[ LineDefs[linedef2].sidedef1].sector = NumSectors - 1;
		}
		else if (s1 > 0 && s1 == s4)
		{
			InsertObject(OBJ_SECTORS,s1,0,0);

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef1].end;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef2].end;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s1;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef2].start;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef1].start;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s1;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

			SideDefs[ LineDefs[linedef1].sidedef1].sector = NumSectors - 1;
			SideDefs[ LineDefs[linedef2].sidedef2].sector = NumSectors - 1;
		}
		else if (s2 > 0 && s2 == s3)
		{
			InsertObject(OBJ_SECTORS,s1,0,0);

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef1].start;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef2].start;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s2;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef2].end;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef1].end;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s2;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

			SideDefs[ LineDefs[linedef1].sidedef2].sector = NumSectors - 1;
			SideDefs[ LineDefs[linedef2].sidedef1].sector = NumSectors - 1;
		}
		else if (s2 > 0 && s2 == s4)
		{
			InsertObject(OBJ_SECTORS,s1,0,0);

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef1].start;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef2].end;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s2;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

    	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
    	LineDefs[ NumLineDefs - 1].start = LineDefs[ linedef2].start;
    	LineDefs[ NumLineDefs - 1].end = LineDefs[ linedef1].end;
    	LineDefs[ NumLineDefs - 1].flags = 4;
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	SideDefs[ NumSideDefs - 1].sector = s2;
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
    	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
    	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
    	LineDefs[ NumLineDefs - 1].sidedef2 = NumSideDefs - 2;

			SideDefs[ LineDefs[linedef1].sidedef2].sector = NumSectors - 1;
			SideDefs[ LineDefs[linedef2].sidedef2].sector = NumSectors - 1;
		}
		else
		{
			Beep();
			sprintf( msg, "LineDefs #%d And #%d Are Not Adjacent To The Same Sector", linedef1, linedef2);
			Notify( -1, -1, msg, NULL);
			return;
		}

    MadeChanges = TRUE;
    MadeMapChanges = TRUE;
}

/*
   connect two 1S LineDefs which face away from each other,
	 forming a new sector in void space that connects two sectors
   */

void ConnectLineDefsMakeHallway(BCINT linedef1, BCINT linedef2)
{
    BCINT    s1, s2, s3, s4;
		int      dx1, dy1, dx2, dy2, dx3, dy3;
		long     d,t,f;
    char     msg[80];
    
    /* check if the two LineDefs are 1S and face away from each other */
    s1 = LineDefs[ linedef1].sidedef1;
    s2 = LineDefs[ linedef1].sidedef2;
    s3 = LineDefs[ linedef2].sidedef1;
    s4 = LineDefs[ linedef2].sidedef2;
    if (s2 >= 0 || s4 >= 0 || s1 < 0 || s3 < 0)
		{
			Beep();
			sprintf( msg, "Both lines marked must be 1S\n");
			Notify( -1, -1, msg, NULL);
			return;
    }

		dx1 = Vertexes[LineDefs[linedef1].end].x-Vertexes[LineDefs[linedef1].start].x;
		dy1 = Vertexes[LineDefs[linedef1].end].y-Vertexes[LineDefs[linedef1].start].y;
		dx2 = Vertexes[LineDefs[linedef2].end].x-Vertexes[LineDefs[linedef2].start].x;
		dy2 = Vertexes[LineDefs[linedef2].end].y-Vertexes[LineDefs[linedef2].start].y;
		dx3 = Vertexes[LineDefs[linedef2].end].x-Vertexes[LineDefs[linedef1].start].x;
		dy3 = Vertexes[LineDefs[linedef2].end].y-Vertexes[LineDefs[linedef1].start].y;
	  d = dx1*dy2 - dx2*dy1;			// T-S X Q-P
	  t = dy3*dx1 - dy1*dx3;			// T-P X Q-P   >0 means left of
	  f = t-d;										// S-P X Q-P	 >0 means left of

		if (dx1*dx2+dy1*dy2 > 0 || t<0 || f<0)
		{
			Beep();
			sprintf( msg, "The two lines marked must face away from each other\n");
			Notify( -1, -1, msg, NULL);
			return;
    }

		InsertObject(OBJ_SECTORS,SideDefs[s1].sector,0,0);

   	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
   	LineDefs[ NumLineDefs - 1].start = LineDefs[linedef2].start;
   	LineDefs[ NumLineDefs - 1].end = LineDefs[linedef1].end;
   	LineDefs[ NumLineDefs - 1].flags = 1;

   	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
   	SideDefs[ NumSideDefs - 1].sector = NumSectors - 1;
   	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
   	LineDefs[ NumLineDefs - 1].sidedef2 = -1;

   	InsertObject( OBJ_LINEDEFS, -1, 0, 0);
   	LineDefs[ NumLineDefs - 1].start = LineDefs[linedef1].start;
   	LineDefs[ NumLineDefs - 1].end = LineDefs[linedef2].end;
   	LineDefs[ NumLineDefs - 1].flags = 1;

   	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
   	SideDefs[ NumSideDefs - 1].sector = NumSectors - 1;
   	LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs - 1;
   	LineDefs[ NumLineDefs - 1].sidedef2 = -1;

   	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
   	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
		SideDefs[NumSideDefs-1].sector = NumSectors - 1;

   	InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
   	strncpy( SideDefs[ NumSideDefs - 1].tex3, "-", 8);
		SideDefs[NumSideDefs-1].sector = NumSectors - 1;

		LineDefs[linedef1].sidedef2 = NumSideDefs-2;
   	LineDefs[linedef1].flags = 4;
   	strncpy(SideDefs[ LineDefs[linedef1].sidedef1].tex3, "-", 8);
		LineDefs[linedef2].sidedef2 = NumSideDefs-1;
   	LineDefs[linedef2].flags = 4;
   	strncpy(SideDefs[ LineDefs[linedef2].sidedef1].tex3, "-", 8);

    MadeChanges = TRUE;
    MadeMapChanges = TRUE;
}


/*
   merge two or more Sectors into one
   */

void MergeSectors( SelPtr *slist) /* SWAP! */
{
    SelPtr cur;
    BCINT    n, olds, news;
    
    /* save the first Sector number */
    news = (*slist)->objnum;
    UnSelectObject( slist, news);
    
    /* change all SideDefs references to the other Sectors */
    for (cur = *slist; cur; cur = cur->next) {
		olds = cur->objnum;
		for (n = 0; n < NumSideDefs; n++) {
			if (SideDefs[ n].sector == olds)
				SideDefs[ n].sector = news;
		}
    }
    
    /* delete the Sectors */
    DeleteObjects( OBJ_SECTORS, slist);
    
    /* the returned list contains only the first Sector */
    SelectObject( slist, news);
}



/*
   delete one or several two-sided LineDefs and join the two Sectors
   */

void DeleteLineDefsJoinSectors( SelPtr *ldlist) /* SWAP! */
{
    SelPtr cur, slist;
    BCINT  sd1, sd2, s1, s2;
    char   msg[ 80];
    
    /* first, do the tests for all LineDefs */
    for (cur = *ldlist; cur; cur = cur->next) {
		sd1 = LineDefs[ cur->objnum].sidedef1;
		sd2 = LineDefs[ cur->objnum].sidedef2;
		if (sd1 < 0 || sd2 < 0) {
			Beep();
			sprintf( msg, "ERROR: LineDef #%d Has Only One Side", cur->objnum);
			Notify( -1, -1, msg, NULL);
			return;
		}
		s1 = SideDefs[ sd1].sector;
		s2 = SideDefs[ sd2].sector;
		if (s1 < 0 || s2 < 0) {
			Beep();
			sprintf( msg, "ERROR: LineDef #%d Has Two Sides, But One", cur->objnum);
			Notify( -1, -1, msg, "Side Is Not Bound To Any Sector");
			return;
		}
    }
    
    /* then join the Sectors and delete the LineDefs */
    for (cur = *ldlist; cur; cur = cur->next) {
		sd1 = LineDefs[ cur->objnum].sidedef1;
		sd2 = LineDefs[ cur->objnum].sidedef2;
		s1 = SideDefs[ sd1].sector;
		s2 = SideDefs[ sd2].sector;
		slist = NULL;
		SelectObject( &slist, s2);
		SelectObject( &slist, s1);
		MergeSectors( &slist);
		ForgetSelection( &slist);
    }
    DeleteObjects( OBJ_LINEDEFS, ldlist);
}




/*
   turn a Sector into a door: change the LineDefs and SideDefs
   */

void MakeDoorFromSector( BCINT sector) /* SWAP! */
{
    BCINT    sd1, sd2;
    BCINT    n, s;
    SelPtr ldok, ldflip, ld1s;
    
    ldok = NULL;
    ldflip = NULL;
    ld1s = NULL;
    s = 0;
    /* build lists of LineDefs that border the Sector */
    for (n = 0; n < NumLineDefs; n++) {
		sd1 = LineDefs[ n].sidedef1;
		sd2 = LineDefs[ n].sidedef2;
		if (sd1 >= 0 && sd2 >= 0) {
			if (SideDefs[ sd2].sector == sector) {
				SelectObject( &ldok, n); /* already ok */
				s++;
			}
			if (SideDefs[ sd1].sector == sector) {
				SelectObject( &ldflip, n); /* must be flipped */
				s++;
			}
		}
		else if (sd1 >= 0 && sd2 < 0) {
			if (SideDefs[ sd1].sector == sector)
				SelectObject( &ld1s, n); /* wall (one-sided) */
		}
    }
    /* a normal door has two sides... */
    if (s < 2) {
		Beep();
		Notify( -1, -1, "The Door Must Be Connected To Two Other Sectors.", NULL);
		ForgetSelection( &ldok);
		ForgetSelection( &ldflip);
		ForgetSelection( &ld1s);
		return;
    }
    if ((s > 2) && !(Expert || Confirm( -1, -1, "The Door Will Have More Than Two Sides.", "Do You Still Want To Create It?"))) {
		ForgetSelection( &ldok);
		ForgetSelection( &ldflip);
		ForgetSelection( &ld1s);
		return;
    }
    /* flip the LineDefs that have the wrong orientation */
    if (ldflip != NULL)
		FlipLineDefs( ldflip, TRUE);
    /* merge the two selection lists */
    while (ldflip != NULL) {
		if (!IsSelected( ldok, ldflip->objnum))
			SelectObject( &ldok, ldflip->objnum);
		UnSelectObject( &ldflip, ldflip->objnum);
    }
    /* change the LineDefs and SideDefs */
    while (ldok != NULL) {
		/* give the "normal door" type and flags to the LineDef */
		n = ldok->objnum;
		LineDefs[ n].special = 1;
		LineDefs[ n].flags = 0x04;
		sd1 = LineDefs[ n].sidedef1;
		sd2 = LineDefs[ n].sidedef2;
		/* adjust the textures for the SideDefs */
		if (strncmp( SideDefs[ sd1].tex3, "-", 8)) {
			if (!strncmp( SideDefs[ sd1].tex1, "-", 8))
				strncpy( SideDefs[ sd1].tex1, SideDefs[ sd1].tex3, 8);
			strncpy( SideDefs[ sd1].tex3, "-", 8);
		}
		if (!strncmp( SideDefs[ sd1].tex1, "-", 8))
			strncpy( SideDefs[ sd1].tex1, DefaultDoorTexture, 8);
		strncpy( SideDefs[ sd2].tex3, "-", 8);
		UnSelectObject( &ldok, n);
    }
    while (ld1s != NULL) {
		/* give the "door side" flags to the LineDef */
		n = ld1s->objnum;
		LineDefs[ n].flags = 0x11;
		sd1 = LineDefs[ n].sidedef1;
		/* adjust the textures for the SideDef */
		/* if (!strncmp( SideDefs[ sd1].tex3, "-", 8)) */
		strncpy( SideDefs[ sd1].tex3, DefaultDoorTrack, 8);
		strncpy( SideDefs[ sd1].tex1, "-", 8);
		strncpy( SideDefs[ sd1].tex2, "-", 8);
		UnSelectObject( &ld1s, n);
    }
    /* adjust the ceiling height */
    Sectors[ sector].ceilh = Sectors[ sector].floorh;
}



/*
   turn a Sector into a lift: change the LineDefs and SideDefs
   */

void MakeLiftFromSector( BCINT sector) /* SWAP! */
{
    BCINT    sd1, sd2;
    BCINT    n, s, tag;
    SelPtr ldok, ldflip, ld1s;
    SelPtr sect, curs;
    BCINT    minh, maxh;
    
    ldok = NULL;
    ldflip = NULL;
    ld1s = NULL;
    sect = NULL;
    /* build lists of LineDefs that border the Sector */
    for (n = 0; n < NumLineDefs; n++) {
		sd1 = LineDefs[ n].sidedef1;
		sd2 = LineDefs[ n].sidedef2;
		if (sd1 >= 0 && sd2 >= 0) {
			if (SideDefs[ sd2].sector == sector) {
				SelectObject( &ldok, n); /* already ok */
				s = SideDefs[ sd1].sector;
				if (s != sector && !IsSelected( sect, s))
					SelectObject( &sect, s);
			}
			if (SideDefs[ sd1].sector == sector) {
				SelectObject( &ldflip, n); /* will be flipped */
				s = SideDefs[ sd2].sector;
				if (s != sector && !IsSelected( sect, s))
					SelectObject( &sect, s);
			}
		}
		else if (sd1 >= 0 && sd2 < 0) {
			if (SideDefs[ sd1].sector == sector)
				SelectObject( &ld1s, n); /* wall (one-sided) */
		}
    }
    /* there must be a way to go on the lift... */
    if (sect == NULL) {
		Beep();
		Notify( -1, -1, "The Lift Must Be Connected To At Least One Other Sector.", NULL);
		ForgetSelection( &ldok);
		ForgetSelection( &ldflip);
		ForgetSelection( &ld1s);
		return;
    }
    /* flip the LineDefs that have the wrong orientation */
    if (ldflip != NULL)
		FlipLineDefs( ldflip, TRUE);
    /* merge the two selection lists */
    while (ldflip != NULL) {
		if (!IsSelected( ldok, ldflip->objnum))
			SelectObject( &ldok, ldflip->objnum);
		UnSelectObject( &ldflip, ldflip->objnum);
    }
    /* find a free tag number */
    tag = FindFreeTag();
    /* find the minimum altitude */
    minh = 32767;
    maxh = -32767;
    for (curs = sect; curs; curs = curs->next) {
		if (Sectors[ curs->objnum].floorh < minh)
			minh = Sectors[ curs->objnum].floorh;
		if (Sectors[ curs->objnum].floorh > maxh)
			maxh = Sectors[ curs->objnum].floorh;
    }
    ForgetSelection( &sect);
    
    /* change the Sector altitude if necessary */
    if (Sectors[ sector].floorh < maxh)
		Sectors[ sector].floorh = maxh;
    
    /* change the lift's ceiling height if necessary */
    if (Sectors[ sector].ceilh < maxh + 56)
		Sectors[ sector].ceilh = maxh + 56;     
    
    /* assign the new tag number to the lift */
    Sectors[ sector].tag = tag;
    
    /* change the LineDefs and SideDefs */
    while (ldok != NULL) {
		/* give the "lower lift" type and flags to the LineDef */
		n = ldok->objnum;
		LineDefs[ n].special = 62; /* lower lift (switch) */
		LineDefs[ n].flags = 0x04;
#ifdef GAME_HEXEN
		LineDefs[ n].arg1 = tag;
#else
		LineDefs[ n].tag = tag;
#endif
		sd1 = LineDefs[ n].sidedef1;
		sd2 = LineDefs[ n].sidedef2;
		/* adjust the textures for the SideDefs visible from the outside */
		if (strncmp( SideDefs[ sd1].tex3, "-", 8)) {
			if (!strncmp( SideDefs[ sd1].tex2, "-", 8))
				strncpy( SideDefs[ sd1].tex2, SideDefs[ sd1].tex3, 8);
			strncpy( SideDefs[ sd1].tex3, "-", 8);
		}
		if (!strncmp( SideDefs[ sd1].tex2, "-", 8))
			strncpy( SideDefs[ sd1].tex2, "SHAWN2", 8);
		/* adjust the textures for the SideDef visible from the lift */
		strncpy( SideDefs[ sd2].tex3, "-", 8);
		s = SideDefs[ sd1].sector;
		if (Sectors[ s].floorh > minh) {
			if (strncmp( SideDefs[ sd2].tex3, "-", 8)) {
				if (!strncmp( SideDefs[ sd2].tex2, "-", 8))
					strncpy( SideDefs[ sd2].tex2, SideDefs[ sd1].tex3, 8);
				strncpy( SideDefs[ sd2].tex3, "-", 8);
			}
			if (!strncmp( SideDefs[ sd2].tex2, "-", 8))
				strncpy( SideDefs[ sd2].tex2, "SHAWN2", 8);
		}
		else {
			strncpy( SideDefs[ sd2].tex2, "-", 8);
		}
		strncpy( SideDefs[ sd2].tex3, "-", 8);
		
		/* if the ceiling of the Sector is lower than that of the lift */
		if (Sectors[ s].ceilh < Sectors[ sector].ceilh) {
			if (strncmp( SideDefs[ sd2].tex1, "-", 8))
				strncpy( SideDefs[ sd2].tex1, DefaultUpperTexture, 8);
		}
		
		/* if the floor of the Sector is above the lift */
		if (Sectors[ s].floorh >= Sectors[ sector].floorh) {
			LineDefs[ n].special = 88; /* lower lift (walk through) */
			/* flip it, just for fun */
			curs = NULL;
			SelectObject( &curs, n);
			FlipLineDefs( curs, TRUE);
			ForgetSelection( &curs);
		}
		/* done with this LineDef */
		UnSelectObject( &ldok, n);
    }
    while (ld1s != NULL) {
		/* these are the lift walls (one-sided) */
		n = ld1s->objnum;
		LineDefs[ n].flags = 0x01;
		sd1 = LineDefs[ n].sidedef1;
		/* adjust the textures for the SideDef */
		if (!strncmp( SideDefs[ sd1].tex3, "-", 8))
			strncpy( SideDefs[ sd1].tex3, DefaultWallTexture, 8);
		strncpy( SideDefs[ sd1].tex1, "-", 8);
		strncpy( SideDefs[ sd1].tex2, "-", 8);
		UnSelectObject( &ld1s, n);
    }
}



/*
   get the absolute height from which the textures are drawn
   */

BCINT GetTextureRefHeight( BCINT sidedef)
{
  BCINT l, sector;
  BCINT otherside = -1;
  
  /* find the SideDef on the other side of the LineDef, if any */
  for (l = 0; l < NumLineDefs; l++)
	{
		if (LineDefs[l].sidedef1 == sidedef)
		{
			otherside = LineDefs[l].sidedef2;
			break;
		}
		if (LineDefs[ l].sidedef2 == sidedef)
		{
		  	otherside = LineDefs[l].sidedef1;
		  	break;
		}
  }

  /* get the Sector number */
  sector = SideDefs[sidedef].sector;

  /* if the upper texture is displayed, then the reference
      is taken from the other Sector */

  if (otherside >= 0)
	{
		l = SideDefs[ otherside].sector;
		if (l > 0)
		{
		  	if (Sectors[l].ceilh < Sectors[sector].ceilh &&
		   		Sectors[l].ceilh > Sectors[sector].floorh)
				sector = l;
		}
  }
  /* return the altitude of the ceiling */
	/* textures are drawn from the ceiling down */
  if (sector >= 0)
		return Sectors[ sector].ceilh;
	else
		return 0; /* yuck! */
}



/*
   Align all textures for the given SideDefs
*/
void AlignTexturesY( SelPtr *list, Bool NoChecking, Bool UseOffset)
{
	BCINT InitOffset, RefHeight = -1, offset;
	char TextureName[9] = "\0\0\0\0\0\0\0\0\0",
	PreviousTexture[9] = "\0\0\0\0\0\0\0\0\0";
	BCINT sidedef, side, linedef;
	Bool FirstTime = TRUE;
	BCINT v, vert1, vert2, PrevLinedef = -1;
	SelPtr ldlist, listhead;
	Texture *t_info;
	
	listhead = ldlist = rev_list(*list);
	
	while(ldlist)
	{
		linedef = ldlist->objnum;
		
		if(ldlist->next)
		{
			if((v = CommonVertex(linedef, ldlist->next->objnum)) == -1)
				return;
			side = (v == LineDefs[linedef].end) ? 1 : 2;
			PrevLinedef = linedef;
		}
		else
		{
			if((v = CommonVertex(linedef, PrevLinedef)) == -1)
				return;
			side = (v == LineDefs[linedef].start) ? 1 : 2;
		}
		
		
		if(side == 1)
			sidedef = LineDefs[linedef].sidedef1;
		else
			sidedef = LineDefs[linedef].sidedef2;
		
		if(sidedef == -1)
			goto next;
		
		vert1 = LineDefs[linedef].start;
		vert2 = LineDefs[linedef].end;
		
		if(strcmp(SideDefs[sidedef].tex3, "-"))
		{
			strncpy(TextureName, SideDefs[sidedef].tex3, 8);
			goto got_one;
		}
		
		if(strcmp(SideDefs[sidedef].tex1, "-"))
		{
			strncpy(TextureName, SideDefs[sidedef].tex1, 8);
			goto got_one;
		}
		
		if(strcmp(SideDefs[sidedef].tex2, "-")) {
			strncpy(TextureName, SideDefs[sidedef].tex2, 8);
			goto got_one;
		}
		
		goto next;
		
	got_one:
		
		if (FirstTime)
		{
			FirstTime = FALSE;
			InitOffset = SideDefs[sidedef].yoff;
			
			/* ask for initial offset if UseOffset is true */
			
			if (UseOffset == TRUE)
			{
				BCINT  x0, y0, key;   
				char prompt[80]; 
				
				if(UseMouse)
					HideMousePointer();
				
				sprintf(prompt, "Enter Initial Offset Between -16383 And 16383");
				/*JFF* allow full -16383 to 16383 range of offsets */
				
				x0 = (ScrMaxX - 25 - 8 * strlen(prompt)) / 2;
				y0 = (ScrMaxY - 55) / 2;
				
				DrawScreenBox3D( x0, y0, x0 + 25 + 8 * strlen( prompt), y0 + 55);
				SetColor(WHITE);
				DrawScreenText( x0 + 10, y0 + 8, prompt);
				
				/*JFF* allow full -16383 to 16383 range of offsets */
				while (((key=InputInteger(-1, -1, &InitOffset, -16383, 16383)) & 0x00FF)!=0x000D && (key & 0x00FF) != 0x001B)
					Beep();
				if (InitOffset < -16383 || InitOffset > 16383)
					return;
				
				if (UseMouse)
					ShowMousePointer();
			}
			
			RefHeight = GetTextureRefHeight(sidedef);
			SideDefs[sidedef].yoff = InitOffset;
			
			strncpy(PreviousTexture, TextureName, 8);
		 	goto next; 
		}
		
		if((strcmp(PreviousTexture, TextureName)) && !NoChecking)
		{
			RefHeight = GetTextureRefHeight(sidedef);
		}
		else
		{
			t_info = FindTexture(TextureName);
			offset = RefHeight + InitOffset - GetTextureRefHeight(sidedef);
			
			while(offset < 0)
				offset += t_info->height;
			
			while(offset >= t_info->height)
				offset -= t_info->height;
			
			SideDefs[sidedef].yoff = offset;
		}
		
		strncpy(PreviousTexture, TextureName, 8);
		
	next:
		ldlist = (ldlist)->next;
	}
	
	delete_list(listhead);
  MadeChanges = TRUE;
}



/*
   SO: This function has been re-written from scratch.
   It now accepts 3 parameters:
   sdlist is the list of currently highlighted sidedefs.
   NoChecking is whether to keep the accumulated offset even when
   the texture changes from one sidedef to the next.
   UseOffset is whether or not to ask the user for an initial X offset,
   or to simply start at 0.
   
   sdlist is immediately converted to ldlist, a list of LINEDEFS
   currently highlighted. We then follow the chain of linedefs,
   and align the textures along the right-hand side, working out
   each step of the way whether it is the first or second sidedef.
   
   e.g.
   
   --ld1--> <--ld2-- --ld3-->  
   
   Selecting the linedefs in the order ld1, ld2, ld3 will align the first
   sidedefs of ld1 & ld3, and the second sidedef of ld2.
   To align the second sidedefs of ld3 & ld1, and the first sidedef of
   ld2, select them in the order ld3, ld2, ld1.
*/

void AlignTexturesX(SelPtr *list, Bool NoChecking, Bool UseOffset)
{
	BCINT InitOffset, CurrentOffset, PreviousOffset = 0;
	char TextureName[9] = "\0\0\0\0\0\0\0\0\0",
	PreviousTexture[9] = "\0\0\0\0\0\0\0\0\0";
	BCINT sidedef, side, linedef;
	BCINT TextureWidth, PreviousWidth = 0;
	Bool FirstTime = TRUE;
	BCINT v, vert1, vert2, PrevLinedef = -1;
	SelPtr ldlist, listhead;
	Texture *t_info;
	
	listhead = ldlist = rev_list(*list);
	
	while(ldlist)
	{
		linedef = ldlist->objnum;
		
		if (ldlist->next)
		{
			if ((v = CommonVertex(linedef, ldlist->next->objnum)) == -1)
				return;
			side = (v == LineDefs[linedef].end) ? 1 : 2;
			PrevLinedef = linedef;
		}
		else
		{
			if (PrevLinedef == -1)
				return;
			if ((v = CommonVertex(linedef, PrevLinedef)) == -1)
				return;
			side = (v == LineDefs[linedef].start) ? 1 : 2;
		}
		
		
		if (side == 1)
			sidedef = LineDefs[linedef].sidedef1;
		else
			sidedef = LineDefs[linedef].sidedef2;
		
		if (sidedef == -1)
			goto next;
		
		vert1 = LineDefs[linedef].start;
		vert2 = LineDefs[linedef].end;
		
		if (strcmp(SideDefs[sidedef].tex3, "-"))
		{
			strncpy(TextureName, SideDefs[sidedef].tex3, 8);
			goto got_one;
		}
		
		if (strcmp(SideDefs[sidedef].tex1, "-"))
		{
			strncpy(TextureName, SideDefs[sidedef].tex1, 8);
			goto got_one;
		}
		
		if (strcmp(SideDefs[sidedef].tex2, "-")) {
			strncpy(TextureName, SideDefs[sidedef].tex2, 8);
			goto got_one;
		}
		
		CurrentOffset = 0;
		PreviousWidth = 0;
		PreviousOffset = 0;
		goto next;
		
	got_one:
		
		if (FirstTime)
		{
			FirstTime = FALSE;
			InitOffset = SideDefs[sidedef].xoff;
			
			/* ask for initial offset if UseOffset is true */
			
			if(!(t_info = FindTexture(TextureName)))
				goto next;
			TextureWidth = t_info->width;
			
			if(UseOffset == TRUE)
			{
				BCINT  x0, y0, key;   
				char prompt[80]; 
				
				if(UseMouse)
					HideMousePointer();
				
				/*JFF* allow full -16383 to 16383 range of offsets */
				sprintf(prompt, "Enter Initial Offset Between -16383 And 16383");
				
				x0 = (ScrMaxX - 25 - 8 * strlen(prompt)) / 2;
				y0 = (ScrMaxY - 55) / 2;
				
				DrawScreenBox3D( x0, y0, x0 + 25 + 8 * strlen( prompt), y0 + 55);
				SetColor(WHITE);
				DrawScreenText( x0 + 10, y0 + 8, prompt);
				
				/*JFF* allow full -16383 to 16383 range of offsets */
				while (((key=InputInteger(-1, -1, &InitOffset, -16383, 16383))&0x00FF)!=0x000D && (key & 0x00FF) != 0x001B)
					Beep();
				if (InitOffset < -16383 || InitOffset > 16383)
					return;
				
				if (UseMouse)
					ShowMousePointer();
			}
			
	 	  CurrentOffset = InitOffset; 
			PreviousOffset = CurrentOffset;
			PreviousWidth = ComputeDist(Vertexes[vert2].x - Vertexes[vert1].x, Vertexes[vert2].y - Vertexes[vert1].y);
			strncpy(PreviousTexture, TextureName, 8);
			if (side == 1)
			{
				SideDefs[sidedef].xoff = CurrentOffset;
			}
			else
			{
				SideDefs[sidedef].xoff = TextureWidth - CurrentOffset;
			}
		 	goto next; 
		}
		
		if(!(t_info = FindTexture(TextureName)))
			goto next;
		TextureWidth = t_info->width;
		
		if((strcmp(PreviousTexture, TextureName)) && !NoChecking)
		{
			CurrentOffset = 0;
			PreviousOffset = 0;
		}
		else
		{
			CurrentOffset = (PreviousOffset + PreviousWidth) % TextureWidth;
			PreviousOffset = CurrentOffset;
		}
		
		SideDefs[sidedef].xoff = CurrentOffset;
		
		strncpy(PreviousTexture, TextureName, 8);
		PreviousWidth = ComputeDist(Vertexes[vert2].x - Vertexes[vert1].x, Vertexes[vert2].y - Vertexes[vert1].y);
		
	next:
		ldlist = ldlist->next;
	}
	delete_list(listhead);
  MadeChanges = TRUE;
}

SelPtr rev_list(SelPtr list)
{
	SelPtr newlist = NULL, t;
	
	while(list) {
		t = GetMemory(sizeof(struct SelectionList));
		t->objnum = list->objnum;
		t->next = newlist;
		newlist = t;
		list = list->next;
	}
	return newlist;
}

void delete_list(SelPtr list)
{
	SelPtr t;
	
	while(list) {
		t = list->next;
		FreeMemory(list);
		list = t;
	}
}

BCINT CommonVertex(BCINT ld1, BCINT ld2)
{
	BCINT ld1s = LineDefs[ld1].start;
	BCINT ld1e = LineDefs[ld1].end;
	BCINT ld2s = LineDefs[ld2].start;
	BCINT ld2e = LineDefs[ld2].end;
	char errormessage[80];
	
	if((ld1s == ld2s) || (ld1s == ld2e))
		return ld1s;
	
	if((ld1e == ld2s) || (ld1e == ld2e))
		return ld1e;
	
	Beep();
	sprintf( errormessage, "LineDefs #%d And #%d Are Not Joined", ld1, ld2);
	Notify( -1, -1, errormessage, NULL);
	return -1;
}


/*
   Distribute sector light levels
   */

void DistributeLightLevels( SelPtr obj) /* SWAP! */
{
    SelPtr cur;
    BCINT  n, num, light1, light2;
    
    
    num = 0;
    for (cur = obj; cur->next; cur = cur->next)
		num++;
    
    light1 = Sectors[ obj->objnum].light;
    light2 = Sectors[ cur->objnum].light;
    
    n = 0;
    for (cur = obj; cur; cur = cur->next) {
		Sectors[ cur->objnum].light = light1 + n * (light2 - light1) / num;
		n++;
    }
    MadeChanges = TRUE;
}



/*
   Change sector ceiling heights by a value 
   */

void ChangeCeilingHeights( SelPtr obj) /* SWAP! */
{
    SelPtr cur;
    BCINT  val = 0;
    
    
	val = InputIntegerValue(-1, -1, -16384, 16383, 0);
    
    for (cur = obj; cur; cur = cur->next) {
		Sectors[ cur->objnum].ceilh = Sectors[ cur->objnum].ceilh + val;
    }
    MadeChanges = TRUE;
}





/*
   Distribute sector ceiling heights
   */

void DistributeSectorCeilings( SelPtr obj) /* SWAP! */
{
    SelPtr cur;
    BCINT n, num, ceil1h, ceil2h;
    
    
    num = 0;
    for (cur = obj; cur->next; cur = cur->next)
		num++;
    
    ceil1h = Sectors[ obj->objnum].ceilh;
    ceil2h = Sectors[ cur->objnum].ceilh;
    
    n = 0;
    for (cur = obj; cur; cur = cur->next) {
		Sectors[ cur->objnum].ceilh = ceil1h + n * (ceil2h - ceil1h) / num;
		n++;
    }
    MadeChanges = TRUE;
}


/*
   Distribute sector floor heights
   */

void DistributeSectorFloors( SelPtr obj) /* SWAP! */
{
    SelPtr cur;
    BCINT n, num, floor1h, floor2h;
    
    
    num = 0;
    for (cur = obj; cur->next; cur = cur->next)
		num++;
    
    floor1h = Sectors[ obj->objnum].floorh;
    floor2h = Sectors[ cur->objnum].floorh;
    
    n = 0;
    for (cur = obj; cur; cur = cur->next) {
		Sectors[ cur->objnum].floorh = floor1h + n * (floor2h - floor1h) / num;
		n++;
    }
    MadeChanges = TRUE;
}

/*
BCINT GetSectorForThing(BCINT thingnum)
{
  int y, n, x, bestx;
  int lx0, ly0, lx1, ly1;
  LDPtr ldptr, bestldptr;

  y = Things[thingnum].ypos;
  bestx = MapMaxX + 1;
  bestldptr = NULL;
  ldptr = LineDefs;
  for (n = 0; n < NumLineDefs; n++) {
      ly0 = Vertexes[ldptr->start].y;
      ly1 = Vertexes[ldptr->end].y;
      if ((ly0 > y) != (ly1 > y)) {
          lx0 = Vertexes[ldptr->start].x;
          lx1 = Vertexes[ldptr->end].x;
          x = lx0 + (int) ((long) (y - ly0) * (long) (lx1 - lx0) / (long) (ly1 - ly0));
          if (x >= Things[thingnum].xpos && x < bestx) {
              if (ly1 == y && ly0 <= y)
                x++;
              else if (ly0 == y && ly1 >= y)
                x++;
              if (x < bestx) {
                  bestx     = x;     
                  bestldptr = ldptr; 
                }
            }
        }
      else if ((ly0 == y) && (ly1 == y)) {
          lx0 = Vertexes[ldptr->start].x;
          lx1 = Vertexes[ldptr->end].x;
          x = Things[thingnum].xpos;
          if (x >= MIN(lx0, lx1) && x <= MAX(lx0, lx1)) {
              n = ldptr->sidedef1;
              if (n >= 0)
                return SideDefs[n].sector;
              else
                return -1;
            }
        }

      ldptr++;
    }
  if (bestldptr == NULL)
    return -1;
  if (Vertexes[bestldptr->start].y > Vertexes[bestldptr->end].y)
    n = bestldptr->sidedef1;
  else
    n = bestldptr->sidedef2;
  if (n >= 0)
    return SideDefs[n].sector;
  else
    return -1;
}
*/
/* end of file */
