#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "draw.h"
#include "runner.h"

int main(void){
    cursesSetup();

    renderConfig ray_config;
    ray_config.frameColumnsImport = 160;
    ray_config.frameRowsImport = 70;
    frameBuffer canvas, old_canvas;
    initialiseFrameBuffer(&canvas, ray_config);
    initialiseFrameBuffer(&old_canvas, ray_config);

    scene scene;
    fillScene(&scene);

    vector origin;
    vector D;
    int colour[3] = {0,0,0};
    initialiseVector(&origin);
    initialiseVector(&D);

    for (int x = -(canvas.width/2); x < (canvas.width/2); x++)
    {
        for (int y = -(canvas.height/2); y < (canvas.height/2); y++)
        {
            D = CanvasToViewport(canvas, x, y);
            TraceRay(colour, &scene, origin, D, 1, INFINITY);
            putPixel(&canvas, x, y, colour);
        }
    }

    //display what is stored on the canvas
    displayFrameBuffer3(canvas,old_canvas);
    
    //hold it, so the person can actually see it
    getchar();

    deleteScene(&scene);
    deleteFrameBuffer(&canvas);
    deleteFrameBuffer(&old_canvas);
    cursesEnd();
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
        .radius = 1,
        .specular = 500.0,
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
        .radius = 1,
        .specular = 500.0,
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
        .radius = 1,
        .specular = 10.0,
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
        .radius = 5000,
        .specular = 1000.0,
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