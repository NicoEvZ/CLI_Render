#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"

#define BLANK ' ' //SPACE character ASCII code
#define LINE '#' //'#' character ASCII code
#define DOT '@'
#define BORDER '*'

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

int pixelInTriangle(triangle inputTri, int x, int y, double* z)
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
    
    P.z = 1/(((1/A.z) * a) + ((1/B.z) * b) + ((C.z * c)));
    // printf("P.x, P.y, P.z = %lf, %lf, %lf\n", P.x, P.y, P.z);
    *z = P.z;

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

    vector bbmin = {INFINITY, INFINITY, 0};
    vector bbmax = {-INFINITY, -INFINITY, 0};

    for (int i = 0; i < 3; i++)
    {
        if (inputTri.p[i].x < bbmin.x)
        {
            bbmin.x = inputTri.p[i].x;
        }
        if (inputTri.p[i].y < bbmin.y)
        {
            bbmin.y = inputTri.p[i].y;
        }
        if (inputTri.p[i].x > bbmax.x)
        {
            bbmax.x = inputTri.p[i].x;
        }
        if (inputTri.p[i].y > bbmax.y)
        {
            bbmax.y = inputTri.p[i].y;
        }
    }
    #ifdef DEBUG_POINTS_BBs
    printf("bbmin = (%lf,%lf)\tbbmax = (%lf,%lf)\n",bbmin.x,bbmin.y,bbmax.x,bbmax.y);
    #endif
    double z = 0;
    for (int x = bbmin.x; x < bbmax.x; x++)
    {
        for (int y = bbmin.y; y < bbmax.y; y++)
        {
            if (pixelInTriangle(inputTri,x,y,&z))
            {
                #ifdef DEBUG_POINTS_ZBUFFER
                printf("\tpixel (%d,%d) in triangle! Distance to triangle = %lf\n",x,y,z);
                printf("z(%lf) needs to be smalled than: %lf...\n",z,screen.zBuffer[x][y]);
                #endif
                if (z < screen.zBuffer[x][y])
                {
                    #ifdef DEBUG_POINTS_ZBUFFER
                    printf("And it is! ");
                    printf("\t\tComputed z = %lf\n",z);
                    printf("\t\tcurrent z at screen[%d][%d] = %lf\n",x,y,screen.zBuffer[x][y]);
                    printf("\t\tcurrent z smaller than zbuffer, updating buffer...\n\n");
                    #endif;
                    screen.zBuffer[x][y] = z;
                    drawInScreen(screen, x, y, inputTri.symbol);
                }
            }
        }
    }
    #ifdef DEBUG_POINTS_BBs
    vector boundingBoxP1;
    vector boundingBoxP2;
    vector boundingBoxP3;
    vector boundingBoxP4;
    boundingBoxP1.x = bbmin.x;
    boundingBoxP1.y = bbmin.y;
    boundingBoxP2.x = bbmin.x;
    boundingBoxP2.y = bbmax.y;
    boundingBoxP3.x = bbmax.x;
    boundingBoxP3.y = bbmax.y;
    boundingBoxP4.x = bbmax.x;
    boundingBoxP4.y = bbmin.y;
    BresenhamPlotLine(boundingBoxP1, boundingBoxP2, screen);
    BresenhamPlotLine(boundingBoxP2, boundingBoxP3, screen);
    BresenhamPlotLine(boundingBoxP3, boundingBoxP4, screen);
    BresenhamPlotLine(boundingBoxP4, boundingBoxP1, screen);
    #endif
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

void clearScreen(screenStruct *screen) 
{
    for (int x = 0; x < screen->width; x++) 
    {
        for (int y = 0; y < screen->height; y++) 
        {
            screen->screen[x][y]=BLANK;
            screen->zBuffer[x][y]=INFINITY;
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
    screen->zBuffer = malloc(screen->width* sizeof(double *));
    for (int i = 0; i < screen->width; i++)
    {
        screen->screen[i] = malloc(screen->height * sizeof(int));
        screen->zBuffer[i] = malloc(screen->height * sizeof(double));
    }

    for (int x = 0; x < screen->width; x++)
    {
        for (int y = 0; y < screen->height; y++)
        {
            screen->zBuffer[x][y] = INFINITY;
        }
    }
}

void deleteScreen(screenStruct *screen)
{
    for (int i = 0; i < screen->width; i++)
    {
        free(screen->screen[i]);
        free(screen->zBuffer[i]);
    }
    free(screen->screen);
    free(screen->zBuffer);
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
    #ifndef DEBUG_POINTS_NO_CLEARSCREEN
    printf("\033[H\033[J"); //clears the screen
    #endif
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