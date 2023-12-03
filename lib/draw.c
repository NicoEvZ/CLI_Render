#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "draw.h"

#define SCALE_FACTOR 150
#define BLANK ' ' //SPACE character ASCII code
#define LINE '#' //'#' character ASCII code
#define DOT '@'
#define BORDER '*'
//#define DEBUG_POINTS

#define MAX_LINE_LENGTH 256

mesh importMeshFromOBJFile (char * pathToFile) 
{
    FILE *obj = fopen(pathToFile, "r");

    mesh newMesh;
    mesh zerodMesh;
    newMesh.numOfTris = 0;

    if (NULL == obj) 
    {
        printf("Error: .OBJ file not found\n");
        printf("Try to run the program from top level \"Cube/\" dir\n");
        return newMesh;
    }

    char line[MAX_LINE_LENGTH];
    int verts = 0;
    int faces = 0;
    vector pointCoords;
    vector average;
    vector runningTotal;
    int p0, p1, p2;
    
    //counts faces and verticies.
    while (fgets(line, sizeof(line), obj) != NULL) 
    {
        if (line[0] == 'v' && line[1] == ' ') 
        {
            verts++;
        }
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            faces++;
        }
    }

    newMesh.numOfTris = faces;
    newMesh.numOfVerts = verts;

    vector (*vectorArray) = malloc(newMesh.numOfVerts * sizeof(vector)); //dynamically allocates an array of vectors, based off the number verticies.

    newMesh.tris = (triangle*) malloc(newMesh.numOfTris * sizeof(triangle)); //dynamically allocates memory for the number of triangles
    zerodMesh = newMesh;

    rewind(obj);

    int vCount = 0;
    int fCount = 0;
    while (fgets(line, sizeof(line), obj) != NULL)
    {
        //reads all lines of obj that start with "v " into the dynamically assigned array of vectors.
        if (line[0] == 'v' && line[1] == ' ') 
        {
            sscanf(line,"v %lf %lf %lf", &pointCoords.x, &pointCoords.y, &pointCoords.z);

            vectorArray[vCount] = pointCoords;

            vCount++;

            runningTotal = addVec(runningTotal, pointCoords);
        }
        //reads all lines of obj that start with "f ".
        //each int after 'f ' represents an index (starting at 1), of the array of verticies.
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            sscanf(line,"f %d %d %d", &p0, &p1, &p2);

            newMesh.tris[fCount].p[0] = vectorArray[(p0-1)];
            newMesh.tris[fCount].p[1] = vectorArray[(p1-1)];
            newMesh.tris[fCount].p[2] = vectorArray[(p2-1)];

            fCount++;
        }
    }

    #ifdef DEBUG_POINTS
    //Handy debug for seeing if points were imported properly
    for (int i = 0; i < vCount; i++) 
    {
          printf("%d: %lf, %lf, %lf\n", i, vectorArray[i].x, vectorArray[i].y, vectorArray[i].z);
    }
    #endif

    average = divVecByScalar(runningTotal, vCount);

    for (int j = 0; j < fCount; j++) 
    {
        for (int i = 0; i < 3; i++) 
        {
            zerodMesh.tris[j].p[i] = subVec(newMesh.tris[j].p[i], average);
        }
    }

    free(vectorArray);
    fclose(obj);

    return zerodMesh;
}

//vec1.elements plus vec2.elements
vector addVec( vector vec1, vector vec2) 
{
    vector returnVec;

    returnVec.x = vec1.x + vec2.x;
    returnVec.y = vec1.y + vec2.y;
    returnVec.z = vec1.z + vec2.z;

    return returnVec;
}

//vec1.elements minus vec2.elements
vector subVec(vector vec1, vector vec2) 
{
    vector returnVec;

    returnVec.x = vec1.x - vec2.x;
    returnVec.y = vec1.y - vec2.y;
    returnVec.z = vec1.z - vec2.z;

    return returnVec;
}

//devides each element of the vector by the value given
vector divVecByScalar(vector vec, int scalar) 
{
    vector returnVec;

    returnVec.x = vec.x/scalar;
    returnVec.y = vec.y/scalar;
    returnVec.z = vec.z/scalar;

    return returnVec;
}

void meshToVertexArray(double arr[][3], const mesh mesh) 
{
    int count = 0;
    for (int i = 0; i < mesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {  
            arr[count][0] = mesh.tris[i].p[j].x;
            arr[count][1] = mesh.tris[i].p[j].y;
            arr[count][2] = mesh.tris[i].p[j].z;
            count++;    
        }
    }
}

