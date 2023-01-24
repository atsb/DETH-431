/**
 ** BCC2GRX  -  Interfacing Borland based graphics programs to LIBGRX
 ** Copyright (C) 1993-97 by Hartmut Schirmer
 **
 **
 ** Contact :                Hartmut Schirmer
 **                          Feldstrasse 118
 **                  D-24105 Kiel
 **                          Germany
 **
 ** e-mail : hsc@techfak.uni-kiel.de
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

#ifdef __TURBOC__
#  include <graphics.h>
#endif
#ifdef __GNUC__
#  include "../include/libbcc.h"
#endif

#include "bgiext.h"

unsigned char _dac_g256[256][3] = {
	 { 0, 0, 0},   { 1, 0, 0},   { 1, 0, 2},   { 0, 1, 1},
	 { 1, 1, 1},   { 2, 1, 1},   { 2, 1, 3},   { 1, 2, 2},
	 { 2, 2, 2},   { 3, 2, 2},   { 3, 2, 4},   { 2, 3, 3},
	 { 3, 3, 3},   { 4, 3, 3},   { 4, 3, 5},   { 3, 4, 4},
	 { 4, 4, 4},   { 5, 4, 4},   { 5, 4, 6},   { 4, 5, 5},
	 { 5, 5, 5},   { 6, 5, 5},   { 6, 5, 7},   { 5, 6, 6},
	 { 6, 6, 6},   { 7, 6, 6},   { 7, 6, 8},   { 6, 7, 7},
	 { 7, 7, 7},   { 8, 7, 7},   { 8, 7, 9},   { 7, 8, 8},
	 { 8, 8, 8},   { 9, 8, 8},   { 9, 8,10},   { 8, 9, 9},
	 { 9, 9, 9},   {10, 9, 9},   {10, 9,11},   { 9,10,10},
	 {10,10,10},   {11,10,10},   {11,10,12},   {10,11,11},
	 {11,11,11},   {12,11,11},   {12,11,13},   {11,12,12},
	 {12,12,12},   {13,12,12},   {13,12,14},   {12,13,13},
	 {13,13,13},   {14,13,13},   {14,13,15},   {13,14,14},
	 {14,14,14},   {15,14,14},   {15,14,16},   {14,15,15},
	 {15,15,15},   {16,15,15},   {16,15,17},   {15,16,16},
	 {16,16,16},   {17,16,16},   {17,16,18},   {16,17,17},
	 {17,17,17},   {18,17,17},   {18,17,19},   {17,18,18},
	 {18,18,18},   {19,18,18},   {19,18,20},   {18,19,19},
	 {19,19,19},   {20,19,19},   {20,19,21},   {19,20,20},
	 {20,20,20},   {21,20,20},   {21,20,22},   {20,21,21},
	 {21,21,21},   {22,21,21},   {22,21,23},   {21,22,22},
	 {22,22,22},   {23,22,22},   {23,22,24},   {22,23,23},
	 {23,23,23},   {24,23,23},   {24,23,25},   {23,24,24},
	 {24,24,24},   {25,24,24},   {25,24,26},   {24,25,25},
	 {25,25,25},   {26,25,25},   {26,25,27},   {25,26,26},
	 {26,26,26},   {27,26,26},   {27,26,28},   {26,27,27},
	 {27,27,27},   {28,27,27},   {28,27,29},   {27,28,28},
	 {28,28,28},   {29,28,28},   {29,28,30},   {28,29,29},
	 {29,29,29},   {30,29,29},   {30,29,31},   {29,30,30},
	 {30,30,30},   {31,30,30},   {31,30,32},   {30,31,31},
	 {31,31,31},   {32,31,31},   {32,31,33},   {31,32,32},
	 {32,32,32},   {33,32,32},   {33,32,34},   {32,33,33},
	 {33,33,33},   {34,33,33},   {34,33,35},   {33,34,34},
	 {34,34,34},   {35,34,34},   {35,34,36},   {34,35,35},
	 {35,35,35},   {36,35,35},   {36,35,37},   {35,36,36},
	 {36,36,36},   {37,36,36},   {37,36,38},   {36,37,37},
	 {37,37,37},   {38,37,37},   {38,37,39},   {37,38,38},
	 {38,38,38},   {39,38,38},   {39,38,40},   {38,39,39},
	 {39,39,39},   {40,39,39},   {40,39,41},   {39,40,40},
	 {40,40,40},   {41,40,40},   {41,40,42},   {40,41,41},
	 {41,41,41},   {42,41,41},   {42,41,43},   {41,42,42},
	 {42,42,42},   {43,42,42},   {43,42,44},   {42,43,43},
	 {43,43,43},   {44,43,43},   {44,43,45},   {43,44,44},
	 {44,44,44},   {45,44,44},   {45,44,46},   {44,45,45},
	 {45,45,45},   {46,45,45},   {46,45,47},   {45,46,46},
	 {46,46,46},   {47,46,46},   {47,46,48},   {46,47,47},
	 {47,47,47},   {48,47,47},   {48,47,49},   {47,48,48},
	 {48,48,48},   {49,48,48},   {49,48,50},   {48,49,49},
	 {49,49,49},   {50,49,49},   {50,49,51},   {49,50,50},
	 {50,50,50},   {51,50,50},   {51,50,52},   {50,51,51},
	 {51,51,51},   {52,51,51},   {52,51,53},   {51,52,52},
	 {52,52,52},   {53,52,52},   {53,52,54},   {52,53,53},
	 {53,53,53},   {54,53,53},   {54,53,55},   {53,54,54},
	 {54,54,54},   {55,54,54},   {55,54,56},   {54,55,55},
	 {55,55,55},   {56,55,55},   {56,55,57},   {55,56,56},
	 {56,56,56},   {57,56,56},   {57,56,58},   {56,57,57},
	 {57,57,57},   {58,57,57},   {58,57,59},   {57,58,58},
	 {58,58,58},   {59,58,58},   {59,58,60},   {58,59,59},
	 {59,59,59},   {60,59,59},   {60,59,61},   {59,60,60},
	 {60,60,60},   {61,60,60},   {61,60,62},   {60,61,61},
	 {61,61,61},   {62,61,61},   {62,61,63},   {61,62,62},
	 {62,62,62},   {63,62,62},   {63,62,63},   {62,63,63},
	 {63,63,63},   {63,63,63},   {63,63,63},   {63,63,63}
};

void setrgbgray256(void)
{
  int ci;

  for (ci = 0; ci < 256; ++ci)
    setrgbpalette( ci, _dac_g256[ci][0], _dac_g256[ci][1], _dac_g256[ci][2]);
}
