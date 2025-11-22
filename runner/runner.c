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
    ray_config.frameColumnsImport = 600; //width
    ray_config.frameRowsImport = 600; //height
    frameBuffer canvas;
    initialiseFrameBuffer(&canvas, ray_config);

    scene scene;
    fillScene(&scene);

    vector origin;
    vector D;
    initialiseVector(&origin);
    initialiseVector(&D);
    int recursionDepth = 3;
    bool subSample = true;

    int sampleSize = 4;
    double spacing = 1.0/ ((double)sampleSize - 1.0);

    for (int x = -(canvas.width/2); x < (canvas.width/2); x++)
    {
        for (int y = -(canvas.height/2); y < (canvas.height/2); y++)
        {   
            int colour[3] = {0,0,0};
            if (subSample)
            {
                
                int colour2[3] = {0,0,0};
                int *colourArray = malloc(sizeof(colour)*sampleSize*sampleSize);
                
                
                for (int i = 0; i < sampleSize; i++)
                {
                    for (int j = 0; j < sampleSize; j++)
                    {
                        double x1 = (double)x + i * spacing;
                        double y1 = (double)y + j * spacing;

                        D = CanvasToViewport(canvas, x1, y1);
                        TraceRay(colour, &scene, origin, D, 1, INFINITY,recursionDepth);

                        colourArray[j*3 + i*sampleSize*3] = colour[0];
                        colourArray[j*3 + i*sampleSize*3 + 1] = colour[1];
                        colourArray[j*3 + i*sampleSize*3 + 2] = colour[2];

                    }
                }
                double R;
                double G;
                double B;
                for (int c = 0; c < sampleSize*sampleSize*3; c+=3)
                {
                    // normalise to 0-1 scale norm = (x/255)
                    // inverse gamma correction lin = sqrt(norm)
                    // accumulate all values for each channel lin_tot = lin_1 + lin_2...
                    R += sqrt((double)colourArray[c]/255.0);
                    G += sqrt((double)colourArray[c+1]/255.0);
                    B += sqrt((double)colourArray[c+2]/255.0);

                    // colour2[0] += (colourArray[c]);
                    // colour2[1] += (colourArray[c+1]);
                    // colour2[2] += (colourArray[c+2]);
                }
                
                // take average of linear values lin_avg = lin_tot/n_lin
                R = R/(double)(sampleSize*sampleSize);
                G = G/(double)(sampleSize*sampleSize);
                B = B/(double)(sampleSize*sampleSize);

                // apply gamma correction (lin_avg)^2
                R = (R*R);
                G = (G*G);
                B = (B*B);

                // convert back to 0-255 scale norm_avg*255
                colour2[0] = (int)(R*255);
                colour2[1] = (int)(G*255);
                colour2[2] = (int)(B*255);
                // colour2[0] = (int)((double)colour2[0]/(double)(sampleSize*sampleSize*3));
                // colour2[1] = (int)((double)colour2[1]/(double)(sampleSize*sampleSize*3));
                // colour2[2] = (int)((double)colour2[2]/(double)(sampleSize*sampleSize*3));
                putPixel(&canvas, x, y, colour2);
            }
            else
            {
                D = CanvasToViewport(canvas, x, y);
                TraceRay(colour, &scene, origin, D, 1, INFINITY,recursionDepth);
                putPixel(&canvas, x, y, colour);
            }
        }
    }
    calcTimer = clock() - calcTimer;
    double calcTime = ((double)calcTimer/ CLOCKS_PER_SEC ) * 1000;
    printf("Calculation time: %3.3lfms\n",calcTime);

    //display what is stored on the canvas
    // displayFrameBuffer3(canvas,old_canvas);
    clock_t genTimer = clock();
    char *output_path = "output.ppm"; 
    generatePPMImage(&canvas, output_path);
    genTimer = clock() - genTimer;
    double genTime = ((double)genTimer/CLOCKS_PER_SEC) * 1000;
    printf("Generation time: %3.3fms\n",genTime);
    // //hold it, so the person can actually see it
    // getchar();

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