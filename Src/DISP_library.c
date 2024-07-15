#include "DISP_library_driver.h"
#include "DISP_library.h"

// https://www.oracle.com/technical-resources/articles/it-infrastructure/mixing-c-and-cplusplus.html
#ifdef __cplusplus
extern "C" {
#endif

// static void (*my_DrawPixel)(int x, int y, uint32_t color) = NULL;
static ptrDrawPixelFn_t _DrawPixel = NULL;
static ptrGetPixelFn_t _GetPixel = NULL;
static ptrRefreshFn_t _RefreshViewport = NULL;

static int _cursor_y = 0;
static int _cursor_x = 0;

static int _pixelsWidth = 0, _pixelsHeight = 0;
static DISP_pixel_format _pixFormat = pixel_format_1bpp;

DISP_Description DISP_library_CreateDescr(void)
{
  DISP_Description data;

  memset(&data, 0, sizeof(data));
  return data;
}

bool DISP_library_Init(DISP_Description *pDescr)
{
  _pixelsWidth = pDescr->resX;
  _pixelsHeight = pDescr->resY;
  _pixFormat = pDescr->pixFormat;

  _DrawPixel = pDescr->fnDrawPixel;
  _GetPixel = pDescr->fnGetPixel;
  _RefreshViewport = pDescr->fnRefresh;

  //TODO store as structure

  return true;
}

#ifdef __GNUC__
__inline
#endif
int DISP_GetWidth(void) { return _pixelsWidth; }
#ifdef __GNUC__
__inline
#endif
int DISP_GetHeight(void) { return _pixelsHeight; }

bool DISP_Refresh(void)
{
  if (_RefreshViewport != NULL)
    _RefreshViewport();
  //HACL pokud je NULL, tak se o refresh zrejme stara automatika - Timer + DMA

  return true;
}

//! new wonring style - X/Y in pixels
static int textX = 0, textY = 0;
static bool autoScroll = true;

#include "font_atari_8x8.h"
#include "font_lcd_5x7.h"

static uint8_t* bpFontBase = NULL;
static FONT_Header_t* ptrFontHeader = NULL;
static bool bbPixSpace = false;
static bool bbLineSpace = false;
static int iFontMultiply = 1;

bool DISP_SetFont(eFontsAvailable fnt)
{
  bbPixSpace = false;
  bbLineSpace = false;
  switch (fnt)
  {
    case font_atari_8x8:
    default:
      ptrFontHeader = (FONT_Header_t*)font_atari_8x8_data;

      //TODO select start of char defines, defaultne ZA hlavickou
      bpFontBase = (uint8_t*)font_atari_8x8_data + sizeof(FONT_Header_t);
      break;
    case font_lcd_5x7:
      ptrFontHeader = (FONT_Header_t*)font_lcd_5x7_data;
      bpFontBase = (uint8_t*)font_lcd_5x7_data + sizeof(FONT_Header_t);     // data za hlavickou
      bbPixSpace = true;
      break;
  }

  return true;
}

#ifdef __GNUC__
__inline
#endif
int DISP_GetCharPerRow(void)
{
  return _pixelsWidth / (ptrFontHeader->width + (bbPixSpace ? 1 : 0));   // add 1 pixel space
}

#ifdef __GNUC__
__inline
#endif
int DISP_GetRowPerDisp(void)
{
  return _pixelsHeight / (ptrFontHeader->height + (bbLineSpace ? 1 : 0));
}

#ifdef __GNUC__
__inline
#endif
int DISP_GetFontWidth(void)
{
//  return ptrFontHeader->width + (bbPixSpace ? 1 : 0);   // add 1 pixel space
  return iFontMultiply * (ptrFontHeader->width + (bbPixSpace ? 1 : 0));   // add 1 pixel space
}

#ifdef __GNUC__
__inline
#endif
int DISP_GetFontHeight(void)
{
//  return ptrFontHeader->height + (bbLineSpace ? 1 : 0);
  return iFontMultiply * (ptrFontHeader->height + (bbLineSpace ? 1 : 0));
}

void DISP_GotoXY(int xPix, int yPix)
{
  textX = xPix;
  textY = yPix;
}

void DISP_GotoColRow(int col, int row)
{
  textX = col * DISP_GetFontWidth();
  textY = row * DISP_GetFontHeight();
}

bool DISP_SetFontMultiply(int m)
{
  switch(m)
  {
    case 1:
    case 2:
    case 4:
    case 8:
      iFontMultiply = m;
      return true;
    default:
      return false;
  }
}

void DISP_WriteChar(char c)
{
  DISP_WriteCharXY(textX, textY, c);

  textX += DISP_GetFontWidth();
  if (textX >= DISP_GetWidth())
  {
    textX = 0;

    textY += DISP_GetFontHeight();
    if (textY >= DISP_GetHeight())
    {
      /* zatim nescrolujeme ?
      if (autoScroll && (my_ScrollText != NULL))
      {
        my_ScrollText(1);       // positive = up
        textY--;
        for (int i = DISP_GetCharPerRow() * 8 - 1; i >= 0; i--) // abych nemusel vzdy testovat mez
          DISP_WriteCharXY(i, textY, ' ');
      }
      else
      */
        textY = 0;      // start from disp beginnig
    }
  }
}

void DISP_WriteString(char* cp)
{
  while (*cp)
  {
    DISP_WriteChar(*cp);
    cp++;
  }
}

static uint32_t _textForeColor = 0xffffffff;    // white
static uint32_t _textBackColor = 0x00000000;    // black

void DISP_TextForeColor(uint32_t color)
{
  _textForeColor = color;
}

void DISP_TextBackColor(uint32_t color)
{
  _textBackColor = color;
}

uint32_t DISP_GetTextForeColor(void)
{
  return _textForeColor;
}

uint32_t DISP_GetTextBackColor(void)
{
  return _textBackColor;
}

//! X a Y jsou sloupce a radky v pocitani znaku !!
void DISP_WriteCharXY(int xPix, int yPix, char c)
{
  if (_DrawPixel == NULL)
    return;       //TODO emit FAIL

  int i, j, ii, jj;
  uint8_t* bp;

  if (ptrFontHeader == NULL)
    DISP_SetFont(font_unknown);     // use default font

  if ((c >= (ptrFontHeader->first + ptrFontHeader->count)) || (c < ptrFontHeader->first))
    return;

  c -= ptrFontHeader->first;
  bp = bpFontBase + (c * ptrFontHeader->width);

#if 0       // bez multiply a jen pro 8x8 fonty
  for (i = 0; i < ptrFontHeader->width; i++)
  {
    uint8_t b = bp[i];
    if (ptrFontHeader->height <= 8)
    {
      for (j = 0; j < ptrFontHeader->height; j++)
      {
        _DrawPixel(xPix + i, yPix + j, (b & 1) ? _textForeColor : _textBackColor);  // LSB first
        b >>= 1;
      }

      if (bbLineSpace)
        _DrawPixel(xPix + i, yPix + j, _textBackColor);          // j je pix na radku "pod"

      continue;     //? proc ??
    }
  }

  if (bbPixSpace)
  {
    if (ptrFontHeader->height <= 8)
    {
      for (j = 0; j < ptrFontHeader->height; j++)
        _DrawPixel(xPix + i, yPix + j, _textBackColor);

      if (bbLineSpace)
        _DrawPixel(xPix + i, yPix + j, _textBackColor);          // j je pix na radku "pod"
    }
  }
#else
  if (ptrFontHeader->height > 8)
    return;                     // zatim neumime

  int xr = xPix, yr = yPix;
  uint8_t  b;
  for (i = 0; i < ptrFontHeader->width; i++)
  {
    for(ii = 0; ii < iFontMultiply; ii++)
    {
      b = bp[i];          // bajt dle generatoru pro 8x8 fonty
      yr = yPix;

      for (j = 0; j < ptrFontHeader->height; j++)
      {
        for (jj = 0; jj < iFontMultiply; jj++)
        {
          _DrawPixel(xr, yr, (b & 1) ? _textForeColor : _textBackColor);    // LSB first
          yr++;
        }

        b >>= 1;
      }

      if (bbLineSpace)
      {
        for (jj = 0; jj < iFontMultiply; jj++)
        {
          _DrawPixel(xr, yr, _textBackColor); // j je pix na radku "pod"
          yr++;
        }
      }

      xr++;
    }
  }

  if (bbPixSpace)
  {
    // zustava posledni XR
    for (ii = 0; ii < iFontMultiply; ii++)
    {
      yr = yPix;

      for (j = 0; j < ptrFontHeader->height; j++)
      {
        for (jj = 0; jj < iFontMultiply; jj++)
        {
          _DrawPixel(xr, yr, _textBackColor);
          yr++;
        }

        if (bbLineSpace)
        {
          for (jj = 0; jj < iFontMultiply; jj++)
          {
            _DrawPixel(xr, yr, _textBackColor); // j je pix na radku "pod"
            yr++;
          }
        }
      }

      xr++;
    }
  }
#endif
}

bool DISP_Clear()
{
  return DISP_Fill(0);                // black
}

bool DISP_Fill(uint32_t color)
{
  for (int r = 0; r < _pixelsHeight; r++)
  {
    for (int c = 0; c < _pixelsWidth; c++)
      _DrawPixel(c, r, color);
  }

  return true;
}

void DISP_DrawPixel(int x, int y, uint32_t color)
{
  _DrawPixel(x, y, color);
}

uint32_t DISP_GetPixel(int x, int y)
{
  return _GetPixel(x, y);
}

void DISP_DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
  int dx = (x0 < x1) ? (x1 - x0) : (x0 - x1), sx = (x0 < x1) ? 1 : -1;
  int dy = (y0 < y1) ? (y1 - y0) : (y0 - y1), sy = (y0 < y1) ? 1 : -1;
  int err = ((dx > dy) ? dx : -dy) / 2, e2;

  for (; ; )
  {
    if ((x0 == x1) && (y0 == y1))
      break;

    _DrawPixel(x0, y0, color);

    e2 = err;
    if (e2 > -dx)
    {
      err -= dy; x0 += sx;
    }

    if (e2 < dy)
    {
      err += dx; y0 += sy;
    }
  }
}

