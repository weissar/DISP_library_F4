#include "Graphics_3D.h"

#define _USE_MATH_DEFINES       // trick for MSVC to use M_PI from math etc
#include <math.h>

G3D_Vertex G3D_NormalT(G3D_Model* pModel, int tid)
{
  if ((tid >= 0) && (tid < pModel->cntTriangle))
  {
    G3D_Face* pt = &pModel->trians[tid];

    G3D_Vertex* pA = &pModel->vertexes[pt->v1 - 1];
    G3D_Vertex* pB = &pModel->vertexes[pt->v2 - 1];
    G3D_Vertex* pC = &pModel->vertexes[pt->v3 - 1];

    /* not needed
    VERTEX AB = new VERTEX(B.x - A.x, B.y - A.y, B.z - A.z);
    VERTEX AC = new VERTEX(C.x - A.x, C.y - A.y, C.z - A.z);
    */
    // https://stackoverflow.com/questions/20889996/how-do-i-remove-all-non-ascii-characters-with-regex-and-notepad
    G3D_Vertex v =
    {
      (pB->y - pA->y) * (pC->z - pA->z) - (pB->z - pA->z) * (pC->y - pA->y),
      (pB->z - pA->z) * (pC->x - pA->x) - (pB->x - pA->x) * (pC->z - pA->z),
      (pB->x - pA->x) * (pC->y - pA->y) - (pB->y - pA->y) * (pC->x - pA->x),
      0   // W
    };

    return v;
  }
  else
  {
    G3D_Vertex v = { 0, 0, 0, 0 };
    return v;
  }
}

#define M3D_MATRIX_DIM 4
typedef float t_M3D_MatrixElem;       // can be changed to DOUBLE

typedef t_M3D_MatrixElem M3D_Matrix[M3D_MATRIX_DIM][M3D_MATRIX_DIM];

//! here isnt CREATE, use type M3D_Matrix

bool G3D_ApplyToModel(G3D_Model* pModel, G3D_Properties* pProp)
{
  M3D_Matrix m;
  M3D_Init(&m);

  M3D_Resize(&m, pProp->zoom / 100.0);

  M3D_Rotate_X(&m, pProp->angleX);
  M3D_Rotate_Y(&m, pProp->angleY);
  M3D_Rotate_Z(&m, pProp->angleZ);

  M3D_Shift(&m, pProp->offsetX, pProp->offsetY, pProp->offsetZ);

  M3D_Transform(pModel, &m);

  if (pProp->depth != 0)
    M3D_Projection(pModel, pProp->depth);

  return true;
}

void M3D_Transform(G3D_Model* pModel, M3D_Matrix* pm)
{
  for (int i = 0; i < pModel->cntVertex; i++)
  {
    int nx = (int)(pModel->vertexes[i].x * (*pm)[0][0]
      + pModel->vertexes[i].y * (*pm)[1][0]
      + pModel->vertexes[i].z * (*pm)[2][0]
      + (*pm)[3][0]);
    int ny = (int)(pModel->vertexes[i].x * (*pm)[0][1]
      + pModel->vertexes[i].y * (*pm)[1][1]
      + pModel->vertexes[i].z * (*pm)[2][1]
      + (*pm)[3][1]);
    int nz = (int)(pModel->vertexes[i].x * (*pm)[0][2]
      + pModel->vertexes[i].y * (*pm)[1][2]
      + pModel->vertexes[i].z * (*pm)[2][2]
      + (*pm)[3][2]);

    pModel->vertexes[i].x = nx;
    pModel->vertexes[i].y = ny;
    // no Z - rendered to X-Y plane
  }
}

void M3D_Projection(G3D_Model* pModel, int posEye)
{
  if (posEye == 0)          // not needed ?
    return;

  for (int i = 0; i < pModel->cntVertex; i++)
  {
    int sx = pModel->vertexes[i].x;
    int sy = pModel->vertexes[i].y;
    int sz = pModel->vertexes[i].z;

    double t = 1.0 / (1.0 - sz / (t_M3D_MatrixElem)posEye);

    pModel->vertexes[i].x = (int)(sx * t);
    pModel->vertexes[i].y = (int)(sy * t);
  }
}

