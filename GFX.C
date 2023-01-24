/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   GFX.C - Graphics routines.
   */

/* the includes */
#include "deu.h"
#include <math.h>
#include <dos.h>
#include <dir.h>

#define BCabs(x)		((x < 0) ? -(x) : (x))

/*JFF simplifies DrawThickScreenLine*/
#define sgn(x) (((x)<0)? (-1) : (((x)>0)? (1) : (0)))

BCINT Palette[32];
/*
void ScreenBoxArea(int, int, int, int, int);
*/



#define BGI_PATH "."

/* the global variables */
BCINT GfxMode = 0;		/* graphics mode number, or 0 for text */
/* 1 = 320x200, 2 = 640x480, 3 = 800x600, 4 = 1024x768 */
/* positive = 16 colors, negative = 256 colors */
BCINT PointerX;		/* X position of pointer */
BCINT PointerY;		/* Y position of pointer */
int OrigX;			/* the X origin */
int OrigY;			/* the Y origin */
float Scale;		/* the scale value */
int ScrMaxX;		/* maximum X screen coord */
int ScrMaxY;		/* maximum Y screen coord */
int ScrCenterX;		/* X coord of screen center */
int ScrCenterY;		/* Y coord of screen center */


#if defined(__GNUC__)
static int res[6][3] = {{640,480,16},{320,200,256},{640,480,256},
							{800,600,256},{1024,768,256},{1280,1024,256}};
#endif


/*
   initialise the graphics display
   */

void InitGfx()
{
    static int  gdriver;
    static int  gmode;
    int         errorcode = grNoInitGraph;
    char foo[120];

    //jff FakeCursor = FALSE;
    
    if (VideoMode < 0 || VideoMode > 5)
			VideoMode = 0;
    set_BGI_mode_whc( &gdriver, &gmode, res[VideoMode][0], res[VideoMode][1], res[VideoMode][2]);
    initgraph( &gdriver, &gmode, BGI_PATH);
    registerbgifont(&_litt_font);
    errorcode = graphresult();
    if(errorcode != grOk)
			ProgError( "graphics error: %s", grapherrormsg( errorcode));
    
    if (VideoMode == 0)
			GfxMode = 2; /* 640x480x16 */
    else
		{
			GfxMode = -VideoMode; /* 640x480x256, 800x600x256, or 1024x768x256 */
			SetDoomPalette( 0);
    }
    

#ifndef SLIM
	if (!DethFont)
	{
		strcpy(foo, Cwd); /* JFF use cpy not cat so foo isn't garbage at start */ 
		strcat(foo, "/deth.fnt");
		DethFont = installuserfont(foo);
		//jff search path for deth.fnt
		if (DethFont<0)
			DethFont = installuserfont(searchpath("deth.fnt"));
	}
#endif

    setlinestyle(0,0,1);
	  settextstyle(0,0,1); 
    setbkcolor( TranslateToGameColor( BLACK));
    ScrMaxX = getmaxx();
    ScrMaxY = getmaxy();
    ScrCenterX = ScrMaxX / 2;
    ScrCenterY = ScrMaxY / 2;
}



/*
   terminate the graphics display
   */

void TermGfx()
{
    if (GfxMode) {
		closegraph();
		GfxMode = 0;
    }
}


/*
   switch from VGA 16 colours to VGA 256 colours
   */

Bool SwitchToVGA256()
{
    static int gdriver = -1;
    int gmode, errorcode;
    
    if (GfxMode > 0) {
		closegraph();
		set_BGI_mode_whc( &gdriver, &gmode, res[1][0], res[1][1], res[1][2]);
		initgraph( &gdriver, &gmode, BGI_PATH);
		errorcode = graphresult();
		if(errorcode != grOk) {
			/* failed for 256 colors - back to 16 colors */
			set_BGI_mode_whc( &gdriver, &gmode, res[0][0], res[0][1], res[0][2]);
			initgraph( &gdriver, &gmode, BGI_PATH);
			errorcode = graphresult();
		}
		
		
		if (errorcode != grOk) /* shouldn't happen */
			ProgError( "graphics error: %s", grapherrormsg( errorcode));
		GfxMode = -1 /* 320x200x256 */;
		SetDoomPalette( 0);
		ScrMaxX = getmaxx();
		ScrMaxY = getmaxy();
		ScrCenterX = ScrMaxX / 2;
		ScrCenterY = ScrMaxY / 2;
		return TRUE;
	}

    return FALSE;
}


