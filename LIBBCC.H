/*
 * BCC2GRX  -  Interfacing Borland based graphics programs to LIBGRX
 * Copyright (C) 1993-96 by Hartmut Schirmer
 *
 * This library is copyrighted (see above). It might be used and
 * distributed freely as long as all copyright notices are left
 * intact.
 *
 * You may not distribute any changed versions of BCC2GRX without
 * written permission by Hartmut Schirmer.
 *
 * You are permitted to distribute an application linked with BCC2GRX
 * in binary only, provided that the documentation of the program:
 *
 *    a)   informs the user that BCC2GRX is used in the program, AND
 *
 *    b)   provides the user with the necessary information about
 *         how to obtain BCC2GRX. (i.e. ftp site, etc..)
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact :                Hartmut Schirmer
 *                          Feldstrasse 118
 *                  D-24105 Kiel
 *                          Germany
 *
 * e-mail : hsc@techfak.uni-kiel.d400.de
 */

#ifndef __LIBBCC_H
#define __LIBBCC_H

/* Version number, read as
	 0x20a v2.0 alpha
	 0x21b v2.1 beta
	 0x230 v2.3 offical release */
#define __BCC2GRX__ 0x200

#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#  define TRUE (1==1)
#endif
#ifndef FALSE
#  define FALSE (1==0)
#endif

#ifndef near            /* get rid of these stupid keywords */
#define near
#endif
#ifndef far
#define far
#endif
#ifndef huge
#define huge
#endif

#define grOk                0
#define grNoInitGraph      -1
#define grNotDetected      -2
#define grFileNotFound     -3
#define grInvalidDriver    -4
#define grNoLoadMem        -5
#define grNoScanMem        -6
#define grNoFloodMem       -7
#define grFontNotFound     -8
#define grNoFontMem        -9
#define grInvalidMode      -10
#define grError            -11
#define grIOerror          -12
#define grInvalidFont      -13
#define grInvalidFontNum   -14
#define grInvalidVersion   -18

#define DETECT             (-2)
#define NATIVE_GRX         (-3)
#define CURRENT_DRIVER     (-1)
#define VGA                ( 9)
#define EGA                ( 3)
#define IBM8514            ( 6)
#define HERCMONO           ( 7)
#define EGA64              ( 4)
#define EGAMONO            ( 5)
#define CGA                ( 1)
#define MCGA               ( 2)
#define ATT400             ( 8)
#define PC3270             (10)
/* driver definitions from BC++ 4.5 : */
#define DETECTX            (DETECT)
#define VGA256             (11)
#define ATTDEB             (12)
#define TOSHIBA            (13)
#define SVGA16             (14)
#define SVGA256            (15)
#define SVGA32K            (16)
#define SVGA64K            (17)
#define VESA16             (18)
#define VESA256            (19)
#define VESA32K            (20)
#define VESA64K            (21)
#define VESA16M            (22)
#define ATI16              (23)
#define ATI256             (24)
#define ATI32K             (25)
#define COMPAQ             (26)
#define TSENG316           (27)
#define TSENG3256          (28)
#define TSENG416           (29)
#define TSENG4256          (30)
#define TSENG432K          (31)
#define GENOA5             (32)
#define GENOA6             (33)
#define OAK                (34)
#define PARADIS16          (35)
#define PARADIS256         (36)
#define TECMAR             (37)
#define TRIDENT16          (38)
#define TRIDENT256         (39)
#define VIDEO7             (40)
#define VIDEO7II           (41)
#define S3                 (42)
#define ATIGUP             (43)

#define VGALO              0
#define VGAMED             1
#define VGAHI              2
#define IBM8514LO          0
#define IBM8514HI          1
#define HERCMONOHI         0
#define CGAC0              0
#define CGAC1              1
#define CGAC2              2
#define CGAC3              3
#define CGAHI              4
#define MCGAC0             CGAC0
#define MCGAC1             CGAC1
#define MCGAC2             CGAC2
#define MCGAC3             CGAC3
#define MCGAMED            CGAHI
#define MCGAHI             5
#define ATT400C0           MCGAC0
#define ATT400C1           MCGAC1
#define ATT400C2           MCGAC2
#define ATT400C3           MCGAC3
#define ATT400MED          MCGAMED
#define ATT400HI           MCGAHI
#define EGA64LO            0
#define EGA64HI            1
#define EGALO              0
#define EGAHI              1
#define EGAMONOHI          0
#define PC3270HI           0
/* mode definitions from BC++ 4.5 : */
#define RES640x350         0
#define RES640x480         1
#define RES800x600         2
#define RES1024x768        3
#define RES1280x1024       4

