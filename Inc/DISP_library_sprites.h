#pragma once

#ifndef _DISP_LIBRARY_SPRITES_H_
#define _DISP_LIBRARY_SPRITES_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "DISP_library.h"

typedef struct 
{
  //! private, set by ctor !!
  void* spriteData;
  void* bgData;
  int xpos;
  int ypos;
  int wpix;
  int hpix;
  uint32_t transColor;
  // public
} SpriteData;

SpriteData SpriteCreate(void* pixData, int w, int h, uint32_t tColor);
bool SpriteMove(SpriteData* pSprite, int x, int y);
bool SpriteRestoreBg(SpriteData* pSprite);
bool SpritePlaceData(SpriteData* pSprite, int x, int y);

#endif        // _DISP_LIBRARY_SPRITES_H_
