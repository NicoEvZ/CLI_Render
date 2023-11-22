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

    int x = 3;
    vector myVector0;
    vector myVector1;
    vector myVector2;
    vector myVector3;
    vector myVector4;
    vector myVector5;
    triangle myTriangle0;
    mesh myMesh0;

    myVector0.x = 1;
    myVector0.y = -7;
    myVector0.z = 0;

    myVector1.x = 1;
    myVector1.y = -1;
    myVector1.z = 0;

    myVector2.x = 6;
    myVector2.y = -1;
    myVector2.z = 0;

    myTriangle0.p[0] = myVector0;
    myTriangle0.p[1] = myVector1;
    myTriangle0.p[2] = myVector2;

    myMesh0.tris = meshInit(x); //dynamically allocates an array according to number of triangles in mesh (x)

    //display 
        //initiate the screen as blank
        initScreen (screen);
        for (int i = 0; i < x; i++){ //for each triangle
            //iterate over data and plot the triangles
        }
        //rotate points
        //scale points
        //draw lines between points
}