void DISP_DrawHLine(int x0, int y0, int w, uint32_t color)
{
  //TODO swap if x0 > x1
  for (; w; w--, x0++)        //!! fujky C-cko
    _DrawPixel(x0, y0, color);
}

void DISP_DrawVLine(int x0, int y0, int h, uint32_t color)
{
  for (; h; h--, y0++)        //!! fujky C-cko
    _DrawPixel(x0, y0, color);
}

void DISP_DrawRect(int x, int y, int w, int h, uint32_t color)
{
  DISP_DrawLine(x, y, x + w, y, color);
  DISP_DrawLine(x + w, y, x + w, y + h, color);
  DISP_DrawLine(x + w, y + h, x, y + h, color);
  DISP_DrawLine(x, y + h, x, y, color);
}

void DISP_FillRect(int x, int y, int w, int h, uint32_t color)
{
  int ww = w, xx = x;

  for (; h; h--)
  {
    x = xx;

    for (w = ww; w; w--)
    {
      _DrawPixel(x, y, color);
      x++;
    }

    y++;
  }
}

void DISP_DrawCircle(int centerX, int centerY, int radius, uint32_t color)
{
  int d = (5 - radius * 4) / 4;
  int x = 0;
  int y = radius;

  do
  {
    // ensure index is in range before setting (depends on your image implementation)
    // in this case we check if the pixel location is within the bounds of the image before setting the pixel
    if (((centerX + x) >= 0) && ((centerX + x) <= (_pixelsWidth - 1)) && ((centerY + y) >= 0) && ((centerY + y) <= (_pixelsHeight - 1))) _DrawPixel(centerX + x, centerY + y, color);
    if (((centerX + x) >= 0) && ((centerX + x) <= (_pixelsWidth - 1)) && ((centerY - y) >= 0) && ((centerY - y) <= (_pixelsHeight - 1))) _DrawPixel(centerX + x, centerY - y, color);
    if (((centerX - x) >= 0) && ((centerX - x) <= (_pixelsWidth - 1)) && ((centerY + y) >= 0) && ((centerY + y) <= (_pixelsHeight - 1))) _DrawPixel(centerX - x, centerY + y, color);
    if (((centerX - x) >= 0) && ((centerX - x) <= (_pixelsWidth - 1)) && ((centerY - y) >= 0) && ((centerY - y) <= (_pixelsHeight - 1))) _DrawPixel(centerX - x, centerY - y, color);
    if (((centerX + y) >= 0) && ((centerX + y) <= (_pixelsWidth - 1)) && ((centerY + x) >= 0) && ((centerY + x) <= (_pixelsHeight - 1))) _DrawPixel(centerX + y, centerY + x, color);
    if (((centerX + y) >= 0) && ((centerX + y) <= (_pixelsWidth - 1)) && ((centerY - x) >= 0) && ((centerY - x) <= (_pixelsHeight - 1))) _DrawPixel(centerX + y, centerY - x, color);
    if (((centerX - y) >= 0) && ((centerX - y) <= (_pixelsWidth - 1)) && ((centerY + x) >= 0) && ((centerY + x) <= (_pixelsHeight - 1))) _DrawPixel(centerX - y, centerY + x, color);
    if (((centerX - y) >= 0) && ((centerX - y) <= (_pixelsWidth - 1)) && ((centerY - x) >= 0) && ((centerY - x) <= (_pixelsHeight - 1))) _DrawPixel(centerX - y, centerY - x, color);
    if (d < 0)
    {
      d += 2 * x + 1;
    }
    else
    {
      d += 2 * (x - y) + 1;
      y--;
    }
    x++;
  } while (x <= y);
}

