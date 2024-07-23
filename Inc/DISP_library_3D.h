#pragma once

#ifndef _DISP_LIBRARY_3D_H
#define _DISP_LIBRARY_3D_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "DISP_library.h"

#include "Graphics_3D.h"      // for model and properties definition and apply fn

typedef struct _DISP_3D_properties
{
  uint32_t colorBack;
  uint32_t colorAxes;
  uint32_t colorWires;

  bool fillSurfaces;
  bool drawWires;
  bool hideBackWires;
} DISP_3D_properties;

void DISP_3D_SetPropertiesRef(DISP_3D_properties* pProps);

void DISP_3D_SetViewPlane(int minX, int maxX, int minY, int maxY);
void DISP_3D_RenderModel(G3D_Model* pModel);

#endif  // _DISP_LIBRARY_3D_H
