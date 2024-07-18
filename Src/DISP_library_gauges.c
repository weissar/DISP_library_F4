#include "DISP_library_gauges.h"

#define _USE_MATH_DEFINES     //TRICK for MSVC - required for includein M_PI etc. math-constants
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//! pozor, pro Visual Studio je vypnut Warning C4244 a C4267
//! File - Properties - C/C++ - Advanced - Disable Specific Warnings
//! warn - "ztratova konverze float -> int"

// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
uint16_t rainbow16(uint8_t value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  uint8_t red = 0; // Red is the top 5 bits of a 16 bit colour value
  uint8_t green = 0; // Green is the middle 6 bits
  uint8_t blue = 0; // Blue is the bottom 5 bits

  switch (value / 32)  // quadrant
  {
    case 0:
      blue = 31;
      green = 2 * (value % 32);
      red = 0;
      break;
    case 1:
      blue = 31 - (value % 32);
      green = 63;
      red = 0;
      break;
    case 2:
      blue = 0;
      green = 63;
      red = value % 32;
      break;
    case 3:
      blue = 0;
      green = 63 - 2 * (value % 32);
      red = 31;
      break;
  }

  uint16_t color = (red << 11) + (green << 5) + blue;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#ifdef _MSC_VER
  return ((color & 0xff) << 8) | ((color >> 8) & 0xff);   // byte swap .... brrrr
#else
  return  __REVSH(color);
#endif
#else
  return color;
#endif
}

// https://www.arduino.cc/reference/en/language/functions/math/map/
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// zajimave triky s kompilatorem, porovnat s 0.0174532925f
// on to uklada jako double i kdyz konstantu pocita kompilator
// zajimavejsi, ze to snad je little-endian ?? - check https://gregstoll.com/~gregstoll/floattohex/
static const float _pi_180 = (float)M_PI / 180;
//! pozor, v projektu musi byt nastaveno _USE_MATH_DEFINES

uint16_t ringBackground = RGB888(16, 16, 16);

// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
int ringMeter(int value, int vmin, int vmax, int x, int y, int r, char *units,
    uint8_t scheme, bool withgap)
{
  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option

  x += r;
  y += r;   // Calculate coords of centre of ring
  int w = r / 4;    // Width of outer ring is 1/4 of radius
  int angle = 150;  // Half the sweep angle of meter (300 degrees)
  int text_colour = 0; // To hold the text colour

  if (value > vmax)
    value = vmax;
  if (value < vmin)
    value = vmin;

  int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v

  uint8_t seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
  uint8_t inc = withgap ? 10 : seg; // Draw segments every 5 degrees, increase to 10 for segmented ring

  // Draw colour blocks every inc degrees
  for (int i = -angle; i < angle; i += inc)
  {
    // Choose colour from scheme
    int colour = 0;
    switch (scheme)
    {
      case GAUGE_RED2RED:
        colour = TFT16BIT_RED;
        break; // Fixed colour
      case GAUGE_GREEN2GREEN:
        colour = TFT16BIT_GREEN;
        break; // Fixed colour
      case GAUGE_BLUE2BLUE:
        colour = TFT16BIT_BLUE;
        break; // Fixed colour
      case GAUGE_BLUE2RED:
        colour = rainbow16((uint8_t)map(i, -angle, angle, 0, 127));
        break; // Full spectrum blue to red
      case GAUGE_GREEN2RED:
        colour = rainbow16((uint8_t)map(i, -angle, angle, 63, 127));
        break; // Green to red (high temperature etc)
      case GAUGE_RED2GREEN:
        colour = rainbow16((uint8_t)map(i, -angle, angle, 127, 63));
        break; // Red to green (low battery etc)
      default:
        colour = TFT16BIT_BLUE;
        break; // Fixed colour
    }

    // Calculate pair of coordinates for segment start
    float sx = (float)cos((i - 90) * _pi_180);
    float sy = (float)sin((i - 90) * _pi_180);
    uint16_t x0 = (uint16_t)(sx * (r - w) + x);
    uint16_t y0 = (uint16_t)(sy * (r - w) + y);
    uint16_t x1 = (uint16_t)(sx * r + x);
    uint16_t y1 = (uint16_t)(sy * r + y);

    // Calculate pair of coordinates for segment end
    float sx2 = (float)cos((i + seg - 90) * _pi_180);
    float sy2 = (float)sin((i + seg - 90) * _pi_180);
    int x2 = (int)(sx2 * (r - w) + x);
    int y2 = (int)(sy2 * (r - w) + y);
    int x3 = (int)(sx2 * r + x);
    int y3 = (int)(sy2 * r + y);

    if (i < v)
    { // Fill in coloured segments with 2 triangles
      DISP_FillTriangle(x0, y0, x1, y1, x2, y2, colour);
      DISP_FillTriangle(x1, y1, x2, y2, x3, y3, colour);
      text_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      DISP_FillTriangle(x0, y0, x1, y1, x2, y2, ringBackground);
      DISP_FillTriangle(x1, y1, x2, y2, x3, y3, ringBackground);
    }
  }

  // Set the text colour to default
  DISP_TextForeColor(text_colour);
  DISP_TextBackColor(ringBackground);

  int hPix = DISP_GetFontHeight();
  int yOff = y - hPix;

  char buf[10];
#if 0
  // varianta fujky na zjisteni max. hodnoty
  sprintf(buf, "%d", vmax);

  int lenPix = strlen(buf) * DISP_GetFontWidth();
#else
  int lenPix = 0;
  for(int dummy = vmax; dummy > 0; dummy /= 10)
    lenPix++;

  lenPix *= DISP_GetFontWidth();
#endif

  int xOff = x - lenPix / 2;
  DISP_FillRect(xOff, yOff, lenPix, hPix, ringBackground);

#ifdef _MSC_VER
  sprintf_s(buf, sizeof(buf) / sizeof(buf[0]), "%d", value);
  //! cannot use #pragma warning disable: 4996 - this is ERROR !!
  //! Error	C4996	'sprintf' : This function or variable may be unsafe.
  //! or set _CRT_SECURE_NO_WARNINGS
#else
  sprintf(buf, "%d", value);
#endif


  lenPix = (int)strlen(buf) * DISP_GetFontWidth();
  xOff = x - lenPix / 2;

  DISP_GotoXY(xOff, yOff);
  DISP_WriteString(buf);

  if ((units != NULL) && (units[0] != 0))
  {
    lenPix = (int)strlen(units) * DISP_GetFontWidth();
    xOff = x - lenPix / 2;

    // asi zbytecne, nemeni se ... DISP_FillRect(xOff, y + 2, lenPix, hPix, ringBackground);
    DISP_GotoXY(xOff, y + 2);
    DISP_WriteString(units);
  }

  return x + r;   // Calculate and return right hand side x coordinate
}

