#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>   
#include <curses.h>

#include "draw.h"

#define BLANK ' '
#define LINE '#'
#define DOT '@'
#define BORDER '*'

void copyTriangleData(triangle fromTriangle, triangle* toTriangle)
{
    for (int i = 0; i < 3; i++)
    {
        toTriangle->point[i] = fromTriangle.point[i];
        toTriangle->symbol.colour[i] = fromTriangle.symbol.colour[i];
    }
    toTriangle->symbol.character = fromTriangle.symbol.character;
    toTriangle->symbol.brightness = fromTriangle.symbol.brightness;
}

void copyFrameBufferData(frameBuffer fromFrame, frameBuffer* toFrame)
{
    toFrame->height = fromFrame.height;
    toFrame->width = fromFrame.width;
    for (int x = 0; x < fromFrame.width; x++)
    {
        for (int y = 0; y < fromFrame.height; y++)
        {
            toFrame->characterBuffer[x][y] = fromFrame.characterBuffer[x][y];
            toFrame->depthBuffer[x][y] = fromFrame.depthBuffer[x][y];
            for (int i = 0; i < 3; i++)
            {
                toFrame->colourBuffer[x][y][i] = fromFrame.colourBuffer[x][y][i];
            }
        }
    }
}

void setTriangleColour(int colour[3], triangle* Triangle)
{
    for (int i = 0; i < 3; i++)
    {
        Triangle->symbol.colour[i] = colour[i];
    }
}

void cycleMeshColour(mesh* object, int incriment, int totalSteps)
{
    double cycleRatio = (incriment / ((double)totalSteps)) * 2 * PI;

    object->colour[0] = 125.5 * sin(cycleRatio) + 125.5;
    object->colour[1] = 125.5 * sin(cycleRatio + ONE_AND_THIRD_PI) + 125.5;
    object->colour[2] = 125.5 * sin(cycleRatio + TWO_THIRDS_PI) + 125.5; 
}

//output = input, unless it exceeds min or max
int clamp(int input, int min, int max)
{
    if (input < min)
    {
        return min;
    }
    else if (input > max)
    {
        return max;
    }
    return input;
}

double clampDouble(double input, double min, double max)
{
    if (input < min)
    {
        return min;
    }
    else if (input > max)
    {
        return max;
    }
    return input;
}

int checkPixelInTriangle(triangle* inputTriangle, int x, int y, double* z)
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
    
    // printf("a = %f, b = %f, c = %f",a,b,c);

    //find depth of point on triangle for pixel on frame
    *z = 1 / (((1 / A.z) * a) + ((1 / B.z) * b) + ((1 / C.z) * c));

    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        #ifdef DEBUG_TRI_COLOUR
        inputTriangle->symbol.colour[0] = clamp((int)(255 * a), 0, 255);
        inputTriangle->symbol.colour[1] = clamp((int)(255 * b), 0, 255);
        inputTriangle->symbol.colour[2] = clamp((int)(255 * c), 0, 255);
        #endif
        
        return 1;
    } 
    else
    {
        return 0;
    }
}

void drawTriangleOutline(triangle inputTriangle, frameBuffer* frame)
{
    for (int i = 0; i < 3; i++)
    {
        BresenhamPlotLine(inputTriangle.point[i], inputTriangle.point[((i + 1) % 3)], frame);
    }
}

