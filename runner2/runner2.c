#include "draw.h"
#include <math.h>
#include <stdlib.h>

int main(void){
    //import the OBJ file

    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    
    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //keeping origin as (halfx,halfy) i.e. middle of screenspace

    double angle = 0;

    int screen[MAX_X][MAX_Y];

    int x = 2;
    int totalPoints = x*3;
    vector myVector0;
    vector myVector1;
    vector myVector2;
    triangle myTriangle0;
    mesh myMesh0;

    myMesh0.tris = meshInit(x); //dynamically allocates an array according to number of triangles in mesh (x)

    //1
    myMesh0.tris[0].p[0].x = 1;
    myMesh0.tris[0].p[0].y = 1;
    myMesh0.tris[0].p[0].z = 0;
    
    //2
    myMesh0.tris[0].p[1].x = -1;
    myMesh0.tris[0].p[1].y = 1;
    myMesh0.tris[0].p[1].z = 0;

    //3
    myMesh0.tris[0].p[2].x = -1;
    myMesh0.tris[0].p[2].y = -1;
    myMesh0.tris[0].p[2].z = 0;

    //4
    myMesh0.tris[1].p[0].x = -1;
    myMesh0.tris[1].p[0].y = -1;
    myMesh0.tris[1].p[0].z = 0;
    
    //5
    myMesh0.tris[1].p[1].x = 1;
    myMesh0.tris[1].p[1].y = -1;
    myMesh0.tris[1].p[1].z = 0;

    //6
    myMesh0.tris[1].p[2].x = 1;
    myMesh0.tris[1].p[2].y = 1;
    myMesh0.tris[1].p[2].z = 0;
   
    double points[totalPoints][3];
    double p_points[totalPoints][2];
    double DISTANCE = 10;
    //display 
        //initiate the screen as blank
        initScreen (screen);
        plotTrianglePoints(points, myMesh0.tris, x);
        projectTrianglePoints2d(points, p_points, DISTANCE, totalPoints);
        //rotate points
        //scale points
        scaleTriangle2DPoints(p_points, totalPoints);
        //draw lines between points
        drawTriangleOnScreen(p_points, origin, ratio, screen, totalPoints, x);
        displayScreen(screen);
}