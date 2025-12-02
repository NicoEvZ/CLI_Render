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
    clock_t calcTimer = clock();
    renderConfig ray_config;
    ray_config.frameColumnsImport = 1080; //width
    ray_config.frameRowsImport = 1080; //height
    frameBuffer canvas;
    frameBuffer canvasTest;
    initialiseFrameBuffer(&canvas, ray_config);
    initialiseFrameBuffer(&canvasTest, ray_config);

    RGB pink = (RGB){211, 3, 252};
    RGB black = (RGB){0, 0, 0};

    scene scene;
    fillScene(&scene);

    vector origin;
    vector D;
    int i;
    matrix4x4 camera_rotation;

    char buf[256];
    char buf_test[256];

    initialiseVector(&origin);
    initialiseVector(&D);
    int recursionDepth = 3;
    bool superSampleDisabled = false;
    int sampleSize = 4;
    double superSampleThreshold = 18;
    int frames = 5;
    // clock_t frameCompTimer = clock();

    for (i = 0; i < frames; i++)
    {
        // frameCompTimer = clock();
        double angle = (i - (frames/2)) * (PI/180.0);
        snprintf(buf,sizeof(buf), "selective-superSampling-rotation-%02d.ppm",i);
        char *output_path = buf;
        snprintf(buf_test,sizeof(buf_test), "selective-superSampling-rotation-test-%02d.ppm",i);
        char *test_output_path = buf_test;
        initialiseRotateYMatrix(&camera_rotation,angle);
        RGB previousColourX;
        RGB previousColourY;
        for (int x = -(canvas.width/2); x < (canvas.width/2); x++)
        {
            for (int y = -(canvas.height/2); y < (canvas.height/2); y++)
            {   
                RGB currentColour = SuperSamplePixel(&canvas, 1, x, y, scene, origin, camera_rotation, recursionDepth);

                // printf("Current:\n");
                // debugPrintPixelandColour(&canvas, x, y, currentColour);

                if (superSampleDisabled)
                {
                    putPixel(&canvas, x, y, currentColour);
                    continue;
                }

                // look at previous pixel in x direction
                previousColourX = getPixel(&canvas, x-1, y);

                // printf("PreviousX:\n");
                // debugPrintPixelandColour(&canvas,x-1, y, previousColourX);

                bool superSampleX = testColourDistance(previousColourX, currentColour, superSampleThreshold);

                // look at previous pixel in y direction
                previousColourY = getPixel(&canvas, x, y-1);

                // printf("PreviousY:\n");
                // debugPrintPixelandColour(&canvas,x, y-1, previousColourY);

                bool superSampleY = testColourDistance(previousColourY, currentColour, superSampleThreshold);

                // printf("\n");
                
                // if neither x or y beat the difference threshold, move onto the next pixel
                if (!superSampleX && !superSampleY)
                {
                    putPixel(&canvas, x, y, currentColour);
                    putPixel(&canvasTest, x, y, black);
                    continue;
                }
                
                currentColour = SuperSamplePixel(&canvas, sampleSize, x, y, scene, origin, camera_rotation, recursionDepth);
                putPixel(&canvas, x, y, currentColour);
                putPixel(&canvasTest, x, y, pink);

                
                if (superSampleX)
                {
                    previousColourX = SuperSamplePixel(&canvas, sampleSize, x-1, y, scene, origin, camera_rotation, recursionDepth);
                    putPixel(&canvas, x-1, y, previousColourX);
                    putPixel(&canvasTest, x-1, y, pink);
                }

                if (superSampleY)
                {
                    previousColourY = SuperSamplePixel(&canvas, sampleSize, x, y-1, scene, origin,  camera_rotation, recursionDepth);
                    putPixel(&canvas, x, y-1, previousColourY);
                    putPixel(&canvasTest, x, y-1, pink);
                }
            }
        }
        generatePPMImage(&canvas, output_path);
        clearFrameBuffer(&canvas);
        generatePPMImage(&canvasTest, test_output_path);
        clearFrameBuffer(&canvasTest);
        // frameCompTimer = clock() - frameCompTimer;
        // double frameCompTime = ((double)frameCompTimer/ CLOCKS_PER_SEC );
        // printf("compTime: %3.3lfs\n",frameCompTime);
        fflush(stdout);
    }
    calcTimer = clock() - calcTimer;
    double calcTime = ((double)calcTimer/ CLOCKS_PER_SEC );
    printf("Calculation time: %3.3lfs\n",calcTime);
    deleteScene(&scene);
    // cursesEnd();

    deleteFrameBuffer(&canvas);
    deleteFrameBuffer(&canvasTest);

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
        .colour = (RGB){
            .r = 255,
            .g = 0,
            .b = 0,
        },
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
        .colour = (RGB){
            .r = 0,
            .g = 0,
            .b = 255,
        },
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
        .colour = (RGB){
            .r = 0,
            .g = 255,
            .b = 0,
        },
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
        .colour = (RGB){
            .r = 255,
            .g = 255,
            .b = 0,
        },
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
            fputc(frameBuffer->colourBuffer[x][y].r, ppm);
            fputc(frameBuffer->colourBuffer[x][y].g, ppm);
            fputc(frameBuffer->colourBuffer[x][y].b, ppm);
        }
        
    }
    fclose(ppm);
    printf("Generated %s\n",output_path);
}