#pragma once

#define MAX_X 81 //best if odd
#define MAX_Y 35 //best if odd

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

} mesh;

mesh importMeshFromOBJFile (char * pathToFile);

vector addVec( vector vec1, vector vec2);

vector subVec(vector vec1, vector vec2);

vector avgVec(vector sumTotal, int totalNumber);

void meshToVertexArray(double arr[][3], mesh mesh);

void projectVertexArrayTo2D(double arr[][3], double p_points[][2], const double DISTANCE, int iter);

void drawTriangleOnScreen(double arr[][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y], int iter, int totaltris);

void scaleTriangle2DPoints(double arr[][2], int iter);

void initPoints(double arr[8][3], double basePoints[8][3]);
void initScreen(int screenArr[MAX_X][MAX_Y]);

void rotatePointsAroundX(double arr[8][3], double angle);
void rotatePointsAroundY(double arr[8][3], double angle);
void rotatePointsAroundZ(double arr[8][3], double angle);

void rotateVertexsAroundX(double arr[][3], int totalPoints, double angle);
void rotateVertexsAroundY(double arr[][3], int totalPoints, double angle);
void rotateVertexsAroundZ(double arr[][3], int totalPoints, double angle);

void plotPointRel(double point[2], double origin[2], double ratio, int screen[MAX_X][MAX_Y]);
void plotPointAbs(double point[2], int screen[MAX_X][MAX_Y]);

void plotMultiPointsRel(double arr[8][2], double origin[2], double ratio, int screen[MAX_X][MAX_Y]);
void plotMultiPointAbs(double arr[8][2], double origin[2], double ratio, int screen[MAX_X][MAX_Y]);

void scaleMulti3DPoints(double arr[8][3]);
void scaleMulti2DPoints(double arr[8][2]);

void projectPoints2d(double arr[8][3], double p_points[8][2], const double DISTANCE);

void plotLineLow(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]);
void plotLineHigh(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]);
void BresenhamPlotLine(double pointA[2], double pointB[2], int screen[MAX_X][MAX_Y]);

void drawCubeOnScreen(double arr[8][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y]);

void displayScreen(int arr[MAX_X][MAX_Y]);