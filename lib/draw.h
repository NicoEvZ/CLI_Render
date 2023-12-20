#pragma once

#define PI 3.14159

typedef struct vector
{
    double x, y, z;
}vector;

typedef struct triangle
{
    vector p[3];
}triangle;

typedef struct mesh
{
    triangle* tris;
    int numOfTris;
    int numOfVerts;
}mesh;

typedef struct screenStruct
{ 
    int** screen;
    int width;
    int height;
}screenStruct;

typedef struct data
{
    double distance;
    double scale;
    char objPathBuffer[64];
    int i;
    int rotationX;
    int rotationY;
    int rotationZ;
    int screenWidthImprt;
    int screenHeightImprt;
    int rasteriseBool;
}data;

mesh importMeshFromOBJFile(char * pathToFile);

int importJSON(const char *file_path, data *importData_struct);

char *stripString(char *inputString, char stripChar);

mesh copyMeshData(mesh fromMesh, mesh toMesh);

vector addVec(vector vec1, vector vec2);

vector subVec(vector vec1, vector vec2);

vector divVecByScalar(vector vec, int scalar);

vector vecCrossProduct(vector vec1, vector vec2);

void projectMeshTo2D(mesh inputMesh, const double distance);

void drawMeshOnScreen(mesh inputMesh, vector origin, screenStruct screen, vector *inputVecArr);

void rasteriseMeshOnScreen(mesh inputMesh, vector origin, screenStruct screen, vector *inputVecArr);

void scale2DPoints(mesh inputMesh, const double scaleFactor);

void clearScreen(screenStruct *screen);

void initScreen(screenStruct *screen);

void deleteScreen(screenStruct *screen);

mesh rotateMeshAroundX(mesh inputMesh, const double angle);

mesh rotateMeshAroundY(mesh inputMesh, const double angle);

mesh rotateMeshAroundZ(mesh inputMesh, const double angle);

vector calculateTriangleNormal(triangle inputTri);

void calculateMeshNormals(mesh inputMesh, vector *inputArray);

void plotLineLow(int x0, int y0, int x1, int y1, screenStruct screen);

void plotLineHigh(int x0, int y0, int x1, int y1, screenStruct screen);

void BresenhamPlotLine(vector pointA, vector pointB, screenStruct screen);

void displayScreen(screenStruct *screen);

char getGrad(double lum);