void DISP_FillCircle(int x0, int y0, int radius, uint32_t color)
{
  int f = 1 - radius;
  int ddF_x = 0;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = (int)radius;
  int t1, t2;

  _DrawPixel(x0, y0 + radius, color);
  t1 = y0 - radius;
  _DrawPixel(x0, (t1 > 0) ? t1 : 0, color);
  t1 = x0 - radius;
  DISP_DrawLine(x0 + radius, y0, (t1 > 0) ? t1 : 0, y0, color);

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x + 1;

    t1 = x0 - x;
    t2 = y0 - y;
    DISP_DrawLine((t1 > 0) ? t1 : 0, y0 + y, x0 + x, y0 + y, color);
    DISP_DrawLine((t1 > 0) ? t1 : 0, (t2 > 0) ? t2 : 0, x0 + x, (t2 > 0) ? t2 : 0, color);
    t1 = x0 - y;
    t2 = y0 - x;
    DISP_DrawLine((t1 > 0) ? t1 : 0, y0 + x, x0 + y, y0 + x, color);
    DISP_DrawLine((t1 > 0) ? t1 : 0, (t2 > 0) ? t2 : 0, x0 + y, (t2 > 0) ? t2 : 0, color);
  }
}

static void _swap16(int16_t* a, int16_t* b)
{
  register int16_t c = *a;
  *a = *b;
  *b = c;
}