void drawTriangleOnFrame(triangle inputTriangle, frameBuffer* frame, int fillBool)
{
    vector boundingBoxMin, boundingBoxMax;
    initialiseVector(&boundingBoxMin);
    initialiseVector(&boundingBoxMax);
    boundingBoxMin = (vector){.x = INFINITY, .y = INFINITY};
    boundingBoxMax = (vector){.x = -INFINITY, .y = -INFINITY};
    
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
            // int result = checkColourOfPixelInTriangle(&inputTriangle,x,y,&z);
            // setCursorBelowFrame(*frame);
            int result = checkPixelInTriangle(&inputTriangle,x,y,&z);
            if (result == 0)
            {
                #ifdef DEBUG_POINTS_BBs
                printf("x:%d, y:%d of bounding box in triangle? NO , moving to next...\n",x,y);
                #endif   
                continue;
            }

            #ifdef DEBUG_POINTS_BBs
            printf("x:%d, y:%d of bounding box in triangle? YES, moving to next...\n",x,y);
            #endif 
                        
            int xCheck = x;
            int yCheck = y;

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\tpixel (%d,%d) in triangle! Distance to triangle = %lf\n",x,y,z);
            printf("\tz(%lf) needs to be smaller than: %lf...\n",z,frame->depthBuffer[x][y]);
            #endif
            
            //prevent segfault from attempting to draw outside frame bounds
            xCheck = clamp(x, 0, (frame->width -1));
            yCheck = clamp(y, 0, (frame->height -1));
            
            // skip drawing pixel if it appears behind something already in the depth buffer
            if (z > frame->depthBuffer[xCheck][yCheck])
            {
                //if pixel depth further than current depth, dont draw pixel
                continue;
            }

            setDepthValue(frame, xCheck, yCheck, &z);

            if (fillBool)
            {
                drawInFrame(frame, xCheck, yCheck, inputTriangle.symbol);
                #ifdef DEBUG_POINTS_RENDER_INDIVIDUAL
                displayFrameBufferSlowColour(frame);
                fflush(stdout);
                // frameDelay(60);
                #endif
            }
            else
            {
                drawTriangleOutline(inputTriangle, frame);
            }

            #ifdef DEBUG_POINTS_ZBUFFER
            printf("\t\frame[%d][%d] = %c\n\n",x,y,inputTriangle.symbol);
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
    BresenhamPlotLine(boundingBoxP2, boundingBoxP3, frame);
    BresenhamPlotLine(boundingBoxP1, boundingBoxP2, frame);
    BresenhamPlotLine(boundingBoxP3, boundingBoxP4, frame);
    BresenhamPlotLine(boundingBoxP4, boundingBoxP1, frame);
    #endif
}

void setDepthValue(frameBuffer* frame, int x, int y, double* z)
{
    #ifdef DEBUG_POINTS_ZBUFFER
    printf("\tpixel (%d,%d) in triangle! Distance to triangle = %lf\n",x,y,z);
    printf("\tz(%lf) needs to be smaller than: %lf...\n",z,frame->depthBuffer[x][y]);
    #endif

    if (*z < frame->depthMinimum)
    {
        frame->depthMinimum = *z;
    }

    if (*z > frame->depthMaximum)
    {
        frame->depthMaximum = *z;
    } 

    #ifdef DEBUG_POINTS_ZBUFFER
    printf("\t\tAnd it is! ");
    printf("\tComputed z = %lf\n",z);
    printf("\t\tcurrent z at frame[%d][%d] = %lf\n",x,y,frame->depthBuffer[x][y]);
    printf("\t\tcurrent z smaller than zbuffer, updating buffer...\n");
    #endif

    frame->depthBuffer[x][y] = *z;
}

void illuminateTriangle(triangle* inputTriangle, vector inputTriangleNormal, vector lightDirection)
{
    lightDirection = normaliseVector(lightDirection);

    double luminance = dotProduct(inputTriangleNormal, lightDirection);

    #ifdef DEBUG_POINTS_LIGHT_LEVEL
    printf("lightDirection: (%lf, %lf, %lf)\n", lightDirection.x, lightDirection.y, lightDirection.z);
    printf("inputTriangleNormal: (%lf, %lf, %lf)\n\n", inputTriangleNormal.x, inputTriangleNormal.y, inputTriangleNormal.z);
    #endif

    getGradient(luminance, &inputTriangle->symbol);
}

void getGradient (double luminance, visual* inputSymbol)
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

