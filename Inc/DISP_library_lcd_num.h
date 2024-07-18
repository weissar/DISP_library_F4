#ifndef _DISP_LIBRARY_LCD_NUM_H
#define _DISP_LIBRARY_LCD_NUM_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "DISP_library.h"

bool DISP_LCD_ShowNum(int val, int x, int y, int w, int h);

void DISP_LCD_SetFat(int fatPixels);
void DISP_LCD_SetGap(int gapPixels);

#endif    // _DISP_LIBRARY_LCD_NUM_H
