#pragma once
#ifndef _GRAPHICS_3D_H
#define _GRAPHICS_3D_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _G3D_Vertex
{
  int x;
  int y;
  int z;
  int w;
} G3D_Vertex;

typedef struct _G3D_Edge
{
  int v1;
  int v2;
} G3D_Edge;

typedef struct _G3D_Face
{
  int v1;
  int v2;
  int v3;
  uint32_t color;
} G3D_Face;

typedef struct _G3D_Model
{
  int cntVertex;
  G3D_Vertex* vertexes;
  int cntEdge;
  G3D_Edge* edges;
  int cntTriangle;
  G3D_Face* trians;
} G3D_Model;

G3D_Vertex G3D_NormalT(G3D_Model* pModel, int tid);

typedef struct _G3D_Properties
{
  int angleX, angleY, angleZ;
  int zoom;
  int offsetX, offsetY, offsetZ;
  int depth;
} G3D_Properties;

bool G3D_ApplyToModel(G3D_Model* pModel, G3D_Properties* pProp);

#define M3D_MATRIX_DIM 4
typedef float t_M3D_MatrixElem;       // can be changed to DOUBLE

typedef t_M3D_MatrixElem M3D_Matrix[M3D_MATRIX_DIM][M3D_MATRIX_DIM];

void M3D_Transform(G3D_Model* pModel, M3D_Matrix* pm);
void M3D_Projection(G3D_Model* pModel, int posEye);
void M3D_Init(M3D_Matrix* pm);
void M3D_Copy(M3D_Matrix* pmDest, M3D_Matrix* pmSrc);
void M3D_Multiply(M3D_Matrix* pmResult, M3D_Matrix* pm1, M3D_Matrix* pm2);
void M3D_Shift(M3D_Matrix* pm, int px, int py, int pz);
void M3D_Resize(M3D_Matrix* pm, double f);
void M3D_Resize3(M3D_Matrix* pm, double fx, double fy, double fz);
void M3D_Rotate_Z(M3D_Matrix* pm, int angle);
void M3D_Rotate_X(M3D_Matrix* pm, int angle);
void M3D_Rotate_Y(M3D_Matrix* pm, int angle);

#endif // _GRAPHICS_3D_H

