#include "DISP_library_sprites.h"

SpriteData SpriteCreate(void* pixData, int w, int h, uint32_t tColor)
{
  //! check parameters, return NULL if fail
  SpriteData sprite;
  memset(&sprite, 0, sizeof(SpriteData));     // pro jistotu

  sprite.spriteData = pixData;
  sprite.wpix = w;
  sprite.hpix = h;
  sprite.transColor = tColor;     //? a co kdyz nechci ?
  sprite.xpos = sprite.ypos = -1;

  sprite.bgData = malloc(w * h * sizeof(uint16_t));   //TODO get pixel size from global DISP info !!

  return sprite;
}

static bool _spriteSaveBg(SpriteData* pSprite)
{
  if (pSprite->bgData != NULL)      // pro jistotu
  {
    for (int y = 0; y < pSprite->hpix; y++)
    {
      //TODO backup under-sprite area with ?? Get LINE function

      uint16_t* bgLinePtr = (uint16_t*)(pSprite->bgData) + y * pSprite->wpix;
      for (int x = 0; x < pSprite->wpix; x++)
      {
        bgLinePtr[x] = (uint16_t)DISP_GetPixel(pSprite->xpos + x, pSprite->ypos + y);
      }
    }

    return true;
  }
  else
    return false;
}

bool SpriteRestoreBg(SpriteData* pSprite)
{
  //! pro zaporne x a y jen zrusi

  if ((pSprite->xpos >= 0) && (pSprite->ypos >= 0))     // mame starou pozici ?
  {
    if (pSprite->bgData != NULL)      // pro jistotu
      DISP_DrawBitmap(pSprite->bgData, pSprite->xpos, pSprite->ypos, pSprite->wpix, pSprite->hpix);
  }

  return true;
}

bool SpritePlaceData(SpriteData* pSprite, int x, int y)
{
  pSprite->xpos = x;
  pSprite->ypos = y;

  _spriteSaveBg(pSprite);

  DISP_DrawBitmapTrans(pSprite->spriteData, pSprite->xpos, pSprite->ypos,
    pSprite->wpix, pSprite->hpix, pSprite->transColor);

  return true;
}

bool SpriteMove(SpriteData* pSprite, int x, int y)
{
  SpriteRestoreBg(pSprite);

  return SpritePlaceData(pSprite, x, y);
}
