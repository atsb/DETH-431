//	 
//  DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
//  HETH - Hexen Editor for Total Headcases, by Antony Burden.
//  
//  You are allowed to use any parts of this code in another program, as
//  long as you give credits to the authors in the documentation and in
//  the program itself.  Read the file README.1ST for more information.
//   
//  This program comes with absolutely no warranty.
//
//  CHECKS.C - Error Check routines.
//

// the includes 
#include "deu.h"
#include "levels.h"
#include "dialog.h"

//
//  display number of objects, etc.
//

void Statistics( BCINT x0, BCINT y0)
{
  if (x0 < 0)
		x0 = (ScrMaxX - 270) / 2;
  if (y0 < 0)
		y0 = (ScrMaxY - 100) / 2;
  if (UseMouse)
		HideMousePointer();

  DrawScreenBox3D(x0, y0, x0 + 270, y0 + 100);
  SetColor(WHITE);
  DrawScreenText(x0 + 10, y0 + 5, "Statistics");

  if (Things == NULL)
		SetColor(DARKGRAY);
  else
		SetColor(BLACK);
  DrawScreenText(-1, y0 + 25, "Number Of Things:   %4d (%lu K)", NumThings, 
			((unsigned long) NumThings * sizeof( struct Thing) + 512L) / 1024L);

  if (Vertexes == NULL)
		SetColor(DARKGRAY);
  else
		SetColor(BLACK);
  DrawScreenText(-1, -1, "Number Of Vertices: %4d (%lu K)", NumVertexes, 
			((unsigned long) NumVertexes * sizeof( struct Vertex) + 512L) / 1024L);

  if (LineDefs == NULL)
		SetColor(DARKGRAY);
  else
		SetColor(BLACK);
  DrawScreenText(-1, -1, "Number Of LineDefs: %4d (%lu K)", NumLineDefs, 
			((unsigned long) NumLineDefs * sizeof( struct LineDef) + 512L) / 1024L);

  if (SideDefs == NULL)
		SetColor(DARKGRAY);
  else
		SetColor(BLACK);
  DrawScreenText(-1, -1, "Number Of SideDefs: %4d (%lu K)", NumSideDefs,
			((unsigned long) NumSideDefs * sizeof( struct SideDef) + 512L) / 1024L);

  if (Sectors == NULL)
		SetColor(DARKGRAY);
  else
		SetColor(BLACK);
  DrawScreenText
	(
		-1,
		-1,
		"Number Of Sectors:  %4d (%lu K)",
		NumSectors,
		((unsigned long) NumSectors * sizeof( struct Sector) + 512L) / 1024L
	);

  SetColor(YELLOW);
  DrawScreenText(x0 + 10, y0 + 85, "Press Any Key To Continue...");
  bioskey(0);
  if (UseMouse)
		ShowMousePointer();
}

//
//  display a message while the user is waiting...
//

void CheckingObjects(BCINT x0, BCINT y0)
{
  if (UseMouse)
		HideMousePointer();
  if (x0 < 0)
		x0 = (ScrMaxX - 172) / 2;
  if (y0 < 0)
		y0 = (ScrMaxY - 30) / 2;

  DrawScreenBox3D(x0, y0, x0 + 172, y0 + 30);
  DrawScreenText(x0 + 10, y0 + 5, "Checking Objects...");
  DrawScreenText(x0 + 10, y0 + 15, "   Please wait");

  if (UseMouse)
		ShowMousePointer();
}

//
// empty the CHECK.LOG file
//

void CreateCheckLog(char *title)
{
	FILE *st;
	st = fopen("CHECK.LOG","w");
	if (st)
	{
		fprintf(st,"%s:\n\n",title);
		fclose(st);
	}
}

//
// Write a string to the CHECK.LOG file
// opening it for append if not already open,
// and closing it when passed NULL as a string.

void WriteCheckLog(char *string)
{
	static FILE *st=NULL;

	if (!CheckReport)
		return;

	if (!string)
	{
		if (st)
			fclose(st);
		st=NULL;
	  return;
	}

	if (!st)
		st = fopen("CHECK.LOG","a");
  if (st)
		fprintf(st,"%s",string);
}


//
//  display a message,
//  then ask if the check should continue (prompt2 may be NULL)
//

Bool CheckFailed(BCINT x0, BCINT y0, char *prompt1, char *prompt2, char *prompt3, Bool fatal)
{
  BCINT key;
  BCINT maxlen;
	char msg[80];
  
	Failed = TRUE;

	if (CheckReport)
	{
		if (fatal==TRUE)
			WriteCheckLog("Fatal");
		else
			WriteCheckLog("Error");

		if (prompt3==NULL)   // no prompt3, prompt 1, possibly followed by prompt2
		{
		  sprintf(msg,"   %s\n", prompt1);
			WriteCheckLog(msg);
			if (prompt2 != NULL)
			{
				sprintf(msg,"        %s\n", prompt2);
				WriteCheckLog(msg);
			}
		}
		else                 // use only prompt3
		{
		  sprintf(msg,"   %s\n", prompt3);
		  WriteCheckLog(msg);
		}
		WriteCheckLog(NULL); // close log 
		return FALSE;
	}

  if (UseMouse)
		HideMousePointer();
  if (fatal == TRUE)
		maxlen = 44;
  else
		maxlen = 27;

  if (strlen( prompt1) > maxlen)
		maxlen = strlen(prompt1);
  if (prompt2 != NULL && strlen( prompt2) > maxlen)
		maxlen = strlen(prompt2);
  if (x0 < 0)
		x0 = (ScrMaxX - 22 - 8 * maxlen) / 2;
  if (y0 < 0)
		y0 = (ScrMaxY - (prompt2 ? 73 : 63)) / 2;

  DrawScreenBox3D( x0, y0, x0 + 22 + 8 * maxlen, y0 + (prompt2 ? 93 : 83));
  SetColor(RED);
  DrawScreenText( x0 + 10, y0 + 8, "Verification Failed:");
  Beep();
  SetColor(WHITE);
  DrawScreenText( x0 + 10, y0 + 28, prompt1);
  LogMessage("\t%s\n", prompt1);

  if (prompt2 != NULL)
	{
		DrawScreenText( x0 + 10, y0 + 38, prompt2);
		LogMessage("\t%s\n", prompt2);
  }

  if (fatal == TRUE)
	{
		DrawScreenText( x0 + 10, y0 + (prompt2 ? 58 : 58), "The Game May Crash If You Play This Level.");
		SetColor(YELLOW);
		DrawScreenText( x0 + 10, y0 + (prompt2 ? 78 : 68), "Press Any Key To See The Object");
		LogMessage("\n");
  }
  else
	{
		SetColor( YELLOW);
		DrawScreenText( x0 + 10, y0 + (prompt2 ? 68 : 58), "Press Esc To See The Object,");
		DrawScreenText( x0 + 10, y0 + (prompt2 ? 78 : 68), "Or Any Other Key To Continue");
  }

  key = bioskey( 0);
  if ((key & 0x00FF) != 0x001B)
	{
		DrawScreenBox3D( x0, y0, x0 + 22 + 8 * maxlen, y0 + (prompt2 ? 73 : 63));
		DrawScreenText( x0 + 10 + 4 * (maxlen - 26), y0 + 28, "Verifying Other Objects...");
  }

  if (UseMouse)
		ShowMousePointer();
  
  return ((key & 0x00FF) == 0x001B);
}


