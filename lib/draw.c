#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>   
#include "draw.h"

#define BLANK ' '
#define LINE '#'
#define DOT '@'
#define BORDER '*'

void copyTriangleData(triangle fromTriangle, triangle *toTriangle)
{
    for (int i = 0; i < 3; i++)
    {
        toTriangle->point[i] = fromTriangle.point[i];
        toTriangle->symbol.colour[i] = fromTriangle.symbol.colour[i];
    }
    toTriangle->symbol.character = fromTriangle.symbol.character;
    toTriangle->symbol.brightness = fromTriangle.symbol.brightness;
}

void copyFrameBufferData(frameBuffer fromScreen, frameBuffer *toScreen)
{
    toScreen->height = fromScreen.height;
    toScreen->width = fromScreen.width;
    for (int x = 0; x < fromScreen.width; x++)
    {
        for (int y = 0; y < fromScreen.height; y++)
        {
            toScreen->characterBuffer[x][y] = fromScreen.characterBuffer[x][y];
            toScreen->depthBuffer[x][y] = fromScreen.depthBuffer[x][y];
            for (int i = 0; i < 3; i++)
            {
                toScreen->colourBuffer[x][y][i] = fromScreen.colourBuffer[x][y][i];
            }
        }
    }
}

void inheritColourFromMesh(int fromMeshColour[3], triangle *toTriangle)
{
    for (int i = 0; i < 3; i++)
    {
        toTriangle->symbol.colour[i] = fromMeshColour[i];
    }
}

void cycleMeshColour(mesh *object, int incriment, int totalSteps)
{
    object->colour[0] = 125.5 * sin(( incriment / ((double)totalSteps)) * 2 * PI) + 125.5;
    object->colour[1] = 125.5 * sin((( incriment / ((double)totalSteps)) * 2 * PI) + (1.33 * PI)) + 125.5;
    object->colour[2] = 125.5 * sin((( incriment / ((double)totalSteps)) * 2 * PI) + (0.66 * PI)) + 125.5; 

    /*
    when this was in runner.c it looked like:
    baseMesh.colour[0] = 125.5*sin((i/((double)importData.iterations))*2*PI)+125.5;
    baseMesh.colour[1] = 125.5*sin(((i/((double)importData.iterations))*2*PI)+(1.33*PI))+125.5;
    baseMesh.colour[2] = 125.5*sin(((i/((double)importData.iterations))*2*PI)+(0.66*PI))+125.5; 
    */
}

//vector1.elements plus vector2.elements
vector addVector(vector vector1, vector vector2) 
{
    vector returnVector;

    returnVector.x = vector1.x + vector2.x;
    returnVector.y = vector1.y + vector2.y;
    returnVector.z = vector1.z + vector2.z;

    return returnVector;
}

//vector1.elements minus vector2.elements
vector subtractVector(vector vector1, vector vector2) 
{
    vector returnVector;

    returnVector.x = vector1.x - vector2.x;
    returnVector.y = vector1.y - vector2.y;
    returnVector.z = vector1.z - vector2.z;

    return returnVector;
}

//divides each element of the vector by the value given
vector divideVectorByScalar(vector inputVector, double scalar) 
{
    vector returnVector;

    returnVector.x = inputVector.x / scalar;
    returnVector.y = inputVector.y / scalar;
    returnVector.z = inputVector.z / scalar;

    return returnVector;
}

vector multiplyVectorByScalar(vector inputVector, double scalar)
{
    vector returnVector;

    returnVector.x = inputVector.x * scalar;
    returnVector.y = inputVector.y * scalar;
    returnVector.z = inputVector.z * scalar;

    return returnVector;   
}

vector CrossProduct(vector vector1, vector vector2)
{
    vector output;

    output.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
    output.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
    output.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

    return output;
}

vector normaliseVector(vector inputVec)
{
    if (inputVec.x == 0 && inputVec.y == 0 && inputVec.z == 0)
    {
        return inputVec;
    }
    double length = sqrtl(dotProduct(inputVec, inputVec));

    // printf("normaliseVector length = %lf\n", length);

    vector returnVector = divideVectorByScalar(inputVec, length);
    
    return returnVector;
}

