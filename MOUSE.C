/*
   Mouse interface by Rapha‰l Quinet <quinet@montefiore.ulg.ac.be>
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.
   
   MOUSE.C - Mouse driver routines.
   */

/* the includes */
#include "deu.h"
#include <dos.h>

/* mouse interrupt number */
#define MOUSE 0x33

/* the global data */
Bool UseMouse;						/* is there a mouse driver? */


/*
   initialize the mouse driver
   */


#include <mousex.h>

void CheckMouseDriver()
{
    if (MouseDetect()) {
			UseMouse = TRUE;
			MouseEventMode(0);
			MouseInit();
			MouseSetColors(TranslateToGameColor(WHITE),GrNOCOLOR);
    }
    else {
			UseMouse = FALSE;
    }
}


void ShowMousePointer()
{
	if (!UseMouse)
		return;
  MouseDisplayCursor();
  MouseEventEnable(0,1);
    
}



void HideMousePointer()
{
	if (!UseMouse)
		return;
  MouseEraseCursor();
  MouseEventEnable(0,0);
}


void GetMouseCoords(BCINT *x, BCINT *y, BCINT *buttons)
{
    MouseEvent mevent;
		if (!UseMouse)
			return;
    MouseGetEvent(M_POLL|M_BUTTON_DOWN|M_BUTTON_UP|M_NOPAINT,&mevent);
    *x = mevent.x;
    *y = mevent.y;
    *buttons = mevent.buttons;
}



void SetMouseCoords( BCINT x, BCINT y)
{
	if (!UseMouse)
		return;
    MouseWarp(x,y);
}



void SetMouseLimits( BCINT x0, BCINT y0, BCINT x1, BCINT y1)
{
	if (!UseMouse)
		return;
    MouseSetLimits(x0,y0,x1,y1);
}


Bool MouseInArea(BCINT x, BCINT y, BCINT length, BCINT height)
{
	BCINT PointerX, PointerY, buttons;
	if (!UseMouse)
		return FALSE;
	GetMouseCoords(&PointerX, &PointerY, &buttons);
	if (PointerX >= x && PointerX <= x + length && PointerY >= y && PointerY <= y + height)
			return TRUE;
	else	
			return FALSE;
}

Bool MouseClickedArea(BCINT x, BCINT y, BCINT length, BCINT height)
{
	BCINT PointerX, PointerY, buttons;
	if (!UseMouse)
		return FALSE;
	GetMouseCoords(&PointerX, &PointerY, &buttons);
	if (PointerX >= x && PointerX <= x + length && PointerY >= y && PointerY <= y + height)
		while (buttons == 1) 
			GetMouseCoords( &PointerX, &PointerY, &buttons);
		if (buttons == 0 && PointerX >= x && PointerX <= x + length && PointerY >= y && PointerY <= y + height)
			return TRUE;
	else	
		return FALSE;
}

void ResetMouseLimits()
{
	if (!UseMouse)
		return;
    MouseSetLimits(0,0,ScrMaxX,ScrMaxY);
}


/* end of file */