/* NATIVE_GRX modes : */
#define GRX_DEFAULT_GRAPHICS                0
#define GRX_BIGGEST_NONINTERLACED_GRAPHICS  1
#define GRX_BIGGEST_GRAPHICS                2
#define GRX_BGI_EMULATION                   3
#define __FIRST_DRIVER_SPECIFIC_MODE        4


#ifndef   BLACK
#  define BLACK         0
#endif
#ifndef   BLUE
#  define BLUE          1
#endif
#ifndef   GREEN
#  define GREEN         2
#endif
#ifndef   CYAN
#  define CYAN          3
#endif
#ifndef   RED
#  define RED           4
#endif
#ifndef MAGENTA
#  define MAGENTA       5
#endif
#ifndef   BROWN
#  define BROWN         6
#endif
#ifndef   LIGHTGRAY
#  define LIGHTGRAY     7
#endif
#ifndef   DARKGRAY
#  define DARKGRAY      8
#endif
#ifndef   LIGHTBLUE
#  define LIGHTBLUE     9
#endif
#ifndef   LIGHTGREEN
#  define LIGHTGREEN   10
#endif
#ifndef   LIGHTCYAN
#  define LIGHTCYAN    11
#endif
#ifndef   LIGHTRED
#  define LIGHTRED     12
#endif
#ifndef   LIGHTMAGENTA
#  define LIGHTMAGENTA 13
#endif
#ifndef   YELLOW
#  define YELLOW       14
#endif
#ifndef   WHITE
#  define WHITE        (__gr_White())
#endif

#define EGA_BLACK        0
#define EGA_BLUE         1
#define EGA_GREEN        2
#define EGA_RED          4
#define EGA_LIGHTBLUE    57
#define EGA_LIGHTGREEN   58
#define EGA_LIGHTRED     60
#define EGA_CYAN         3
#define EGA_LIGHTCYAN    59
#define EGA_MAGENTA      5
#define EGA_LIGHTMAGENTA 61
#define EGA_BROWN        20
#define EGA_LIGHTGRAY    7
#define EGA_DARKGRAY     56
#define EGA_YELLOW       62
#define EGA_WHITE        63

#define SOLID_LINE   0
#define DOTTED_LINE  1
#define CENTER_LINE  2
#define DASHED_LINE  3
#define USERBIT_LINE 4

#define NORM_WIDTH   1
#define THICK_WIDTH  3

#define DEFAULT_FONT      0    /* 8x8 bit mapped font */
#define TRIPLEX_FONT      1
#define SMALL_FONT        2
#define SANS_SERIF_FONT   3
#define GOTHIC_FONT       4
#define SCRIPT_FONT       5
#define SIMPLEX_FONT      6
#define TRIPLEX_SCR_FONT  7
#define COMPLEX_FONT      8
#define EUROPEAN_FONT     9
#define BOLD_FONT         10

#define HORIZ_DIR   0       /* left to right */
#define VERT_DIR    1       /* bottom to top */

#define USER_CHAR_SIZE  0   /* user-defined char size */

enum fill_patterns {        /* Fill patterns for get/setfillstyle */
	EMPTY_FILL,     /* fills area in background color */
	SOLID_FILL,     /* fills area in solid fill color */
	LINE_FILL,      /* --- fill */
	LTSLASH_FILL,       /* /// fill */
	SLASH_FILL,     /* /// fill with thick lines */
	BKSLASH_FILL,       /* \\\ fill with thick lines */
	LTBKSLASH_FILL,     /* \\\ fill */
	HATCH_FILL,     /* light hatch fill */
	XHATCH_FILL,        /* heavy cross hatch fill */
	INTERLEAVE_FILL,    /* interleaving line fill */
	WIDE_DOT_FILL,      /* Widely spaced dot fill */
	CLOSE_DOT_FILL,     /* Closely spaced dot fill */
	USER_FILL       /* user defined fill */
};

#define COPY_PUT 0
#define XOR_PUT  1
#define OR_PUT   2
#define AND_PUT  3
/*      NOT_PUT  not available */

#define LEFT_TEXT   0
#define CENTER_TEXT 1
#define RIGHT_TEXT  2
#define BOTTOM_TEXT 0
#define TOP_TEXT    2

#define MAXCOLORS 15

struct palettetype {
	unsigned char size;
	  signed char colors[MAXCOLORS+1];
};

struct linesettingstype {
	int            linestyle;
	unsigned short upattern;    /* Note : BCC defines unsigned ! */
	int            thickness;
};

struct textsettingstype {
	int font;
	int direction;
	int charsize;
	int horiz;
	int vert;
};