double dotProduct(vector vector1, vector vector2)
{
    double output = (vector1.x * vector2.x) +
                    (vector1.y * vector2.y) +
                    (vector1.z * vector2.z);
    
    return output;
}

triangle matrixTriangleMultiply(triangle input, matrix4x4 inputMatrix)
{
    triangle output = input;
    for (int i = 0; i < 3; i++)
    {
        output.point[i].x = (input.point[i].x * inputMatrix.matrix[0][0]) + 
                            (input.point[i].y * inputMatrix.matrix[1][0]) + 
                            (input.point[i].z * inputMatrix.matrix[2][0]) + 
                            (inputMatrix.matrix[3][0]);

        output.point[i].y = (input.point[i].x * inputMatrix.matrix[0][1]) + 
                            (input.point[i].y * inputMatrix.matrix[1][1]) + 
                            (input.point[i].z * inputMatrix.matrix[2][1]) + 
                            (inputMatrix.matrix[3][1]);

        output.point[i].z = (input.point[i].x * inputMatrix.matrix[0][2]) + 
                            (input.point[i].y * inputMatrix.matrix[1][2]) + 
                            (input.point[i].z * inputMatrix.matrix[2][2]) + 
                            (inputMatrix.matrix[3][2]);

        double w      = (input.point[i].x * inputMatrix.matrix[0][3]) + 
                        (input.point[i].y * inputMatrix.matrix[1][3]) + 
                        (input.point[i].z * inputMatrix.matrix[2][3]) + 
                        (inputMatrix.matrix[3][3]);

        if (w != 0.0)
        {
            output.point[i] = divideVectorByScalar(output.point[i],w);
        }
    }
    return output;
}