//
//  check if all sectors are closed
//

void CheckSectors( void)
{
  BCINT s, n, sd;
  char huge *ends;
  char msg1[80], msg2[80], msg3[80];
  
  CheckingObjects( -1, -1);
  LogMessage("\nVerifying Sectors...\n");
  WriteCheckLog("\nVerifying Sectors...\n");
  WriteCheckLog("\n...Cycle Test\n");

  ends = GetFarMemory( NumVertexes * sizeof( char));
  for (s = 0; s < NumSectors; s++)
	{
		/* clear the "ends" array */
		for (n = 0; n < NumVertexes; n++)
			ends[n] = 0;

		/* for each SideDef bound to the Sector, store a "1" in the "ends" */
		/* array for its starting Vertex, and a "2" for its ending Vertex  */
		for (n = 0; n < NumLineDefs; n++)
		{
			sd = LineDefs[n].sidedef1;
			if (sd >= 0 && SideDefs[sd].sector == s)
			{
				ends[ LineDefs[n].start] |= 1;
				ends[ LineDefs[n].end] |= 2;
			}
			sd = LineDefs[n].sidedef2;
			if (sd >= 0 && SideDefs[sd].sector == s)
			{
				ends[ LineDefs[n].end] |= 1;
				ends[ LineDefs[n].start] |= 2;
			}
		}

		/* every entry in the "ends" array should be "0" or "3" */
		for (n = 0; n < NumVertexes; n++)
		{
			if (ends[n] == 1)
			{
				sprintf(msg1, "Sector #%d Is Not Closed!", s);
				sprintf(msg2, "There Is No SideDef Ending At Vertex #%d", n);
				sprintf(msg3, "Sector #%d Unclosed at Vertex #%d", s, n);
				if (CheckFailed( -1, -1, msg1, msg2, msg3, FALSE))
				{
					GoToObject(OBJ_VERTEXES, n);
					return;
				}
			}
			if (ends[n] == 2)
			{
				sprintf(msg1, "Sector #%d is not closed!", s);
				sprintf(msg2, "There Is No SideDef Starting At Vertex #%d", n);
				sprintf(msg3, "Sector #%d Unclosed at Vertex #%d", s, n);
				if (CheckFailed(-1, -1, msg1, msg2, msg3, FALSE))
				{
					GoToObject(OBJ_VERTEXES, n);
					return;
				}
			}
		}
  }
  FreeFarMemory( ends);
  
  /*
      Note from RQ:
      This is a very simple idea, but it works!  The first test (above)
      checks that all Sectors are closed.  But if a closed set of LineDefs
      is moved out of a Sector and has all its "external" SideDefs pointing
      to that Sector instead of the new one, then we need a second test.
      That's why I check if the SideDefs facing each other are bound to
      the same Sector.
      
      Other note from RQ:
      Nowadays, what makes the power of a good editor is its automatic tests.
      So, if you are writing another Doom editor, you will probably want
      to do the same kind of tests in your program.  Fine, but if you use
      these ideas, don't forget to credit DEU...  Just a reminder... :-)
      */
  
	if (SpecialEffects)
	  WriteCheckLog("\n...Facing Sidedef Test Skipped\n");
	else
	{
  	WriteCheckLog("\n...Facing Sidedef Test\n");
  	/* now check if all SideDefs are facing a SideDef with the same Sector number */
  	for (n = 0; n < NumLineDefs; n++)
		{
			sd = LineDefs[n].sidedef1;
			if (sd >= 0)
			{
				s = GetOppositeSector(n, TRUE);
				if (s < 0 || SideDefs[sd].sector != s)
				{
					if (s < 0)
					{
						sprintf( msg1, "Sector #%d Is Not Closed!", SideDefs[sd].sector);
						sprintf( msg2, "Check LineDef #%d (First SideDef: #%d)", n, sd);
						sprintf( msg3, "Sector #%d Unclosed, Facing LineDef #%d (Side1: #%d)",SideDefs[sd].sector, n, sd);
					}
					else
					{
						sprintf( msg1, "Sectors #%d And #%d Are Not Closed!", SideDefs[sd].sector, s);
						sprintf( msg2, "Check LineDef #%d (First SideDef: #%d) And The One Facing It", n, sd);
						sprintf( msg3, "Sector #%d Unclosed, Facing LineDef #%d (Side1: #%d)",SideDefs[sd].sector, n, sd);
					}

					if (CheckFailed( -1, -1, msg1, msg2, msg3, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
			}

			sd = LineDefs[n].sidedef2;
			if (sd >= 0)
			{
				s = GetOppositeSector(n, FALSE);
				if (s < 0 || SideDefs[sd].sector != s)
				{
					if (s < 0)
					{
						sprintf( msg1, "Sector #%d Is Not Closed!", SideDefs[sd].sector);
						sprintf( msg2, "-1 LineDef #%d (Second SideDef: #%d)", n, sd);
						sprintf( msg3, "Sector #%d Unclosed, Facing LineDef #%d (Side2: #%d)",SideDefs[sd].sector, n, sd);
					}
					else
					{
						sprintf( msg1, "Sectors #%d And #%d Are Not Closed!", SideDefs[ sd].sector, s);
						sprintf( msg2, "Check LineDef #%d (Second SideDef: #%d) And The One Facing It", n, sd);
						sprintf( msg3, "Sector #%d Unclosed, Facing LineDef #%d (Side2: #%d)",SideDefs[sd].sector, n, sd);
					}

					if (CheckFailed(-1, -1, msg1, msg2, msg3, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
			}
  	}
	}
}



//
//  check cross-references and delete unused objects
//	  

void CheckCrossReferences(void)
{
    char   msg[80];
    BCINT	n, m;
    SelPtr cur;
    
    CheckingObjects( -1, -1);
    LogMessage("\nVerifying cross-references...\n");
    WriteCheckLog("\nVerifying cross-references...\n");
    WriteCheckLog("\n...Fatal linedef flaw checks\n");
    for (n = 0; n < NumLineDefs; n++)
		{
			/* check for missing first SideDefs */
			if (LineDefs[n].sidedef1 < 0)
			{
				sprintf( msg, "LineDef #%d Has No First SideDef!", n);
				CheckFailed(-1, -1, msg, NULL, msg, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, n);
					return;
				}
			}
			/* check for SideDefs used twice in the same LineDef */
			if (LineDefs[n].sidedef1 == LineDefs[n].sidedef2)
			{
				sprintf( msg, "LineDef #%d Uses The Same SideDef Twice (#%d)", n, LineDefs[ n].sidedef1);
				CheckFailed(-1, -1, msg, NULL, msg, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, n);
					return;
				}
			}
			/* check for Vertices used twice in the same LineDef */
			if (LineDefs[n].start == LineDefs[n].end)
			{
				sprintf( msg, "LineDef #%d Uses The Same Vertex Twice (#%d)", n, LineDefs[ n].start);
				CheckFailed(-1, -1, msg, NULL, msg, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, n);
					return;
				}
			}
    }
    
    /* check if there aren't two LineDefs between the same Vertices */
    cur = NULL;
    for (n = NumLineDefs - 1; n >= 1; n--)
		{
			for (m = n - 1; m >= 0; m--)
				if
				(
					(LineDefs[n].start == LineDefs[m].start &&
					  LineDefs[n].end == LineDefs[m].end)
					||
					(LineDefs[n].start == LineDefs[m].end &&
					  LineDefs[n].end == LineDefs[m].start)
				)
				{
					SelectObject(&cur, n);
					break;
				}
    }
    if (cur && (Expert || Confirm(-1, -1, "There Are Multiple LineDefs Between The Same Vertices", "Do You Want To Delete The Redundant LineDefs?")))
		{
			DeleteObjects(OBJ_LINEDEFS, &cur);
			WriteCheckLog("Multiple linedefs between vertices deleted\n");
		}
    else
		{
			if (cur)
				WriteCheckLog("Multiple linedefs between vertices ignored\n");
			ForgetSelection(&cur);
		}
    
    CheckingObjects( -1, -1);
    /* check for invalid flags in the LineDefs */
    for (n = 0; n < NumLineDefs; n++)
			if ((LineDefs[n].flags & 0x01) == 0 && LineDefs[ n].sidedef2 < 0)
				SelectObject(&cur, n);
    if (cur && (Expert || Confirm(-1, -1, "Some LineDefs Have Only One Side But Their Im Bit Is Not Set", "Do You Want To Set The 'Impassible' Flag?")))
		{
			while (cur)
			{
				LineDefs[cur->objnum].flags |= 0x01;
				UnSelectObject( &cur, cur->objnum);
			}
			WriteCheckLog("Impassible Flag set on all one sided linedefs\n");
    }
    else
		{
			if (cur)
				WriteCheckLog("One-sided lines without Impassible Flag still exist\n");
			ForgetSelection(&cur);
		}
    
    CheckingObjects( -1, -1);
    for (n = 0; n < NumLineDefs; n++)
			if ((LineDefs[n].flags & 0x04) != 0 && LineDefs[ n].sidedef2 < 0)
				SelectObject(&cur, n);
    if (cur && (Expert || Confirm(-1, -1, "Some LineDefs Have Only One Side But Their 2S Bit Is Set", "Do You Want To Clear The 'Two-Sided' Flag?")))
		{
			while (cur)
			{
				LineDefs[cur->objnum].flags &= ~0x04;
				UnSelectObject( &cur, cur->objnum);
			}
			WriteCheckLog("2S Flag cleared on all one sided linedefs\n");
    }
    else
		{
			if (cur)
				WriteCheckLog("One-sided lines with 2S Flag still exist\n");
			ForgetSelection(&cur);
		}
    
    CheckingObjects( -1, -1);
    for (n = 0; n < NumLineDefs; n++)
		if ((LineDefs[n].flags & 0x04) == 0 && LineDefs[ n].sidedef2 >= 0)
			SelectObject(&cur, n);
    if (cur && (Expert || Confirm(-1, -1, "Some LineDefs Have Two Sides But Their 2S Bit Is Not Set", "Do You Want To Set The 'Two-Sided' Flag?")))
		{
			while (cur)
			{
				LineDefs[cur->objnum].flags |= 0x04;
				UnSelectObject(&cur, cur->objnum);
			}
			WriteCheckLog("2S Flag set on all two sided linedefs\n");
    }
    else
		{
			if (cur)
				WriteCheckLog("Two-sided lines without 2S Flag still exist\n");
			ForgetSelection(&cur);
		}
    
    CheckingObjects( -1, -1);
    /* select all Vertices */
    for (n = 0; n < NumVertexes; n++)
			SelectObject(&cur, n);
    /* unselect Vertices used in a LineDef */
    for (n = 0; n < NumLineDefs; n++)
		{
			m = LineDefs[n].start;
			if (cur && m >= 0)
				UnSelectObject(&cur, m);
			m = LineDefs[n].end;
			if (cur && m >= 0)
				UnSelectObject(&cur, m);
			continue;
    }
		/* check if there are any Vertices left */
    if
		(
		  cur && (Expert ||
			Confirm(-1, -1, "Some Vertices Are Not Bound To Any LineDef",
			                "Do You Want To Delete These Unused Vertices?"))
		)
		{
			DeleteObjects(OBJ_VERTEXES, &cur);
			WriteCheckLog("All vertices not bound to a linedef deleted\n");
		}
    else
		{
			if (cur)
				WriteCheckLog("Vertices not bound to a linedef still exist\n");
			ForgetSelection(&cur);
		}
    
    CheckingObjects( -1, -1);
    /* select all SideDefs */
    for (n = 0; n < NumSideDefs; n++)
		SelectObject(&cur, n);
    /* unselect SideDefs bound to a LineDef */
    for (n = 0; n < NumLineDefs; n++)
		{
			m = LineDefs[n].sidedef1;
			if (cur && m >= 0)
				UnSelectObject(&cur, m);
			m = LineDefs[n].sidedef2;
			if (cur && m >= 0)
				UnSelectObject(&cur, m);
			continue;
    }
    /* check if there are any SideDefs left */
    if
		(
		  cur && (Expert ||
		  Confirm(-1, -1, "Some SideDefs Are Not Bound To Any LineDef",
			                "Do You Want To Delete These Unused SideDefs?"))
		)
		{
			DeleteObjects(OBJ_SIDEDEFS, &cur);
			WriteCheckLog("All sidedefs not bound to a linedef deleted\n");
		}
    else
		{
			if (cur)
				WriteCheckLog("Sidedefs not bound to a linedef still exist\n");
			ForgetSelection(&cur);
		}
    
    CheckingObjects( -1, -1);
		/* select all Sectors */
    for (n = 0; n < NumSectors; n++)
			SelectObject( &cur, n);
    /* unselect Sectors bound to a SideDef */
    for (n = 0; n < NumLineDefs; n++)
		{
			m = LineDefs[n].sidedef1;
			if (cur && m >= 0 && SideDefs[m].sector >= 0)
				UnSelectObject(&cur, SideDefs[m].sector);
			m = LineDefs[n].sidedef2;
			if (cur && m >= 0 && SideDefs[m].sector >= 0)
				UnSelectObject(&cur, SideDefs[m].sector);
			continue;
    }

    /* if there are any Sectors left */
    if (cur && (Expert || Confirm(-1, -1, "Some Sectors Are Not Bound To Any SideDef", "Do You Want To Delete These Unused Sectors?")))
		{
			DeleteObjects(OBJ_SECTORS, &cur);
			WriteCheckLog("All sectors not bound to a sidedef deleted\n");
		}
    else
		{
			if (cur)
				WriteCheckLog("Sectors not bound to a sidedef still exist\n");
			ForgetSelection( &cur);
		}
}

//
// Return TRUE if the linedef type is known
//

Bool IsValidLineType(int special)
{
	if (BOOMEnable)
	{
		if (0x2f80<=special && special<0x8000)
			return TRUE;
		else if (0<=special && special<=MaxExtendedType)
			return TRUE;
	}
	else if (0<=special && special<=141 && special!=78 && special!=85)
		return TRUE;
	return FALSE;
}

//
// Return TRUE if the linedef type acts on tagged lines
//

Bool IsLineLineType(unsigned special)
{
	if (BOOMEnable)
		switch(special)
		{
				case 260:
				case 243:
				case 244:
				case 262:
				case 263:
				case 264:
				case 265:
				case 266:
				case 267:
				case 218:
				case 249:
				case 254:
					return TRUE;
				default:
					break;
		}
	return FALSE;
}

//
//  check to see if a line tagged to the function is required
//

Bool RequiresTaggedLine(unsigned special)
{
	if (!BOOMEnable)
		return FALSE;     // no line-line tags if not BOOM
	switch(special)
	{
		case 243:					// all line-line tags but 260
		case 244:
		case 262:
		case 263:
		case 264:
		case 265:
		case 266:
		case 267:
		case 218:
		case 249:
		case 254:
			return TRUE;		// only any of the above
			break;

		default:
			break;
	}
	return FALSE;
}

//
//  check to see if a tagged sector is required by function
//

Bool RequiresTaggedSector(unsigned special)
{
	if (BOOMEnable && 0x2f80<=special && special<0x8000)
	{
		if (((special>>TriggerTypeShift)&PushOnce)==PushOnce)
			return FALSE; 		// not if a generalized manual(push) type
	}
	else
		switch (special)
		{
			case 0:
			case 1:		//manual triggers
			case 31:
			case 26:
			case 27:
			case 28:
			case 32:
			case 33:
			case 34:
			case 117:
			case 118:

			case 11:	//exits
			case 52:
			case 51:
			case 124:
			case 197:
			case 198:

			case 48:  //scrollers
			case 85:
			case 255:

			case 260:	//optional line-line tag

			case 243:	//line-line tags
			case 244:
			case 262:
			case 263:
			case 264:
			case 265:
			case 266:
			case 267:
			case 218:
			case 249:
			case 254:
				return FALSE;		// not if any of the above
				break;

			default:
				break;
		}
	return TRUE;					// a tagged sector is required
}

//
// check to see if a second side is required by linedef function
//

Bool RequiresSecondSide(unsigned special)
{
	if (BOOMEnable && 0x2f80<=special && special<0x8000)
	{
		if (((special>>TriggerTypeShift)&PushOnce)==PushOnce)
			return TRUE; 			// does if a generalized manual(push) type
	}
	else
		switch (special)
		{
			case 1:						//manual triggers
			case 31:
			case 26:
			case 27:
			case 28:
			case 32:
			case 33:
			case 34:
			case 117:
			case 118:
				return TRUE;		// manual triggers do
				break;

			default:
				break;
		}
	return FALSE;					// no second side required
}

// if lineno requires and has a tag
// return whether a line or sector (as appropriate) is tagged to it

Bool TaggedObjectExists(int lineno)
{
	int j;
	unsigned short s,t;

	s = LineDefs[lineno].special;
	t = LineDefs[lineno].tag;
	if (BOOMEnable)
	{
		if (0<s && s<=MaxExtendedType)
			switch(s)
			{
				case 260:
				case 243:
				case 244:
				case 262:
				case 263:
				case 264:
				case 265:
				case 266:
				case 267:
				case 218:
				case 249:
				case 254:
					for (j=0; j<NumLineDefs; j++)
						if (j!=lineno && LineDefs[j].tag==t)
							return TRUE;
					return FALSE;
				default:
					break;
			}
	}
	for (j=0; j<NumSectors; j++)
		if (Sectors[j].tag==t)
			return TRUE;
	return FALSE;
}

// return TRUE if a linedef that acts on lines with
// the same tag as this one exists

Bool BOOMLineLineFunctionExists(int lineno)
{
	int j;
	unsigned short s;

	if (BOOMEnable)
		for (j = 0; j<NumLineDefs; j++)
		{
			if (j!=lineno && LineDefs[lineno].tag==LineDefs[j].tag)
			{
				s = LineDefs[j].special;
				if (0<s && s<=MaxExtendedType)
					switch(s)
					{
						case 260:
						case 243:
						case 244:
						case 262:
						case 263:
						case 264:
						case 265:
						case 266:
						case 267:
						case 218:
						case 249:
						case 254:
							return TRUE;
							break;
						default:
							break;
					}
			}
		}
	return FALSE;
}

//
//  check for triggers and tags
// jff
//

void CheckTriggersAndTags(void)
{
	int i,j;
	unsigned short s;
	char msg[80];
	Bool reqline,reqsect,req2S,isvalid,onesided;
	SelPtr cur=NULL,csec=NULL;

  LogMessage("\nVerifying Triggers and Tags...\n");
  WriteCheckLog("\nVerifying Triggers and Tags...\n");
  CheckingObjects( -1, -1);

	// Report any lines using specials requring tags that have tag 0

	for (i=0; i<NumLineDefs; i++)
	{
		s = LineDefs[i].special;
		isvalid = IsValidLineType(s);
		reqline = RequiresTaggedLine(s);
		reqsect = RequiresTaggedSector(s);
		req2S   = RequiresSecondSide(s);
		onesided = (LineDefs[i].sidedef2<0);


		// Report any lines using unknown specials 
		// but skip remaining processing

		if (!isvalid)
		{
			sprintf(msg, "LineDef #%d Unknown Type: %u", i,LineDefs[i].special);
			if (CheckFailed(-1, -1, msg, NULL, msg, FALSE))
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, i);
					return;
				}
			continue;
		}

		if (onesided && req2S)
		{
			sprintf(msg, "1S LineDef #%d Manual Type: %u", i,LineDefs[i].special);
			if (CheckFailed(-1, -1, msg, NULL, msg, !BOOMEnable))
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, i);
					return;
				}
		}

		if ((reqline || reqsect) && LineDefs[i].tag==0)
		{
			sprintf(msg, "LineDef #%d Acts on Tag 0", i);
			if (CheckFailed(-1, -1, msg, NULL, msg, !BOOMEnable))
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, i);
					return;
				}
		}

		if ((reqline || reqsect) && LineDefs[i].tag>0 && !TaggedObjectExists(i))
		{
			sprintf(msg, "LineDef #%d, Tag %d, No Tagged Object", i,LineDefs[i].tag);
			if (CheckFailed(-1, -1, msg, NULL, msg, !BOOMEnable))
				if (!CheckReport)
				{
					GoToObject(OBJ_LINEDEFS, i);
					return;
				}
		}
		
		if (!reqline && !reqsect && LineDefs[i].tag!=0)
		{
			if (!BOOMLineLineFunctionExists(i))
			{
				if (!BOOMEnable)
					sprintf(msg, "LineDef #%d has Tag %d, but no Special", i,LineDefs[i].tag);
				else
					sprintf(msg, "LineDef #%d has Tag %d, no Special, Tag not Used", i, LineDefs[i].tag);
				SelectObject(&cur, i);
				if (CheckReport)
					CheckFailed(-1, -1, msg, NULL, msg, FALSE);
			}
		}
	}

  CheckingObjects( -1, -1);
  if (cur && Confirm(-1, -1, "Some LineDefs Have Tags That Aren't Used", "Do You Want To Zero Those Tags?"))
	{
		while (cur)
		{
			LineDefs[cur->objnum].tag = 0;
			UnSelectObject( &cur, cur->objnum);
		}
		WriteCheckLog("Unused tags cleared from all linedefs\n");
  }

	for (i=0; i<NumSectors; i++)
	{
		if (Sectors[i].tag!=0 && Sectors[i].tag!=666 && Sectors[i].tag!=667)
		{
			for (j=0; j<NumLineDefs; j++)
				if (IsValidLineType(LineDefs[j].special) &&
					  RequiresTaggedSector(LineDefs[j].special) &&
						LineDefs[j].tag==Sectors[i].tag)
					break;
			if (j==NumLineDefs)
			{
				SelectObject(&csec,i);
				sprintf(msg, "Sector #%d has Tag %d, not used", i,Sectors[i].tag);
				if (CheckReport)
					CheckFailed(-1, -1, msg, NULL, msg, FALSE);
			}
		}
	}

  CheckingObjects( -1, -1);
  if (csec && Confirm(-1, -1, "Some Sectors Have Tags That Aren't Used", "Do You Want To Zero Those Tags?"))
	{
		while (csec)
		{
			Sectors[csec->objnum].tag = 0;
			UnSelectObject(&csec, csec->objnum);
		}
		WriteCheckLog("Unused tags cleared from all Sectors\n");
  }
}

