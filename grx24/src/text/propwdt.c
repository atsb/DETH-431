/**
 ** propwdt.c ---- calculate the width of a string using a proportional font
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

int GrProportionalTextWidth(GrFont *font,void *text,int len,int type)
{
	char *txp = (char *)text;
	int   wdt = 0;
	while(--len >= 0) {
	    wdt += GrFontCharWidth(font,GR_TEXTSTR_CODE(txp,type));
	    txp += GR_TEXTCHR_SIZE(type);
	}
	return(wdt);
}