/*
   switch from VGA 256 colours to VGA 16 colours
   */

Bool SwitchToVGA16()
{
    if (GfxMode == -1) { 
		TermGfx();   
		InitGfx();  
		CheckMouseDriver();
		if (UseMouse)
			ShowMousePointer();
		return TRUE;
    }
    return FALSE;
}



/*
   clear the screen
   */

void ClearScreen()
{
    cleardevice();
}

/*
   clear the map screen
   */




/*AJB*/
void ClearMapScreen( BCINT top, BCINT maxy)
{
	setviewport(0, top, ScrMaxX, ScrMaxY - maxy, FALSE);
	clearviewport();
	setviewport(0, 0, ScrMaxX, ScrMaxY, FALSE);
}



/*
   set the current drawing color
   */

void SetColor( BCINT color)
{
    if (GfxMode < 0)
		setcolor( TranslateToGameColor(color));
    else
		setcolor( color);
}

/*
   draw a line on the screen from map coords
   */

void DrawMapLine( BCINT mapXstart, BCINT mapYstart, BCINT mapXend, BCINT mapYend)
{
	if ((SCREENX(mapXstart) < -20000) || (SCREENX(mapXstart) > 20000) || (SCREENY(mapYstart) < -20000) || (SCREENY(mapYstart) > 20000))
		return;
	/*JFF pipe thru logical routine to get thick lines*/
	DrawScreenLine(SCREENX( mapXstart), SCREENY( mapYstart), SCREENX( mapXend), SCREENY( mapYend));
}

void DrawMapLineOff( BCINT mapXstart, BCINT mapYstart, BCINT mapXend, BCINT mapYend, BCINT Yoff)
{
	if ((SCREENX(mapXstart) < -20000) || (SCREENX(mapXstart) > 20000) || (SCREENY(mapYstart) < -20000) || (SCREENY(mapYstart) > 20000))
		return;
	/*JFF pipe thru logical routine to get thick lines*/
	DrawScreenLine(SCREENX( mapXstart), SCREENY( mapYstart) - Yoff, SCREENX( mapXend), SCREENY( mapYend) - Yoff);
}

/*
   draw a circle on the screen from map coords
   */

void DrawMapCircle( BCINT mapXcenter, BCINT mapYcenter, BCINT mapRadius)
{
	if ((SCREENX(mapXcenter) < -20000) || (SCREENX(mapXcenter) > 20000) || (SCREENY(mapYcenter) < -20000) || (SCREENY(mapYcenter) > 20000))
		return;
	circle( SCREENX( mapXcenter), SCREENY( mapYcenter), (int) (mapRadius * Scale));
}

/*
   draw an arrow on the screen from map coords
   */

void DrawMapVector( BCINT mapXstart, BCINT mapYstart, BCINT mapXend, BCINT mapYend)
{
	BCINT  scrXstart = SCREENX( mapXstart);
	BCINT  scrYstart = SCREENY( mapYstart);
	BCINT  scrXend   = SCREENX( mapXend);
	BCINT  scrYend   = SCREENY( mapYend);
	double r         = hypot( (double) (scrXstart - scrXend), (double) (scrYstart - scrYend));
	BCINT  scrXoff   = (r >= 1.0) ? (BCINT) ((scrXstart - scrXend) * 4.0 / r * Scale) : 0;
	BCINT  scrYoff   = (r >= 1.0) ? (BCINT) ((scrYstart - scrYend) * 4.0 / r * Scale) : 0;
	
	if ((SCREENX(mapXstart) < -20000) || (SCREENX(mapXstart) > 20000) || (SCREENY(mapYstart) < -20000) || (SCREENY(mapYstart) > 20000))
		return;
	/*JFF pipe thru logical routine to get thick lines*/
	DrawScreenLine( scrXstart, scrYstart, scrXend, scrYend);

	scrXstart = scrXend + 2 * scrXoff;
	scrYstart = scrYend + 2 * scrYoff;
	/*JFF pipe thru logical routine to get thick lines*/
	DrawScreenLine( scrXstart - scrYoff, scrYstart + scrXoff, scrXend, scrYend);
	DrawScreenLine( scrXstart + scrYoff, scrYstart - scrXoff, scrXend, scrYend);
}