//
//   check for missing textures
//

void CheckTextures(Bool AskConf)
{
  BCINT  n;
  BCINT  sd1, sd2;
  BCINT  s1, s2;
  char msg1[80], msg2[80];
  int count = 0;
	int x0 = (ScrMaxX - 280) / 2;
	int y0 = (ScrMaxY - 68) / 2;
    
  CheckingObjects(-1, -1);
  LogMessage("\nVerifying Textures...\n");
  WriteCheckLog("\nVerifying Textures...\n");
  if (!AskConf)
	{
		DrawScreenBox3D(x0, y0, x0 + 280, y0 + 68);
		DrawScreenButtonIn( x0 + 3, y0 + 21, 280 - 6, 50 - 6, "", FALSE);
		SetColor(ORANGE);
		DrawScreenBox(x0 + 3, y0 + 3, x0 + 280 - 3, y0 + 17);
		SetColor(BLACK);
		DrawScreenTextFonted(2, 0, x0 + 10, y0 + 4, "Results:");
		DrawScreenText(x0 + 10, y0 + 30, "Number of Textures Changed: %d", count);
	}
	else
	{
		for (n = 0; n < NumSectors; n++)
		{
			if (Sectors[n].ceilt[0] == '-' && Sectors[n].ceilt[1] == '\0') //jff[1]
			{
				sprintf(msg1, "Sector #%d Has No Ceiling Texture", n);
				sprintf(msg2, "You Probably Used A Brain-Damaged Editor To Do That...");
				CheckFailed(-1, -1, msg1, msg2, msg1, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_SECTORS, n);
					return;
				}
			}
			if (Sectors[n].floort[0] == '-' && Sectors[n].floort[1] == '\0') //jff[1]
			{
				sprintf(msg1, "Sector #%d Has No Floor Texture", n);
				sprintf(msg2, "You Probably Used A Brain-Damaged Editor To Do That...");
				CheckFailed(-1, -1, msg1, msg2, msg1, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_SECTORS, n);
					return;
				}
			}
			if (!SpecialEffects && Sectors[n].ceilh < Sectors[n].floorh)
			{
				sprintf(msg1, "Sector #%d Has Its Ceiling Lower Than Its Floor", n);
				sprintf(msg2, "The Textures Will Never Be Displayed If You Cannot Go There");
				CheckFailed(-1, -1, msg1, msg2, msg1, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_SECTORS, n);
					return;
				}
			}
			if (!SpecialEffects && Sectors[n].ceilh - Sectors[n].floorh > 16384)
			{
				sprintf(msg1, "Sector #%d Has Its Ceiling Too High", n);
				sprintf(msg2, "The Maximum Difference Allowed Is 16384 (Ceiling - Floor)");
				CheckFailed(-1, -1, msg1, msg2, msg1, TRUE);
				if (!CheckReport)
				{
					GoToObject(OBJ_SECTORS, n);
					return;
				}
			}
		}
  }
  count = 0;
  for (n = 0; n < NumLineDefs; n++)
	{
		sd1 = LineDefs[n].sidedef1;
		sd2 = LineDefs[n].sidedef2;
		if (sd1 >= 0)
			s1 = SideDefs[sd1].sector;
		else
			s1 = -1;
		if (sd2 >= 0)
			s2 = SideDefs[sd2].sector;
		else
			s2 = -1;
		if (s1 >= 0 && s2 < 0)
		{
			if (SideDefs[sd1].tex3[0] == '-' && SideDefs[sd1].tex3[1] == '\0')
			{
				if (AskConf)
				{
					sprintf(msg1, "In One-Sided Linedef #%d: SideDef #%d Has No Normal Texture", n, sd1);
					sprintf(msg2, "Do You Want To Set The Texture To \"%s\" And Continue?", DefaultWallTexture);
					if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
				Failed = FALSE;
				strncpy( SideDefs[sd1].tex3, DefaultWallTexture, 8);
				if (!AskConf)
				{
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 30, "%d", count);
				}
				else 
					CheckingObjects(-1, -1);
			}
		}
		if (s1 >= 0 && s2 >= 0 && Sectors[ s1].ceilh > Sectors[ s2].ceilh)
		{
			if
			(
				!SpecialEffects &&
				SideDefs[sd1].tex1[0] == '-' && SideDefs[sd1].tex1[1] == '\0'	&&
				(
					strncmp(Sectors[s1].ceilt, "F_SKY1", 8) ||
					strncmp(Sectors[s2].ceilt, "F_SKY1", 8)
				)
			)
			{
				if (AskConf)
				{
					sprintf(msg1, "In First SideDef Of Linedef #%d: SideDef #%d Has No Upper Texture", n, sd1);
					sprintf(msg2, "Do You Want To Set The Texture To \"%s\" And Continue?", DefaultUpperTexture);
					if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
				Failed = FALSE;
				strncpy( SideDefs[ sd1].tex1, DefaultUpperTexture, 8);
				if (! AskConf)
				{
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 30, "%d", count);
				}
				else 
					CheckingObjects(-1, -1);
			}
		}
		if (s1 >= 0 && s2 >= 0 && Sectors[s1].floorh < Sectors[s2].floorh)
		{
			if
			(
				!SpecialEffects &&
				SideDefs[sd1].tex2[0] == '-' &&
				SideDefs[sd1].tex2[1] == '\0'
			)
			{
				if (AskConf)
				{
					sprintf(msg1, "In First SideDef Of Linedef #%d: SideDef #%d Has No Lower Texture", n, sd1);
					sprintf(msg2, "Do You Want To Set The Texture To \"%s\" And Continue?", DefaultLowerTexture);
					if (CheckFailed( -1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
				Failed = FALSE;
				strncpy(SideDefs[sd1].tex2, DefaultLowerTexture, 8);
				if (! AskConf)
				{
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 30, "%d", count);
				}
				else 
					CheckingObjects(-1, -1);
			}
		}
		if (s1 >= 0 && s2 >= 0 && Sectors[s2].ceilh > Sectors[s1].ceilh)
		{
			if
			(
				!SpecialEffects &&
				SideDefs[sd2].tex1[0] == '-' &&
				SideDefs[sd2].tex1[1] == '\0' &&
				(
					strncmp(Sectors[s1].ceilt, "F_SKY1", 8) ||
					strncmp(Sectors[s2].ceilt, "F_SKY1", 8)
				)
			)
			{
				if (AskConf)
				{
					sprintf(msg1, "In Second SideDef Of Linedef #%d: SideDef #%d Has No Upper Texture", n, sd2);
					sprintf(msg2, "Do You Want To Set The Texture To \"%s\" And Continue?", DefaultUpperTexture);
					if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
				Failed = FALSE;
				strncpy(SideDefs[sd2].tex1, DefaultUpperTexture, 8);
				if (!AskConf)
				{
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 30, "%d", count);
				}
				else 
					CheckingObjects(-1, -1);
			}
		}
		if (s1 >= 0 && s2 >= 0 && Sectors[s2].floorh < Sectors[s1].floorh)
		{
			if
			(
				!SpecialEffects &&
				SideDefs[sd2].tex2[0] == '-' &&
				SideDefs[sd2].tex2[1] == '\0'
			)
			{
				if (AskConf)
				{
					sprintf(msg1, "In Second SideDef Of Linedef #%d: SideDef #%d Has No Lower Texture", n, sd2);
					sprintf(msg2, "Do You Want To Set The Texture To \"%s\" And Continue?", DefaultLowerTexture);
					if(CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_LINEDEFS, n);
						return;
					}
				}
				Failed = FALSE;
				strncpy(SideDefs[sd2].tex2, DefaultUpperTexture, 8);
				if (!AskConf)
				{
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 30, "%d", count);
				}
				else 
					CheckingObjects(-1, -1);
			}
		}
  }
  if (!AskConf)
	{
    SetColor(YELLOW);
		DrawScreenText(x0 + 10, y0 + 50, "Press any key to continue.");
    bioskey(0);
  }
}



