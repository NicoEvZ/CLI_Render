#pragma once

#define PI 3.14159
#define RAD 0.01745329
// #define DEBUG_POINTS_NO_CLEARSCREEN
// #define DEBUG_POINTS_BBs
// #define DEBUG_POINTS_ZBUFFER
// #define DEBUG_POINTS_LIGHT_LEVEL
// #define DEBUG_POINTS_TRI_DATA
// #define DEBUG_POINTS_RENDER_INDIVIDUAL

typedef struct
{
    char character;
    int colour[3];
    double brightness;
}visual;


typedef struct
{
    double x, y, z, w;
}vector;

typedef struct
{
    vector point[3];
    visual symbol;
}triangle;

typedef struct
{
    triangle* trianglePointer;
    int numberOfTriangles;
    int numberOfVertices;
    int colour[3];
}mesh;

typedef struct
{ 
    int** characterBuffer;
    int width;
    int height;
    double** depthBuffer;
    double depthMinimum;
    double depthMaximum;
    int*** colourBuffer; 
} frameBuffer;

typedef struct
{
    double distance;
    double fov;
    char objPathBuffer[64];
    int iterations;
    int startFrame;
    int rotationX;
    int rotationY;
    int rotationZ;
    int screenWidthImport;
    int screenHeightImport;
    int rasteriseBool;
    double framesPerSecond;
    double characterRatio;
} renderConfig;

typedef struct
{
    double matrix[4][4];
}matrix4x4;

//draw.c:

void copyTriangleData(triangle fromTriangle, triangle* toTriagle);

void copyFrameBufferData(frameBuffer fromScreen, frameBuffer* toScreen);

void inheritColourFromMesh(int fromMeshColour[3], triangle* toTriangle);

void cycleMeshColour(mesh* object, int incriment, int totalSteps);

int clamp(int input, int min, int max);

int checkPixelInTriangle(triangle* inputTriangle, int x, int y, double* z);

void drawTriangleOutline(triangle inputTriangle, frameBuffer* screen);

void drawTriangleOnScreen(triangle inputTriangle, frameBuffer* screen, int fillBool);

void setDepthValue(frameBuffer* screen, int x, int y, double* z);

void illuminateTriangle(triangle* inputTriangle, vector inputTriangleNormal, vector lightDirection);

char getGradientCharacter(double luminamce);

void  getGradient(double luminamce, visual* inputSymbol);

void scaleTriangle(triangle* inputTriangle, frameBuffer screen);

vector calculateTriangleNormal(triangle inputTriangle);

void clearFrameBuffer(frameBuffer* screen);

void drawScreenBorder(frameBuffer* screen);

void initialiseFrameBuffer(frameBuffer* screen, renderConfig importData);

void deleteFrameBuffer(frameBuffer* screen);

void drawInScreen(frameBuffer* screen, int x, int y, visual symbol);

void displayDepthBuffer(frameBuffer screen, frameBuffer oldScreen);

void displayFrameBuffer(frameBuffer* screen);

void displayFrameBufferFastColour(frameBuffer screen, frameBuffer oldScreen);

void displayFrameBufferSlowColour(frameBuffer* screen);

void plotLineLow(int x0, int y0, int x1, int y1, frameBuffer* screen);

void plotLineHigh(int x0, int y0, int x1, int y1, frameBuffer* screen);

void BresenhamPlotLine(vector pointA, vector pointB, frameBuffer* screen);

void frameDelay(double framesPerSecond);

void cursesSetup();

void cursesEnd();

// draw-matrix.c:

void initialiseVector(vector*  input);

vector addVector(vector vector1, vector vector2);

vector subtractVector(vector vector1, vector vector2);

vector divideVectorByScalar(vector vector, double scalar);

vector multiplyVectorByScalar(vector vector, double scalar);

vector CrossProduct(vector vector1, vector vector2);

double vectorLength(vector inputVector);

vector normaliseVector(vector inputVector);

double dotProduct(vector vector1, vector vector2);

triangle matrixTriangleMultiply(triangle inputTriangle, matrix4x4 matrix);

vector matrixVectorMultiply(vector inputVector, matrix4x4 matrix);

matrix4x4 matrixMatrixMultiply(matrix4x4 matrix1, matrix4x4 matrix2);

void initialiseProjectionMatrix(int screenHeight, int screenWidth, double fov, matrix4x4* ProjectionMatrix);

void initialiseRotateXMatrix(matrix4x4* matrixX, double angle);

void initialiseRotateYMatrix(matrix4x4* matrixY, double angle);

void initialiseRotateZMatrix(matrix4x4* matrixZ, double angle);

void initialiseTranslationMatrix(matrix4x4* translate, double x, double y, double z);

void intialiseIdentityMatrix(matrix4x4* input);

void InitialisePointAtMatrix(matrix4x4* input, vector position, vector target, vector up);

matrix4x4 quickMatrixInverse(matrix4x4 input);

void copyMatrix(matrix4x4 from, matrix4x4* to);