void scaleTriangle(triangle* inputTriangle, frameBuffer frame)
{
    vector offset;
    initialiseVector(&offset);
    offset = (vector){.x = 1, .y = 1};
    for (int i = 0; i < 3; i++)
    {
        inputTriangle->point[i] = addVector(inputTriangle->point[i], offset);

        inputTriangle->point[i].x *= (0.5 * (double)frame.width);
        inputTriangle->point[i].y *= (0.5 * (double)frame.height);
    }
}

vector calculateTriangleNormal(triangle inputTriangle)
{   
    vector U,V,normal;
    initialiseVector(&U);
    initialiseVector(&V);
    initialiseVector(&normal);

    
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

void clearFrameBuffer(frameBuffer* frame) 
{
    for (int x = 0; x < frame->width; x++) 
    {
        for (int y = 0; y < frame->height; y++) 
        {
            frame->characterBuffer[x][y]=BLANK;
            frame->depthBuffer[x][y]=1000;
            frame->depthMinimum = 1000;
            frame->depthMaximum = 0;
            for (int i = 0; i < 3; i++)
            {
                frame->colourBuffer[x][y][i] = 127;
            }
        }
    }
}

void drawFrameBorder(frameBuffer* frame)
{
    for (int x = 0; x < frame->width; x++) 
    {
        for (int y = 0; y < frame->height; y++) 
        {
            if ((x == 0) | (y == 0) | (x == (frame->width-1)) | (y == (frame->height-1))) 
            {   
                frame->characterBuffer[x][y]=BORDER;
                for (int i = 0; i < 3; i++)
                {
                    frame->colourBuffer[x][y][i]=255;
                }
            }
        }
    }
}

void initialiseFrameBuffer(frameBuffer* frame, renderConfig importData)
{
    frame->width = importData.frameColumnsImport;
    frame->height = importData.frameRowsImport*2;


    frame->depthMinimum = 0;
    frame->depthMaximum = 1000;

    frame->characterBuffer = malloc(frame->width * sizeof(int*));
    frame->colourBuffer = malloc(frame->width * sizeof(int**));
    frame->depthBuffer = malloc(frame->width * sizeof(double*));

    for (int i = 0; i < frame->width; i++)
    {
        frame->characterBuffer[i] = malloc(frame->height * sizeof(int));
        frame->colourBuffer[i] = malloc(frame->height * sizeof(int*));
        frame->depthBuffer[i] = malloc(frame->height * sizeof(double));
        for (int j = 0; j < frame->height; j++)
        {
            frame->colourBuffer[i][j] =  malloc(3 * sizeof(int));
        }
    }

    for (int x = 0; x < frame->width; x++)
    {
        for (int y = 0; y < frame->height; y++)
        {
            for (int j = 0; j < 3; j++)
            {
                frame->colourBuffer[x][y][j] = 232;
            }
            frame->depthBuffer[x][y] = 1000;
        }
    }

    clearFrameBuffer(frame);
}

void deleteFrameBuffer(frameBuffer* frame)
{
    for (int i = 0; i < frame->width; i++)
    {
        for (int j = 0; j < frame->height; j++)
        {
            free(frame->colourBuffer[i][j]);
        }
        free(frame->characterBuffer[i]);
        free(frame->colourBuffer[i]);
        free(frame->depthBuffer[i]);
    }
    free(frame->characterBuffer);
    free(frame->colourBuffer);
    free(frame->depthBuffer);
}

double map (double input, double input_start, double input_end, double output_start, double output_end)
{
   return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
}

void drawInFrame(frameBuffer* frame, int x, int y, visual symbol)
{
    x = clamp(x, 0, (frame->width - 1));
    y = clamp(y, 0, (frame->height - 1));

    // if ((frame->characterBuffer[x][y] == symbol.character) && 
    //     (frame->colourBuffer[x][y][0] == ((int)rint(symbol.colour[0] * symbol.brightness))) &&
    //     (frame->colourBuffer[x][y][1] == ((int)rint(symbol.colour[1] * symbol.brightness))) &&
    //     (frame->colourBuffer[x][y][2] == ((int)rint(symbol.colour[2] * symbol.brightness))))
    // {
    //     return;
    // }

    frame->characterBuffer[x][y] = symbol.character;
    
    for (int i = 0; i < 3; i++)
    {
        frame->colourBuffer[x][y][i] = clamp(((int)rint(symbol.colour[i] * symbol.brightness)), 0, 255);
    }
}

void displayDepthBuffer(frameBuffer frame, frameBuffer oldFrame)
{   
    visual outputSymbol;
    int characterColumn = 1;
    int characterRow = 1;
    for (int y = (frame.height-1); y >= 0; y-=2)
    {
        // ensure we dont refrence outside of frame limits.
        int nextY = clamp(y-1,0,frame.height);

        for (int x = (frame.width-1); x >= 0; x--)
        {               
            // escape code sequence for moving cursor, and printing a coloured ' '
            printf("\e[%d;%dH\e[48;2;%d;%d;%dm\e[38;2;%d;%d;%dm\u2584\e[m", characterRow, 
                                                                            characterColumn, 
                                                                            getConvertedDepthValue(frame, x, y), 
                                                                            getConvertedDepthValue(frame, x, y), 
                                                                            getConvertedDepthValue(frame, x, y), 
                                                                            getConvertedDepthValue(frame, x, y-1), 
                                                                            getConvertedDepthValue(frame, x, y-1), 
                                                                            getConvertedDepthValue(frame, x, y-1)
                                                                        );  
                                                       
            characterColumn++;
        }
        characterColumn = 1;
        characterRow++;
    }
    //reset cursor and style, and flush the buffer.
    printf("\e[%d;%dH", frame.height/2, frame.width);
}

int getConvertedDepthValue(frameBuffer frame, int x, int y)
{
    //Depth will be value between depthMinimum and depthMaximum.

    //Desired output is far = dark, near = light.
    
    //For colouring, 255 = light, 0 = dark.

    //Therefore we need to map far = 0, near = 255.

    //Now depth is a value between 255 for near, and 0 for far.

    //This is a linear mapping, we can apply a tone map to the value, for example square.

    //New lets clamp it, to make sure it doesnt fall outside of this range.

    return (int)rint(clampDouble(map(pow(map(frame.depthBuffer[x][y], frame.depthMinimum, frame.depthMaximum, 0, 1),2), 0, 1, 255, 0), 0, 255));
}

void displayFrameBuffer3(frameBuffer frame, frameBuffer oldFrame)
{   
    #ifdef DEBUG_POINTS_NO_CLEARSCREEN
    // printf("Screen Area: (%d + 1 + 17) x (%d + 1) = %d (%ld bytes)\n",frame.width, frame.height, (frame.width + 18 * frame.height +1),sizeOfScreen);
    // printf("BUFFSIZ: %d\n",BUFSIZ);
    #endif

    //start with cursor in top left (1,1)
    int characterRow = 1;
    int characterColumn = 1;

    // since rows are printed from the top of the screen, start from the highest y index and work backwards
    // each row represents 2 pixels, one is the background colour, other is unicode "lower half block"
    for (int y = (frame.height-1); y >= 0; y-=2)
    {
        // ensure we dont refrence outside of frame limits.
        int nextY = clamp(y-1,0,frame.height);
     
        for (int x = 0; x < (frame.width); x++)
        {
            //only update pixels that change (might be pre-mature optimisation)
            if (isPixelColourNew(frame, oldFrame, x, y))
            {
                //escape code sequence for moving cursor (row,column) (\e[%d;%dH), 
                //  setting 24bit background colour (\e[48;2;%d;%d;%dm)
                //  setting 24bit background colour (\e[48;2;%d;%d;%dm)
                //  printing a unicode lower half block (\u2584)
                // and finally reset cursor (\e[m)
                printf("\e[%d;%dH\e[48;2;%d;%d;%dm\e[38;2;%d;%d;%dm\u2584\e[m", characterRow, 
                                                                                characterColumn, 
                                                                                frame.colourBuffer[x][y][0],
                                                                                frame.colourBuffer[x][y][1], 
                                                                                frame.colourBuffer[x][y][2],
                                                                                frame.colourBuffer[x][nextY][0], 
                                                                                frame.colourBuffer[x][nextY][1], 
                                                                                frame.colourBuffer[x][nextY][2]
                                                                                );

            }
            characterColumn++;
        }
        characterColumn = 1;
        characterRow++;
    }
}

int isPixelColourNew(frameBuffer frame, frameBuffer oldFrame, int x, int y)
{
    // compare the old previous frame framebuffer, to the one about to be displayed.
    // only return true if there is an update in the colour information to be displayed.

    int nextY = clamp(y-1, 0, frame.height);

    if ((frame.colourBuffer[x][y][0] != oldFrame.colourBuffer[x][y][0]) ||
    (frame.colourBuffer[x][y][1] != oldFrame.colourBuffer[x][y][1]) ||
    (frame.colourBuffer[x][y][2] != oldFrame.colourBuffer[x][y][2]) ||
    (frame.colourBuffer[x][nextY][0] != oldFrame.colourBuffer[x][nextY][0]) ||
    (frame.colourBuffer[x][nextY][1] != oldFrame.colourBuffer[x][nextY][1]) ||
    (frame.colourBuffer[x][nextY][2] != oldFrame.colourBuffer[x][nextY][2]))
    {
        return 1;
    }

    return 0;
}

void plotLineLow(int x0, int y0, int x1, int y1, frameBuffer* frame)
{
    visual outputSymbol;
    outputSymbol = (visual){ .colour[0] = 255, .colour[1] = 255, .colour[2] = 255};

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
        if ((x <= 0) | (y <= 0) | (x >= (frame->width-1)) | (y >= (frame->height-1))) 
        {
            outputSymbol.character = BORDER;
            drawInFrame(frame, x, y, outputSymbol);
        }
        else
        {
            outputSymbol.character = LINE;
            drawInFrame(frame, x, y, outputSymbol);
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

void plotLineHigh(int x0, int y0, int x1, int y1, frameBuffer* frame)
{
    visual outputSymbol;
    outputSymbol = (visual){ .colour[0] = 255, .colour[1] = 255, .colour[2] = 255};


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
        if ((x <= 0) | (y <= 0) | (x >= (frame->width-1)) | (y >= (frame->height-1))) 
        {
            outputSymbol.character = BORDER;
            drawInFrame(frame, x, y, outputSymbol);
        }
        else
        {
            outputSymbol.character = LINE;
            drawInFrame(frame, x, y, outputSymbol);
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

void BresenhamPlotLine(vector pointA, vector pointB, frameBuffer* frame)
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
            plotLineLow(x1, y1, x0, y0, frame);
        }
        else
        {
            plotLineLow(x0, y0, x1, y1, frame);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            plotLineHigh(x1, y1, x0, y0, frame);
        }
        else
        {
            plotLineHigh(x0, y0, x1, y1, frame);
        }
    }
}

void setCursorBelowFrame(frameBuffer frame)
{
    printf("\e[%d;1H",frame.height);
}

void frameDelay(double framesPerSecond)
{   
    long int delayTimeNanoSeconds = 0;
    long int delayTimeSeconds = 0;
    if (framesPerSecond > 1)
    {
        delayTimeNanoSeconds = (long)rint((1 / framesPerSecond)*  1000000000);
    }
    else
    {
        delayTimeSeconds = (long)rint(1 / framesPerSecond);
    }

    nanosleep((const struct timespec[]){{delayTimeSeconds, delayTimeNanoSeconds}}, NULL);
}

void cursesSetup()
{
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

void cursesEnd()
{
    endwin();
}