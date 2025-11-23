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

    //bound the bounding boxes
    boundingBoxMin.x = clampDouble(boundingBoxMin.x, 0, frame->width - 1);
    boundingBoxMin.y = clampDouble(boundingBoxMin.y, 0, frame->height - 1);
    boundingBoxMax.x = clampDouble(boundingBoxMax.x, 0, frame->width - 1);
    boundingBoxMax.y = clampDouble(boundingBoxMax.y, 0, frame->height - 1);

    // if a triangle is off screen, get outa here!
    if (boundingBoxMax.x < 0 || boundingBoxMin.x >= frame->width ||
        boundingBoxMax.y < 0 || boundingBoxMin.y >= frame->height)
    {
        return;
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
    frame->depthMinimum = 1000;
    frame->depthMaximum = 0;
    for (int x = 0; x < frame->width; x++) 
    {
        for (int y = 0; y < frame->height; y++) 
        {
            frame->characterBuffer[x][y]=BLANK;
            frame->depthBuffer[x][y]=1000;
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
    frame->height = importData.frameRowsImport;


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

void putPixel(frameBuffer* canvas, int x, int y, int colour[3])
{
    // clamp input canvas coords
    x = clamp(x, -(canvas->width/2), (canvas->width/2)-1);
    y = clamp(y, -(canvas->height/2), (canvas->height/2) - 1);

    // convert from canvas coords to screen coords
    int screen_x = clamp(((canvas->width/2) + x),0,canvas->width-1);
    int screen_y = clamp(((canvas->height/2) - y)-1,0,canvas->height-1);

    // debugPrintPixelandColour(screen_x,screen_y,colour);
    
    // per channel, copy the input colour to the position in the frame colour buffer (screen)
    for (int i = 0; i < 3; i++)
    {
        canvas->colourBuffer[screen_x][screen_y][i]  = clamp(colour[i], 0, 255);
    }
}

vector CanvasToViewport(frameBuffer canvas, double x, double y)
{
    return (vector){(double)x*((double)VIEWPORT_WIDTH/(double)canvas.width),(double)y*((double)VIEWPORT_HEIGHT/(double)canvas.height),(double)VIEWPORT_DEPTH,1.0};
}

void TraceRay(int out_colour[3], scene* scene, vector rayOriginVector, vector rayDirectionVector, double t_min, double t_max, int recursionDepth)
{
    double closest_t = INFINITY;
    sphere* closest_sphere = NULL;
    ClosestIntersection(&closest_sphere, &closest_t, scene, rayOriginVector, rayDirectionVector, t_min, t_max);
    if (closest_sphere == NULL)
    {
        for (int channel = 0; channel < 3; channel++)
        {
            out_colour[channel] = 0;
        }
        return; 
    }
    
    // compute local colour
    vector point = addVector(rayOriginVector, (multiplyVectorByScalar(rayDirectionVector, closest_t)));
    vector viewDirection = multiplyVectorByScalar(rayDirectionVector, -1.0);
    vector normal = normaliseVector(subtractVector(point, closest_sphere->center));
    for (int channel = 0; channel < 3; channel++)
    {
        out_colour[channel] = closest_sphere->colour[channel] * computeLighting(scene,point,normal,viewDirection,closest_sphere->specular);
    }

    // check for recursion limit or non-reflective object.
    double reflective = closest_sphere->reflective;
    if (recursionDepth <= 0 || reflective <= 0)
    {
        return;
    }

    vector reflectedRay = reflectRay(viewDirection,normal);
    int reflectedColour[3];
    TraceRay(reflectedColour,scene,point,reflectedRay,0.001,INFINITY,recursionDepth-1);

    for (int channel = 0; channel < 3; channel++)
    {
        out_colour[channel] = out_colour[channel]*(1 - closest_sphere->reflective) + reflectedColour[channel]*closest_sphere->reflective;
    }


}

void ClosestIntersection(sphere** closest_sphere, double* closest_t, scene* scene, vector rayOriginVector, vector rayDirectionVector, double t_min, double t_max)
{
    if ((*closest_sphere != NULL))
    {
        printf("closest_sphere not set up correctly. Must be a NULL pointer\r\n");
        getchar();
    }
    if (*closest_t != INFINITY)
    {
        printf("closest_t not set up correctly. Must be = INFINITY\r\n");
        getchar();
    }
    double t1, t2;
    for (int i = 0; i < scene->sphereCount; i++)
    {
        IntersectRaySphere(&t1,&t2,rayOriginVector,rayDirectionVector,&scene->sphereArray[i]);

        if (((t1 >= t_min) && (t1 <= t_max )) && (t1 < *closest_t))
        {
            *closest_t = t1;
            *closest_sphere = &scene->sphereArray[i];
        }
        if (((t2 >= t_min) && (t2 <= t_max )) && (t2 < *closest_t))
        {
            *closest_t = t2;
            *closest_sphere = &scene->sphereArray[i];
        }
    }
}

void IntersectRaySphere(double* t1, double* t2, vector rayOriginVector, vector rayDirectionVector, sphere* test_sphere)
{
    double r = test_sphere->radius;
    vector CO = subtractVector(rayOriginVector,test_sphere->center);

    double a = dotProduct(rayDirectionVector,rayDirectionVector);
    double b = 2*dotProduct(CO, rayDirectionVector);
    double c = dotProduct(CO, CO) - (r *r);

    double discriminant = (b*b) - (4*a*c);

    if (discriminant < 0)
    {
        *t1 = INFINITY;
        *t2 = INFINITY;
        return;
    }
    
    *t1 = (-b + sqrt(discriminant)) / (2*a);
    *t2 = (-b - sqrt(discriminant)) / (2*a);
    return;
}

double computeLighting(scene* scene, vector point_to_compute, vector normal_to_point, vector view_vector, double specular_exponent)
{
    double i = 0.0;
    vector L;
    double t_max;
    initialiseVector(&L);
    for (int currentLightIter = 0; currentLightIter < scene->lightCount; currentLightIter++)
    {
        light currentLight = scene->lightArray[currentLightIter];
        
        if (currentLight.lightType == ambient)
        {
            i += currentLight.intensity;
        }
        else
        {
            if (currentLight.lightType == point){
                //here the vector is position
                L = subtractVector(currentLight.pos_or_dir, point_to_compute);
                t_max = 1.0;
            }
            else
            {
                //here the vector is direction
                L = currentLight.pos_or_dir;
                t_max = INFINITY;
            }

            //shadow check
            double shadow_t = INFINITY;
            sphere* shadow_sphere = NULL;
            ClosestIntersection(&shadow_sphere, &shadow_t, scene, point_to_compute, L, 0.001, t_max);
            if (shadow_sphere != NULL)
            {
                continue;
            }

            //diffuse light
            double n_dot_l = dotProduct(normal_to_point, L);
            if (n_dot_l > 0)
            {
                i += (currentLight.intensity * (n_dot_l/((vectorLength(normal_to_point) * vectorLength(L)))));
            }

            //specular light
            vector reflection_vector;
            if (specular_exponent != -1.0)
            {
                // reflection_vector = 2*N*dot(N,L)-L
                reflection_vector = reflectRay(L,normal_to_point);
                double r_dot_v = dotProduct(reflection_vector, view_vector);
                if (r_dot_v > 0)
                {
                    i += (currentLight.intensity * pow((r_dot_v / (vectorLength(reflection_vector)*vectorLength(view_vector))), specular_exponent));
                }
            }
        }
    };
    return i;
}

void debugPrintPixelandColour(int x, int y, int colour[3])
{
    printf("x:%d,y:%d,R:%d,G:%d,B:%d\n",x,y,colour[0],colour[1],colour[2]);
}

void colourAverage(int outColour[], int arrayOfColours[], int n_arrayElements)
{
    double R;
    double G;
    double B;
    int n_colours = n_arrayElements/3;
    for (int c = 0; c < n_arrayElements; c+=3)
    {
        // normalise to 0-1 scale norm = (x/255)
        // inverse gamma correction lin = sqrt(norm)
        // accumulate all values for each channel lin_tot = lin_1 + lin_2...
        R += sqrt((double)arrayOfColours[c]/255.0);
        G += sqrt((double)arrayOfColours[c+1]/255.0);
        B += sqrt((double)arrayOfColours[c+2]/255.0);
    }
    
    // take average of linear values lin_avg = lin_tot/n_lin
    R = R/(double)(n_colours);
    G = G/(double)(n_colours);
    B = B/(double)(n_colours);

    // apply gamma correction (lin_avg)^2
    R = (R*R);
    G = (G*G);
    B = (B*B);

    // convert back to 0-255 scale norm_avg*255
    outColour[0] = (int)(R*255);
    outColour[1] = (int)(G*255);
    outColour[2] = (int)(B*255);
}

void displayDepthBuffer(frameBuffer frame)
{   
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
                                                                            getConvertedDepthValue(frame, x, nextY), 
                                                                            getConvertedDepthValue(frame, x, nextY), 
                                                                            getConvertedDepthValue(frame, x, nextY)
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