vector matrixVectorMultiply(vector input, matrix4x4 inputMatrix)
{
    vector output;
    
    output.x = (input.x * inputMatrix.matrix[0][0]) + 
                (input.y * inputMatrix.matrix[1][0]) + 
                (input.z * inputMatrix.matrix[2][0]) + 
                (inputMatrix.matrix[3][0]);

    output.y = (input.x * inputMatrix.matrix[0][1]) + 
                (input.y * inputMatrix.matrix[1][1]) + 
                (input.z * inputMatrix.matrix[2][1]) + 
                (inputMatrix.matrix[3][1]);

    output.z = (input.x * inputMatrix.matrix[0][2]) + 
                (input.y * inputMatrix.matrix[1][2]) + 
                (input.z * inputMatrix.matrix[2][2]) + 
                (inputMatrix.matrix[3][2]);

    double w = (input.x * inputMatrix.matrix[0][3]) + 
                (input.y * inputMatrix.matrix[1][3]) + 
                (input.z * inputMatrix.matrix[2][3]) + 
                (inputMatrix.matrix[3][3]);

    if (w != 0.0)
    {
        output = divideVectorByScalar(output,w);
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

double clampDouble(double input, double min, double max)
{
    double output = input;
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

void initialiseProjectionMatrix(renderConfig importData, matrix4x4 *ProjectionMatrix)
{
    double near = 0.1;
    double far = 1000;
    double fov = importData.fov;
    double aspectRatio = ((double)importData.screenHeightImport / (double)importData.screenWidthImport);
    double fovRadians = 1 / tanf(fov * 0.5 / 180 * PI);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ProjectionMatrix->matrix[i][j] = 0;
        }
    }

    ProjectionMatrix->matrix[0][0] = aspectRatio * fovRadians;
    ProjectionMatrix->matrix[1][1] = fovRadians;
    ProjectionMatrix->matrix[2][2] = far / (far - near);
    ProjectionMatrix->matrix[3][2] = (-far * near) / (far - near);
    ProjectionMatrix->matrix[2][3] = 1;
    ProjectionMatrix->matrix[3][3] = 0;
}

int checkColourOfPixelInTriangle(triangle *inputTriangle, int x, int y, double* z)
{
    vector A = inputTriangle->point[0];
    vector B = inputTriangle->point[1];
    vector C = inputTriangle->point[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
    
    *z = 1 / (((1 / A.z) * a) + ((1 / B.z) * b) + ((1 / C.z) * c));

    // int rgbArray[3];

    // rgbArray[0] = (int)rint(a * 255);
    // rgbArray[1] = (int)rint(b * 255);
    // rgbArray[2] = (int)rint(c * 255);

    // for (int i = 0; i < 3; i++)
    // {
    //     inputTriangle->symbol.colour[i] = clamp(rgbArray[i], 0, 255);
    // }

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

int checkPixelInTriangle(triangle inputTriangle, int x, int y, double* z)
{
    vector A = inputTriangle.point[0];
    vector B = inputTriangle.point[1];
    vector C = inputTriangle.point[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
    
    P.z = 1 / (((1 / A.z) * a) + ((1 / B.z) * b) + ((1 / C.z) * c));
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

void drawTriangleOutline(triangle inputTriangle, frameBuffer *screen)
{
    for (int i = 0; i < 3; i++)
    {
        BresenhamPlotLine(inputTriangle.point[i], inputTriangle.point[((i + 1) % 3)], screen);
    }
}

void drawTriangleOnScreen(triangle inputTriangle, frameBuffer *screen, int fillBool)
{
    vector boundingBoxMin = {INFINITY, INFINITY, 0};
    vector boundingBoxMax = {-INFINITY, -INFINITY, 0};

    for (int i = 0; i < 3; i++)
    {
        if (inputTriangle.point[i].x < boundingBoxMin.x)
        {
            boundingBoxMin.x = inputTriangle.point[i].x;
        }
        if (inputTriangle.point[i].y < boundingBoxMin.y)
        {
            boundingBoxMin.y = inputTriangle.point[i].y;
        }
        if (inputTriangle.point[i].x > boundingBoxMax.x)
        {
            boundingBoxMax.x = inputTriangle.point[i].x;
        }
        if (inputTriangle.point[i].y > boundingBoxMax.y)
        {
            boundingBoxMax.y = inputTriangle.point[i].y;
        }
    }

    #ifdef DEBUG_POINTS_BBs
    printf("boundingBoxMin = (%lf,%lf)\tboundingBoxMax = (%lf,%lf)\n",boundingBoxMin.x,boundingBoxMin.y,boundingBoxMax.x,boundingBoxMax.y);
    #endif
    
    double z = 0;
    for (int y = boundingBoxMin.y; y < boundingBoxMax.y; y++) 
    {
        for (int x = boundingBoxMin.x; x < boundingBoxMax.x; x++)
        {
            int result = checkColourOfPixelInTriangle(&inputTriangle,x,y,&z);
            if (result == 0)
            {
                continue;
            }
                        
            int xCheck = x;
            int yCheck = y;

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\tpixel (%d,%d) in triangle! Distance to triangle = %lf\n",x,y,z);
            printf("\tz(%lf) needs to be smaller than: %lf...\n",z,screen.depthBuffer[x][y]);
            #endif
            
            //prevent segfault from attempting to draw outside screen bounds
            xCheck = clamp(x, 0, (screen->width -1));
            yCheck = clamp(y, 0, (screen->height -1));
            
            if (z > screen->depthBuffer[xCheck][yCheck])
            {
                continue;;
            }

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\t\tAnd it is! ");
            printf("\tComputed z = %lf\n",z);
            printf("\t\tcurrent z at screen[%d][%d] = %lf\n",x,y,screen.depthBuffer[x][y]);
            printf("\t\tcurrent z smaller than zbuffer, updating buffer...\n");
            #endif

            screen->depthBuffer[xCheck][yCheck] = z;

            if (fillBool)
            {
                drawInScreen(screen, x, y, inputTriangle.symbol);
            }
            else
            {
                drawTriangleOutline(inputTriangle, screen);
            }

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\t\tscreen[%d][%d] = %c\n\n",x,y,inputTriangle.symbol);
            #endif
        }
    }
    #ifdef DEBUG_POINTS_BBs
    vector boundingBoxP1;
    vector boundingBoxP2;
    vector boundingBoxP3;
    vector boundingBoxP4;
    boundingBoxP1.x = boundingBoxMin.x;
    boundingBoxP1.y = boundingBoxMin.y;
    boundingBoxP2.x = boundingBoxMin.x;
    boundingBoxP2.y = boundingBoxMax.y;
    boundingBoxP3.x = boundingBoxMax.x;
    boundingBoxP3.y = boundingBoxMax.y;
    boundingBoxP4.x = boundingBoxMax.x;
    boundingBoxP4.y = boundingBoxMin.y;
    BresenhamPlotLine(boundingBoxP1, boundingBoxP2, screen);
    BresenhamPlotLine(boundingBoxP2, boundingBoxP3, screen);
    BresenhamPlotLine(boundingBoxP3, boundingBoxP4, screen);
    BresenhamPlotLine(boundingBoxP4, boundingBoxP1, screen);
    #endif
}

void illuminateTriangle(triangle *inputTriangle, vector inputTriangleNormal, vector lightDirection)
{
    lightDirection = normaliseVector(lightDirection);

    double luminance = dotProduct(inputTriangleNormal, lightDirection);

    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("lightDirection: (%lf, %lf, %lf)\n", lightDirection.x, lightDirection.y, lightDirection.z);
    printf("inputTriangleNormal: (%lf, %lf, %lf)\n\n", inputTriangleNormal.x, inputTriangleNormal.y, inputTriangleNormal.z);
    #endif

    // int colour[3] = {255, 0, 0};

    getGradient(luminance, &inputTriangle->symbol);

    // for (int i = 0; i < 3; i++)
    // {
    //     inputTriangle->symbol.colour[i] = colour[i];
    // }
}

void getGradient (double luminance, visual *inputSymbol)
{
    inputSymbol->character = getGradientCharacter(luminance);
    inputSymbol->brightness = clampDouble(luminance, 0, 1);
}

char  getGradientCharacter(double luminance)
{
    // " .:-=+*#%@"
    
    char outputCharacter;

    double gradient = (9 * luminance);
    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    char gradientString[] = "0123456789";
    char defaultChar = '?';
    #endif

    #ifndef DEBUG_POINTS_LIGHT_LEVEL
    char gradientString[] = " .:-=+*#%@";
    char defaultChar = ' ';
    #endif

    if (luminance < 0)
    {
        outputCharacter = defaultChar;
    }
    else
    {
        // printf("gradient = %lf\n", gradient);
        outputCharacter = gradientString[(int)(rint(gradient))];
    }

    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("Lum: %lf\tGrad: %lf\tChar: %c\n\n",luminance, rint(9 * luminance), outputCharacter);
    #endif

    return outputCharacter;
}

void scaleTriangle(triangle *inputTriangle, frameBuffer screen)
{
    for (int i = 0; i < 3; i++)
    {
        inputTriangle->point[i].x += 1;
        inputTriangle->point[i].y += 1;

        inputTriangle->point[i].x *= (0.5 * (double)screen.width);
        inputTriangle->point[i].y *= (0.5 * (double)screen.height);
    }
}

void initialiseRotateXMatrix(matrix4x4 * rotateX, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotateX->matrix[i][j] = 0;
        }
    }

    rotateX->matrix[0][0] = 1;
    rotateX->matrix[1][1] = cosf(angle);
    rotateX->matrix[1][2] = sinf(angle);
    rotateX->matrix[2][1] = -sinf(angle);
    rotateX->matrix[2][2] = cosf(angle);
    rotateX->matrix[3][3] = 1;
}

void initialiseRotateYMatrix(matrix4x4 * rotateY, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotateY->matrix[i][j] = 0;
        }
    }

    rotateY->matrix[0][0] = cosf(angle);;
    rotateY->matrix[0][2] = -sinf(angle);
    rotateY->matrix[1][1] = 1;
    rotateY->matrix[2][0] = sinf(angle);
    rotateY->matrix[2][2] = cosf(angle);
    rotateY->matrix[3][3] = 1;
}

void initialiseRotateZMatrix(matrix4x4 * rotateZ, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotateZ->matrix[i][j] = 0;
        }
    }

    rotateZ->matrix[0][0] = cosf(angle);
    rotateZ->matrix[0][1] = sinf(angle);
    rotateZ->matrix[1][0] = -sinf(angle);
    rotateZ->matrix[1][1] = cosf(angle);
    rotateZ->matrix[2][2] = 1;
    rotateZ->matrix[3][3] = 1;
}

