#pragma once

#define PI 3.14159
#define CHARACHTER_RATIO 1.8
// #define DEBUG_POINTS_NO_CLEARSCREEN
// #define DEBUG_POINTS_BBs
// #define DEBUG_POINTS_ZBUFFER
// #define DEBUG_POINTS_LIGHT_LEVEL

typedef struct
{
    char character;
    int colour[3];
    double brightness;
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
    int colour[3];
}mesh;

typedef struct
{ 
    int **characterBuffer;
    int width;
    int height;
    double **depthBuffer;
    int ***colourBuffer; 
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
    double framesPerSecond;
} renderConfig;

typedef struct
{
    double matrix[4][4];
}matrix4x4;

void copyTriangleData(triangle fromTriangle, triangle *toTriagle);

void copyFrameBufferData(frameBuffer fromScreen, frameBuffer *toScreen);

void inheritColourFromMesh(int fromMeshColour[3], triangle *toTriangle);

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

void drawTriangleOutline(triangle inputTriangle, frameBuffer *screen);

void drawTriangleOnScreen(triangle inputTriangle, frameBuffer *screen, int fillBool);

void illuminateTriangle(triangle *inputTriangle, vector inputTriangleNormal, vector lightDirection);

char getGradientCharacter(double luminamce);

void  getGradient(double luminamce, visual *inputSymbol);

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

void initialiseFrameBuffer(frameBuffer *screen, renderConfig importData);

void deleteFrameBuffer(frameBuffer *screen);

void drawInScreen(frameBuffer *screen, int x, int y, visual symbol);

void displayDepthBuffer(frameBuffer *screen);

void displayFrameBuffer(frameBuffer *screen);

void displayFrameBuffer2(frameBuffer *screen, frameBuffer *oldScreen);

void displayFrameBuffer3(frameBuffer screen, frameBuffer oldScreen);

void plotLineLow(int x0, int y0, int x1, int y1, frameBuffer *screen);

void plotLineHigh(int x0, int y0, int x1, int y1, frameBuffer *screen);

void BresenhamPlotLine(vector pointA, vector pointB, frameBuffer *screen);

void frameDelay(double framesPerSecond);