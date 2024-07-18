#include "DISP_library_lcd_num.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *     a            0
 *   f   b        5   1
 *     g            6
 *   e   c        4   2
 *     d            3
 */

static uint8_t segments[10] = 
{
  0x3f,   // 0 = 0011 1111
  0x06,   // 1 = 0000 0110
  0x5b,   // 2 = 0101 1011
  0x4f,   // 3 = 0100 1111
  0x66,   // 4 = 0110 0110
  0x6d,   // 5 = 0110 1101
  0x7d,   // 6 = 0111 1101
  0x07,   // 7 = 0000 0111
  0x7f,   // 8 = 0111 1111
  0x6f    // 9 = 0110 1111
};

static int _segmentFat_Pix = 20;
static int _segmentGap_Pix = 3;

void DISP_LCD_SetFat(int fatPixels)
{
  _segmentFat_Pix = fatPixels;
}

void DISP_LCD_SetGap(int gapPixels)
{
  _segmentGap_Pix = gapPixels;
}

bool DISP_LCD_ShowNum(int val, int x, int y, int w, int h)
{
  if (val >= sizeof(segments))
    return false;

  DISP_point_t pFA = { x, y };
  DISP_point_t pAB = { x + w - 1, y };
  DISP_point_t pBC = { x + w - 1, y + h / 2 - 1 };  // eg G right
  DISP_point_t pCD = { x + w - 1, y + h - 1 };
  DISP_point_t pDE = { x, y + h - 1 };
  DISP_point_t pEF = { x, y + h / 2 - 1 };    // eq G left

  uint8_t seg = segments[val];

  uint32_t penDark = DISP_GetTextBackColor();
  uint32_t penFore = DISP_GetTextForeColor();

  DISP_point_t p1, p2;

  uint32_t p = ((seg & 0x01) != 0) ? penFore : penDark;       // segment A
  p1 = pFA; p2 = pAB;
  DISP_PointOffset(&p1, _segmentGap_Pix, 0); DISP_PointOffset(&p2, -_segmentGap_Pix, 0);
  DISP_PointOffset(&p1, _segmentFat_Pix, 0); DISP_PointOffset(&p2, -_segmentFat_Pix, 0);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, 1); DISP_PointOffset(&p2, 1, 1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, 1); DISP_PointOffset(&p2, -1, 1);
  }

  p = ((seg & 0x02) != 0) ? penFore : penDark;        // segment B
  p1 = pAB; p2 = pBC;
  DISP_PointOffset(&p1, 0, _segmentGap_Pix + _segmentFat_Pix); DISP_PointOffset(&p2, 0, -_segmentGap_Pix - _segmentFat_Pix / 2);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, -1); DISP_PointOffset(&p2, -1, 1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, 1); DISP_PointOffset(&p2, -1, -1);
  }

  p = ((seg & 0x04) != 0) ? penFore : penDark;        // segment C
  p1 = pBC; p2 = pCD;
  DISP_PointOffset(&p1, 0, _segmentGap_Pix + _segmentFat_Pix / 2); DISP_PointOffset(&p2, 0, -_segmentGap_Pix - _segmentFat_Pix);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, -1); DISP_PointOffset(&p2, -1, 1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, 1); DISP_PointOffset(&p2, -1, -1);
  }

  p = ((seg & 0x08) != 0) ? penFore : penDark;        // segment D
  p1 = pDE; p2 = pCD;
  DISP_PointOffset(&p1, _segmentGap_Pix + _segmentFat_Pix, 0); DISP_PointOffset(&p2, -_segmentGap_Pix - _segmentFat_Pix, 0);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, -1); DISP_PointOffset(&p2, 1, -1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, -1); DISP_PointOffset(&p2, -1, -1);
  }

  p = ((seg & 0x10) != 0) ? penFore : penDark;        // segment E
  p1 = pEF; p2 = pDE;
  DISP_PointOffset(&p1, 0, _segmentGap_Pix + _segmentFat_Pix / 2); DISP_PointOffset(&p2, 0, -_segmentGap_Pix - _segmentFat_Pix);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, -1); DISP_PointOffset(&p2, 1, 1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, 1); DISP_PointOffset(&p2, 1, -1);
  }

  p = ((seg & 0x20) != 0) ? penFore : penDark;        // segment F
  p1 = pFA; p2 = pEF;
  DISP_PointOffset(&p1, 0, _segmentGap_Pix + _segmentFat_Pix); DISP_PointOffset(&p2, 0, -_segmentGap_Pix - _segmentFat_Pix / 2);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, -1); DISP_PointOffset(&p2, 1, 1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, 1); DISP_PointOffset(&p2, 1, -1);
  }

  p = ((seg & 0x40) != 0) ? penFore : penDark;       // segment G
  p1 = pEF; p2 = pBC;
  DISP_PointOffset(&p1, _segmentGap_Pix + _segmentFat_Pix, -_segmentFat_Pix / 2); DISP_PointOffset(&p2, -_segmentGap_Pix - _segmentFat_Pix, -_segmentFat_Pix / 2);
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, -1, 1); DISP_PointOffset(&p2, 1, 1);
  }
  for (int i = 0; i < _segmentFat_Pix / 2; i++)
  {
    DISP_DrawLine(p1.x, p1.y, p2.x, p2.y, p);
    DISP_PointOffset(&p1, 1, 1); DISP_PointOffset(&p2, -1, 1);
  }

  return true;
}

#ifdef __cplusplus
}
#endif