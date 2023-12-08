#pragma once

#define MAX_X 101 //best if odd
#define MAX_Y 37 //best if odd

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

mesh importMeshFromOBJFile (char * pathToFile);

mesh copyMeshData(mesh fromMesh, mesh toMesh);

vector addVec( vector vec1, vector vec2);

vector subVec(vector vec1, vector vec2);

vector divVecByScalar(vector vec, int scalar);

void projectMeshTo2D(mesh inputMesh, const double DISTANCE);

void drawMeshOnScreen(mesh inputMesh, double origin[2], double ratio, int screen[MAX_X][MAX_Y]);

void scale2DPoints(mesh inputMesh);

void initScreen(int screenArr[MAX_X][MAX_Y]);

mesh rotateMeshAroundX(mesh inputMesh, const double angle);

mesh rotateMeshAroundY(mesh inputMesh, const double angle);

mesh rotateMeshAroundZ(mesh inputMesh, const double angle);

vector calculateTriangleNormal(triangle inputTri);

void plotLineLow(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]);

void plotLineHigh(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]);

void BresenhamPlotLine(vector pointA, vector pointB, int screen[MAX_X][MAX_Y]);

void displayScreen(int arr[MAX_X][MAX_Y]);