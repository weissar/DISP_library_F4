#include "DISP_library_fonts.h"

// https://www.google.com/search?q=arduino+raster+fonts&sca_esv=d5f0ac5cf98c6d9c&biw=1316&bih=1311&ei=lRSQZvPvErn77_UPtJqY4AQ&ved=0ahUKEwizjs3wv5-HAxW5_bsIHTQNBkwQ4dUDCA8&uact=5&oq=arduino+raster+fonts&gs_lp=Egxnd3Mtd2l6LXNlcnAiFGFyZHVpbm8gcmFzdGVyIGZvbnRzMggQIRigARjDBEiiJVC7D1jCHHABeAGQAQCYAW-gAa4FqgEDNy4xuAEDyAEA-AEBmAIJoALABcICChAAGLADGNYEGEfCAgcQABiABBgNwgIGEAAYDRgewgIIEAAYDRgeGA_CAggQABgKGA0YHsICCBAAGAUYDRgewgIIEAAYCBgNGB7CAgoQABgFGA0YHhgPwgIIEAAYgAQYogTCAgoQIRigARjDBBgKmAMAiAYBkAYGkgcDOC4xoAexJA&sclient=gws-wiz-serp
// https://github.com/adafruit/Adafruit-GFX-Library/tree/master/Fonts
// d:\Documents\Arduino\libraries\Adafruit_GFX_Library\gfxfont.h 

//! aby to umelo najit i include v <>, je do "include dirs" pridana i "."

//TODO make it configurable
#include "fonts_gfx/FreeMono18pt7b.h"
#include "fonts_gfx/FreeSans12pt7b.h"
#include "fonts_gfx/FreeSansBold12pt7b.h"
#include "fonts_gfx/Orbitron_Light_24.h"

static GFXfont* _pFontGfx = NULL;

bool DISP_SetFont_Gfx(eFontsGfx font)
{
  //? dynamic include ???

  switch (font)
  {
    case fontGfx_FreeMono18pt7b:
      _pFontGfx = &FreeMono18pt7b;
      break;
    case fontGfx_FreeSans12pt7b:
      _pFontGfx = &FreeSans12pt7b;
      break;
    case fontGfx_FreeSansBold12pt7b:
      _pFontGfx = &FreeSansBold12pt7b;
      break;
    case fontGfx_Orbitron_Light_24:
      _pFontGfx = &Orbitron_Light_24;
      break;
    default:
      _pFontGfx = NULL;
      return false;
  }

  return true;
}

void DISP_WriteString_Gfx(char* cp)
{
  while (*cp)
  {
    DISP_WriteChar_Gfx(*cp);
    cp++;
  }
}

// ne, jsou static - pouzij DISP_TextForeColor ... extern uint32_t _textForeColor, _textBackColor;

void DISP_WriteChar_Gfx(char c)
{
  int textX = DISP_GetCursorX(), textY = DISP_GetCursorY();

  DISP_size_t sz = DISP_WriteCharXY_Gfx(textX, textY, c);

  textX += sz.w;
  if (textX >= DISP_GetWidth())
  {
    textX = 0;

    textY += sz.h;
    if (textY >= DISP_GetHeight())
    {
      //! no scroll yet
      textY = 0;      // start from disp beginnig
    }
  }

  DISP_GotoXY(textX, textY);
}

// https://adafruit.github.io/Adafruit-GFX-Library/html/struct_g_f_xglyph.html

DISP_size_t DISP_WriteCharXY_Gfx(int x, int y, char c)
{
  DISP_size_t sz = { 0,0 };
  if (_pFontGfx == NULL)
    return sz;

  if ((c < _pFontGfx->first) || (c > _pFontGfx->last))       // znaky jsou pouze v tomto rozsahu 
    return sz;                        // cili ta 0,0 jako priznak chyby

  GFXglyph glyph = _pFontGfx->glyph[c - 0x20];
  uint8_t* pdata = _pFontGfx->bitmap + glyph.bitmapOffset;

  x += glyph.xOffset;
  y += _pFontGfx->yAdvance + glyph.yOffset;    // u glyphu je zaporny !!
  
  uint32_t foreColor = DISP_GetTextForeColor(), backColor = DISP_GetTextBackColor();
  //! nedava to smysl, maji okraje ... bool trans = DISP_GetTextBackTransparent();

  int b = 7;

  for (int py = 0; py < glyph.height; py++)
  {
    for (int px = 0; px < glyph.width; px++)
    {
      bool fore = (*pdata) & (1 << b);
      b--;
      if (b < 0)
      {
        b = 7;
        pdata++;
      }

#if 0       // nejde pouzit pozadi, musi byt transparent
      if (trans)
      {
        if (fore)
          DISP_DrawPixel(x + px, y + py, foreColor);
      }
      else
        DISP_DrawPixel(x + px, y + py, fore ? foreColor : backColor);
#else
      if (fore)
        DISP_DrawPixel(x + px, y + py, foreColor);
#endif
    }
  }

  sz.w = glyph.xAdvance;
  sz.h = +_pFontGfx->yAdvance;           // ne vyska znaku, ale offset na dalsi radek
  return sz;
}