/*
   draw an arrow on the screen from map coords and angle (0 - 65535)
   */

void DrawMapArrow( BCINT mapXstart, BCINT mapYstart, UBCINT angle)
{
	BCINT  mapXend   = mapXstart + (BCINT) (50 * cos(angle / 10430.37835));
	BCINT  mapYend   = mapYstart + (BCINT) (50 * sin(angle / 10430.37835));
	BCINT  scrXstart = SCREENX( mapXstart);
	BCINT  scrYstart = SCREENY( mapYstart);
	BCINT  scrXend   = SCREENX( mapXend);
	BCINT  scrYend   = SCREENY( mapYend);
	double r         = hypot( scrXstart - scrXend, scrYstart - scrYend);
	BCINT  scrXoff   = (r >= 1.0) ? (BCINT) ((scrXstart - scrXend) * 4.0 / r * Scale) : 0;
	BCINT  scrYoff   = (r >= 1.0) ? (BCINT) ((scrYstart - scrYend) * 4.0 / r * Scale) : 0;

	if ((SCREENX(mapXstart) < -20000) || (SCREENX(mapXstart) > 20000) || (SCREENY(mapYstart) < -20000) || (SCREENY(mapYstart) > 20000))
		return;
	
	/*JFF pipe thru logical routine to get thick lines*/
	DrawScreenLine( scrXstart, scrYstart, scrXend, scrYend);
	scrXstart = scrXend + 2 * scrXoff;
	scrYstart = scrYend + 2 * scrYoff;
	/*JFF pipe thru logical routine to get thick lines*/
	DrawScreenLine( scrXstart - scrYoff, scrYstart + scrXoff, scrXend, scrYend);
	DrawScreenLine( scrXstart + scrYoff, scrYstart - scrXoff, scrXend, scrYend);
}
/*
   draw a line on the screen from screen coords
   */

void DrawScreenLine( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend)
{
	struct linesettingstype linfo;

	// use "line" or "DrawThickScreenLine" depending on line thickness
	// setting currently present in library
	// JFF

	getlinesettings(&linfo);
	if (linfo.thickness==THICK_WIDTH)
		DrawThickScreenLine(Xstart, Ystart, Xend, Yend);
	else
	    line( Xstart, Ystart, Xend, Yend);
}

/*
	JFF: This is a hack to make up for the lack of the "thick line" support
	in the DGJPP v2 BCC2GRX/GRX20 API. When this routine is called in place
	of "line", a triple thickness line is drawn.

	DrawMapLine, DrawMapLineOff, DrawMapVector, and DrawMapArrow have been
	modified to call DrawScreenLine, instead of calling line directly. This
	routine has been changed to check linesettings and to call the present
	DrawThickScreenLine	if the thickness field is WIDTH_THICK.
*/

void DrawThickScreenLine( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend)
{
	int dx = Xend-Xstart;
	int dy = Yend-Ystart;
	int sx,sy;

	/* jff do an ad-hoc "thick" line based on rough direction of line */

	if (abs(dy)>2*abs(dx))				/* more vertical than horizontal? */
	{
		sx = -1; sy = 0;
	}
	else if (abs(dx)>2*abs(dy))			/* more horizontal than vertical? */
	{
		sx = 0; sy = -1;
	}
	else 	 		   					/* roughly 45 degrees? */
	{
	    sx = -sgn(dy); sy = sgn(dx); 	/* smallest "normal" integer vector */
	}

    line( Xstart+sx, Ystart+sy, Xend+sx, Yend+sy);
    line( Xstart, Ystart, Xend, Yend);
    line( Xstart-sx, Ystart-sy, Xend-sx, Yend-sy);
}

