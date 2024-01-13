#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"

#define BLANK ' '
#define LINE '#'
#define DOT '@'
#define BORDER '*'

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

    returnVec.x = vec.x / scalar;
    returnVec.y = vec.y / scalar;
    returnVec.z = vec.z / scalar;

    return returnVec;
}

vector mulVecByScalar(vector vec, double scalar)
{
    vector returnVec;

    returnVec.x = vec.x * scalar;
    returnVec.y = vec.y * scalar;
    returnVec.z = vec.z * scalar;

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

vector vecNormaliseVector(vector inputVec)
{
    double length = sqrtl(vecDotProduct(inputVec, inputVec));

    vector returnVec = divVecByScalar(inputVec, length);
    
    return returnVec;
}

double   vecDotProduct(vector vec1, vector vec2)
{
    double output = (vec1.x * vec2.x) +
                    (vec1.y * vec2.y) +
                    (vec1.z * vec2.z);
    
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

//output = input, unless it exceeds min or max
int clamp(int input, int min, int max)
{
    int output = input;
    if (input < min)
    {
        output = min;
    }
    else if (input > max)
    {
        output = max;
    }
    return output;
}

void initProjectMat(renderConfig importData, mat4x4 *matProj)
{
    double near = 0.1;
    double far = 1000;
    double fov = importData.fov;
    double aspectRatio = ((double)importData.screenHeightImprt / (double)importData.screenWidthImprt);
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
    matProj->m[3][3] = 0;
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
    
    P.z = 1 / (((1/A.z) * a) + ((1/B.z) * b) + ((1/C.z) * c));
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

void drawTriOutline(triangle inputTri, screenStruct screen)
{
    for (int i = 0; i < 3; i++)
    {
        BresenhamPlotLine(inputTri.p[i], inputTri.p[((i + 1) % 3)], screen);
    }
}

void drawTriangleOnScreen(triangle inputTri, screenStruct screen, int fillBool)
{
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
    for (int y = bbmin.y; y < bbmax.y; y++) 
    {
        for (int x = bbmin.x; x < bbmax.x; x++)
        {
            int res = pixelInTriangle(inputTri,x,y,&z);
            if (res == 0)
            {
                continue;
            }
                        
            int xCheck = x;
            int yCheck = y;

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\tpixel (%d,%d) in triangle! Distance to triangle = %lf\n",x,y,z);
            printf("\tz(%lf) needs to be smaller than: %lf...\n",z,screen.zBuffer[x][y]);
            #endif
            
            //prevent segfault from attempting to draw outside screen bounds
            xCheck = clamp(x, 0, (screen.width -1));
            yCheck = clamp(y, 0, (screen.height -1));
            
            if (z > screen.zBuffer[xCheck][yCheck])
            {
                continue;;
            }

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\t\tAnd it is! ");
            printf("\tComputed z = %lf\n",z);
            printf("\t\tcurrent z at screen[%d][%d] = %lf\n",x,y,screen.zBuffer[x][y]);
            printf("\t\tcurrent z smaller than zbuffer, updating buffer...\n");
            #endif

            screen.zBuffer[xCheck][yCheck] = z;

            if (fillBool)
            {
                drawInScreen(screen, x, y, inputTri.symbol);
            }
            else
            {
                drawTriOutline(inputTri,screen);
            }

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\t\tscreen[%d][%d] = %c\n\n",x,y,inputTri.symbol);
            #endif
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
    lightDirection = vecNormaliseVector(lightDirection);

    double dp = vecDotProduct(inputTriNorm, lightDirection);

    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("lightDirection: (%lf, %lf, %lf)\n", lightDirection.x, lightDirection.y, lightDirection.z);
    printf("inputTriNorm: (%lf, %lf, %lf)\n\n", inputTriNorm.x, inputTriNorm.y, inputTriNorm.z);
    #endif

    inputTri->symbol = getGrad(dp);
}

char  getGrad(double lum)
{
    // " .:-=+*#%@"
    
    char outputChar;
    double grad = (9 * lum);
    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    switch ((int)(rint(grad)))
    {
    case 0:
        outputChar = '0';
        break;
    case 1:
        outputChar = '1';
        break;
    case 2:
        outputChar = '2';
        break;
    case 3: 
        outputChar = '3';
        break;
    case 4:
        outputChar = '4';
        break;
    case 5:
        outputChar = '5';
        break;
    case 6:
        outputChar = '6';
        break;
    case 7:
        outputChar = '7';
        break;
    case 8:
        outputChar = '8';
        break;
    case 9:
        outputChar = '9';
        break;
    default:
        outputChar = '?';
    }
    printf("Lum: %lf\tGrad: %lf\tChar: %c\n\n",lum, rint(9 * lum), outputChar);
    #endif

    #ifndef DEBUG_POINTS_LIGHT_LEVEL
    switch ((int)(rint(grad)))
    {
    case 0:
        outputChar = ' ';
        break;
    case 1:
        outputChar = '.';
        break;
    case 2:
        outputChar = ':';
        break;
    case 3: 
        outputChar = '-';
        break;
    case 4:
        outputChar = '=';
        break;
    case 5:
        outputChar = '+';
        break;
    case 6:
        outputChar = '*';
        break;
    case 7:
        outputChar = '#';
        break;
    case 8:
        outputChar = '%';
        break;
    case 9:
        outputChar = '@';
        break;
    default:
        outputChar = ' ';
    }
    #endif

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

void translateTriangleX(triangle *triToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triToTranslate->p[i].x = triToTranslate->p[i].x + distance;
    }
}

void translateTriangleY(triangle *triToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triToTranslate->p[i].y = triToTranslate->p[i].y + distance;
    }
}

void translateTriangleZ(triangle *triToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triToTranslate->p[i].z = triToTranslate->p[i].z + distance;
    }
}

vector calculateTriangleNormal(triangle inputTri)
{   
    vector U,V,normal;
    
    U = (subVec(inputTri.p[2],inputTri.p[1]));
    V = (subVec(inputTri.p[1],inputTri.p[0]));

    normal = vecCrossProduct(U, V);
    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("U: (%lf, %lf, %lf)\n", U.x, U.y, U.z);
    printf("V: (%lf, %lf, %lf)\n", V.x, V.y, V.z);
    printf("normal(before normalising): (%lf, %lf, %lf)\n", normal.x, normal.y, normal.z);
    #endif

    //its normally normal to normalise the normal
    normal = vecNormaliseVector(normal);
    
    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("normal(after normalising): (%lf, %lf, %lf)\n\n", normal.x, normal.y, normal.z);
    #endif

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
        }
    }
}

void drawScreenBorder(screenStruct *screen)
{
    for (int x = 0; x < screen->width; x++) 
    {
        for (int y = 0; y < screen->height; y++) 
        {
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

void displayZBuffer(screenStruct *screen)
{   
    #ifndef DEBUG_POINTS_NO_CLEARSCREEN
    printf("\033[H\033[J"); //clears the screen
    #endif
    // Iterate over y axis
    double outputString[screen->width+1];
    for (int y = 0; y < screen->height; y++)
    {
        for (int x = 0; x < screen->width; x++)
        {
            // Store current value in array at point(x,y), as char in string
            // String is length of screen.width
            outputString[x]=screen->zBuffer[x][y];
            printf("%lf,",outputString[x]);
        }
        // Display filled string, and new line character, before moving onto the next value of y
        // outputString[screen->width]='\0';
        printf("\n");
    }
}

void displayScreen(screenStruct *screen)
{   
    size_t sizeOfScreen = (sizeof(char)*((screen->width+1) * screen->height));
    #ifdef DEBUG_POINTS_NO_CLEARSCREEN
    printf("Screen Area: %d x %d = %d (%ld bytes)\n",screen->width, screen->height, (screen->width * screen->height),sizeOfScreen);
    printf("BUFFSIZ: %d\n",BUFSIZ);
    #endif

    #ifndef DEBUG_POINTS_NO_CLEARSCREEN
    //clears screen escape code sequence
    printf("\033[H\033[J"); 
    #endif

    setvbuf(stdout,NULL,_IOFBF,sizeOfScreen);

    char outputStringArr[sizeOfScreen];
    int invY = 0;
    for (int y = (screen->height-1); y >= 0; y--)
    {
        for (int x = 0; x < screen->width; x++)
        {
            //fancy maths to work out index of 1d array from 2d.
            outputStringArr[(invY * (screen->width+1))+x]=screen->screen[x][y];
        }
        //add newline char for each y incriment
        outputStringArr[(invY * (screen->width+1))+(screen->width)]='\n';
        invY++;
    }
    //write tring to print buffer, and fluse to screen.
    fwrite(outputStringArr,sizeof(char),sizeOfScreen,stdout);
    fflush(stdout);
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