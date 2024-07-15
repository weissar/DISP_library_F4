#ifndef _DISP_LIBRARY_DRIVER_H
#define _DISP_LIBRARY_DRIVER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef bool (*ptrDrawPixelFn_t)(int x, int y, uint32_t color);
typedef uint32_t (*ptrGetPixelFn_t)(int x, int y);
typedef void (*ptrRefreshFn_t)(void);
typedef bool (*ptrInitInternalFn_t)(void);

typedef enum {
  pixel_format_1bpp,
  pixel_format_16b,
  pixel_format_16b_swap,
  pixel_format_24b,
  pixel_format_32b,
} DISP_pixel_format;

typedef struct DISP_Description_struct
{
  int resX;
  int resY;
  DISP_pixel_format pixFormat;

  ptrDrawPixelFn_t fnDrawPixel;
  ptrGetPixelFn_t fnGetPixel;
  ptrRefreshFn_t fnRefresh;

  bool IsFramebuf;
  bool IsAutoRefreshed;
} DISP_Description;

DISP_Description DISP_library_CreateDescr(void);
bool DISP_library_Init(DISP_Description *pDescr);

#endif  // _DISP_LIBRARY_DRIVER_H