/*
   draw a filled in box on the screen from screen coords
   */

void DrawScreenBox( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend)
{
    setfillstyle( 1, getcolor());
    bar( Xstart, Ystart, Xend, Yend);
}

void DrawScreenFrame( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend)
{
    line( Xstart, Yend, Xend, Yend);
    line( Xend, Ystart, Xend, Yend);
    line( Xstart, Ystart, Xend, Ystart);
    line( Xstart, Ystart, Xstart, Yend);
}


/*
   draw a filled-in 3D-box on the screen from screen coords
   */

void DrawScreenBox3D( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend)
{
    setfillstyle( 1, TranslateToGameColor(DARKGRAY));
    bar( Xstart + 1, Ystart + 1, Xend - 1, Yend - 1);
    SetColor(DARKERGRAY);
    line( Xstart, Yend, Xend, Yend);
    line( Xend, Ystart, Xend, Yend);
    SetColor(LIGHTGRAY);
    line( Xstart, Ystart, Xend, Ystart);
    line( Xstart, Ystart, Xstart, Yend);
    SetColor( BLACK);
}



/*
   draw a hollow 3D-box on the screen from screen coords
   */

void DrawScreenBoxHollow( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend)
{
    setfillstyle( 1, TranslateToGameColor( BLACK));
    bar( Xstart + 1, Ystart + 1, Xend - 1, Yend - 1);
    SetColor(LIGHTGRAY);
    line( Xstart, Yend, Xend, Yend);
    line( Xend, Ystart, Xend, Yend);
    SetColor(DARKERGRAY);
    line( Xstart, Ystart, Xend, Ystart);
    line( Xstart, Ystart, Xstart, Yend);
    SetColor(LIGHTGRAY);
}



void DrawScreenBar( BCINT Xstart, BCINT Ystart, BCINT Width, Bool Reversed)
{
	SetColor(Reversed ? DARKERGRAY : LIGHTGRAY);
    line( Xstart, Ystart, Xstart + Width, Ystart);
	SetColor(Reversed ? LIGHTGRAY : DARKERGRAY);
    line( Xstart, Ystart + 1, Xstart + Width, Ystart + 1);
}

void DrawScreenButton( BCINT Xstart, BCINT Ystart, BCINT Width, BCINT Height, char *Text, Bool Filled)
{
	if (Filled == TRUE) {
		setfillstyle( 1, TranslateToGameColor( DARKGRAY));
		bar( Xstart + 1, Ystart + 1, Xstart + Width - 1, Ystart + Height - 1);
   	}
    SetColor(DARKERGRAY);
    line( Xstart, Ystart + Height, Xstart + Width, Ystart + Height);
    line( Xstart + Width, Ystart, Xstart + Width, Ystart + Height); 
	SetColor(LIGHTGRAY);
	line( Xstart, Ystart, Xstart + Width, Ystart);
	line( Xstart, Ystart, Xstart, Ystart + Height); 
	ScreenBoxArea(Xstart - 1, Ystart - 1, Width + 2, Height + 2, BLACK);
	if (Text != "" || Text != NULL) {
		settextstyle(2,0,0);
		settextjustify(1,2);
		DrawScreen3DText(Xstart + (Width / 2) + 2, Ystart + (Height / 2) - 5, BLACK, TRUE, Text);
		settextstyle(0,0,1);
		settextjustify(0,2);
	}
}

