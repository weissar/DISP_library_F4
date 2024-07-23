#ifndef _DISP_LIBRARY_H
#define _DISP_LIBRARY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// here isn't DISP_init or similar, must be called in implementation
typedef struct _DISP_size_struct
{
  int w;
  int h;
} DISP_size_t;

typedef struct _DISP_point_struct
{
  int x;
  int y;
} DISP_point_t;

void DISP_PointOffset(DISP_point_t* pPoint, int xOffset, int yOffset);

int DISP_GetWidth(void);
int DISP_GetHeight(void);

void DISP_DrawLine(int x0, int y0, int x1, int y1, uint32_t color);
void DISP_DrawHLine(int x0, int y0, int w, uint32_t color);
void DISP_DrawVLine(int x0, int y0, int h, uint32_t color);

void DISP_DrawRect(int x, int y, int w, int h, uint32_t color);
void DISP_FillRect(int x, int y, int w, int h, uint32_t color);
void DISP_DrawCircle(int centerX, int centerY, int radius, uint32_t colorSet);
void DISP_FillCircle(int x0, int y0, int radius, uint32_t color);

void DISP_FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

uint32_t DISP_GetPixel(int x, int y);
void DISP_DrawPixel(int x, int y, uint32_t color);

bool DISP_SetViewPort(float left, float right, float top, float bottom);
float DISP_GetWidthEx(void);
float DISP_GetHeightEx(void);

float DISP_GetMinX(void);
float DISP_GetMaxX(void);
float DISP_GetMinY(void);
float DISP_GetMaxY(void);

void DISP_DrawLineEx(float x0, float y0, float x1, float y1, uint32_t color);
void DISP_DrawHLineEx(float x0, float y0, float w, uint32_t color);

void DISP_DrawRectEx(float x, float y, float w, float h, uint32_t color);
void DISP_FillRectEx(float x, float y, float w, float h, uint32_t color);
void DISP_DrawCircleEx(float centerX, float centerY, float radius, uint32_t colorSet);
void DISP_FillCircleEx(float x0, float y0, float radius, uint32_t color);

void DISP_FillTriangleEx(float x0, float y0, float x1, float y1, float x2, float y2, uint32_t color);

void DISP_TextForeColor(uint32_t color);
void DISP_TextBackColor(uint32_t color);

uint32_t DISP_GetTextForeColor(void);
uint32_t DISP_GetTextBackColor(void);

void DISP_WriteCharXY(int xPix, int yPix, char c);
void DISP_WriteChar(char c);
void DISP_WriteString(char* cp);
void DISP_GotoXY(int xPix, int yPix);
int DISP_GetCursorX(void);
int DISP_GetCursorY(void);
void DISP_GotoColRow(int col, int row);
bool DISP_Clear(void);
bool DISP_Fill(uint32_t color);

bool DISP_Refresh(void);

typedef enum {
  font_unknown = -1,
  font_atari_8x8,
  font_lcd_5x7,
  /* font_thin_8x8, font_system_5x7, font_system_5x7p, font_system_5x7pr,
  font_wendy_3x5, font_newbasic_3x6 */
} eFontsAvailable;

bool DISP_SetFont(eFontsAvailable fnt);
bool DISP_SetFontMultiply(int m);
int DISP_GetCharPerRow(void);
int DISP_GetRowPerDisp(void);
int DISP_GetFontWidth(void);
int DISP_GetFontHeight(void);

void DISP_DrawSpriteMono8(int x, int y, uint8_t *data, int rows, int zoom, uint32_t foreColor);
bool DISP_DrawBitmap(void* pData, int xPos0, int yPos, int w, int h);
bool DISP_DrawBitmapTrans(void* pData, int xPos, int yPos, int w, int h, uint32_t tColor);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
// uint16 ... gggb bbbb . rrrr rggg
//TODO !!! #define RGB565(r,g,b)  ((((r) & 0x001f) << 11) | (((g) & 0x3f) << 5) | ((b) & 0x1f))
#define RGB888(r,g,b)  (((((r) >> 3) & 0x001fu) << ((uint16_t)3)) \
      | ((((g) >> 5) & 0x07u) << ((uint16_t)0)) \
      | (((g) & 0x07u) << ((uint16_t)13)) \
      | (((b) >> 3) & 0x1fu) << ((uint16_t)8))
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
// uint16 ... rrrr rggg . gggb bbbb
//#define RGB565(r,g,b)  ((((r) & 0x001f) << 11) | (((g) & 0x3f) << 5) | ((b) & 0x1f))
#define RGB888(r,g,b)  (((((r) >> 3) & 0x001f) << 11) | ((((g) >> 2) & 0x3f) << 5) | (((b) >> 3) & 0x1f))
#else
#error Endianity not set !!
#endif

#define TFT16BIT_BLACK       RGB888(   0,   0,   0 )
#define TFT16BIT_NAVY        RGB888(   0,   0, 128 )
#define TFT16BIT_DARKGREEN   RGB888(   0, 128,   0 )
#define TFT16BIT_DARKCYAN    RGB888(   0, 128, 128 )
#define TFT16BIT_MAROON      RGB888( 128,   0,   0 )
#define TFT16BIT_PURPLE      RGB888( 128,   0, 128 )
#define TFT16BIT_OLIVE       RGB888( 128, 128,   0 )
#define TFT16BIT_LIGHTGREY   RGB888( 192, 192, 192 )
#define TFT16BIT_DARKGREY    RGB888( 128, 128, 128 )
#define TFT16BIT_BLUE        RGB888(   0,   0, 255 )
#define TFT16BIT_GREEN       RGB888(   0, 255,   0 )
#define TFT16BIT_CYAN        RGB888(   0, 255, 255 )
#define TFT16BIT_RED         RGB888( 255,   0,   0 )
#define TFT16BIT_MAGENTA     RGB888( 255,   0, 255 )
#define TFT16BIT_YELLOW      RGB888( 255, 255,   0 )
#define TFT16BIT_WHITE       RGB888( 255, 255, 255 )
#define TFT16BIT_ORANGE      RGB888( 255, 165,   0 )
#define TFT16BIT_GREENYELLOW RGB888( 173, 255,  47 )
#define TFT16BIT_DARKESTGRAY RGB888(  32,  32,  32 ) 

#endif  // _DISP_LIBRARY_H