struct fillsettingstype {
	int pattern;
	int color;
};

/* This definition is compatible with the grx
   definition 'int pts[][2]' used to define polygons */
struct pointtype {
	int x, y;
};

struct viewporttype {
	int left, top, right, bottom;
	int clip;
};

struct arccoordstype {
	int x, y;
	int xstart, ystart, xend, yend;
};

/* ------------------------------------------------------------------ */
/* ---                  Internal definitions                      --- */
/* --- Don't access one of these variable and functions directly! --- */

extern int           __gr_Mode;
extern int           __gr_INIT;
extern int           __gr_MaxMode;
extern int           __gr_Result;
extern int           __gr_X, __gr_Y;            /* graphics cursor pos */
extern int           __gr_vpl, __gr_vpt,        /* actual viewport     */
			 __gr_vpr, __gr_vpb;
extern int           __gr_color;                /* drawing color       */
extern int           __gr_colorbg;              /* background color    */
extern int           __gr_colorfill;            /* fill color          */
extern int           __gr_WR;                   /* Write mode          */
extern int           __gr_Xasp;                 /* Aspect ratio        */
extern int           __gr_Yasp;
extern int           __gr_fpatno;
extern int           __gr_lstyle;
extern int           __gr_clip;                 /* actual clipping state    */
extern int           __gr_ADAPTER;              /* Adapter used             */
extern char          __gr_fpatterns[][8];
extern struct palettetype __gr_EGAdef;
extern int           __gr_BGI_w;                /* Width, height and color  */
extern int           __gr_BGI_h;                /* used in                  */
extern int           __gr_BGI_c;                /* GRX_BGI_EMULATION mode   */
extern int           __gr_TextLineStyle;        /* use setlinestyle() while
						   plotting .chr fonts      */

void     __gr_set_up_modes(void);
void     __gr_bar(int left, int top, int right, int bottom);
void     __gr_circle(int x, int y, int radius);
void     __gr_cleardevice(void);
void     __gr_drawpoly(int numpoints, void *polypoints, int close);
void     __gr_ellipse(int x,int y,int stangle,int endangle,int xradius,int yradius);
void     __gr_line(int x1, int y1, int x2, int y2);
void     __gr_restorecrtmode(void);
void     __gr_sector(int x,int y,int stangle,int endangle,int xradius,int yradius);
int      __gr_White(void);
int      __gr_getmodemaxcolor(int mode);
int      __gr_getmodemaxx(int mode);
int      __gr_getmodemaxy(int mode);
int      __gr_setrgbcolor(int r, int g, int b);
void     __gr_setactivepage(int p);
int      __gr_getactivepage(void);
void     __gr_setvisualpage(int p);
int      __gr_getvisualpage(void);
int      __gr_get_BGI_mode_pages(void);
void     __gr_set_BGI_mode_pages(int p);

/* ------------------------------------------------------------------ */
/* ---                  BGI - API definitions                     --- */

void     detectgraph(int *graphdriver,int *graphmode);
void     initgraph(int *graphdriver, int *graphmode, char *pathtodriver);
void     setgraphmode(int mode);
char    *getmodename( int mode_number );
void     graphdefaults(void);
char    *getdrivername( void );
char    *grapherrormsg(int errorcode);
int      getmaxx(void);
int      getmaxy(void);
int      getmaxcolor(void);
void     getviewsettings(struct viewporttype  *viewport);
void     setviewport(int left, int top, int right, int bottom, int clip);
void     getlinesettings(struct linesettingstype  *lineinfo);
void     setlinestyle(int linestyle, unsigned upattern, int thickness);
void     clearviewport(void);
unsigned getpixel(int x, int y);
void     putpixel(int x, int y, int color);
void     bar3d(int left,int top,int right,int bottom,int depth, int topflag);
void     rectangle(int left, int top, int right, int bottom);
void     fillpoly(int numpoints, void *polypoints);
void     fillellipse( int x, int y, int xradius, int yradius );
void     getarccoords(struct arccoordstype  *arccoords);
void     floodfill(int x, int y, int border);
void     setfillpattern( char  *upattern, int color);
void     setfillstyle(int pattern, int color);
void     getimage(int left, int top, int right, int bottom, void  *bitmap);
void     putimage(int left, int top,  void  *bitmap, int op);
unsigned imagesize(int left, int top, int right, int bottom);
void     gettextsettings(struct textsettingstype  *texttypeinfo);
void     settextjustify(int horiz, int vert);
void     settextstyle(int font, int direction, int charsize);
void     setrgbpalette(int color, int red, int green, int blue);
void     setusercharsize(int multx, int divx, int multy, int divy);
void     setwritemode( int mode );
void     outtext(char *textstring);
void     outtextxy(int x, int y, char *textstring);
int      textheight( char *textstring);
int      textwidth( char  *textstring);