void translateTriangleX(triangle *triangleToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triangleToTranslate->point[i].x = triangleToTranslate->point[i].x + distance;
    }
}

void translateTriangleY(triangle *triangleToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triangleToTranslate->point[i].y = triangleToTranslate->point[i].y + distance;
    }
}

void translateTriangleZ(triangle *triangleToTranslate, double distance)
{
    for (int i = 0; i < 3; i++)
    {
        triangleToTranslate->point[i].z = triangleToTranslate->point[i].z + distance;
    }
}

vector calculateTriangleNormal(triangle inputTriangle)
{   
    vector U,V,normal;
    
    U = (subtractVector(inputTriangle.point[2],inputTriangle.point[1]));
    V = (subtractVector(inputTriangle.point[1],inputTriangle.point[0]));

    normal = CrossProduct(U, V);
    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("U: (%lf, %lf, %lf)\n", U.x, U.y, U.z);
    printf("V: (%lf, %lf, %lf)\n", V.x, V.y, V.z);
    printf("normal(before normalising): (%lf, %lf, %lf)\n", normal.x, normal.y, normal.z);
    #endif

    //its normally normal to normalise the normal
    normal = normaliseVector(normal);
    
    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("normal(after normalising): (%lf, %lf, %lf)\n\n", normal.x, normal.y, normal.z);
    #endif

    return normal;
}

