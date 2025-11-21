#pragma once

#define PI 3.14159
#define ONE_AND_THIRD_PI 1.33333 * PI
#define TWO_THIRDS_PI 0.66666 * PI
#define RAD 0.01745329

#define VIEWPORT_HEIGHT 1
#define VIEWPORT_WIDTH 1
#define VIEWPORT_DEPTH 1
// #define DEBUG_POINTS_NO_CLEARSCREEN
// #define DEBUG_POINTS_BBs
// #define DEBUG_POINTS_ZBUFFER
// #define DEBUG_POINTS_LIGHT_LEVEL
// #define DEBUG_POINTS_TRI_DATA
// #define DEBUG_POINTS_RENDER_INDIVIDUAL
// #define DEBUG_TRI_COLOUR

typedef struct
{
    double x, y, z, w;
}vector;

typedef enum
{
    ambient = 0,
    point,
    directional,
}lightEnum;

typedef struct
{
    lightEnum lightType;
    double intensity;
    vector pos_or_dir;
}light;

typedef struct 
{
    vector center;
    double radius;
    int colour[3];
    double specular;
    double reflective;
}sphere;

typedef struct
{
    sphere* sphereArray;
    light* lightArray;
    int lightCount;
    int sphereCount;
}scene;

typedef struct
{
    char character;
    int colour[3];
    double brightness;
}visual;

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
    vector* triangleNormalsArray;
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
    int frameRowsImport;
    int frameColumnsImport;
    int rasteriseBool;
    double framesPerSecond;
    double characterRatio;
} renderConfig;

typedef struct
{
    double matrix[4][4];
}matrix4x4;

//draw.c:
void debugPrintPixelandColour(int x, int y, int colour[3]);

void copyTriangleData(triangle fromTriangle, triangle* toTriagle);

void copyFrameBufferData(frameBuffer fromFrame, frameBuffer* toFrame);

void setTriangleColour(int colour[3], triangle* Triangle);

void cycleMeshColour(mesh* object, int incriment, int totalSteps);

int clamp(int input, int min, int max);

int checkPixelInTriangle(triangle* inputTriangle, int x, int y, double* z);

void drawTriangleOutline(triangle inputTriangle, frameBuffer* frame);

void drawTriangleOnFrame(triangle inputTriangle, frameBuffer* frame, int fillBool);

void setDepthValue(frameBuffer* frame, int x, int y, double* z);

void illuminateTriangle(triangle* inputTriangle, vector inputTriangleNormal, vector lightDirection);

char getGradientCharacter(double luminamce);

void  getGradient(double luminamce, visual* inputSymbol);

void scaleTriangle(triangle* inputTriangle, frameBuffer frame);

vector calculateTriangleNormal(triangle inputTriangle);

void clearFrameBuffer(frameBuffer* frame);

void drawFrameBorder(frameBuffer* frame);

void initialiseFrameBuffer(frameBuffer* frame, renderConfig importData);

void deleteFrameBuffer(frameBuffer* frame);

void drawInFrame(frameBuffer* frame, int x, int y, visual symbol);

void putPixel(frameBuffer* frame, int x, int y, int color[3]);

vector CanvasToViewport(frameBuffer canvas, double x, double y);

void TraceRay(int out_colour[3], scene* scene, vector ray_origin_vector, vector ray_direction_vector, double t_min, double t_max, int recursionDepth);

void ClosestIntersection(sphere** closest_sphere, double* closest_t, scene* scene, vector rayOriginVector, vector rayDirectionVector, double t_min, double t_max);

void IntersectRaySphere(double* t1, double* t2, vector ray_origin_vector, vector ray_direction_vector, sphere* test_sphere);

double computeLighting(scene* scene, vector point_to_compute, vector normal_to_point, vector view_vector, double specular_exponent);

void displayDepthBuffer(frameBuffer frame);

int getConvertedDepthValue(frameBuffer frame, int x, int y);

void displayFrameBuffer3(frameBuffer frame, frameBuffer oldFrame);

int isPixelColourNew(frameBuffer frame, frameBuffer oldFrame, int x, int y);

void plotLineLow(int x0, int y0, int x1, int y1, frameBuffer* frame);

void plotLineHigh(int x0, int y0, int x1, int y1, frameBuffer* frame);

void BresenhamPlotLine(vector pointA, vector pointB, frameBuffer* frame);

void setCursorBelowFrame(frameBuffer frame);

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

vector reflectRay(vector incomingRay, vector normal);

double dotProduct(vector vector1, vector vector2);

triangle matrixTriangleMultiply(triangle inputTriangle, matrix4x4 matrix);

vector matrixVectorMultiply(vector inputVector, matrix4x4 matrix);

matrix4x4 matrixMatrixMultiply(matrix4x4 matrix1, matrix4x4 matrix2);

void initialiseProjectionMatrix(int frameHeight, int frameWidth, double fov, matrix4x4* ProjectionMatrix);

void initialiseRotateXMatrix(matrix4x4* matrixX, double angle);

void initialiseRotateYMatrix(matrix4x4* matrixY, double angle);

void initialiseRotateZMatrix(matrix4x4* matrixZ, double angle);

void initialiseTranslationMatrix(matrix4x4* translate, double x, double y, double z);

void intialiseIdentityMatrix(matrix4x4* input);

void InitialisePointAtMatrix(matrix4x4* input, vector position, vector target, vector up);

matrix4x4 quickMatrixInverse(matrix4x4 input);

void copyMatrix(matrix4x4 from, matrix4x4* to);