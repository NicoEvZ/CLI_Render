#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "draw.h"
#include "runner.h"

int main(void){
    // cursesSetup();
    // clock_t calcTimer = clock();
    renderConfig ray_config;
    ray_config.frameColumnsImport = 600; //width
    ray_config.frameRowsImport = 600; //height
    frameBuffer canvas;
    initialiseFrameBuffer(&canvas, ray_config);

    scene scene;
    fillScene(&scene);

    vector origin;
    vector D;
    int i;
    matrix4x4 camera_rotation;

    char buf[256];

    initialiseVector(&origin);
    initialiseVector(&D);
    int recursionDepth = 3;
    bool subSample = false;

    int sampleSize = 4;
    int n_totalChannels = sampleSize*sampleSize*3;
    double spacing = 1.0/ ((double)sampleSize - 1.0);

    for (i = 0; i < 90; i++)
    {
        double angle = (i - 45) * (PI/180.0);
        snprintf(buf,sizeof(buf), "rotation-%02d.ppm",i);
        char *output_path = buf;
        initialiseRotateYMatrix(&camera_rotation,angle);

        for (int x = -(canvas.width/2); x < (canvas.width/2); x++)
        {
            for (int y = -(canvas.height/2); y < (canvas.height/2); y++)
            {   
                int colour[3] = {0,0,0};
                if (subSample)
                {
                    
                    int colour2[3] = {0,0,0};
                    int *colourArray = (int *)malloc(sizeof(int)*(n_totalChannels));  
                    
                    for (int i = 0; i < sampleSize; i++)
                    {
                        for (int j = 0; j < sampleSize; j++)
                        {
                            double x1 = (double)x + i * spacing;
                            double y1 = (double)y + j * spacing;

                            D = CanvasToViewport(canvas, x1, y1);
                            TraceRay(colour, &scene, origin, D, 1, INFINITY,recursionDepth);

                            int arrayIndex = j*3 + i*sampleSize*3;

                            colourArray[arrayIndex] = colour[0];
                            colourArray[arrayIndex + 1] = colour[1];
                            colourArray[arrayIndex + 2] = colour[2];

                        }
                    }
                    colourAverage(colour2, colourArray, n_totalChannels);
                    putPixel(&canvas, x, y, colour2);
                    free(colourArray);
                }
                else
                {
                    D = CanvasToViewport(canvas, x, y);
                    D = matrixVectorMultiply(D,camera_rotation);
                    TraceRay(colour, &scene, origin, D, 1, INFINITY,recursionDepth);
                    putPixel(&canvas, x, y, colour);
                }
            }
        }
        generatePPMImage(&canvas, output_path);
    }
    // calcTimer = clock() - calcTimer;
    // double calcTime = ((double)calcTimer/ CLOCKS_PER_SEC );
    // printf("Calculation time: %3.3lfs\n",calcTime);
    deleteScene(&scene);
    deleteFrameBuffer(&canvas);
    // cursesEnd();
    return 0;
}

void fillScene(scene* scene)
{

    int n_spheres = 4; 
    int n_lights = 3;

    vector emptyVector;
    initialiseVector(&emptyVector);

    scene->sphereCount = n_spheres;
    scene->lightCount = n_lights;
    scene->sphereArray = malloc(scene->sphereCount*sizeof(sphere));
    scene->lightArray = malloc(scene->lightCount*sizeof(light));

    sphere sphereRed = (sphere){
        .center = (vector){
            .x = 0.0,
            .y = -1.0,
            .z = 3.0,
            .w = 1.0},
        //red
        .colour[0] = 255,
        .colour[1] = 0,
        .colour[2] = 0,
        .radius = 1.0,
        .specular = 500.0,
        .reflective = 0.2
    };

    sphere sphereBlue = (sphere){
        .center = (vector){
            .x = 2.0,
            .y = 0.0,
            .z = 4.0,
            .w = 1.0},
        //blue
        .colour[0] = 0,
        .colour[1] = 0,
        .colour[2] = 255,
        .radius = 1.0,
        .specular = 500.0,
        .reflective = 0.3
    };

    sphere sphereGreen = (sphere){
        .center = (vector){
            .x = -2.0,
            .y = 0.0,
            .z = 4.0,
            .w = 1.0},
        //green
        .colour[0] = 0,
        .colour[1] = 255,
        .colour[2] = 0,
        .radius = 1.0,
        .specular = 10.0,
        .reflective = 0.4
    };

    sphere sphereYellow = (sphere){
        .center = (vector){
            .x = 0.0,
            .y = -5001,
            .z = 0.0,
            .w = 1.0},
        //yellow
        .colour[0] = 255,
        .colour[1] = 255,
        .colour[2] = 0,
        .radius = 5000.0,
        .specular = 1000.0,
        .reflective = 0.5
    };

    light ambientLight = (light){
        .lightType = ambient,
        .intensity = 0.2,
        .pos_or_dir = emptyVector,
    };

    light pointLight = (light){
        .lightType = point,
        .intensity = 0.6,
        .pos_or_dir = (vector){
            .x = 2.0,
            .y = 1.0,
            .z = 0.0,
            .w = 1.0,
        }
    };

    light directionalLight = (light){
        .lightType = directional,
        .intensity = 0.2,
        .pos_or_dir = (vector){
            .x = 1.0,
            .y = 4.0,
            .z = 4.0,
            .w = 1.0,
        }
    };

    scene->sphereArray[0] = sphereRed;
    scene->sphereArray[1] = sphereBlue;
    scene->sphereArray[2] = sphereGreen;
    scene->sphereArray[3] = sphereYellow;   
    scene->lightArray[0] = pointLight;
    scene->lightArray[1] = ambientLight;
    scene->lightArray[2] = directionalLight;
}

void deleteScene(scene* scene)
{
    free(scene->lightArray);
    free(scene->sphereArray);
}

void generatePPMImage(frameBuffer *frameBuffer, char *output_path)
{
    FILE *ppm = fopen(output_path,"wb");
    int width = frameBuffer->width;
    int height = frameBuffer->height;
    fprintf(ppm,"P6\n");
    fprintf(ppm,"%d %d\n", width, height);
    fprintf(ppm,"255\n");

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            fputc(frameBuffer->colourBuffer[x][y][0],ppm);
            fputc(frameBuffer->colourBuffer[x][y][1],ppm);
            fputc(frameBuffer->colourBuffer[x][y][2],ppm);
        }
        
    }
    fclose(ppm);
    printf("Generated %s\n",output_path);
}