void clearFrameBuffer(frameBuffer *screen) 
{
    for (int x = 0; x < screen->width; x++) 
    {
        for (int y = 0; y < screen->height; y++) 
        {
            screen->characterBuffer[x][y]=BLANK;
            screen->depthBuffer[x][y]=INFINITY;
            for (int i = 0; i < 3; i++)
            {
                screen->colourBuffer[x][y][i] = 127;
            }
        }
    }
}

void drawScreenBorder(frameBuffer *screen)
{
    for (int x = 0; x < screen->width; x++) 
    {
        for (int y = 0; y < screen->height; y++) 
        {
            if ((x == 0) | (y == 0) | (x == (screen->width-1)) | (y == (screen->height-1))) 
            {   
                screen->characterBuffer[x][y]=BORDER;
                for (int i = 0; i < 3; i++)
                {
                    screen->colourBuffer[x][y][i]=255;
                }
            }
        }
    }
}

void initialiseFrameBuffer(frameBuffer *screen, renderConfig importData)
{
    screen->width = importData.screenWidthImport;
    screen->height = importData.screenHeightImport;

    screen->characterBuffer = malloc(screen->width * sizeof(int *));
    screen->colourBuffer = malloc(screen->width * sizeof(int **));
    screen->depthBuffer = malloc(screen->width* sizeof(double *));
    for (int i = 0; i < screen->width; i++)
    {
        screen->characterBuffer[i] = malloc(screen->height * sizeof(int));
        screen->colourBuffer[i] = malloc(screen->height * sizeof(int *));
        screen->depthBuffer[i] = malloc(screen->height * sizeof(double));
        for (int j = 0; j < screen->height; j++)
        {
            screen->colourBuffer[i][j] =  malloc(3 * sizeof(int));
        }
    }

    for (int x = 0; x < screen->width; x++)
    {
        for (int y = 0; y < screen->height; y++)
        {
            for (int j = 0; j < 3; j++)
            {
                screen->colourBuffer[x][y][j] = 232;
            }
            screen->depthBuffer[x][y] = INFINITY;
        }
    }

    clearFrameBuffer(screen);
}

void deleteFrameBuffer(frameBuffer *screen)
{
    for (int i = 0; i < screen->width; i++)
    {
        for (int j = 0; j < screen->height; j++)
        {
            free(screen->colourBuffer[i][j]);
        }
        free(screen->characterBuffer[i]);
        free(screen->colourBuffer[i]);
        free(screen->depthBuffer[i]);
    }
    free(screen->characterBuffer);
    free(screen->colourBuffer);
    free(screen->depthBuffer);
}

