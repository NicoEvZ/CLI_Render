#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"
#include "runner.h"

// #include "quick_sort.h"

#define BLANK ' ' //SPACE character ASCII code
#define LINE '#' //'#' character ASCII code
#define DOT '@'
#define BORDER '*'

#define CHAR_CONST 2.3

vector camera = {0,0,0};

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

void copyTriangleData(triangle fromTri, triangle *toTri)
{
    for (int i = 0; i < 3; i++)
    {
        toTri->p[i] = fromTri.p[i];
    }
    toTri->symbol = fromTri.symbol;
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
vector divVecByScalar(vector vec, double scalar) 
{
    vector returnVec;

    returnVec.x = vec.x/scalar;
    returnVec.y = vec.y/scalar;
    returnVec.z = vec.z/scalar;

    return returnVec;
}

vector vecCrossProduct(vector vec1, vector vec2)
{
    vector output;

    output.x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
    output.y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
    output.z = (vec1.x * vec2.y) - (vec1.y * vec2.x);

    return output;
}

triangle matrixVectorMultiply(triangle input, mat4x4 m)
{
    triangle output = input;
    for (int i = 0; i < 3; i++)
    {
        output.p[i].x = (input.p[i].x * m.m[0][0]) + (input.p[i].y * m.m[1][0]) + (input.p[i].z * m.m[2][0]) + (m.m[3][0]);
        output.p[i].y = (input.p[i].x * m.m[0][1]) + (input.p[i].y * m.m[1][1]) + (input.p[i].z * m.m[2][1]) + (m.m[3][1]);
        output.p[i].z = (input.p[i].x * m.m[0][2]) + (input.p[i].y * m.m[1][2]) + (input.p[i].z * m.m[2][2]) + (m.m[3][2]);
        double w      = (input.p[i].x * m.m[0][3]) + (input.p[i].y * m.m[1][3]) + (input.p[i].z * m.m[2][3]) + (m.m[3][3]);

        if (w != 0.0)
        {
            output.p[i] = divVecByScalar(output.p[i],w);
        }
    }
    return output;
}

void initProjectMat(renderConfig importData, mat4x4 *matProj)
{
    double near = 0.1;
    double far = 1000;
    double fov = importData.fov;
    double aspectRatio = (double)importData.screenHeightImprt / (double)importData.screenWidthImprt;
    double fovRad = 1 / tanf(fov * 0.5 / 180 * PI);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matProj->m[i][j] = 0;
        }
    }

    matProj->m[0][0] = aspectRatio * fovRad;
    matProj->m[1][1] = fovRad;
    matProj->m[2][2] = far / (far - near);
    matProj->m[3][2] = (-far * near) / (far - near);
    matProj->m[2][3] = 1;
}

void projectMeshTo2D(mesh inputMesh, const double distance) 
{
    vector tempVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++)
        {
        tempVec = inputMesh.tris[i].p[j];

        tempVec.x = tempVec.x * CHAR_CONST;

        double zPerspective = 1/(distance - tempVec.z);

        double p_Mat[2][3] = {{zPerspective,0,0},{0,zPerspective,0}};
        
        double x_p = (p_Mat[0][0]*tempVec.x)+(p_Mat[0][1]*tempVec.y)+(p_Mat[0][2]*tempVec.z);
        double y_p = (p_Mat[1][0]*tempVec.x)+(p_Mat[1][1]*tempVec.y)+(p_Mat[1][2]*tempVec.z);
        
        inputMesh.tris[i].p[j].x = x_p;
        inputMesh.tris[i].p[j].y = y_p;

        }
    }
}

void drawMeshOnScreen(mesh inputMesh, vector origin, screenStruct screen, vector *inputVecArr) 
{
    printf("\033[H\033[J"); //clears the screen

    triangle output = {{{0,0,0}}};
    vector normal = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        normal = inputVecArr[i];
        // double normalCheck = normal.x * (inputMesh.tris[i].p[0].x - camera.x) +
        //                      normal.y * (inputMesh.tris[i].p[0].y - camera.y) +
        //                      normal.z * (inputMesh.tris[i].p[0].z - camera.z);

        double normalCheck = normal.z;
        // printf("Triangle: %d\n",i);
        // printf("normal.z: %lf\n", normalCheck);

        if (normalCheck > 0)
        {
            for (int j = 0; j < 3; j++)
            {
                //translates from unity to screenspace, and does aspect ratio adustment
                // output.p[j].x = origin[0] + (inputMesh.tris[i].p[j].x); 
                output.p[j].x = origin.x + (inputMesh.tris[i].p[j].x); 
                output.p[j].y = origin.y + inputMesh.tris[i].p[j].y;
            
            }
            BresenhamPlotLine(output.p[0],output.p[1],screen);
            BresenhamPlotLine(output.p[1],output.p[2],screen);
            BresenhamPlotLine(output.p[2],output.p[0],screen);
        }
        // displayScreen(&screen);
    }
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

