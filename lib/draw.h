#pragma once

#define PI 3.14159
#define CHARACHTER_RATIO 2.2
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
    triangle *tris;
    int numOfTris;
    int numOfVerts;
}mesh;

typedef struct
{ 
    int **screen;
    int width;
    int height;
    double **zBuffer;
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

void copyTriangleData(triangle fromTri, triangle *toTri);

vector addVec(vector vec1, vector vec2);

vector subVec(vector vec1, vector vec2);

vector divVecByScalar(vector vec, double scalar);

vector mulVecByScalar(vector vec, double scalar);

vector vecCrossProduct(vector vec1, vector vec2);

vector vecNormaliseVector(vector inputVec);

double vecDotProduct(vector vec1, vector vec2);

triangle matrixVectorMultiply(triangle input, mat4x4 m);

int clamp(int input, int min, int max);

void initProjectMat(renderConfig importData, mat4x4 *matProj);

int pixelInTriangle(triangle inputTri, int x, int y, double* z);

void drawTriOutline(triangle inputTri, screenStruct screen);

void drawTriangleOnScreen(triangle inputTri, screenStruct screen, int fillBool);

void illuminateTriangle(triangle *inputTri, vector inputTriNorm, vector lightDirection);

char getGrad(double lum);

void scaleTriangle(triangle *inputTriangle, screenStruct screen);

void initRotateXMat(mat4x4 *matX, double angle);

void initRotateYMat(mat4x4 *matY, double angle);

void initRotateZMat(mat4x4 *matZ, double angle);

void translateTriangleX(triangle *triToTranslate, double distance);

void translateTriangleY(triangle *triToTranslate, double distance);

void translateTriangleZ(triangle *triToTranslate, double distance);

vector calculateTriangleNormal(triangle inputTri);

void clearScreen(screenStruct *screen);

void drawScreenBorder(screenStruct *screen);

void initScreen(screenStruct *screen);

void deleteScreen(screenStruct *screen);

void drawInScreen(screenStruct screen, int x, int y, const char ASCII);

void displayZBuffer(screenStruct *screen);

void displayScreen(screenStruct *screen);

void plotLineLow(int x0, int y0, int x1, int y1, screenStruct screen);

void plotLineHigh(int x0, int y0, int x1, int y1, screenStruct screen);

void BresenhamPlotLine(vector pointA, vector pointB, screenStruct screen);