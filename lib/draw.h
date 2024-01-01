#pragma once

#define PI 3.14159
#define CHARACHTER_RATIO 2.3
// #define DEBUG_POINTS_NO_CLEARSCREEN
// #define DEBUG_POINTS_BBs
// #define DEBUG_POINTS_ZBUFFER
// #define DEBUG_POINTS_LIGHT_LEVEL

typedef struct
{
    double x, y, z;
}vector;

typedef struct
{
    vector p[3];
    char symbol;
}triangle;

typedef struct
{
    triangle* tris;
    int numOfTris;
    int numOfVerts;
}mesh;

typedef struct
{ 
    int** screen;
    int width;
    int height;
    double** zBuffer;
} screenStruct;

typedef struct
{
    double distance;
    double fov;
    char objPathBuffer[64];
    int i;
    int rotationX;
    int rotationY;
    int rotationZ;
    int screenWidthImprt;
    int screenHeightImprt;
    int rasteriseBool;
} renderConfig;

typedef struct
{
    double m[4][4];
}mat4x4;

mesh copyMeshData(mesh fromMesh, mesh toMesh);

void copyTriangleData(triangle fromTri, triangle *toTri);

vector addVec(vector vec1, vector vec2);

vector subVec(vector vec1, vector vec2);

vector divVecByScalar(vector vec, double scalar);

vector vecCrossProduct(vector vec1, vector vec2);

triangle matrixVectorMultiply(triangle input, mat4x4 m);

void drawTriangleOnScreen(triangle inputTri, screenStruct screen);

void rasteriseTriangleOnScreen(triangle inputTri, screenStruct screen);

void scaleTriangle(triangle *inputTriangle, screenStruct screen);

void clearScreen(screenStruct *screen);

void initScreen(screenStruct *screen);

void initProjectMat(renderConfig importData, mat4x4 *matProj);

void initRotateXMat(mat4x4 * matX, double angle);

void initRotateYMat(mat4x4 * matY, double angle);

void initRotateZMat(mat4x4 * matZ, double angle);

void translateTriangle(triangle *triToTranslate, double distance);

void deleteScreen(screenStruct *screen);

vector calculateTriangleNormal(triangle inputTri);

void plotLineLow(int x0, int y0, int x1, int y1, screenStruct screen);

void plotLineHigh(int x0, int y0, int x1, int y1, screenStruct screen);

void BresenhamPlotLine(vector pointA, vector pointB, screenStruct screen);

void drawInScreen(screenStruct screen, int x, int y, const char ASCII);

void displayZBuffer(screenStruct *screen);

void displayScreen(screenStruct *screen);

void drawScreenBorder(screenStruct *screen);

void illuminateTriangle(triangle *inputTri, vector inputTriNorm, vector lightDirection);

char getGrad(double lum);