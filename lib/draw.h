#pragma once

#define PI 3.14159
#define CHARACHTER_RATIO 2.2
// #define DEBUG_POINTS_NO_CLEARSCREEN
// #define DEBUG_POINTS_BBs
// #define DEBUG_POINTS_ZBUFFER
// #define DEBUG_POINTS_LIGHT_LEVEL

typedef struct
{
    char character;
    int colour;
}visual;


typedef struct
{
    double x, y, z;
}vector;

typedef struct
{
    vector point[3];
    visual symbol;
}triangle;

typedef struct
{
    triangle *trianglePointer;
    int numberOfTriangles;
    int numberOfVertices;
}mesh;

typedef struct
{ 
    int **characterBuffer;
    int width;
    int height;
    double **depthBuffer;
    int **colourBuffer;
} frameBuffer;

typedef struct
{
    double distance;
    double fov;
    char objPathBuffer[64];
    int iterations;
    int rotationX;
    int rotationY;
    int rotationZ;
    int screenWidthImport;
    int screenHeightImport;
    int rasteriseBool;
} renderConfig;

typedef struct
{
    double matrix[4][4];
}matrix4x4;

void copyTriangleData(triangle fromTriangle, triangle *toTriagle);

vector addVector(vector vector1, vector vector2);

vector subtractVector(vector vector1, vector vector2);

vector divideVectorByScalar(vector vector, double scalar);

vector multiplyVectorByScalar(vector vector, double scalar);

vector CrossProduct(vector vector1, vector vector2);

vector normaliseVector(vector inputVector);

double dotProduct(vector vector1, vector vector2);

triangle matrixVectorMultiply(triangle inputTriangle, matrix4x4 matrix);

int clamp(int input, int min, int max);

void initialiseProjectionMatrix(renderConfig importData, matrix4x4 *projectionMatrix);

int checkPixelInTriangle(triangle inputTriangle, int x, int y, double* z);

void drawTriangleOutline(triangle inputTriangle, frameBuffer screen);

void drawTriangleOnScreen(triangle inputTriangle, frameBuffer screen, int fillBool);

void illuminateTriangle(triangle *inputTriangle, vector inputTriangleNormal, vector lightDirection);

char getGradientCharacter(double luminamce);

visual getGradient2(double luminamce);

visual getGradientColour(double luminamce);

void scaleTriangle(triangle *inputTriangle, frameBuffer screen);

void initialiseRotateXMatrix(matrix4x4 *matrixX, double angle);

void initialiseRotateYMatrix(matrix4x4 *matrixY, double angle);

void initialiseRotateZMatrix(matrix4x4 *matrixZ, double angle);

void translateTriangleX(triangle *triToTranslate, double distance);

void translateTriangleY(triangle *triToTranslate, double distance);

void translateTriangleZ(triangle *triToTranslate, double distance);

vector calculateTriangleNormal(triangle inputTriangle);

void clearFrameBuffer(frameBuffer *screen);

void drawScreenBorder(frameBuffer *screen);

void initialiseFrameBuffer(frameBuffer *screen);

void deleteFrameBuffer(frameBuffer *screen);

void drawInScreen(frameBuffer screen, int x, int y, visual symbol);

void drawInScreen2(frameBuffer screen, int x, int y, int z, visual symbol);

void displayDepthBuffer(frameBuffer *screen);

void displayFrameBuffer(frameBuffer *screen);

void displayFrameBuffer2(frameBuffer *screen);

void plotLineLow(int x0, int y0, int x1, int y1, frameBuffer screen);

void plotLineHigh(int x0, int y0, int x1, int y1, frameBuffer screen);

void BresenhamPlotLine(vector pointA, vector pointB, frameBuffer screen);