void rasteriseMeshOnScreen(mesh inputMesh, vector origin, screenStruct screen, vector *inputVecArr)
{
    printf("\033[H\033[J"); //clears the screen

    triangle output = {{{0,0,0}}};
    double trisToRasterCounter = 0;


    vector lightDirection = {0, 1, 0};
    double l = sqrtl(lightDirection.x * lightDirection.x + 
                     lightDirection.y * lightDirection.y + 
                     lightDirection.z * lightDirection.z);
    lightDirection = divVecByScalar(lightDirection, l);

    vector normal = {0,0,0};
    
    for (int i = 0; i < inputMesh.numOfTris; i++)
    {
        normal = inputVecArr[i];
        double dp = normal.x * lightDirection.x + normal.y * lightDirection.y + normal.z * lightDirection.z;
        char c = getGrad(dp);
        // double normalCheck = normal.x * (inputMesh.tris[i].p[0].x - camera.x) +
        //                      normal.y * (inputMesh.tris[i].p[0].y - camera.y) +
        //                      normal.z * (inputMesh.tris[i].p[0].z - camera.z);

        double normalCheck = normal.z;
        // printf("Triangle: %d\n",i);
        // printf("normal.z: %lf\n", normalCheck);

        if (normalCheck > 0)
        {
            trisToRasterCounter++;
            for (int j = 0; j < 3; j++)
            {
                //translates from unity to screenspace, and does aspect ratio adustment
                // output.p[j].x = origin[0] + (inputMesh.tris[i].p[j].x); 
                output.p[j].x = origin.x + (inputMesh.tris[i].p[j].x); 
                output.p[j].y = origin.y + inputMesh.tris[i].p[j].y;
            }

            for (int x = 0; x < screen.width; x++)
            {
                for (int y = 0; y < screen.height; y++)
                {
                    if (pixelInTriangle(output,x,y))
                    {
                        screen.screen[x][y] = c;
                    }
                }
            }
            // fillTriangle(output.p[0], output.p[1], output.p[2], screen);
            // BresenhamPlotLine(output.p[0],output.p[1],screen);
            // BresenhamPlotLine(output.p[1],output.p[2],screen);
            // BresenhamPlotLine(output.p[2],output.p[0],screen);
        }
        // printf("\033[H\033[J"); //clears the screen
        // displayScreen(&screen);

    }
}

void drawTriangleOnScreen(triangle inputTri, screenStruct screen)
{
    // printf("\033[H\033[J"); //clears the screen

    for (int i = 0; i < 3; i++)
    {
       BresenhamPlotLine(inputTri.p[i],inputTri.p[((i+1)%3)],screen);
    }    
}


void rasteriseTriangleOnScreen(triangle inputTri, screenStruct screen)
{
    // printf("\033[H\033[J"); //clears the screen

    for (int x = 0; x < screen.width; x++)
    {
        for (int y = 0; y < screen.height; y++)
        {
            if (pixelInTriangle(inputTri,x,y))
            {
                screen.screen[x][y] = inputTri.symbol;
            }
        }
    }
}

void illuminateTriangle(triangle *inputTri, vector inputTriNorm, vector lightDirection)
{
    double l = sqrtl(lightDirection.x * lightDirection.x + 
                     lightDirection.y * lightDirection.y + 
                     lightDirection.z * lightDirection.z);

    lightDirection = divVecByScalar(lightDirection, l);

    double dp = inputTriNorm.x * lightDirection.x + 
                inputTriNorm.y * lightDirection.y + 
                inputTriNorm.z * lightDirection.z;

    inputTri->symbol = getGrad(dp);
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

char  getGrad(double lum)
{
    // 
    // .
    // :
    // -
    // =
    // +
    // *
    // #
    // %
    // @

    char outputChar;
    int grad = (int)9 * lum;
    switch (grad)
    {
    case 0:
        outputChar = '.';
        break;
    case 1:
        outputChar = ':';
        break;
    case 2:
        outputChar = '-';
        break;
    case 3: 
        outputChar = '=';
        break;
    case 4:
        outputChar = '+';
        break;
    case 5:
        outputChar = '*';
        break;
    case 6:
        outputChar = '#';
        break;
    case 7:
        outputChar = '%';
        break;
    case 8:
        outputChar = '@';
        break;
    default:
        outputChar = '`';
    }
    return outputChar;
}

void scale2DPoints(mesh inputMesh, const double scaleFactor) 
{
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++)
        {
            //extract x and y from array.
            double x = inputMesh.tris[i].p[j].x;
            double y = inputMesh.tris[i].p[j].y;

            x = x * scaleFactor;
            y = y * scaleFactor;

            inputMesh.tris[i].p[j].x = x;
            inputMesh.tris[i].p[j].y = y;
        }
    }
}