void drawInScreen(frameBuffer *screen, int x, int y, visual symbol)
{
    x = clamp(x, 0, (screen->width - 1));
    y = clamp(y, 0, (screen->height - 1));

    if ((screen->characterBuffer[x][y] == symbol.character) && 
        (screen->colourBuffer[x][y][0] == ((int)rint(symbol.colour[0] * symbol.brightness))) &&
        (screen->colourBuffer[x][y][1] == ((int)rint(symbol.colour[1] * symbol.brightness))) &&
        (screen->colourBuffer[x][y][2] == ((int)rint(symbol.colour[2] * symbol.brightness))))
    {
        return;
    }

    screen->characterBuffer[x][y] = symbol.character;
    
    for (int i = 0; i < 3; i++)
    {
        screen->colourBuffer[x][y][i] = clamp(((int)rint(symbol.colour[i] * symbol.brightness)), 0, 255);
    }
}

void displayDepthBuffer(frameBuffer screen, frameBuffer oldScreen)
{   
    // #ifdef DEBUG_POINTS_NO_CLEARSCREEN
    // printf("\033[H\033[J"); //clears the screen
    // #endif
    // // Iterate over y axis
    // double outputString[screen->width+1];
    // for (int y = 0; y < screen->height; y++)
    // {
    //     for (int x = 0; x < screen->width; x++)
    //     {
    //         // Store current value in array at point(x,y), as char in string
    //         // String is length of screen.width
    //         outputString[x]=screen->depthBuffer[x][y];
    //         printf("%lf,",outputString[x]);
    //     }
    //     // Display filled string, and new line character, before moving onto the next value of y
    //     // outputString[screen->width]='\0';
    //     printf("\n");
    // }

    printf("\e[?25l");

    visual outputSymbol;
    int invertedY = 0;
    for (int y = (screen.height-1); y >= 0; y--)
    {
        int invertedX = 0;
        for (int x = (screen.width-1); x >= 0; x--)
        {
            outputSymbol.colour[0] = (int)rint(clampDouble((screen.depthBuffer[x][y]),0,255));
            outputSymbol.colour[1] = (int)rint(clampDouble((screen.depthBuffer[x][y]),0,255));
            outputSymbol.colour[2] = (int)rint(clampDouble((screen.depthBuffer[x][y]),0,255));

            if (screen.depthBuffer[x][y] != oldScreen.depthBuffer[x][y])
            {
                //escape code sequence for moving cursor, and printing a coloured ' '
                printf("\e[%d;%dH\e[48;2;%d;%d;%dm%c\e[m", invertedY+1, 
                                                           invertedX+1, 
                                                           outputSymbol.colour[0], 
                                                           outputSymbol.colour[1], 
                                                           outputSymbol.colour[2], 
                                                           ' ');
            }
            invertedX++;
        }
        invertedY++;
    }
    //reset cursor and style, and flush the buffer.
    printf("\e[%d;%dH", screen.height, screen.width);
    printf("\e[m\e[?25h");
    fflush(stdout);
}

void displayFrameBuffer2(frameBuffer screen, frameBuffer oldScreen)
{   
    #ifdef DEBUG_POINTS_NO_CLEARSCREEN
    // printf("Screen Area: (%d + 1 + 17) x (%d + 1) = %d (%ld bytes)\n",screen.width, screen.height, (screen.width + 18 * screen.height +1),sizeOfScreen);
    // printf("BUFFSIZ: %d\n",BUFSIZ);
    #endif

    int invertedY = 0;
    for (int y = (screen.height-1); y >= 0; y--)
    {
        int invertedX = 0;
        for (int x = (screen.width-1); x >= 0; x--)
        {
            if ((screen.colourBuffer[x][y][0] != oldScreen.colourBuffer[x][y][0]) ||
                (screen.colourBuffer[x][y][1] != oldScreen.colourBuffer[x][y][1]) ||
                (screen.colourBuffer[x][y][2] != oldScreen.colourBuffer[x][y][2]))
            {
                //escape code sequence for moving cursor, and printing a coloured ' '
                printf("\e[%d;%dH\e[48;2;%d;%d;%dm%c\e[m", invertedY+1, 
                                                           invertedX+1, 
                                                           screen.colourBuffer[x][y][0], 
                                                           screen.colourBuffer[x][y][1], 
                                                           screen.colourBuffer[x][y][2], 
                                                           ' ');
            }
            invertedX++;
        }
        invertedY++;
    }
    //reset cursor and style, and flush the buffer.
    printf("\e[%d;%dH", screen.height, screen.width);
    fflush(stdout);
}

