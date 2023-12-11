#include "draw.h"
#include <math.h>
#include <time.h>
#include <stdio.h>

int main(void){
    //import the OBJ file

    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    double angle = 0;
    double DISTANCE = 60;

    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //origin is middle of screenspace

    int screen[MAX_X][MAX_Y];

    char importPath[] = "data/videoShip.obj";

    //Store OBJ data in mesh struct
    mesh baseMesh = importMeshFromOBJFile(importPath); 
    mesh rotatedMesh;
    mesh projectedMesh;
    
    //failsafe that exits code if the importMeshFromOBJFile didn't succeed.
    if (baseMesh.numOfTris == 0)
    {
        return 0;
    }
 
    //display 
    for (int i = 0; i < 1080; i++)
    {
        //clear screen
        initScreen(screen);

        rotatedMesh = copyMeshData(baseMesh, rotatedMesh);

        // rotate around axes
        rotatedMesh = rotateMeshAroundX(rotatedMesh, (angle * (PI/180)));
        rotatedMesh = rotateMeshAroundY(rotatedMesh, (angle * (PI/180)));
        rotatedMesh = rotateMeshAroundZ(rotatedMesh, (angle * (PI/180)));

        projectedMesh = copyMeshData(rotatedMesh, projectedMesh);

        DISTANCE =  30 * (sin(0.1 * i)+ 1.8);

        //project 3D --> 2D
        projectMeshTo2D(projectedMesh, DISTANCE);
        
        //scale points
        scale2DPoints(projectedMesh);

        //draw lines
        drawMeshOnScreen(projectedMesh, origin, ratio, screen);

        displayScreen(screen);

        angle = angle + 1;
        nanosleep((const struct timespec[]){{0, 41600000L}}, NULL);
    }
    return 0;
}