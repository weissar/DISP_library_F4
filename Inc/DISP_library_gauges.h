/*
 * gauges.h
 *
 *  Created on: 15. 1. 2022
 *      Author: Weissar
 */

#ifndef _DISP_LIBRARY_GAUGES_H_
#define _DISP_LIBRARY_GAUGES_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "DISP_library.h"

// Meter colour schemes
typedef enum
{
  GAUGE_RED2RED = 0,
  GAUGE_GREEN2GREEN,
  GAUGE_BLUE2BLUE,
  GAUGE_BLUE2RED,
  GAUGE_GREEN2RED,
  GAUGE_RED2GREEN
} eGaugeSchemes;

typedef struct
{
  int _curVal;
  int valMin;
  int valMax;
  int x;
  int y;
  int r;
  int angleStart;
  int angleEnd;
  int segDegWidth;
  int segDegStep;
  uint16_t ringBack;
  uint16_t textBack;
  uint16_t textFore;
  eGaugeSchemes scheme;
  bool textForeAuto;
  char *units;
} GaugeRingMeter;

bool DrawGauge_RingMeter(GaugeRingMeter *grm, int value);

int ringMeter(int value, int vmin, int vmax, int x, int y, int r, char *units,
    uint8_t scheme, bool withgap);

#endif // _DISP_LIBRARY_GAUGES_H_
