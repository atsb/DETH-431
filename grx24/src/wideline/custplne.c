/**
 ** custplne.c ---- draw an open ended dashed and/or wide polygon
 **
 ** Copyright (c) 1995 Csaba Biegl, 820 Stirrup Dr, Nashville, TN 37221
 ** [e-mail: csaba@vuse.vanderbilt.edu]
 **
 ** This file is part of the GRX graphics library.
 **
 ** The GRX graphics library is free software; you can redistribute it
 ** and/or modify it under some conditions; see the "copying.grx" file
 ** for details.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 **
 **/

#include "libgrx.h"
#include "shapes.h"

void GrCustomPolyLine(int n,int pt[][2],GrLineOption *o)
{
	GrFillArg fval;
	fval.color = o->lno_color;
	_GrDrawCustomPolygon(n,pt,o,&_GrSolidFiller,fval,FALSE,FALSE);
}