bool DrawGauge_RingMeter(GaugeRingMeter *grm, int value)
{
  if (grm == NULL)
    return false;

  grm->_curVal = value;

  int x = grm->x;
  int y = grm->y;
  int r = grm->r;

  x += r;
  y += r;          // coords of centre of ring
  int w = r / 4;   // Width of outer ring is 1/4 of radius
  int angleStart = (grm->angleStart == grm->angleEnd) ? -150 : grm->angleStart;
  int angleEnd = (grm->angleStart == grm->angleEnd) ? 150 : grm->angleEnd;
  uint16_t val_colour = 0;  // To hold the last colour for text

  if (value > grm->valMax)
    value = grm->valMax;
  if (value < grm->valMin)
    value = grm->valMin;

  int v = map(value, grm->valMin, grm->valMax, angleStart, angleEnd);

  uint8_t seg = (grm->segDegWidth == 0) ? 5 : grm->segDegWidth;
  uint8_t inc = (grm->segDegStep == 0) ? seg : grm->segDegStep;

  for (int i = angleStart; i < angleEnd; i += inc)
  {
    int colour = 0;
    switch (grm->scheme)
    {
      case GAUGE_RED2RED: colour = TFT16BIT_RED; break;       // Fixed colour
      case GAUGE_GREEN2GREEN: colour = TFT16BIT_GREEN; break; // Fixed colour
      case GAUGE_BLUE2BLUE: colour = TFT16BIT_BLUE; break;    // Fixed colour
      case GAUGE_BLUE2RED: colour = rainbow16((uint8_t)map(i, angleStart, angleEnd, 0, 127)); break; // Full spectrum blue to red
      case GAUGE_GREEN2RED: colour = rainbow16((uint8_t)map(i, angleStart, angleEnd, 63, 127)); break; // Green to red (high temperature etc)
      case GAUGE_RED2GREEN: colour = rainbow16((uint8_t)map(i, angleStart, angleEnd, 127, 63)); break; // Red to green (low battery etc)
      default: colour = TFT16BIT_BLUE;break; // Fixed colour
    }

    // Calculate pair of coordinates for segment start
    float sx = (float)cos((i - 90) * _pi_180);
    float sy = (float)sin((i - 90) * _pi_180);
    uint16_t x0 = (uint16_t)(sx * (r - w) + x);
    uint16_t y0 = (uint16_t)(sy * (r - w) + y);
    uint16_t x1 = (uint16_t)(sx * r + x);
    uint16_t y1 = (uint16_t)(sy * r + y);

    // Calculate pair of coordinates for segment end
    float sx2 = (float)cos((i + seg - 90) * _pi_180);
    float sy2 = (float)sin((i + seg - 90) * _pi_180);
    int x2 = (int)(sx2 * (r - w) + x);
    int y2 = (int)(sy2 * (r - w) + y);
    int x3 = (int)(sx2 * r + x);
    int y3 = (int)(sy2 * r + y);

    if (i < v)
    { // Fill in coloured segments with 2 triangles
      DISP_FillTriangle(x0, y0, x1, y1, x2, y2, colour);
      DISP_FillTriangle(x1, y1, x2, y2, x3, y3, colour);
      val_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      DISP_FillTriangle(x0, y0, x1, y1, x2, y2, grm->ringBack);
      DISP_FillTriangle(x1, y1, x2, y2, x3, y3, grm->ringBack);
    }
  }

  // Set the text colour to default
  DISP_TextForeColor(grm->textForeAuto ? val_colour : grm->textFore);
  DISP_TextBackColor(grm->textBack);

  int hPix = DISP_GetFontHeight();
  int yOff = y - hPix;

  char buf[10];
  int lenPix = 0;
  for(int dummy = grm->valMax; dummy > 0; dummy /= 10)
    lenPix++;

  lenPix *= DISP_GetFontWidth();
  int xOff = x - lenPix / 2;
  DISP_FillRect(xOff, yOff, lenPix, hPix, ringBackground);

  //! POSIX VS error ... itoa(value, buf, 10);             

#ifdef _MSC_VER
  sprintf_s(buf, sizeof(buf) / sizeof(buf[0]), "%d", value);
#else
  sprintf(buf, "%d", value);
#endif

  lenPix = (int)strlen(buf) * DISP_GetFontWidth();
  xOff = x - lenPix / 2;

  DISP_GotoXY(xOff, yOff);
  DISP_WriteString(buf);

  if ((grm->units != NULL) && (grm->units[0] != 0))
  {
    lenPix = (int)strlen(grm->units) * DISP_GetFontWidth();   //? strlen returns size_t ??
    xOff = x - lenPix / 2;

    // asi zbytecne, nemeni se ... DISP_FillRect(xOff, y + 2, lenPix, hPix, ringBackground);
    DISP_GotoXY(xOff, y + 2);
    DISP_WriteString(grm->units);
  }

  return true;
}

#ifdef __cplusplus
}
#endif
