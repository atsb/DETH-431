/**
 ** udrwchar.c
 **
 ** Copyright (C) 1992, Csaba Biegl
 **   820 Stirrup Dr, Nashville, TN, 37221
 **   csaba@vuse.vanderbilt.edu
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
#include "usercord.h"

void GrUsrDrawChar(int chr,int x,int y,GrTextOption *opt)
{
	U2SX(x,CURC);
	U2SY(y,CURC);
	GrDrawChar(chr,x,y,opt);
}