static void _swap_int(int* a, int* b)
{
  register int16_t c = *a;
  *a = *b;
  *b = c;
}

void DISP_FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
  int a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    _swap_int(&y0, &y1);
    _swap_int(&x0, &x1);
  }
  if (y1 > y2)
  {
    _swap_int(&y2, &y1);
    _swap_int(&x2, &x1);
  }
  if (y0 > y1)
  {
    _swap_int(&y0, &y1);
    _swap_int(&x0, &x1);
  }

  if (y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else
      if (x1 > b)
        b = x1;
    if (x2 < a)
      a = x2;
    else
      if (x2 > b)
        b = x2;
    DISP_DrawHLine(a, y0, b - a + 1, color);
    return;
  }

  int dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 =
    x2 - x1, dy12 = y2 - y1, sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1;   // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++)
  {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if (a > b)
      _swap_int(&a, &b);
    DISP_DrawHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++)
  {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if (a > b)
      _swap_int(&a, &b);
    DISP_DrawHLine(a, y, b - a + 1, color);
  }
}

#include <math.h>

static float _viewPortMinX = 0, _viewPortMaxX = 0, _viewPortMinY = 0, _viewPortMaxY = 0;
static float _viewKoefX = 0, _viewKoefY = 0;
static bool _viewSwapX = false, _viewSwapY = false;

bool DISP_SetViewPort(float left, float right, float top, float bottom)
{
  _viewPortMinX = fminf(left, right);
  _viewPortMaxX = fmaxf(left, right);
  _viewSwapX = left > right;          //! prirozene zleva-doprava

  _viewPortMinY = fminf(top, bottom);
  _viewPortMaxY = fmaxf(top, bottom);
  _viewSwapX = top > bottom;          //! pozor - viewPort ma max nahore, ale zobrazeni roste dolu

  _viewKoefX = DISP_GetWidth() / DISP_GetWidthEx();
  if (_viewSwapX)
    _viewKoefX *= -1;
  _viewKoefY = DISP_GetHeight() / DISP_GetHeightEx();
  if (_viewSwapY)
    _viewKoefY *= -1;

  return true;
}

float DISP_GetWidthEx(void)
{
  return _viewPortMaxX - _viewPortMinX;
}
float DISP_GetHeightEx(void)
{
  return _viewPortMaxY - _viewPortMinY;
}

float DISP_GetMinX(void) { return ((_viewPortMinX == 0) && (_viewPortMaxX == 0)) ? 0 : _viewPortMinX; }
float DISP_GetMaxX(void) { return ((_viewPortMinX == 0) && (_viewPortMaxX == 0)) ? _pixelsWidth : _viewPortMaxX; }
float DISP_GetMinY(void) { return ((_viewPortMinY == 0) && (_viewPortMaxY == 0)) ? 0 : _viewPortMinY; }
float DISP_GetMaxY(void) { return ((_viewPortMinY == 0) && (_viewPortMaxY == 0)) ? _pixelsHeight : _viewPortMaxY; }

