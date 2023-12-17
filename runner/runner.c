#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "draw.h"

int main(void){
    data importData;
    screenStruct screen;

    const char jsonImportPath[] = "data/inputData.json";

    //failsafe that exits code if the importMeshFromOBJFile didn't succeed.
    if (importJSON(jsonImportPath, &importData))
    {
        printf("Import of JSON failed, exiting here");
        return 1;
    }

    screen.width = importData.screenWidthImprt;
    screen.height = importData.screenHeightImprt;

    initScreen(&screen);

    const double half_x = (double)screen.width * 0.5;
    const double half_y = (double)screen.height * 0.5;
    double ratio = ((double)screen.width/(double)screen.height);
    printf("width: %d\nheight: %d\nhalf_x: %lf\nhalf_y: %lf\nratio: %lf\n",screen.width,screen.height,half_x,half_y,ratio);
    double angle = 0;

    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //origin is middle of screenspace

    //Store OBJ data in mesh struct
    mesh baseMesh = importMeshFromOBJFile(importData.objPathBuffer); 
    if (baseMesh.numOfTris == 0)
    {
        return 0;
    }

    mesh rotatedMesh;
    mesh projectedMesh;
    vector (*normalsVecArr) = malloc(baseMesh.numOfTris * sizeof(vector));    
 
    //display 
    for (int i = 0; i < importData.i; i++)
    {
        //clear screen
        clearScreen(&screen);

        rotatedMesh = copyMeshData(baseMesh, rotatedMesh);

        // rotate around axes
        if(importData.rotationX)
        {
            rotatedMesh = rotateMeshAroundX(rotatedMesh, (angle * (PI/180)));
        }

        if(importData.rotationY)
        {
            rotatedMesh = rotateMeshAroundY(rotatedMesh, (angle * (PI/180)));
        }

        if(importData.rotationZ)
        {
            rotatedMesh = rotateMeshAroundZ(rotatedMesh, (angle * (PI/180)));
        }
        projectedMesh = copyMeshData(rotatedMesh, projectedMesh);

        // distance =  30 * (sin(0.1 * i)+ 1.8);

        //calculate normals
        calculateMeshNormals(projectedMesh, normalsVecArr);

        //project 3D --> 2D
        projectMeshTo2D(projectedMesh, importData.distance);
        
        //scale points
        scale2DPoints(projectedMesh,importData.scale);

        //draw lines
        drawMeshOnScreen(projectedMesh, origin, screen, normalsVecArr);

        displayScreen(&screen);

        angle = angle + 1;
        nanosleep((const struct timespec[]){{0, 83300000L}}, NULL);
    }
    free(normalsVecArr);
    deleteScreen(&screen);
    return 0;
}