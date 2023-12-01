#include "draw.h"
#include <math.h>
#include <time.h>
#include <stdio.h>

#define PI 3.14159

int main(void){
    //import the OBJ file

    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    
    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //keeping origin as (halfx,halfy) i.e. middle of screenspace

    double angle = 0;

    int screen[MAX_X][MAX_Y];

    char importPath[] = "data/testCube.obj";

    mesh BaseMesh = importMeshFromOBJFile(importPath); //dynamically allocates an array according to number of triangles in mesh (numOfTris)
    if (BaseMesh.numOfTris == 0)
        {
            return 0;
        }
    int totalPoints = BaseMesh.numOfTris*3;
   
    double points[totalPoints][3];
    double p_points[totalPoints][2];
    double DISTANCE = 50;
    //display 
    for(int i = 0; i < 360; i++){
        //initiate the screen as blank
        initScreen (screen);

        //init points
        meshToVertexArray(points, BaseMesh);

        rotateVertexsAroundX(points, totalPoints, (angle * (PI/180)));
        rotateVertexsAroundY(points, totalPoints, (angle * (PI/180)));
        rotateVertexsAroundZ(points, totalPoints, (angle * (PI/180)));

        projectVertexArrayTo2D(points, p_points, DISTANCE, totalPoints);
        //rotate points

        //scale points
        scaleTriangle2DPoints(p_points, totalPoints);
        //draw lines between points
        drawTriangleOnScreen(p_points, origin, ratio, screen, totalPoints, BaseMesh.numOfTris);

        displayScreen(screen);
        //printf("i = %d\n",i);
        angle = angle + 1;
        nanosleep((const struct timespec[]){{0, 41600000L}}, NULL);
    }
    return 0;
}