void projectVertexArrayTo2D(double arr[][3], double p_points[][2], const double DISTANCE, int iter) 
{
    for (int i = 0; i < iter; i++) 
    {
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        double zPerspective = 1/(DISTANCE - z);

        double p_Mat[2][3] = {{zPerspective,0,0},{0,zPerspective,0}};
        
        double x_p = (p_Mat[0][0]*x)+(p_Mat[0][1]*y)+(p_Mat[0][2]*z);
        double y_p = (p_Mat[1][0]*x)+(p_Mat[1][1]*y)+(p_Mat[1][2]*z);
        
        p_points[i][0] = x_p;
        p_points[i][1] = y_p;
    }
}

void drawTriangleOnScreen(double arr[][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y], int iter, int totaltris) 
{
    double pointA[2] = {0,0};
    double pointB[2] = {0,0};
    double pointC[2] = {0,0};

    for (int i = 0; i < iter; i = i + 3) 
    {
        for (int j = 0; j < 2; j++) 
        {
            if (j == 0) 
            {
                pointA[j] = origin[j] + (arr[i][j] * ratio); //translates from unity to screenspace, and does aspect ratio adustment
                pointB[j] = origin[j] + (arr[(i+1)][j] * ratio);
                pointC[j] = origin[j] + (arr[(i+2)][j] * ratio);
            }
            // if y, dont modify by ratio
            else
            {
                //drawing front face
                pointA[j] = origin[j] + (arr[i][j]); //translates from unity to screenspace, and does aspect ratio adustment
                pointB[j] = origin[j] + (arr[(i+1)][j]);
                pointC[j] = origin[j] + (arr[(i+2)][j]);
            } 
        }
        BresenhamPlotLine(pointA,pointB,screen);
        BresenhamPlotLine(pointB,pointC,screen);
        BresenhamPlotLine(pointC,pointA,screen);
    }
}

void scale2DPoints(double arr[][2], int iter) 
{
    //iterate over every point in points array
    for (int i = 0; i < iter; i++) 
    {
        //extract x and y from array.
        double x = arr[i][0];
        double y = arr[i][1];

        x = x * SCALE_FACTOR;
        y = y * SCALE_FACTOR;

        //draw on screenspace
        arr[i][0] = x;
        arr[i][1] = y;
    }
}

void rotateVerticiesAroundX(double arr[][3], int totalPoints, double angle) 
{
    for (int i = 0; i < totalPoints; i++) 
    {
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        //rotate points by angle
        double x_r = x;
        double y_r = (y * cos(angle)) - (z * sin(angle));
        double z_r = (y * sin(angle)) + (z * cos(angle));

        arr[i][0] = x_r;
        arr[i][1] = y_r;
        arr[i][2] = z_r;
    }
}

void rotateVerticiesAroundY(double arr[][3], int totalPoints,double angle) 
{
    for (int i = 0; i < totalPoints; i++) 
    {
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        //rotate points by angle
        double x_r = (x * cos(angle)) + (z * sin(angle));
        double y_r = y;
        double z_r = (-x * sin(angle)) + (z * cos(angle));

        arr[i][0] = x_r;
        arr[i][1] = y_r;
        arr[i][2] = z_r;
    }
}

void rotateVerticiesAroundZ(double arr[][3], int totalPoints, double angle) 
{
    for (int i = 0; i < totalPoints; i++) 
    {
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        //rotate points by angle
        double x_r = (x * cos(angle)) - (y * sin(angle));
        double y_r = (x * sin(angle)) + (y * cos(angle));
        double z_r = z;

        arr[i][0] = x_r;
        arr[i][1] = y_r;
        arr[i][2] = z_r;
    }
}

void initScreen(int screenArr[MAX_X][MAX_Y]) 
{
    for (int x = 0; x < MAX_X; x++) 
    {
        for (int y = 0; y < MAX_Y; y++) 
        {
            screenArr[x][y]=BLANK;
            if ((x == 0) | (y == 0) | (x == (MAX_X-1)) | (y == (MAX_Y-1))) 
            {   
                screenArr[x][y]=BORDER;
            }
        }
    }
}

void drawInScreen(int screenArr[MAX_X][MAX_Y], int x, int y, const char ASCII) 
{
    if (x < 0) 
    {

        x = 0;
    }

    if (x > (MAX_X-1)) 
    {

        x = MAX_X-1;
    }

    if (y < 0) 
    {

        y = 0;
    }

    if (y > (MAX_Y-1)) 
    {

        y = MAX_Y-1;
    }

    screenArr[x][y] = ASCII;
}

void displayScreen(int arr[MAX_X][MAX_Y])
{
    // Iterate over y axis
    char outputString[MAX_X+1];
    for (int y = 0; y < MAX_Y; y++)
    {
        for (int x = 0; x < MAX_X; x++)
        {
            // Store current value in array at point(x,y), as char in string
            // String is length of MAX_X
            outputString[x]=arr[x][y];
        }
        // Display filled string, and new line character, before moving onto the next value of y
        outputString[MAX_X]='\0';
        printf("%s\n",outputString);
    }
}