void displayFrameBuffer(frameBuffer *screen)
{   
    size_t sizeOfScreen = (sizeof(char) * ((screen->width + 1) * screen->height));
    #ifdef DEBUG_POINTS_NO_CLEARSCREEN
    printf("Screen Area: (%d + 1) x %d = %d (%ld bytes)\n",screen->width + 1, screen->height, (screen->width +1 * screen->height),sizeOfScreen);
    printf("BUFFSIZ: %d\n",BUFSIZ);
    #endif
    printf("\e[?25l");
    #ifndef DEBUG_POINTS_NO_CLEARSCREEN
    //clears screen escape code sequence
    // printf("\033[H\033[J"); 
    #endif
    char outputStringArr[sizeOfScreen];
    int invertedY = 0;
    for (int y = (screen->height - 1); y >= 0; y--)
    {
        int invertedX = 0;
        for (int x = (screen->width - 1); x >= 0; x--)
        {
            //fancy maths to work out index of 1d array from 2d.
            outputStringArr[(invertedY * (screen->width + 1)) + invertedX] = screen->characterBuffer[x][y];
            invertedX++;
        }
        //add newline char for each y incriment
        outputStringArr[(invertedY * (screen->width + 1)) + (screen->width)] = '\n';
        invertedY++;
    }
    //write tring to print buffer, and fluse to screen.
    printf("\e[1;1;H");
    fwrite(outputStringArr, sizeof(char), sizeOfScreen, stdout);
    fflush(stdout);
    printf("\e[m\e[?25h");
}

void plotLineLow(int x0, int y0, int x1, int y1, frameBuffer *screen)
{
    visual outputSymbol;

    outputSymbol.colour[0] = 255;
    outputSymbol.colour[1] = 255;
    outputSymbol.colour[2] = 255;

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
        if ((x <= 0) | (y <= 0) | (x >= (screen->width-1)) | (y >= (screen->height-1))) 
        {
            outputSymbol.character = BORDER;
            drawInScreen(screen, x, y, outputSymbol);
        }
        else
        {
            outputSymbol.character = LINE;
            drawInScreen(screen, x, y, outputSymbol);
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

void plotLineHigh(int x0, int y0, int x1, int y1, frameBuffer *screen)
{
    visual outputSymbol;

    outputSymbol.colour[0] = 255;
    outputSymbol.colour[1] = 255;
    outputSymbol.colour[2] = 255;

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
        if ((x <= 0) | (y <= 0) | (x >= (screen->width-1)) | (y >= (screen->height-1))) 
        {
            outputSymbol.character = BORDER;
            drawInScreen(screen, x, y, outputSymbol);
        }
        else
        {
            outputSymbol.character = LINE;
            drawInScreen(screen, x, y, outputSymbol);
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

void BresenhamPlotLine(vector pointA, vector pointB, frameBuffer *screen)
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

void frameDelay(double framesPerSecond)
{   
    long int delayTimeNanoSeconds = 0;
    long int delayTimeSeconds = 0;
    if (framesPerSecond > 1)
    {
        delayTimeNanoSeconds = (long)rint((1 / framesPerSecond) * 1000000000);
    }
    else
    {
        delayTimeSeconds = (long)rint(1 / framesPerSecond);
    }

    nanosleep((const struct timespec[]){{delayTimeSeconds, delayTimeNanoSeconds}}, NULL);
}