void DrawScreenButtonGray( BCINT Xstart, BCINT Ystart, BCINT Width, BCINT Height, char *Text, Bool Filled)
{
	BCINT width;
	if (Filled == TRUE) {
		setfillstyle( 1, TranslateToGameColor( DARKGRAY));
		bar( Xstart + 1, Ystart + 1, Xstart + Width - 1, Ystart + Height - 1);
   	}
    SetColor(DARKERGRAY);
    line( Xstart, Ystart + Height, Xstart + Width, Ystart + Height);
    line( Xstart + Width, Ystart, Xstart + Width, Ystart + Height); 
	SetColor(LIGHTGRAY);
	line( Xstart, Ystart, Xstart + Width, Ystart);
	line( Xstart, Ystart, Xstart, Ystart + Height); 
	ScreenBoxArea(Xstart - 1, Ystart - 1, Width + 2, Height + 2, BLACK);
	if (Text != "" || Text != NULL) {
		settextstyle(2,0,0);
		width = textwidth(Text);
		settextjustify(1,2);
		DrawScreen3DText(Xstart + (Width / 2) + 2, Ystart + (Height / 2) - 5, GRAY, TRUE, Text);
		settextstyle(0,0,1);
		settextjustify(0,2);
	}
}

void DrawScreenButtonIn( BCINT Xstart, BCINT Ystart, BCINT Width, BCINT Height, char *Text, Bool Filled)
{
	if (Filled) {
		setfillstyle( 1, TranslateToGameColor( DARKGRAY));
		bar( Xstart + 1, Ystart + 1, Xstart + Width - 1, Ystart + Height - 1);
   	}
	SetColor(LIGHTGRAY);
	line( Xstart, Ystart + Height, Xstart + Width, Ystart + Height);
	line( Xstart + Width, Ystart, Xstart + Width, Ystart + Height); 
	SetColor(DARKERGRAY);
	line( Xstart, Ystart, Xstart + Width, Ystart);
	line( Xstart, Ystart, Xstart, Ystart + Height); 
	if (Text != " " || Text != NULL) {
		settextstyle(2,0,0);
		settextjustify(1,2);
		DrawScreen3DText(Xstart + (Width / 2) + 3, Ystart + (Height / 2) - 4, BLACK, TRUE, Text);
		settextstyle(0,0,1); 
		settextjustify(0,2);
	}
}

//jff add this to get legible buttons!

void DrawScreenButtonInPlainColor( BCINT Xstart, BCINT Ystart, BCINT Width, BCINT Height, char *Text, int color, Bool Filled)
{
	if (Filled) {
		setfillstyle( 1, TranslateToGameColor( DARKGRAY));
		bar( Xstart + 1, Ystart + 1, Xstart + Width - 1, Ystart + Height - 1);
   	}
	SetColor(LIGHTGRAY);
	line( Xstart, Ystart + Height, Xstart + Width, Ystart + Height);
	line( Xstart + Width, Ystart, Xstart + Width, Ystart + Height); 
	SetColor(DARKERGRAY);
	line( Xstart, Ystart, Xstart + Width, Ystart);
	line( Xstart, Ystart, Xstart, Ystart + Height); 
	if (Text != " " || Text != NULL) {
		settextstyle(2,0,0);
		settextjustify(1,2);
		SetColor(color);
		DrawScreenText(Xstart + (Width / 2) , Ystart + (Height / 2) - 3 ,  Text);
		settextstyle(0,0,1); 
		settextjustify(0,2);
	}
}

void DrawScreenButtonOut( BCINT Xstart, BCINT Ystart, BCINT Width, BCINT Height, Bool Filled)
{
	if (Filled) {
		setfillstyle( 1, TranslateToGameColor( BLUE));
		bar( Xstart + 1, Ystart + 1, Xstart + Width - 1, Ystart + Height - 1);
   	}
	SetColor(DARKERGRAY);
	line( Xstart, Ystart + Height, Xstart + Width, Ystart + Height);
	line( Xstart + Width, Ystart, Xstart + Width, Ystart + Height); 
	SetColor(LIGHTGRAY);
	line( Xstart, Ystart, Xstart + Width, Ystart);
	line( Xstart, Ystart, Xstart, Ystart + Height); 
}


void DrawScreenWindow(int x, int y, int width, int height, char *title, int color)
{
	int dx = x + width;
	int dy = y + height;
	
	SetColor(LIGHTGRAY);
	line(x, y, dx - 1, y);
	line(x, y, x, dy - 1);
	SetColor(DARKERGRAY);
	line(x, dy, dx , dy);
	line(dx, y, dx, dy);
	setfillstyle( 1, TranslateToGameColor(DARKGRAY));
	bar( x + 1, y + 1, dx - 1, dy - 1);
	setfillstyle( 1, TranslateToGameColor(color));
	bar( x + 3, y + 3, dx - 3, y + 17);
	SetColor(BLACK);
	DrawScreenTextFonted(2, 0, x + 6, y + 4, "%s", title);
}


