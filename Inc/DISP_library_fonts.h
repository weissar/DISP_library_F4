#pragma once

#ifndef _DISP_LIBRARY_FONTS_H_
#define _DISP_LIBRARY_FONTS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "DISP_library.h"

//TODO make it configurable
typedef enum 
{
  fontGfx_FreeMono18pt7b,
  fontGfx_FreeSans12pt7b,
  fontGfx_FreeSansBold12pt7b,
  fontGfx_Orbitron_Light_24,
} eFontsGfx;

bool DISP_SetFont_Gfx(eFontsGfx font);
void DISP_WriteString_Gfx(char* cp);
void DISP_WriteChar_Gfx(char c);
DISP_size_t DISP_WriteCharXY_Gfx(int x, int y, char c);

#endif // _DISP_LIBRARY_FONTS_H_