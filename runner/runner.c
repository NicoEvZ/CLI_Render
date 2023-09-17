#include "draw.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

int main(void){
    
    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    
    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //keeping origin as (halfx,halfy) i.e. middle of screenspace

    double angle = 0;

    int screen[MAX_X][MAX_Y];

    double cubeBasePoints[8][3]={
        //front face of Cube
        {-1,-1,1},  //0
        {1,-1,1},   //1
        {1,1,1},    //2
        {-1,1,1},   //3
        //back face of Cube
        {-1,-1,-1}, //4
        {1,-1,-1},  //5
        {1,1,-1},   //6
        {-1,1,-1},  //7
    };

    double pyramidBasePoints[8][3]={
        //Pyramid can be drawn by making the points that form the top square all coincide on the same place, (0,-1,0).
        {0,-1,0},   //0
        {0,-1,0},   //1
        {1,1,1},    //2
        {-1,1,1},   //3
        {0,-1,0},   //4
        {0,-1,0},   //5
        {1,1,-1},   //6
        {-1,1,-1},  //7
    };

    double points[8][3];
    double p_points[8][2];
    for (int i = 0; i < 999; i++) {
        initScreen(screen); //fills screenspace array with dots. can act as a blank to re-draw over.
        
        initPoints(points,cubeBasePoints); //resets points back to unity values

        rotatePointsAroundX(points, (angle * (M_PI/180))); //takes output of "scalePoints", rotates by "angle"
        rotatePointsAroundY(points, (angle * (M_PI/180))); //takes output of "scalePoints", rotates by "angle"
        rotatePointsAroundZ(points, (angle * (M_PI/180))); //takes output of "scalePoints", rotates by "angle"

        double DISTANCE =  10.0 * (sin(0.1 * i)+ 2.1); //values chose by trial to avoid segfault
        projectPoints2d(points,p_points, DISTANCE);

        scaleMulti2DPoints(p_points);

        drawCubeOnScreen(p_points,origin,ratio,screen); // take output of "rotate points", translate to screenspace(and adjust for aspect ratio) (without modifying points!) and draw lines between points in array

        plotMultiPointAbs(p_points,origin,ratio,screen); // take output of "rotate points", translate to screenspace(and adjust for aspect ratio) (without modifying points!) and plots a "DOT" on points in array

        displayScreen(screen);

        angle = angle + 1;
        nanosleep((const struct timespec[]){{0, 41600000L}}, NULL);
    }
    
    return 0;
}