int      registerbgifont(void *font);
int      installuserfont(char *name);

int      getpalettesize(void);
void     getpalette(struct palettetype  *palette);
void     __gr_setpalette(int colornum, int color);
void     setallpalette( struct palettetype  *palette);

/* ------------------------------------------------------------------ */
/* ---                  BGI - API extensions                      --- */

extern int           _bold_font, _euro_font,    /* Linkable font files      */
			 _goth_font, _lcom_font,
			 _litt_font, _sans_font,
			 _scri_font, _simp_font,
			 _trip_font, _tscr_font;
void     set_BGI_mode(int *graphdriver, int *graphmode);
/*void   set_BGI_mode_whc() declared inline */
void     __getrgbpalette(int color, int *red, int *green, int *blue);

#ifdef __cplusplus
}
#endif

#define __ABS(a) (a<0 ? -a : a)
#define __INL__ __inline__ static
#define YR(r)  ((r)*__gr_Xasp/__gr_Yasp)
#define XR(r)  (r)

/* ----------------------------------------------------------------- */
__INL__ void restorecrtmode(void)
{
  __gr_restorecrtmode();
}

/* ----------------------------------------------------------------- */
__INL__ void closegraph(void)
{
  restorecrtmode();
  __gr_INIT = FALSE;
}

/* ----------------------------------------------------------------- */
__INL__ int getgraphmode(void)
{
  return (__gr_INIT ? __gr_Mode : (__gr_Result=grNoInitGraph));
}

/* ----------------------------------------------------------------- */
__INL__ int getmaxmode(void)
{
  __gr_set_up_modes();
  return __gr_MaxMode;
}

/* ----------------------------------------------------------------- */
__INL__ void getmoderange(int gd, int *lomode, int *himode)
{
  *lomode = 0;
  *himode = getmaxmode();
}

/* ----------------------------------------------------------------- */
__INL__ int graphresult(void) {
  int res = (__gr_INIT ? __gr_Result : grNoInitGraph);
  __gr_Result = grOk;
  return res;
}

/* ----------------------------------------------------------------- */
__INL__ int getx(void) {
  return __gr_X;
}

/* ----------------------------------------------------------------- */
__INL__ int gety(void) {
  return __gr_Y;
}

/* ----------------------------------------------------------------- */
__INL__ void moveto(int x, int y) {
  __gr_X = x; __gr_Y = y;
}

/* ----------------------------------------------------------------- */
__INL__ void moverel(int dx, int dy) {
  moveto( getx()+dx, gety()+dy);
}

/* ----------------------------------------------------------------- */
__INL__ int getbkcolor(void) {
  return __gr_colorbg;
}

/* ----------------------------------------------------------------- */
__INL__ int getcolor(void) {
  return __gr_color;
}

/* ----------------------------------------------------------------- */
__INL__ void cleardevice(void) {
  __gr_cleardevice();
}

/* ----------------------------------------------------------------- */
__INL__ void setbkcolor(int color) {
  __gr_colorbg= color;
}

/* ----------------------------------------------------------------- */
__INL__ void setcolor(int color) {
  __gr_color= color;
}

/* ----------------------------------------------------------------- */
__INL__ void line(int x1, int y1, int x2, int y2) {
  __gr_line(x1,y1,x2,y2);
}

/* ----------------------------------------------------------------- */
__INL__ void linerel(int dx, int dy) {
  register int x = getx();
  register int y = gety();
  __gr_line(x,y,x+dx,y+dy);
}

/* ----------------------------------------------------------------- */
__INL__ void lineto(int x, int y) {
  __gr_line( getx(), gety(), x, y);
}

/* ----------------------------------------------------------------- */
__INL__ void  drawpoly(int numpoints, void *polypoints) {
  __gr_drawpoly(numpoints, polypoints, FALSE);
}

/* ----------------------------------------------------------------- */
__INL__ void bar(int left, int top, int right, int bottom) {
  __gr_bar(left,top,right, bottom);
}

/* ----------------------------------------------------------------- */
__INL__ void circle(int x, int y, int radius) {
  __gr_circle(x,y,radius);
}

/* ----------------------------------------------------------------- */
__INL__ void ellipse( int x, int y, int stangle, int endangle,
					int xradius, int yradius  ) {
  __gr_ellipse( x, y, stangle, endangle, xradius, yradius);
}

