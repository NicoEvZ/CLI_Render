#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"

#define SCALE_FACTOR 500
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
        //each int after 'f ' represents an index (starting at 1), of the vector array of verticies.
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
            newMesh.tris[j].p[i] = subVec(newMesh.tris[j].p[i], average);
        }
    }

    free(vectorArray);
    fclose(obj);

    return newMesh;
}

mesh copyMeshData(mesh fromMesh, mesh toMesh)
{
    toMesh.numOfTris = fromMesh.numOfTris;
    toMesh.numOfVerts = fromMesh.numOfVerts;
    toMesh.tris = (triangle*) malloc(toMesh.numOfTris * sizeof(triangle));
    for (int i = 0; i < toMesh.numOfTris; i++)
    {
        toMesh.tris[i] = fromMesh.tris[i];
    }
    return toMesh;
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

void projectMeshTo2D(mesh inputMesh, const double DISTANCE) 
{
    vector tempVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++)
        {
        tempVec = inputMesh.tris[i].p[j];

        double zPerspective = 1/(DISTANCE - tempVec.z);

        double p_Mat[2][3] = {{zPerspective,0,0},{0,zPerspective,0}};
        
        double x_p = (p_Mat[0][0]*tempVec.x)+(p_Mat[0][1]*tempVec.y)+(p_Mat[0][2]*tempVec.z);
        double y_p = (p_Mat[1][0]*tempVec.x)+(p_Mat[1][1]*tempVec.y)+(p_Mat[1][2]*tempVec.z);
        
        inputMesh.tris[i].p[j].x = x_p;
        inputMesh.tris[i].p[j].y = y_p;

        }
    }
}

void drawMeshOnScreen(mesh inputMesh, double origin[2], double ratio, int screen[MAX_X][MAX_Y]) 
{
    triangle output = {{0,0,0}};
    vector normal= {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        normal = calculateTriangleNormal(inputMesh.tris[i]);
        if (normal.z > 0)
            for (int j = 0; j < 3; j++)
            {
                //translates from unity to screenspace, and does aspect ratio adustment
                output.p[j].x = origin[0] + (inputMesh.tris[i].p[j].x * ratio); 
                output.p[j].y = origin[1] + inputMesh.tris[i].p[j].y;
            
            }
            BresenhamPlotLine(output.p[0],output.p[1],screen);
            BresenhamPlotLine(output.p[1],output.p[2],screen);
            BresenhamPlotLine(output.p[2],output.p[0],screen);
    }
}

void scale2DPoints(mesh inputMesh) 
{
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++)
        {
            //extract x and y from array.
            double x = inputMesh.tris[i].p[j].x;
            double y = inputMesh.tris[i].p[j].y;

            x = x * SCALE_FACTOR;
            y = y * SCALE_FACTOR;

            inputMesh.tris[i].p[j].x = x;
            inputMesh.tris[i].p[j].y = y;
        }
    }
}

mesh rotateMeshAroundX(mesh inputMesh, const double angle) 
{
    int count = 0;
    vector rotatedVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j =0; j < 3; j++)
        {
            rotatedVec.x = inputMesh.tris[i].p[j].x;

            rotatedVec.y = ((inputMesh.tris[i].p[j].y * cos(angle)) - (inputMesh.tris[i].p[j].z * sin(angle)));

            rotatedVec.z = ((inputMesh.tris[i].p[j].y * sin(angle)) + (inputMesh.tris[i].p[j].z * cos(angle)));

            inputMesh.tris[i].p[j] = rotatedVec;
        }
    }
    return inputMesh;
}

mesh rotateMeshAroundY(mesh inputMesh, const double angle) 
{
    vector rotatedVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j =0; j < 3; j++)
        {
            rotatedVec.x = ((inputMesh.tris[i].p[j].x * cos(angle)) + (inputMesh.tris[i].p[j].z * sin(angle)));

            rotatedVec.y = inputMesh.tris[i].p[j].y;

            rotatedVec.z = ((inputMesh.tris[i].p[j].x * -1 * sin(angle)) + (inputMesh.tris[i].p[j].z * cos(angle)));

            inputMesh.tris[i].p[j] = rotatedVec;
        }
    }
    return inputMesh;
}

mesh rotateMeshAroundZ(mesh inputMesh, const double angle) 
{
    vector rotatedVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j =0; j < 3; j++)
        {
            rotatedVec.x = ((inputMesh.tris[i].p[j].x * cos(angle)) - (inputMesh.tris[i].p[j].y * sin(angle)));

            rotatedVec.y = ((inputMesh.tris[i].p[j].x * sin(angle)) + (inputMesh.tris[i].p[j].y * cos(angle)));

            rotatedVec.z = inputMesh.tris[i].p[j].z;

            inputMesh.tris[i].p[j] = rotatedVec;
        }
    }
    return inputMesh;
}

vector calculateTriangleNormal(triangle inputTri)
{   
    vector U,V,normal;
    
    U = (subVec(inputTri.p[1],inputTri.p[0]));
    V = (subVec(inputTri.p[2],inputTri.p[1]));

    normal.x = (U.y * V.z) - (U.z * V.y);
    normal.y = (U.z * V.x) - (U.x * V.z);
    normal.z = (U.x * V.y) - (U.y * V.x);
    
    return normal;
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

void BresenhamPlotLine(vector pointA, vector pointB, int screen[MAX_X][MAX_Y])
{
    // Initialise points as doubles to do maths nicer
    int x0 = (int)pointA.x;
    int y0 = (int)pointA.y;
    int x1 = (int)pointB.x;
    int y1 = (int)pointB.y;

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

