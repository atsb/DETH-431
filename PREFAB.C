/*   Doom Editor for Total Headcases, by Si6on Oke and Antony Burden.
	 You are allowed to use any parts of this code in another program, as
	 long as you give credits to the authors in the documentation and in
	 the program itself.  Read the file README.1ST for more information.
	 
	 This program comes with absolutely no warranty.
	 
	 EDITOBJ.C - object editing routines.
	 */

/* the includes */
#include "deu.h"
#include "levels.h"
#include <math.h>


/*
   insert a standard object at given position
   */

BCINT InsertStandardObject( BCINT x0, BCINT y0, BCINT xpos, BCINT ypos) /* SWAP! */
{
    BCINT sector;
    BCINT choice, n;
    BCINT a, b;

#ifndef SLIM
  BCINT  stair;
  BCINT  l, h, xw, yh, m, radius, increment, angle;
  BCINT  f, c;
  BCINT  x1, y1, x2, y2;
  double astep = 0.0, astart = 0.0;
#endif

    
    /* show where the object will be inserted */
    if (UseMouse)
			HideMousePointer();
    /*DrawPointer(PointerX,PointerY); */
    if (UseMouse)
			ShowMousePointer();
    /* are we inside a Sector? */
    sector = GetCurObject( OBJ_SECTORS, xpos, ypos, xpos, ypos);
    if (sector >= 0)
		choice = DisplayMenu( x0, y0, ((x0 == -1) ? "Insert An Object (Inside)" : NULL),
							 "Insert Rectangle",
							 "Insert Polygon",
#ifndef SLIM
							 "-",
							 "Stairs",
							 "Teleport",
							 "Teleport Landing",
							 "Structure",
#endif
							 NULL);
    else
		choice = DisplayMenu( x0, y0, ((x0 == -1) ? "Insert An Object (Outside)" : NULL),
							 "Insert Rectangle",
							 "Insert Polygon",
#ifndef SLIM
							 "-",
							 "Stairs",
#endif
							 NULL);
    /* !!!! Should also check for overlapping objects (Sectors) !!!! */
    if (choice == -1 || choice == -2)
    	return choice;
    switch (choice) {
    case 1:
		a = 64;
		b = 64;
		if (Input2Numbers( -1, -1, "Width", "Height", 16384, 16384, &a, &b)) {
			if (a < 8)
				a = 8;
			if (b < 8)
				b = 8;
			xpos = xpos - a / 2;
			ypos = ypos - b / 2;
			InsertObject( OBJ_VERTEXES, -1, xpos, ypos);
			InsertObject( OBJ_VERTEXES, -1, xpos + a, ypos);
			InsertObject( OBJ_VERTEXES, -1, xpos + a, ypos + b);
			InsertObject( OBJ_VERTEXES, -1, xpos, ypos + b);
			if (sector < 0)
				InsertObject( OBJ_SECTORS, -1, 0, 0);
			for (n = 0; n < 4; n++) {
				InsertObject( OBJ_LINEDEFS, -1, 0, 0);
				LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs;
				InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
				if (sector >= 0)
					SideDefs[ NumSideDefs - 1].sector = sector;
			}
			if (sector >= 0) {
				LineDefs[ NumLineDefs - 4].start = NumVertexes - 4;
				LineDefs[ NumLineDefs - 4].end = NumVertexes - 3;
				LineDefs[ NumLineDefs - 3].start = NumVertexes - 3;
				LineDefs[ NumLineDefs - 3].end = NumVertexes - 2;
				LineDefs[ NumLineDefs - 2].start = NumVertexes - 2;
				LineDefs[ NumLineDefs - 2].end = NumVertexes - 1;
				LineDefs[ NumLineDefs - 1].start = NumVertexes - 1;
				LineDefs[ NumLineDefs - 1].end = NumVertexes - 4;
			}
			else {
				LineDefs[ NumLineDefs - 4].start = NumVertexes - 1;
				LineDefs[ NumLineDefs - 4].end = NumVertexes - 2;
				LineDefs[ NumLineDefs - 3].start = NumVertexes - 2;
				LineDefs[ NumLineDefs - 3].end = NumVertexes - 3;
				LineDefs[ NumLineDefs - 2].start = NumVertexes - 3;
				LineDefs[ NumLineDefs - 2].end = NumVertexes - 4;
				LineDefs[ NumLineDefs - 1].start = NumVertexes - 4;
				LineDefs[ NumLineDefs - 1].end = NumVertexes - 1;
			}
		}
		break;
    case 2:
		a = 8;
		b = 128;
		if (Input2Numbers( -1, -1, "Number Of Sides", "Radius", 64, 2048, &a, &b)) {
			if (a < 3)
				a = 3;
			if (b < 8)
				b = 8;
			InsertPolygonVertices( xpos, ypos, a, b);
			if (sector < 0)
				InsertObject( OBJ_SECTORS, -1, 0, 0);
			for (n = 0; n < a; n++) {
				InsertObject( OBJ_LINEDEFS, -1, 0, 0);
				LineDefs[ NumLineDefs - 1].sidedef1 = NumSideDefs;
				InsertObject( OBJ_SIDEDEFS, -1, 0, 0);
				if (sector >= 0)
					SideDefs[ NumSideDefs - 1].sector = sector;
			}
			if (sector >= 0) {
				LineDefs[ NumLineDefs - 1].start = NumVertexes - 1;
				LineDefs[ NumLineDefs - 1].end = NumVertexes - a;
				for (n = 2; n <= a; n++) {
					LineDefs[ NumLineDefs - n].start = NumVertexes - n;
					LineDefs[ NumLineDefs - n].end = NumVertexes - n + 1;
				}
			}
			else {
				LineDefs[ NumLineDefs - 1].start = NumVertexes - a;
				LineDefs[ NumLineDefs - 1].end = NumVertexes - 1;
				for (n = 2; n <= a; n++) {
					LineDefs[ NumLineDefs - n].start = NumVertexes - n + 1;
					LineDefs[ NumLineDefs - n].end = NumVertexes - n;
				}
			}
		}
		break;
#ifndef SLIM
    case 4:
      a = 6;
      b = 8;
      xw = 64;
      yh = 32;
      stair = DisplayMenu(x0, y0, "Choose Type Of Stairs",
                          "Line",
                          "Curve",
                          NULL);
      
      if (!Input2Numbers(-1, -1, "Number Of Steps", "Step Height", 256, 48, &a, &b))
        return 0;
      
      if (b < 1)
        b = 1;
      if (b > 16)
        b = 16;
      if (b & 0x1)
        b++;
      if (a < 2)
        a = 2;
      
      if (stair == 1)
        {
          if (!Input2Numbers(-1, -1, "Step Width", "Step Depth", 256, 256, &xw, &yh))
            return 0;
          if (xw < 8)
            xw = 8;
          if (yh < 8)
            yh = 8;
          if (xw & 0x1)
            xw++;
          if (yh & 0x1)
            yh++;
          xpos = xpos - (xw >> 1);
          ypos = ypos - yh * a;
        }
      else
        {
          angle = 360;
          radius = 128;
          increment = 8;
          if (!Input2Numbers(-1, -1, "Step Width", "Angle", 256, 32767, &xw, &angle))
            return 0;
          if (!Input2Numbers(-1, -1, "Radius", "Increment", 512, 128, &radius, &increment))
            return 0;
          astart = 0.0;
          astep = (double)angle * 0.0174532925 / (double)a ;
        }

      if (sector >= 0)
        {
          l = Sectors[sector].ceilh;
          h = Sectors[sector].floorh;
        }
      else
        {
          l = 256;
          h = 0;
          if (!Input2Numbers(-1, -1, "Floor Height", "Ceiling Height", 1024, 1024, &h, &l))
            return 0;
        }
      
      if ((a * b > l - h) && (sector >= 0))
        {
          Beep();
          Notify(-1, -1, "The Stairs Are Too High For This Sector", NULL);
          return 0;
        }

      for (n = 0; n < a; n++)
        {
          /* Line stairs */
          if (stair == 1)
            {
              if (n == 0)
                {
                  InsertObject(OBJ_VERTEXES, -1, xpos, ypos);
                  InsertObject(OBJ_VERTEXES, -1, xpos + xw, ypos);
                }
              InsertObject(OBJ_VERTEXES, -1, xpos, ypos + yh);
              InsertObject(OBJ_VERTEXES, -1, xpos + xw, ypos + yh);
              ypos += yh;
            }
          /* Curve stairs */
          else
            {
              x1 = xpos + (BCINT) (cos( astart) * (double)radius);
              y1 = ypos + (BCINT) (sin( astart) * (double)radius);
              x2 = xpos + (BCINT) (cos( astart) * (double)(radius + xw));
              y2 = ypos + (BCINT) (sin( astart) * (double)(radius + xw));
              if (n == 0)
                {
                  InsertObject(OBJ_VERTEXES, -1, x1, y1);
                  InsertObject(OBJ_VERTEXES, -1, x2, y2);
                  astart += astep;
                  radius += increment;
                  x1 = xpos + (BCINT) (cos( astart) * (double)radius);
                  y1 = ypos + (BCINT) (sin( astart) * (double)radius);
                  x2 = xpos + (BCINT) (cos( astart) * (double)(radius + xw));
                  y2 = ypos + (BCINT) (sin( astart) * (double)(radius + xw));
                }
              InsertObject(OBJ_VERTEXES, -1, x1, y1);
              InsertObject(OBJ_VERTEXES, -1, x2, y2);
              astart += astep;
              radius += increment;
            }
          InsertObject(OBJ_SECTORS, -1, 0, 0);
          /* growing stairs */
          if (choice == 3)
            {
              /* new stairs */
              if (sector < 0)
                {
                  if (n != 0)
                    {
                      l += b;
                      h += b;
                    }
                }
              else h += b;
            }
          Sectors[NumSectors - 1].floorh = h;
          /* Stair = New Sector */
          if (sector < 0)
            {
              Sectors[NumSectors - 1].ceilh = l;
              for (m = 0; m < 4; m++)
                {
                  if (!((m == 0) && (n > 0)))
                    {
                      InsertObject(OBJ_LINEDEFS, -1, 0, 0);
                      
                      if ((m == 3) && (n != a-1))
                        {
                          LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
                          LineDefs[NumLineDefs - 1].sidedef2 = NumSideDefs + 1;
                          LineDefs[NumLineDefs - 1].flags = 4;
                          InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
                          strncpy(SideDefs[NumSideDefs - 1].tex2, DefaultUpperStepTexture, 8);
                          strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
                        }
                      else
                        LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
                      InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
                      if ((m == 3) && (n != a-1))
                        {
                          SideDefs[NumSideDefs - 1].sector = NumSectors;
                          strncpy(SideDefs[NumSideDefs - 1].tex1, DefaultLowerStepTexture, 8);
                          strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
                        }
                      else
                        strncpy(SideDefs[NumSideDefs - 1].tex3, DefaultWallTexture, 8);
                    }
                }
            }
          /* Stair Inside sector */
          else
            {
              for (m = 0; m < 4; m++)
                {
                  if (!((m == 0) && (n > 0)))
                    {
                      InsertObject(OBJ_LINEDEFS, -1, 0, 0);
                      LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
                      LineDefs[NumLineDefs - 1].sidedef2 = NumSideDefs + 1;
                      LineDefs[NumSideDefs - 1].flags = 4;
                      InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
                      /* Back-Front face */
                      if (m == 3)
                        {
                          SideDefs[NumSideDefs - 1].sector = NumSectors - 1;
                          strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
                          if (n != a-1)
                            strncpy(SideDefs[NumSideDefs - 1].tex2, DefaultUpperStepTexture, 8);
                          InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
                          if (n == a-1)
                            {
                              SideDefs[NumSideDefs - 1].sector = sector;
                              strncpy(SideDefs[NumSideDefs - 1].tex2, DefaultWallTexture, 8);
                            }
                          else
                            SideDefs[NumSideDefs - 1].sector = NumSectors;
                        }
                      else
                        {
                          SideDefs[NumSideDefs - 1].sector = sector;
                          strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
                          if (m == 0)
                            strncpy(SideDefs[NumSideDefs - 1].tex2, DefaultUpperStepTexture, 8);
                          else
                            strncpy(SideDefs[NumSideDefs - 1].tex2, DefaultWallTexture, 8);
                          InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
                          SideDefs[NumSideDefs - 1].sector = NumSectors - 1;
                        }
                      strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
                    }
                }
            }
          
          if (sector >= 0)
            {
              LineDefs[NumLineDefs - 4].start = NumVertexes - 4;
              LineDefs[NumLineDefs - 4].end = NumVertexes - 3;
              LineDefs[NumLineDefs - 3].start = NumVertexes - 3;
              LineDefs[NumLineDefs - 3].end = NumVertexes - 1;
              LineDefs[NumLineDefs - 2].start = NumVertexes - 2;
              LineDefs[NumLineDefs - 2].end = NumVertexes - 4;
			  LineDefs[NumLineDefs - 4].flags = 4;
			  LineDefs[NumLineDefs - 3].flags = 4;
			  LineDefs[NumLineDefs - 2].flags = 4;
			  LineDefs[NumLineDefs - 1].flags = 4;
            }
          else
            {
              if (n == 0)
                {
                  LineDefs[NumLineDefs - 4].start = NumVertexes - 3;
                  LineDefs[NumLineDefs - 4].end = NumVertexes - 4;
                }
              else
                {
                  LineDefs[NumLineDefs - 4].start = NumVertexes - 4;
                  LineDefs[NumLineDefs - 4].end = NumVertexes - 3;
                }
              LineDefs[NumLineDefs - 3].start = NumVertexes - 1;
              LineDefs[NumLineDefs - 3].end = NumVertexes - 3;
              LineDefs[NumLineDefs - 2].start = NumVertexes - 4;
              LineDefs[NumLineDefs - 2].end = NumVertexes - 2;
            }
          LineDefs[NumLineDefs - 1].start = NumVertexes - 2;
          LineDefs[NumLineDefs - 1].end = NumVertexes - 1;
        }
      break;
    case 5:
    case 6:
    	/* Align on a 64 boundary for texture */
      	xpos = (xpos - 32) & 0xffc0;
      	ypos = (ypos - 32) & 0xffc0;

      	l = Sectors[sector].ceilh - 8;
      	h = Sectors[sector].floorh + 8;
      	if ((l - h) < 64) {
      	    Beep();
      	    Notify(-1, -1, "Sector Too Low!", NULL);
       	   return 0;
      	}
      
      	if (choice == 5) {
      	    InsertObject(OBJ_THINGS, -1, xpos + 32, ypos + 32);
      	    Things[NumThings -1 ].type = 14; /*Teleport Landing*/
     	}
      	InsertObject(OBJ_VERTEXES, -1, xpos, ypos);
      	InsertObject(OBJ_VERTEXES, -1, xpos + 64, ypos);
      	InsertObject(OBJ_VERTEXES, -1, xpos + 64, ypos + 64);
      	InsertObject(OBJ_VERTEXES, -1, xpos, ypos + 64);
      	InsertObject(OBJ_SECTORS, -1, 0, 0);
      	Sectors[NumSectors - 1].ceilh  = l;
      	Sectors[NumSectors - 1].floorh = h;
      
  		strncpy(Sectors[NumSectors -1].floort, DefaultTeleportTexture, 8);
  		strncpy(Sectors[NumSectors -1].ceilt, DefaultTeleportTexture, 8);
      
    	for (n = 0; n < 4; n++) {
   	    	InsertObject(OBJ_LINEDEFS, -1, 0, 0);
   	    	LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
   	    	LineDefs[NumLineDefs - 1].sidedef2 = NumSideDefs + 1;
   	    	LineDefs[NumLineDefs - 1].flags = 4;
   	    	if (choice == 5)
				LineDefs[NumLineDefs -1].special = 97;
   	     	InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
	   	    SideDefs[NumSideDefs - 1].sector = sector;
   		    strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
   		    strncpy(SideDefs[NumSideDefs - 1].tex2, "WINN01", 8);
   		    strncpy(SideDefs[NumSideDefs - 1].tex1, "WINN01", 8); /*! texture */
   		    InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
	        SideDefs[NumSideDefs - 1].sector = NumSectors - 1;
        	strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
      	}
      	LineDefs[NumLineDefs - 4].start = NumVertexes - 4;
      	LineDefs[NumLineDefs - 4].end = NumVertexes - 3;
      	LineDefs[NumLineDefs - 3].start = NumVertexes - 3;
      	LineDefs[NumLineDefs - 3].end = NumVertexes - 2;
      	LineDefs[NumLineDefs - 2].start = NumVertexes - 2;
      	LineDefs[NumLineDefs - 2].end = NumVertexes - 1;
      	LineDefs[NumLineDefs - 1].start = NumVertexes - 1;
      	LineDefs[NumLineDefs - 1].end = NumVertexes - 4;
      	break;
    case 7:
      /* Structures */
      a = 4;
      b = 128;
      f = c = 0;
      
      if (Input2Numbers(-1, -1, "Number Of Sides", "Radius", 64, 2048, &a, &b)
          && Input2Numbers(-1, -1, "Floor", "Ceiling", 256, 256, &f, &c))
        {
          l = Sectors[sector].ceilh;
          h = Sectors[sector].floorh;
          if (((l-c) - (h-f)) < 4)
            {
              Beep();
              Notify(-1, -1, "Room Too Low!", NULL);
              return 0;
            }
          
          if (a < 3)
            a = 3;
          if (b < 8)
            b = 8;

          InsertPolygonVertices(xpos, ypos, a, b);
          InsertObject(OBJ_SECTORS, -1, 0, 0);
          Sectors[NumSectors - 1].floorh = h + f;
          Sectors[NumSectors - 1].ceilh = l - c;
          for (n = 0; n < a; n++)
            {
              InsertObject(OBJ_LINEDEFS, -1, 0, 0);
              LineDefs[NumLineDefs - 1].sidedef1 = NumSideDefs;
              LineDefs[NumLineDefs - 1].sidedef2 = NumSideDefs + 1;
              LineDefs[NumLineDefs - 1].flags = 4;
              InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
              SideDefs[NumSideDefs - 1].sector = sector;
              if ((f != 0) || (c != 0))
                {
                  strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
                  if (f != 0)
                    {
                      strncpy(SideDefs[NumSideDefs - 1].tex2, DefaultWallTexture, 8);
                      LineDefs[NumLineDefs - 1].flags |= 0x10;
                    }
                  if (c != 0)
                    {
                      strncpy(SideDefs[NumSideDefs - 1].tex1, DefaultWallTexture, 8);
                      LineDefs[NumLineDefs - 1].flags |= 0x8;
                    }
                }
              else
                strncpy(SideDefs[NumSideDefs - 1].tex3, DefaultWallTexture, 8);
              InsertObject(OBJ_SIDEDEFS, -1, 0, 0);
              SideDefs[NumSideDefs - 1].sector = NumSectors - 1;
              strcpy(SideDefs[NumSideDefs - 1].tex3, "-");
            }
          LineDefs[NumLineDefs - 1].start = NumVertexes - 1;
          LineDefs[NumLineDefs - 1].end = NumVertexes - a;
          for (n = 2; n <= a; n++)
            {
              LineDefs[NumLineDefs - n].start = NumVertexes - n;
              LineDefs[NumLineDefs - n].end = NumVertexes - n + 1;
            }
        }
      break;
#endif
    }
    return 0;
}



/* end of file */