void M3D_Init(M3D_Matrix* pm)
{
  for (int x = 0; x < M3D_MATRIX_DIM; x++)
    for (int y = 0; y < M3D_MATRIX_DIM; y++)
      (*pm)[x][y] = (x == y) ? 1 : 0;
}

void M3D_Copy(M3D_Matrix* pmDest, M3D_Matrix* pmSrc)
{
  for (int x = 0; x < M3D_MATRIX_DIM; x++)
    for (int y = 0; y < M3D_MATRIX_DIM; y++)
      (*pmDest)[x][y] = (*pmSrc)[x][y];
}

//? return new matrix ??
void M3D_Multiply(M3D_Matrix* pmResult, M3D_Matrix* pm1, M3D_Matrix* pm2)
{
  for (int x = 0; x < M3D_MATRIX_DIM; x++)
  {
    for (int y = 0; y < M3D_MATRIX_DIM; y++)
    {
      double suma = 0;
      for (int z = 0; z < M3D_MATRIX_DIM; z++)
        suma += (*pm1)[x][z] * (*pm2)[z][y];
      (*pmResult)[x][y] = suma;
    }
  }
}

void M3D_Shift(M3D_Matrix* pm, int px, int py, int pz)
{
  M3D_Matrix m1, m2;
  M3D_Init(&m1);

  m1[3][0] = px;
  m1[3][1] = py;
  m1[3][2] = pz;

  M3D_Multiply(&m2, &m1, pm);
  M3D_Copy(pm, &m2);
}

void M3D_Resize(M3D_Matrix* pm, double f)
{
  M3D_Matrix m1, m2;
  M3D_Init(&m1);

  m1[0][0] = f;
  m1[1][1] = f;
  m1[2][2] = f;

  M3D_Multiply(&m2, &m1, pm);
  M3D_Copy(pm, &m2);
}

void M3D_Resize3(M3D_Matrix* pm, double fx, double fy, double fz)
{
  M3D_Matrix m1, m2;
  M3D_Init(&m1);

  m1[0][0] = fx;
  m1[1][1] = fy;
  m1[2][2] = fz;

  M3D_Multiply(&m2, &m1, pm);
  M3D_Copy(pm, &m2);
}

void M3D_Rotate_Z(M3D_Matrix* pm, int angle)
{
  if (angle == 0)
    return;

  M3D_Matrix m1, m2;
  M3D_Init(&m1);

  double rad = angle * M_PI * 2 / 360;
  double fc = cos(rad);
  double fs = sin(rad);

  m1[0][0] = fc;
  m1[0][1] = fs;

  m1[1][0] = -fs;
  m1[1][1] = fc;

  M3D_Multiply(&m2, &m1, pm);
  M3D_Copy(pm, &m2);
}

void M3D_Rotate_X(M3D_Matrix* pm, int angle)
{
  if (angle == 0)
    return;

  M3D_Matrix m1, m2;
  M3D_Init(&m1);

  double rad = angle * M_PI * 2 / 360;
  double fc = cos(rad);
  double fs = sin(rad);

  m1[1][1] = fc;
  m1[1][2] = fs;

  m1[2][1] = -fs;
  m1[2][2] = fc;

  M3D_Multiply(&m2, &m1, pm);
  M3D_Copy(pm, &m2);
}

void M3D_Rotate_Y(M3D_Matrix* pm, int angle)
{
  if (angle == 0)
    return;

  M3D_Matrix m1, m2;
  M3D_Init(&m1);

  double rad = angle * M_PI * 2 / 360;
  double fc = cos(rad);
  double fs = sin(rad);

  m1[0][0] = fc;
  m1[0][2] = -fs;

  m1[2][0] = fs;
  m1[2][2] = fc;

  M3D_Multiply(&m2, &m1, pm);
  M3D_Copy(pm, &m2);
}