void DISP_DrawLineEx(float x0, float y0, float x1, float y1, uint32_t color)
{
  DISP_DrawLine(
    (int)((x0 - _viewPortMinX) * _viewKoefX), (int)((y0 - _viewPortMinY) * _viewKoefY),
    (int)((x1 - _viewPortMinX) * _viewKoefX), (int)((y1 - _viewPortMinY) * _viewKoefY),
    color);
}
void DISP_DrawHLineEx(float x0, float y0, float w, uint32_t color)
{
  DISP_DrawHLine(
    (int)((x0 - _viewPortMinX) * _viewKoefX), (int)((y0 - _viewPortMinY) * _viewKoefY),
    (int)(w * fabsf(_viewKoefX)), color);
}

void DISP_DrawRectEx(float x, float y, float w, float h, uint32_t color)
{
  DISP_DrawRect(
    (int)((x - _viewPortMinX) * _viewKoefX), (int)((y - _viewPortMinY) * _viewKoefY),
    (int)(w * fabs(_viewKoefX)), (int)(h * fabs(_viewKoefY)), color);
}

void DISP_FillRectEx(float x, float y, float w, float h, uint32_t color)
{
  DISP_FillRect(
    (int)((x - _viewPortMinX) * _viewKoefX), (int)((y - _viewPortMinY) * _viewKoefY),
    (int)(w * fabs(_viewKoefX)), (int)(h * fabs(_viewKoefY)), color);
}

void DISP_DrawCircleEx(float centerX, float centerY, float radius, uint32_t colorSet)
{
  DISP_DrawCircle(
    (int)((centerX - _viewPortMinX) * _viewKoefX), (int)((centerY - _viewPortMinY) * _viewKoefY),
    (int)(radius * fabs(_viewKoefX)), colorSet);    //! koef pro R se bere z X !!
}

void DISP_FillCircleEx(float x0, float y0, float radius, uint32_t color)
{
  DISP_FillCircle(
    (int)((x0 - _viewPortMinX) * _viewKoefX), (int)((y0 - _viewPortMinY) * _viewKoefY),
    (int)(radius * fabs(_viewKoefX)), color);
}

void DISP_FillTriangleEx(float x0, float y0, float x1, float y1, float x2, float y2, uint32_t color)
{
  DISP_FillTriangle(
    (int)((x0 - _viewPortMinX) * _viewKoefX), (int)((y0 - _viewPortMinY) * _viewKoefY),
    (int)((x1 - _viewPortMinX) * _viewKoefX), (int)((y1 - _viewPortMinY) * _viewKoefY),
    (int)((x2 - _viewPortMinX) * _viewKoefX), (int)((y2 - _viewPortMinY) * _viewKoefY),
    color);
}

void DISP_DrawSpriteMono8(int x, int y, uint8_t *data, int rows, int zoom, uint32_t foreColor)
{
//   DISP_FillRect(x, y, 8, rows, foreColor);
  if (data == NULL)
    return;

  int yy = y;
  for (int r = 0; r < rows; r++)
  {
    for (int zr = 0; zr < zoom; zr++)
    {
      int xx = x;
      uint8_t m = 0x80;
      for (int c = 0; c < 8; c++)
      {
        for (int zc = 0; zc < zoom; zc++)
        {
          if (data[r] & m)
            DISP_DrawPixel(xx, yy, foreColor);

          xx++;
        }

        m >>= 1;
      }

      yy++;
    }
  }
}

bool DISP_DrawBitmap(void* pData, int xPos, int yPos, int w, int h)
{
  //! add format information !!
  // switch(pixelFormat) ...
  // return false if unknown format or invalid parameters

  uint16_t* pixData = (uint16_t*)pData;

  for (int y = 0; y < h; y++)
  {
    //TODO use block (line) copy hw-dependent function
    for (int x = 0; x < w; x++)
    {
      uint16_t color = pixData[y * w + x];

      DISP_DrawPixel(xPos + x, yPos + y, color);
    }
  }

  return true;
}

bool DISP_DrawBitmapTrans(void* pData, int xPos, int yPos, int w, int h, uint32_t tColor)
{
  //! same as non-trans bitmap

  uint16_t* pixData = (uint16_t*)pData;
  uint16_t xtColor = (uint16_t)tColor;

  for (int y = 0; y < h; y++)
  {
    //TODO use block (line) copy hw-dependent function with transparency
    for (int x = 0; x < w; x++)
    {
      uint16_t color = pixData[y * w + x];
      if (color != xtColor)
        DISP_DrawPixel(xPos + x, yPos + y, color);
    }
  }

  return true;
}

#ifdef __cplusplus
}
#endif