void ScreenHighlightArea(int x, int y, int width, int height, char *text, int color, int textcolor)
{
	int dx = x + width;
	int dy = y + height;

	setfillstyle( 1, TranslateToGameColor(color));
	bar( x + 1, y + 1, dx - 1, dy - 1);
	SetColor(textcolor);
	DrawScreenText(x + 3, y + 2, "%s", text);

}

void ScreenBoxArea(int x, int y, int width, int height, int color)
{
	int dx = x + width;
	int dy = y + height;

	SetColor(color);
    line( x, y, dx, y);
    line( x, y, x, dy);
    line( dx, y, dx, dy);
    line( x, dy, dx, dy);
}

/*
   draw a meter bar on the screen from screen coords (in a hollow box); max. value = 1.0
   */

#ifndef SLIM
void DrawScreenMeter( BCINT Xstart, BCINT Ystart, BCINT Xend, BCINT Yend, float value)
{
    if (value < 0.0)
		value = 0.0;
    if (value > 1.0)
		value = 1.0;
    setfillstyle( 1, TranslateToGameColor( BLACK));
    bar( Xstart + 1 + (BCINT) ((Xend - Xstart - 2) * value), Ystart + 1, Xend - 1, Yend - 1);
    setfillstyle( 1, TranslateToGameColor( LIGHTGREEN));
    bar( Xstart + 1, Ystart + 1, Xstart + 1 + (BCINT) ((Xend - Xstart - 2) * value), Yend - 1);
}
#endif


/*
   write text to the screen
   */

void DrawScreenText( BCINT Xstart, BCINT Ystart, char *msg, ...)
{
    static BCINT lastX;
    static BCINT lastY;
    char temp[ 120];
    va_list args;
    va_start( args, msg);
    vsprintf( temp, msg, args);
    va_end( args);

	if (Xstart < -1 || Ystart < -1)
		return;
    if (Xstart == -1)
		Xstart = lastX;
    if (Ystart == -1)
		Ystart = lastY;
	settextstyle(DethFont, 0, 1);
    outtextxy( Xstart, Ystart, temp);
	settextstyle(0,0,1);
    lastX = Xstart;
    lastY = Ystart + 10; 
}

void DrawScreenTextFonted(BCINT Font, BCINT Size, BCINT Xstart, BCINT Ystart, char *msg, ...)
{
    static BCINT lastX;
    static BCINT lastY;
    char temp[ 120];
    va_list args;
    va_start(args, msg);
    vsprintf(temp, msg, args);
    va_end( args);

	if (Xstart < -1 || Ystart < -1)
		return;
    if (Xstart == -1)
		Xstart = lastX;
    if (Ystart == -1)
		Ystart = lastY;
	settextstyle(Font, 0, Size);
    outtextxy(Xstart, Ystart, temp);
	settextstyle(0,0,1);
    lastX = Xstart;
    lastY = Ystart + 10; 
}

void DrawScreen3DText( BCINT Xstart, BCINT Ystart, BCINT col, Bool Bright, char *msg, ...)
{
    static BCINT lastX;
    static BCINT lastY; 
    char temp[ 120];
    va_list args;
    va_start( args, msg);
    vsprintf( temp, msg, args);
    va_end( args);

	if (Xstart < -1 || Ystart < -1)
		return;
    if (Xstart == -1)
		Xstart = lastX;
    if (Ystart == -1)
		Ystart = lastY;
	if (Bright)
		SetColor(LIGHTGRAY);
	else
		SetColor(DARKERGRAY);
    outtextxy( Xstart, Ystart, temp);
	SetColor(col);
    outtextxy( Xstart - 1, Ystart - 1, temp);

    lastX = Xstart;
    lastY = Ystart + 10; 
}