//
//   check if a texture name matches one of the elements of a list
//

Bool IsTextureNameInList( char *name, char **list, BCINT numelems)
{
  BCINT n;
  
  for (n = 0; n < numelems; n++)
		if (!strnicmp( name, list[ n], 8))
			return TRUE;
  return FALSE;
}



//
//  check for invalid texture names
//

void CheckTextureNames(Bool AskConf)
{
  BCINT  n,m;
  char msg1[80], msg2[80];
  int count = 0;
	char name[9];

	int x0 = (ScrMaxX - 280) / 2;
	int y0 = (ScrMaxY - 78) / 2;
    
  LogMessage("\nVerifying texture names...\n");
  WriteCheckLog("\nVerifying texture names...\n");

	name[8]='\0';

	/* SO 9/4/95:
	   Removed another "break level if not registered" check */
    
  if (!AskConf)
	{
		DrawScreenBox3D( x0, y0, x0 + 280, y0 + 78);
		DrawScreenButtonIn( x0 + 3, y0 + 21, 280 - 6, 60 - 6, "", FALSE);
		SetColor(ORANGE);
		DrawScreenBox( x0 + 3, y0 + 3, x0 + 280 - 3, y0 + 17);
		SetColor(BLACK);
		DrawScreenTextFonted(2, 0, x0 + 10, y0 + 4, "Results:");
		DrawScreenText(x0 + 10, y0 + 30, "Number of Flats Changed:    %d", count);
		DrawScreenText(x0 + 10, y0 + 40, "Number of Textures Changed: %d", count);
	}

  for (n = 0; n < NumSectors; n++)
	{
		if (!IsTextureNameInList(Sectors[n].ceilt, FTexture, NumFTexture))
		{
			if (AskConf)
			{
				strncpy(name, Sectors[n].ceilt, 8);
				sprintf(msg1, "Invalid Ceiling Texture %s In Sector #%d", name, n);
				sprintf(msg2, "The Name \"%s\" Is Not A Floor/Ceiling Texture", name);
				if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
				{
					GoToObject(OBJ_SECTORS, n);
					return;
				}						   
				CheckingObjects(-1, -1);
			}
			else
			{
				Failed = FALSE;
				strncpy(Sectors[n].ceilt, DefaultCeilingTexture, 8);
				count++;
				SetColor(DARKGRAY);
				DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
				SetColor(BLACK);
				DrawScreenText(x0 + 230, y0 + 30, "%d", count);
			}
		}
		if (!IsTextureNameInList( Sectors[ n].floort, FTexture, NumFTexture))
		{
			if (AskConf)
			{
				strncpy(name, Sectors[n].floort, 8);
				sprintf(msg1, "Invalid Floor Texture %s In Sector #%d", name, n);
				sprintf(msg2, "The Name \"%s\" Is Not A Floor/Ceiling Texture", name);
				if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
				{
					GoToObject(OBJ_SECTORS, n);
					return;
				}						
				CheckingObjects(-1, -1);
			}
			else
			{
				Failed = FALSE;
				strncpy(Sectors[n].floort, DefaultFloorTexture, 8);
				count++;
				SetColor(DARKGRAY);
				DrawScreenBox(x0 + 230, y0 + 30, x0 + 260, y0 + 40);
				SetColor(BLACK);
				DrawScreenText(x0 + 230, y0 + 30, "%d", count);
			}
		}
  }
  count = 0;
  for (m = 0; m < NumLineDefs; m++)
	{
		int s,sd[2];

		sd[0] = LineDefs[m].sidedef1;
		if (sd[0]<0) continue;
		sd[1] = LineDefs[m].sidedef2;
		for (s = 0; s<(sd[1]<0? 1 : 2); s++)
		{
			int n = sd[s];
			if
			(
				!IsTextureNameInList(SideDefs[n].tex1, WTexture, NumWTexture) &&
			  (
				  !BOOMEnable ||
					LineDefs[m].special!=242 ||
					!IsTextureNameInList(SideDefs[n].tex1, CMaps, NumCMaps)
				)
			)
			{
				if (AskConf)
				{
					strncpy(name, SideDefs[n].tex1, 8);
					sprintf(msg1, "Invalid Upper Texture %s In SideDef #%d", name, n);
					sprintf(msg2, "The Name \"%s\" Is Not A Wall Texture", name);
					if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_SIDEDEFS, n);
						return;			   
					}
					CheckingObjects(-1, -1);
				}
				else
				{
					Failed = FALSE;
					strncpy(SideDefs[n].tex1, DefaultUpperTexture, 8);
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 40, x0 + 260, y0 + 50);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 40, "%d", count);
				}
			}
			if
			(
				!IsTextureNameInList(SideDefs[n].tex2, WTexture, NumWTexture) &&
			  (
				  !BOOMEnable ||
					LineDefs[m].special!=242 ||
					!IsTextureNameInList(SideDefs[n].tex2, CMaps, NumCMaps)
				)
			)
			{
				if (AskConf)
				{
					strncpy(name, SideDefs[n].tex2, 8);
					sprintf(msg1, "Invalid Lower Texture %s In SideDef #%d", name, n);
					sprintf(msg2, "The Name \"%s\" Is Not A Wall Texture", name);
					if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_SIDEDEFS, n);
						return;
					}						
					CheckingObjects(-1, -1);
				}
				else
				{
					Failed = FALSE;
					strncpy(SideDefs[n].tex2, DefaultLowerTexture, 8);
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 40, x0 + 260, y0 + 50);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 40, "%d", count);
				}
			}
			if
			(
				!IsTextureNameInList(SideDefs[n].tex3, WTexture, NumWTexture) &&
			  (
					(
					  !BOOMEnable ||
						LineDefs[m].special!=242 ||
						!IsTextureNameInList(SideDefs[n].tex3, CMaps, NumCMaps)
					) &&
					(
					  !BOOMEnable ||
						LineDefs[m].special!=260 ||
						LineDefs[m].tag==0
					)
				)
			)
			{
				if (AskConf)
				{
					strncpy(name, SideDefs[n].tex3, 8);
					sprintf(msg1, "Invalid Normal Texture %s In SideDef #%d", name, n);
					sprintf(msg2, "The Name \"%s\" Is Not A Wall Texture", name);
					if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
					{
						GoToObject(OBJ_SIDEDEFS, n);
						return;
					}
					CheckingObjects(-1, -1);
				}
				else
				{
					Failed = FALSE;
					strncpy(SideDefs[n].tex3, DefaultWallTexture, 8);
					count++;
					SetColor(DARKGRAY);
					DrawScreenBox(x0 + 230, y0 + 40, x0 + 260, y0 + 50);
					SetColor(BLACK);
					DrawScreenText(x0 + 230, y0 + 40, "%d", count);
				}
			}
  	}
	}
  if (!AskConf)
	{
    SetColor(YELLOW);
		DrawScreenText(x0 + 10, y0 + 60, "Press any key to continue.");
    bioskey(0);
  }
}