void plotLineLow(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y])
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if (dy < 0) 
    {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        if ((x <= 0) | (y <= 0) | (x >= (MAX_X-1)) | (y >= (MAX_Y-1))) 
        {
            drawInScreen(screen,x,y,BORDER);
        }
        else
        {
            drawInScreen(screen,x,y,LINE);
        }
            
        if (D > 0) 
        {
            y = y + yi;
            D = D + (2 * (dy - dx));
        }
        else
        {
            D = D + 2 * dy;
        }
    }
}

void plotLineHigh(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y])
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) 
    {
        xi = -1;
        dx = -dx;
    }   
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++) 
    {
        if ((x <= 0) | (y <= 0) | (x >= (MAX_X-1)) | (y >= (MAX_Y-1))) 
        {
            drawInScreen(screen,x,y,BORDER);
        }
        else
        {
            drawInScreen(screen,x,y,LINE);
        }
        
        if (D > 0) 
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
        {
            D = D + 2 * dx;
        }
    }
}

void BresenhamPlotLine(double pointA[2], double pointB[2], int screen[MAX_X][MAX_Y])
{
    // Initialise points as doubles to do maths nicer
    int x0 = (int)pointA[0];
    int y0 = (int)pointA[1];
    int x1 = (int)pointB[0];
    int y1 = (int)pointB[1];

    if (abs(y1 - y0) < abs(x1 - x0)) 
    {

        if (x0 > x1) 
        {
            plotLineLow(x1, y1, x0, y0, screen);
        }
        else
        {
            plotLineLow(x0, y0, x1, y1, screen);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            plotLineHigh(x1, y1, x0, y0, screen);
        }
        else
        {
            plotLineHigh(x0, y0, x1, y1, screen);
        }
    }
}

void drawCubeOnScreen(double arr[8][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y])
{

    printf("\033[H\033[J"); // Clear screen escape sequence

    double pointA[2] = {0,0};
    double pointB[2] = {0,0};
    double pointC[2] = {0,0};
    double pointD[2] = {0,0};
    double pointE[2] = {0,0};
    double pointF[2] = {0,0};
    //uses the Bresenham algorithm
    for (int i = 0; i < 4; i++) 
    {
        //interates over x [0] and y[1], and assigns which two points will have lines drawn between them
        for (int j = 0; j < 2; j++) 
        {
            // modifying if point is x, by aspect ratio
            if (j == 0) 
            {
                //drawing front face
                pointA[j] = origin[j] + (arr[i][j] * ratio); //translates from unity to screenspace, and does aspect ratio adustment
                pointB[j] = origin[j] + (arr[(i+1)%4][j] * ratio);//(i+1)%4 is used to draw line from point (0 -> 1), (1 -> 2), (2 -> 3), (3 -> 0).
                //drawing back face
                pointC[j] = origin[j] + (arr[(i+4)][j] * ratio);
                pointD[j] = origin[j] + (arr[(((i+1)%4)+4)][j] * ratio);//((i+1)%4)+4 is used to draw line from point (4 -> 5), (5 -> 6), (6 -> 7), (7 -> 4).
                //drawing interconnects between the two faces
                pointE[j] = origin[j] + (arr[i][j] * ratio);
                pointF[j] = origin[j] + (arr[(i+4)][j] * ratio);//(i+4) is used to draw line from point (0 -> 4), (1 -> 5), (2 -> 6), (3 -> 7).
            }
            else
            { // if y, dont modify by ratio
            
                //drawing front face
                pointA[j] = origin[j] + (arr[i][j]); //translates from unity to screenspace, and does aspect ratio adustment
                pointB[j] = origin[j] + (arr[(i+1)%4][j]);//(i+1)%4 is used to draw line from point (0 -> 1), (1 -> 2), (2 -> 3), (3 -> 0).
                //drawing back face
                pointC[j] = origin[j] + (arr[(i+4)][j]);
                pointD[j] = origin[j] + (arr[(((i+1)%4)+4)][j]);//((i+1)%4)+4 is used to draw line from point (4 -> 5), (5 -> 6), (6 -> 7), (7 -> 4).
                //drawing interconnects between the two faces
                pointE[j] = origin[j] + (arr[i][j]);
                pointF[j] = origin[j] + (arr[(i+4)][j]);//(i+4) is used to draw line from point (0 -> 4), (1 -> 5), (2 -> 6), (3 -> 7).
            }
        }
        BresenhamPlotLine(pointA,pointB, screen);
        BresenhamPlotLine(pointC,pointD, screen);
        BresenhamPlotLine(pointE,pointF, screen);
    }
}