/*
   draw (or erase) the pointer if we aren't using the mouse
   */

void DrawPointer()
{
	//jff add coords to call
    /* use XOR mode : drawing the pointer twice erases it */
	if (FakeCursor)
	{
    setwritemode( XOR_PUT);
		SetColor( YELLOW);
		line( PointerX - 12, PointerY - 12, PointerX + 12, PointerY + 12);
		line( PointerX - 12, PointerY + 12, PointerX + 12, PointerY - 12);
    /* restore normal write mode */
    setwritemode( COPY_PUT); 
	}
}



/*
   load one "playpal" palette and change all palette colours
   */

void SetDoomPalette( BCINT playpalnum)
{
    MDirPtr             dir;
    unsigned char huge *dpal;
    BCINT                 n;
    
    if (playpalnum < 0 || /* JFF && isnt too useful */ playpalnum > 13)
		return;
    dir = FindMasterDir( MasterDir, "PLAYPAL");
    if (dir) {
		dpal = (unsigned char*)GetFarMemory( 768 * sizeof( char));
		BasicWadSeek( dir->wadfile, dir->dir.start);
		for (n = 0; n <= playpalnum; n++)
			BasicWadRead( dir->wadfile, dpal, 768L);
		
      if (Gamma > 0 && Gamma <= 4) 
        {
          float gmult[5] = {1.0, 0.75, 0.55, 0.4, 0.3};

          for (n = 0; n < 768; n++)
            dpal[n] = (pow(((double) dpal[n] / 255.0), gmult[Gamma]) 
                               * 255.0);
        }
		
		GrResetColors();
		for(n=0;n<254;n++)
			GrAllocCell();
		
		for(n=0;n<256;n++)
			GrSetColor(n,dpal[3*n],dpal[3*n+1],dpal[3*n+2]); /*JFF*/
		
		
		FreeFarMemory( dpal );
    }
}

/*
   translate a standard color to Doom palette 0 (approx.)
   */

BCINT TranslateToGameColor( BCINT color)
{
	return Palette[color];
}



/*
   translate (dx, dy) into an integer angle value (0-65535)
   */

UBCINT ComputeAngle( BCINT dx, BCINT dy)
{
    return (UBCINT) (atan2( (double) dy, (double) dx) * 10430.37835 + 0.5);
    /* Yes, I know this function could be in another file, but */
    /* this is the only source file that includes <math.h>...  */
}



/*
   compute the distance from (0, 0) to (dx, dy)
   */

UBCINT ComputeDist( BCINT dx, BCINT dy)
{
    return (UBCINT) (hypot( (double) dx, (double) dy) + 0.5);
    /* Yes, I know this function could be in another file, but */
    /* this is the only source file that includes <math.h>...  */
}



/*
   insert the vertices of a new polygon
   */

void InsertPolygonVertices(BCINT centerx, BCINT centery, BCINT sides, BCINT radius)
{
    BCINT n;
    
    for (n = 0; n < sides; n++)
		InsertObject( OBJ_VERTEXES, -1, centerx + (BCINT) ((double) radius * cos( 6.28 * (double) n / (double) sides)), centery + (BCINT) ((double) radius * sin( 6.2832 * (double) n / (double) sides)));
    /* Yes, I know... etc. */
}



/*
   move (x, y) to a new position: rotate and scale around (0, 0)
   */

//void RotateAndScaleCoords(BCINT *x, BCINT *y, double angle, double scale)
//{
//    double r, theta;
//    
//    r = hypot((double) *x, (double) *y);
//    theta = atan2((double) *y, (double) *x);
//    *x = (BCINT) (r * scale * cos(theta + angle) + 0.5);
//    *y = (BCINT) (r * scale * sin(theta + angle) + 0.5);
//}
// jff
// use double throughout, round final answer for increased accuracy
//
void RotateAndScaleCoords(double *x, double *y, double angle, double scale)
{
    double r, theta;
    
    r = hypot(*x,*y);
    theta = atan2(*y,*x);
    *x = r * scale * cos(theta + angle);
    *y = r * scale * sin(theta + angle);
}

/* end of file */
