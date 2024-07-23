#include "DISP_library_3D.h"

static int _minX = 0, _maxX = 0, _minY = 0, _maxY = 0;
static int _lastX = -1, _lastY = -1;

static void _MoveTo(int x, int y);
static void _LineTo(int x, int y);

static void _MoveToVertex(G3D_Vertex vert);
static void _LineToVertex(G3D_Vertex vert);

static int _tempWidth = 0, _tempHeight = 0;

void DISP_3D_SetViewPlane(int minX, int maxX, int minY, int maxY)
{
  _minX = minX;
  _maxX = maxX;

  _minY = minY;
  _maxY = maxY;
}

static DISP_3D_properties* _pProps = NULL;

void DISP_3D_SetPropertiesRef(DISP_3D_properties* pProps)
{
  _pProps = pProps;
}

void DISP_3D_RenderModel(G3D_Model* pModel)
{
  if ((_minX == 0) && (_maxX == 0))       // not initialized ?
  {
    _minX = -DISP_GetWidth() / 2;
    _maxX = DISP_GetWidth() / 2;
    _minY = -DISP_GetHeight() / 2;
    _maxY = DISP_GetHeight() / 2;
  }

  DISP_Fill((_pProps != NULL) ? _pProps->colorBack : 0xffffffff);

  //TODO apply resize viewport from min/max to width/height
  if ((_tempWidth == 0) || (_tempHeight == 0))
  {
    _tempWidth = DISP_GetWidth();
    _tempHeight = DISP_GetHeight();
  }

  if ((_pProps != NULL) && (_pProps->colorAxes != _pProps->colorBack))
  {
    DISP_DrawHLine(0, _tempHeight / 2, _tempWidth, _pProps->colorAxes);
    DISP_DrawVLine(_tempWidth / 2, 0, _tempHeight, _pProps->colorAxes);
  }

  if ((_pProps != NULL) && _pProps->fillSurfaces)
  {
    for (int i = 0; i < pModel->cntTriangle; i++)
    {
      if (G3D_NormalT(pModel, i).z * (((i % 2) == 0) ? 1 : -1) > 0)
      {
        DISP_FillTriangle(
          _tempWidth / 2 + pModel->vertexes[pModel->trians[i].v1 - 1].x,
          _tempHeight / 2 - pModel->vertexes[pModel->trians[i].v1 - 1].y,
          _tempWidth / 2 + pModel->vertexes[pModel->trians[i].v2 - 1].x,
          _tempHeight / 2 - pModel->vertexes[pModel->trians[i].v2 - 1].y,
          _tempWidth / 2 + pModel->vertexes[pModel->trians[i].v3 - 1].x,
          _tempHeight / 2 - pModel->vertexes[pModel->trians[i].v3 - 1].y,
          pModel->trians[i].color);

        if (_pProps->drawWires)     // check if NULL not needed here
        {
          _MoveToVertex(pModel->vertexes[pModel->trians[i].v1 - 1]);
          _LineToVertex(pModel->vertexes[pModel->trians[i].v2 - 1]);
          _LineToVertex(pModel->vertexes[pModel->trians[i].v3 - 1]);
        }
      }
    }
  }
  else
  {
    for (int i = 0; i < pModel->cntEdge; i++)
    {
      _MoveToVertex(pModel->vertexes[pModel->edges[i].v1 - 1]);
      _LineToVertex(pModel->vertexes[pModel->edges[i].v2 - 1]);
    }
  }
}

static void _MoveTo(int x, int y)
{
  _lastX = x;
  _lastY = y;
}

static void _LineTo(int x, int y)
{
  DISP_DrawLine(_tempWidth / 2 + _lastX, _tempHeight / 2 - _lastY,
    _tempWidth / 2 + x, _tempHeight / 2 - y, (_pProps != NULL) ? _pProps->colorWires : 0);

  _lastX = x;
  _lastY = y;
}

static void _MoveToVertex(G3D_Vertex vert)
{
  _lastX = vert.x;
  _lastY = vert.y;
}

static void _LineToVertex(G3D_Vertex vert)
{
  DISP_DrawLine(_tempWidth / 2 + _lastX, _tempHeight / 2 - _lastY,
    _tempWidth / 2 + vert.x, _tempHeight / 2 - vert.y, (_pProps != NULL) ? _pProps->colorWires : 0);

  _lastX = vert.x;
  _lastY = vert.y;
}