//
//  check for players starting points
//

void CheckStartingPos() 
{
	int n;
	int found = 0;
	int p1found = 0;
	int dmfound = 0;
  char msg1[80], msg2[80];

  LogMessage("\nVerifying start positions...\n");
  WriteCheckLog("\nVerifying start positions...\n");

	CheckingObjects(-1, -1);

	for (n = 0; n < NumThings; n++)
		if (Things[n].type == 1)
			found = ++p1found;
		else if (Things[n].type == 11)
			dmfound++;

	for (n = 0; n < NumThings; n++)
		if (Things[n].type == 2)
		{
			found++;
			break;
		}
	for (n = 0; n < NumThings; n++)
		if (Things[n].type == 3)
		{
			found++;
			break;
		}
	for (n = 0; n < NumThings; n++)
		if (Things[n].type == 4)
		{
			found++;
			break;
		}


	if (p1found == 0)
	{
		sprintf(msg1, "Level has no Player 1 start.");
		sprintf(msg2, "The level may crash if played.");
		if (CheckFailed(-1, -1, msg1, msg2, msg1, TRUE))
			return;
	}
	else if (p1found > 1)
	{
		sprintf(msg1, "Level has %d Player 1 starts.",p1found);
		sprintf(msg1, "Is this intentional?");
		if (CheckFailed(-1, -1, msg1, NULL, msg1, TRUE))
			return;
	}
	else if (found < 4)
	{
		sprintf(msg1, "Warning: Level doesn't have all four Player starting points.");
		sprintf(msg2, "Please insert starting positions for all four Players.");
		if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
			return;
	}
	if (dmfound <= 3)
	{
		sprintf(msg1, "Warning: Level has less than four Death Match start points.");
		sprintf(msg2, "Please insert atleast four Death Match starting positions.");
		if (CheckFailed(-1, -1, msg1, msg2, msg1, FALSE))
			return;
	}
	return;
}

