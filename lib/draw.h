#pragma once

#define MAX_X 111 //best if odd
#define MAX_Y 45 //best if odd

typedef struct vector
{
    double x, y, z;
} vector;

typedef struct triangle
{
    vector p[3];
} triangle;

typedef struct mesh
{
    triangle* tris;
    int numOfTris;
    int numOfVerts;
} mesh;

mesh importMeshFromOBJFile (char * pathToFile);

vector addVec( vector vec1, vector vec2);

vector subVec(vector vec1, vector vec2);

vector divVecByScalar(vector vec, int scalar);

void meshToVertexArray(double arr[][3], mesh mesh);

void projectVertexArrayTo2D(double arr[][3], double p_points[][2], const double DISTANCE, int iter);

void drawTriangleOnScreen(double arr[][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y], int iter, int totaltris);

void scale2DPoints(double arr[][2], int iter);

void initScreen(int screenArr[MAX_X][MAX_Y]);

void rotateVerticiesAroundX(double arr[][3], int totalPoints, double angle);

void rotateVerticiesAroundY(double arr[][3], int totalPoints, double angle);

void rotateVerticiesAroundZ(double arr[][3], int totalPoints, double angle);

void plotLineLow(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]);

void plotLineHigh(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]);

void BresenhamPlotLine(double pointA[2], double pointB[2], int screen[MAX_X][MAX_Y]);

void drawCubeOnScreen(double arr[8][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y]);

void displayScreen(int arr[MAX_X][MAX_Y]);