void scaleTriangle(triangle *inputTriangle, screenStruct screen)
{
    for (int i = 0; i < 3; i++)
    {
        inputTriangle->p[i].x += 1;
        inputTriangle->p[i].y += 1;

        inputTriangle->p[i].x *= (0.5 * (double)screen.width);
        inputTriangle->p[i].y *= (0.5 * (double)screen.height);
    }
}


mesh rotateMeshAroundX(mesh inputMesh, const double angle) 
{
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

void initRotateXMat(mat4x4 * matX, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matX->m[i][j] = 0;
        }
    }

    matX->m[0][0] = 1;
    matX->m[1][1] = cosf(angle);
    matX->m[1][2] = sinf(angle);
    matX->m[2][1] = -sinf(angle);
    matX->m[2][2] = cosf(angle);
    matX->m[3][3] = 1;
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

void initRotateYMat(mat4x4 * matY, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matY->m[i][j] = 0;
        }
    }

    matY->m[0][0] = cosf(angle);;
    matY->m[0][2] = -sinf(angle);
    matY->m[1][1] = 1;
    matY->m[2][0] = sinf(angle);
    matY->m[2][2] = cosf(angle);
    matY->m[3][3] = 1;
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

void initRotateZMat(mat4x4 * matZ, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matZ->m[i][j] = 0;
        }
    }

    matZ->m[0][0] = cosf(angle);
    matZ->m[0][1] = sinf(angle);
    matZ->m[1][0] = -sinf(angle);
    matZ->m[1][1] = cosf(angle);
    matZ->m[2][2] = 1;
    matZ->m[3][3] = 1;
}

void translateTriangle(triangle *triToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triToTranslate->p[i].z = triToTranslate->p[i].z + distance;
    }
}

vector calculateTriangleNormal(triangle inputTri)
{   
    vector U,V,normal;
    
    U = (subVec(inputTri.p[1],inputTri.p[0]));
    V = (subVec(inputTri.p[2],inputTri.p[0]));

    normal = vecCrossProduct(U, V);

    //its normally normal to normalise the normal
    double l = sqrtl(normal.x * normal.x + 
                     normal.y * normal.y + 
                     normal.z * normal.z);
    normal = divVecByScalar(normal, l);
    
    return normal;
}

void calculateMeshNormals(mesh inputMesh, vector *inputArray)
{
    vector normal = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++)
    {
        normal = calculateTriangleNormal(inputMesh.tris[i]);
        inputArray[i] = normal;
    }
}

void clearScreen(screenStruct *screen) 
{
    for (int x = 0; x < screen->width; x++) 
    {
        for (int y = 0; y < screen->height; y++) 
        {
            screen->screen[x][y]=BLANK;
            if ((x == 0) | (y == 0) | (x == (screen->width-1)) | (y == (screen->height-1))) 
            {   
                screen->screen[x][y]=BORDER;
            }
        }
    }
}

void initScreen(screenStruct *screen)
{
    screen->screen = malloc(screen->width * sizeof(int *));
    for (int i = 0; i < screen->width; i++)
    {
        screen->screen[i] = malloc(screen->height * sizeof(int));
    }
}

void deleteScreen(screenStruct *screen)
{
    for (int i = 0; i < screen->width; i++)
    {
        free(screen->screen[i]);
    }
    free(screen->screen);
}

void drawInScreen(screenStruct screen, int x, int y, const char ASCII) 
{
    if (x < 0) 
    {

        x = 0;
    }

    if (x > (screen.width-1)) 
    {

        x = screen.width-1;
    }

    if (y < 0) 
    {

        y = 0;
    }

    if (y > (screen.height-1)) 
    {

        y = screen.height-1;
    }

    screen.screen[x][y] = ASCII;
}

void displayScreen(screenStruct *screen)
{
    printf("\033[H\033[J"); //clears the screen
    // Iterate over y axis
    char outputString[screen->width+1];
    for (int y = 0; y < screen->height; y++)
    {
        for (int x = 0; x < screen->width; x++)
        {
            // Store current value in array at point(x,y), as char in string
            // String is length of screen.width
            outputString[x]=screen->screen[x][y];
        }
        // Display filled string, and new line character, before moving onto the next value of y
        outputString[screen->width]='\0';
        printf("%s\n",outputString);
    }
}

void plotLineLow(int x0, int y0, int x1, int y1, screenStruct screen)
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
        if ((x <= 0) | (y <= 0) | (x >= (screen.width-1)) | (y >= (screen.height-1))) 
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

void plotLineHigh(int x0, int y0, int x1, int y1, screenStruct screen)
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
        if ((x <= 0) | (y <= 0) | (x >= (screen.width-1)) | (y >= (screen.height-1))) 
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

void BresenhamPlotLine(vector pointA, vector pointB, screenStruct screen)
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