/*
   check the level consistency
   */								

BCINT CheckLevel( BCINT x0, BCINT y0)
{
	BCINT val;
  val =	DisplayMenu
				(
				  x0,
					y0,
					((x0 == -1) ? "Check Level Consistency" : NULL),
					"Display Number Of Objects",
					"-",
					"Check If All Sectors Are Closed",
					"Check References And Delete Unused Objects",
					"Check for Invalid Triggers and Tags",
					"Check For Missing Textures",
					"Check For Invalid Texture Names",
					"Check Starting Positions",
					"-",
					"Automaticaly Replace Missing Textures",
					"Automaticaly Replace Invalid Texture Names",
					"-",
					"Do Main Checks",
					"Do Texture Checks",
					"Do All Checks",
					"-",
					"List Main Checks",
					"List Texture Checks",
					"List All Checks",
					NULL
				);

	CheckReport=FALSE;
	Failed = FALSE;
	if (val >= 1 && val <= 19)
	{
		switch (val)
		{
			case 1:
				Statistics(-1, -1);
				break;
			case 3:
				CheckSectors();
				break;
			case 4:
				CheckCrossReferences();
				break;
			case 5:
				CheckTriggersAndTags();
				break;
			case 6:
				CheckTextures(TRUE);
				break;
			case 7:
				CheckTextureNames(TRUE);
				break;
			case 8:
				CheckStartingPos();
				break;
			case 10:
				CheckTextures(FALSE);
				break;
			case 11:
				CheckTextureNames(FALSE);
				break;
			case 17: //jff option to list report
				CheckReport=TRUE;
			case 13:
				CreateCheckLog("Check Sectors, Crossreferences, and Triggers");
				CheckSectors();
				if (!CheckReport && Failed)
					break;
				Failed = FALSE;
				CheckCrossReferences();
				if (!CheckReport && Failed)
					break;
				CheckTriggersAndTags();
				break;
			case 18: //jff option to list report
				CheckReport=TRUE;
			case 14:
				CreateCheckLog("Check Textures and Texture Names");
				CheckTextures(TRUE);
				if (!CheckReport && Failed)
					break;
				CheckTextureNames(TRUE);
				break;
			case 19: //jff option to list report
				CheckReport=TRUE;
			case 15:
				CreateCheckLog("Check Sectors, Crossrefs, Triggers, Textures, Starting Pos");
				CheckSectors();
				if (!CheckReport && Failed)
					break;
				Failed = FALSE;
				CheckCrossReferences();
				if (!CheckReport && Failed)
					break;
				Failed = FALSE;
				CheckTriggersAndTags();
				if (!CheckReport && Failed)
					break;
				Failed = FALSE;
				CheckTextures(TRUE);
				if (!CheckReport && Failed)
					break;
				Failed = FALSE;
				CheckTextureNames(TRUE);
				if (!CheckReport && Failed)
					break;
				CheckStartingPos();
				break;
		}
		CheckReport = FALSE;
  }
	else
		return val;
	return 0;
}


/* end of file */
