#include "draw.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main(void){
    //import the OBJ file

    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    
    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //keeping origin as (halfx,halfy) i.e. middle of screenspace

    double angle = 0;

    int screen[MAX_X][MAX_Y];

    char importPath[] = "/home/nico/Cube/src/testCube.obj";

    mesh myMesh0;

    myMesh0 = importMeshFromOBJFile(importPath); //dynamically allocates an array according to number of triangles in mesh (numOfTris)
    int totalPoints = myMesh0.numOfTris*3;
   
    double points[totalPoints][3];
    double p_points[totalPoints][2];
    double DISTANCE = 15;
    //display 
        //initiate the screen as blank
        initScreen (screen);
        plotTrianglePoints(points, myMesh0);
        projectTrianglePoints2d(points, p_points, DISTANCE, totalPoints);
        //rotate points
        //scale points
        scaleTriangle2DPoints(p_points, totalPoints);
        //draw lines between points
        drawTriangleOnScreen(p_points, origin, ratio, screen, totalPoints, myMesh0.numOfTris);
        displayScreen(screen);
}