/* ----------------------------------------------------------------- */
__INL__ void  arc(int x, int y, int stangle, int endangle, int radius) {
  __gr_ellipse(x,y,stangle,endangle,radius,radius);
}

/* ----------------------------------------------------------------- */
#ifdef _TP2BCC_
#  define getaspectratio(xa,ya) ({*(xa)=__gr_Xasp; *(ya)=__gr_Yasp; })
#else
__INL__ void getaspectratio(int *xasp, int *yasp) {
   *xasp = __gr_Xasp; *yasp = __gr_Yasp;
}
#endif

/* ----------------------------------------------------------------- */
__INL__ void setaspectratio( int xasp, int yasp ) {
  __gr_Xasp = xasp; __gr_Yasp = yasp;
}

/* ----------------------------------------------------------------- */
__INL__ void getfillsettings(struct fillsettingstype  *fillinfo) {
  fillinfo->pattern = __gr_fpatno;
  fillinfo->color   = __gr_colorfill;
}

/* ----------------------------------------------------------------- */
__INL__ void getfillpattern(char *pattern) {
  memcpy(pattern, &__gr_fpatterns[USER_FILL], 8);
}

/* ----------------------------------------------------------------- */
__INL__ void sector( int x, int y, int stangle, int endangle,
				   int xradius, int yradius  ) {
  __gr_sector(x,y,stangle,endangle,xradius, yradius);
}

/* ----------------------------------------------------------------- */
__INL__ void pieslice(int x, int y, int stangle, int endangle, int radius) {
  __gr_sector(x,y,stangle,endangle,radius,radius);
}

/* ----------------------------------------------------------------- */
__INL__ unsigned setgraphbufsize(unsigned bufsize) {
  return 4096; /* BCC default value */
}

/* ----------------------------------------------------------------- */
__INL__ struct palettetype *getdefaultpalette(void) {
  return &__gr_EGAdef;
}

/* ----------------------------------------------------------------- */
__INL__ int installbgidriver(char *name, void *detect)
{
  return grError;
}

/* ----------------------------------------------------------------- */
__INL__ int registerfarbgidriver(void *driver)
{
  return grError;
}

/* ----------------------------------------------------------------- */
__INL__ int registerfarbgifont(void *font)
{
  return registerbgifont( font);
}

/* ----------------------------------------------------------------- */
__INL__ void textlinestyle(int on) {
  __gr_TextLineStyle = on;
}

/* ----------------------------------------------------------------- */
__INL__ void setpalette(int colornum, int color) {
  __gr_setpalette(colornum,color);
}

/* ----------------------------------------------------------------- */
__INL__ void set_BGI_mode_whc(int *gd, int *gm,
				  int width, int height, int colors) {
  __gr_set_BGI_mode_pages(1);
  __gr_BGI_w = width;
  __gr_BGI_h = height;
  __gr_BGI_c = colors;
  *gd        = NATIVE_GRX;
  *gm        = GRX_BGI_EMULATION;
}

/* ----------------------------------------------------------------- */
__INL__ void set_BGI_mode_pages(int p) {
  __gr_set_BGI_mode_pages(p);
}

/* ----------------------------------------------------------------- */
__INL__ int get_BGI_mode_pages(void) {
  return __gr_get_BGI_mode_pages();
}

/* ----------------------------------------------------------------- */
__INL__ int getmodemaxcolor(int mode) {
   /* works like getmaxcolor() for mode */
   return __gr_getmodemaxcolor(mode);
}

/* ----------------------------------------------------------------- */
__INL__ int getmodemaxx(int mode) {
   /* works like getmaxx() for mode */
   return __gr_getmodemaxx(mode);
}

/* ----------------------------------------------------------------- */
__INL__ int getmodemaxy(int mode) {
   /* works like getmaxy() for mode */
   return __gr_getmodemaxx(mode);
}

/* ----------------------------------------------------------------- */
__INL__ int setrgbcolor(int r, int g, int b) {
  return __gr_setrgbcolor(r,g,b);
}

/* ----------------------------------------------------------------- */
__INL__ void setactivepage(int p) {
  __gr_setactivepage(p);
}

/* ----------------------------------------------------------------- */
__INL__ int getactivepage(void) {
  return __gr_getactivepage();
}

/* ----------------------------------------------------------------- */
__INL__ void setvisualpage(int p) {
  __gr_setvisualpage(p);
}

/* ----------------------------------------------------------------- */
__INL__ int getvisualpage(void) {
  return __gr_getvisualpage();
}

/* ----------------------------------------------------------------- */

#ifndef HOLD_GR_DEFS
#  undef __ABS
#  undef __INL__
#  undef YR
#  undef XR
#endif

#endif
