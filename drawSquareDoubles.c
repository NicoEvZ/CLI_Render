#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_X 101 //best if odd
#define MAX_Y 41 //best if odd
#define SCALE_FACTOR 110 //
#define DISTANCE 10 
#define BLANK ' ' //SPACE character ASCII code
#define LINE '#' //'#' character ASCII code
#define DOT '@'
#define BORDER '*'
#define DEBUG1 '1'
#define DEBUG2 '2'
#define DEBUG3 '3'

double initPoints(double arr[8][3], double basePoints[8][3]){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++)
        {
            arr[i][j] = basePoints[i][j];
        }
    }
}

double rotatePointsAroundX(double arr[8][3], double angle){
    for (int i = 0; i < 8; i++){
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        //rotate points by angle
        double x_r = x;
        double y_r = (y * cos(angle)) - (z * sin(angle));
        double z_r = (y * sin(angle)) + (z * cos(angle));

        arr[i][0] = x_r;
        arr[i][1] = y_r;
        arr[i][2] = z_r;
    }
};

double rotatePointsAroundY(double arr[8][3], double angle){
    for (int i = 0; i < 8; i++){
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        //rotate points by angle
        double x_r = (x * cos(angle)) + (z * sin(angle));
        double y_r = y;
        double z_r = (-x * sin(angle)) + (z * cos(angle));

        arr[i][0] = x_r;
        arr[i][1] = y_r;
        arr[i][2] = z_r;
    }
};

double rotatePointsAroundZ(double arr[8][3], double angle){
    for (int i = 0; i < 8; i++){
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        //rotate points by angle
        double x_r = (x * cos(angle)) - (y * sin(angle));
        double y_r = (x * sin(angle)) + (y * cos(angle));
        double z_r = z;

        arr[i][0] = x_r;
        arr[i][1] = y_r;
        arr[i][2] = z_r;
    }
};

void initScreen(int screenArr[MAX_X][MAX_Y]){
    for (int x = 0; x < MAX_X; x++)
    {
        for (int y = 0; y < MAX_Y; y++)
        {
            screenArr[x][y]=BLANK;
            if (x == 0 | y == 0 | x == (MAX_X-1) | y == (MAX_Y-1)){
                screenArr[x][y]=BORDER;
            }
            // Fill array with BLANK values, defined as ASCII space
        }
    }
}

void plotPointRel(double point[2], double origin[2], double ratio, int screen[MAX_X][MAX_Y]){
    
    //extract x and y from array.
    double x = point[0];
    double y = point[1];

    //correction for aspect ratio (only applied to x)
    x = x * ratio;

    //scale?? multiply a scaling value to x and y???

    x = x * SCALE_FACTOR;
    y = y * SCALE_FACTOR; 

    //translate relative to origin
    x = (origin[0]+x);
    y = (origin[1]+y);

    //draw on screenspace
    screen[(int)x][(int)y]=DEBUG3;
}

void plotPointAbs(double point[2], int screen[MAX_X][MAX_Y]){
    //extract x and y from array.
    double x = point[0];
    double y = point[1];
    screen[(int)x][(int)y]=DOT;
}

void plotMultiPointsRel(double arr[8][2], double origin[2], double ratio, int screen[MAX_X][MAX_Y]){
    //iterate over every point in points array
    for (int i = 0; i < 8; i++){
        //extract x and y from array.
        double x = arr[i][0];
        double y = arr[i][1];

        //correction for aspect ratio (only applied to x)
        x = x * ratio;

        //scale?? multiply a scaling value to x and y???

        x = x * SCALE_FACTOR;
        y = y * SCALE_FACTOR;
        

        //translate relative to origin
        x = (origin[0]+x);
        y = (origin[1]+y);

        //draw on screenspace
        screen[(int)x][(int)y]=DEBUG3;   
    }
}

double scaleMulti3DPoints(double arr[8][3]){
    /*This function works in the same way as the plotMultiPointsRel,
    But must take the start address of an array which is a copy of the
    "initial" unity points array, and edit the values, so we end up
    with that copy edited to have all co-ords, already scaled, ratio'd
    and translated. This will then be fed to the line plotting function*/

    //iterate over every point in points array
    for (int i = 0; i < 8; i++){
        //extract x and y from array.
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        // //correction for aspect ratio (only applied to x)
        //  x = x * ratio;

        //scale?? multiply a scaling value to x and y???
        x = x * SCALE_FACTOR;
        y = y * SCALE_FACTOR;
        z = z * SCALE_FACTOR;

        //draw on screenspace
        arr[i][0] = x;
        arr[i][1] = y;
        arr[i][2] = z;
    }
}

double scaleMulti2DPoints(double arr[8][2]){
    /*This function works in the same way as the plotMultiPointsRel,
    But must take the start address of an array which is a copy of the
    "initial" unity points array, and edit the values, so we end up
    with that copy edited to have all co-ords, already scaled, ratio'd
    and translated. This will then be fed to the line plotting function*/

    //iterate over every point in points array
    for (int i = 0; i < 8; i++){
        //extract x and y from array.
        double x = arr[i][0];
        double y = arr[i][1];

        // //correction for aspect ratio (only applied to x)
        //  x = x * ratio;

        //scale?? multiply a scaling value to x and y???
        x = x * SCALE_FACTOR;
        y = y * SCALE_FACTOR;

        //draw on screenspace
        arr[i][0] = x;
        arr[i][1] = y;
    }
}

double projectPoints2d(double arr[8][3], double p_points[8][2]){
    for(int i = 0; i < 8; i++){
        //extract x and y from array.
        double x = arr[i][0];
        double y = arr[i][1];
        double z = arr[i][2];

        double zPerspective = 1/(DISTANCE - z);

        // zPerspective = 1 / zPerspective;

        double p_Mat[2][3] = {{1,0,0},{0,1,0}}; //values of 1 give orthgraphic view

        double p_Mat2[2][3] = {{zPerspective,0,0},{0,zPerspective,0}};
        
        double x_p2 = (p_Mat2[0][0]*x)+(p_Mat2[0][1]*y)+(p_Mat2[0][2]*z);
        double y_p2 = (p_Mat2[1][0]*x)+(p_Mat2[1][1]*y)+(p_Mat2[1][2]*z);

        double x_p = (p_Mat[0][0]*x)+(p_Mat[0][1]*y)+(p_Mat[0][2]*z);
        double y_p = (p_Mat[1][0]*x)+(p_Mat[1][1]*y)+(p_Mat[1][2]*z);
        
        // screen[(origin[0]+(x_p*ratio))][origin[1]+y_p]=DOT;
        p_points[i][0] = x_p2;
        p_points[i][1] = y_p2;
    }
}

double plotMultiPointAbs(double arr[8][2], double origin[2], double ratio, int screen[MAX_X][MAX_Y]){
    for(int i = 0; i < 8; i++){
        //extract x and y from array.
        double x = arr[i][0];
        double y = arr[i][1];
        
        screen[(int)(origin[0]+(x*ratio))][(int)(origin[1]+y)]=DOT;
    }
}

void displayScreen(int arr[MAX_X][MAX_Y]){
    // Iterate over y axis
    char outputString[MAX_X+1];
    for(int y = 0; y < MAX_Y; y++)
    {
        for(int x = 0; x < MAX_X; x++)
        {
            // Store current value in array at point(x,y), as char in string
            // String is length of MAX_X
            outputString[x]=arr[x][y];
        }
        // Display filled string, and new line character, before moving onto the next value of y
        outputString[MAX_X]='\0';
        printf("%s\n",outputString);
    }
}

void plotLineLow(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]){
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if (dy < 0){
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x <= x1; x++){
        screen[x][y]=LINE;
        if (D > 0){
            y = y + yi;
            D = D + (2 * (dy - dx));
        }
        else{
            D = D + 2 * dy;
        }
    }
}

void plotLineHigh(int x0, int y0, int x1, int y1, int screen[MAX_X][MAX_Y]){
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0){
        xi = -1;
        dx = -dx;
    }   
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++){
        screen[x][y]=LINE;
        if (D > 0){
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else{
            D = D + 2 * dx;
        }
    }
}

void BresenhamPlotLine(double pointA[2], double pointB[2], int screen[MAX_X][MAX_Y]){
    // Initialise points as doubles to do maths nicer
    int x0 = (int)pointA[0];
    int y0 = (int)pointA[1];
    int x1 = (int)pointB[0];
    int y1 = (int)pointB[1];

    if (fabs(y1 - y0) < fabs(x1 - x0)){
        if (x0 > x1){
            plotLineLow(x1, y1, x0, y0, screen);
        }
        else{
            plotLineLow(x0, y0, x1, y1, screen);
        }
    }
    else{
        if (y0 > y1){
            plotLineHigh(x1, y1, x0, y0, screen);
        }
        else{
            plotLineHigh(x0, y0, x1, y1, screen);
        }
    }
}

void drawCubeOnScreen(double arr[8][2],double origin[2], double ratio, int screen[MAX_X][MAX_Y]){

    
    printf("\033[H\033[J"); // Clear screen escape sequence


    double pointA[2] = {0,0};
    double pointB[2] = {0,0};
    double pointC[2] = {0,0};
    double pointD[2] = {0,0};
    double pointE[2] = {0,0};
    double pointF[2] = {0,0};
    //uses the Bresenham algorithm
    for (int i = 0; i < 4; i++){    
        //interates over x [0] and y[1], and assigns which two points will have lines drawn between them
        
            for (int j = 0; j < 2; j++){
                // modifying if point is x, by aspect ratio
                if (j == 0){
                    //drawing front face
                    pointA[j] = origin[j] + (arr[i][j] * ratio); //translates from unity to screenspace, and does aspect ratio adustment
                    pointB[j] = origin[j] + (arr[(i+1)%4][j] * ratio);//(i+1)%4 is used to draw line from point (0 -> 1), (1 -> 2), (2 -> 3), (3 -> 0).
                    //drawing back face
                    pointC[j] = origin[j] + (arr[(i+4)][j] * ratio);
                    pointD[j] = origin[j] + (arr[(((i+1)%4)+4)][j] * ratio);//((i+1)%4)+4 is used to draw line from point (4 -> 5), (5 -> 6), (6 -> 7), (7 -> 4).
                    //drawing interconnects between the two faces
                    pointE[j] = origin[j] + (arr[i][j] * ratio);
                    pointF[j] = origin[j] + (arr[(i+4)][j] * ratio);//(i+4) is used to draw line from point (0 -> 4), (1 -> 5), (2 -> 6), (3 -> 7).
                }
                // if y, dont modify by ratio
                else{
                   //drawing front face
                    pointA[j] = origin[j] + (arr[i][j]); //translates from unity to screenspace, and does aspect ratio adustment
                    pointB[j] = origin[j] + (arr[(i+1)%4][j]);//(i+1)%4 is used to draw line from point (0 -> 1), (1 -> 2), (2 -> 3), (3 -> 0).
                    //drawing back face
                    pointC[j] = origin[j] + (arr[(i+4)][j]);
                    pointD[j] = origin[j] + (arr[(((i+1)%4)+4)][j]);//((i+1)%4)+4 is used to draw line from point (4 -> 5), (5 -> 6), (6 -> 7), (7 -> 4).
                    //drawing interconnects between the two faces
                    pointE[j] = origin[j] + (arr[i][j]);
                    pointF[j] = origin[j] + (arr[(i+4)][j]);//(i+4) is used to draw line from point (0 -> 4), (1 -> 5), (2 -> 6), (3 -> 7).
                }
            }
        BresenhamPlotLine(pointA,pointB, screen);
        BresenhamPlotLine(pointC,pointD, screen);
        BresenhamPlotLine(pointE,pointF, screen);
    }
}

int main(void){
    
    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;

    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //keeping origin as (halfx,halfy) i.e. middle of screenspace

    double angle = 0;

    int screen[MAX_X][MAX_Y];

    double basePoints[8][3]={
        //front face of Cube
        {-1,-1,1}, 
        {1,-1,1}, 
        {1,1,1},
        {-1,1,1},
        //back face of Bube
        {-1,-1,-1},
        {1,-1,-1},
        {1,1,-1},
        {-1,1,-1},
    };

    // initScreen(screen);
    // initPoints(points);
    // plotMultiPointsRel(points);
    // displayScreen(screen);

    double points[8][3];
    double p_points[8][2];
    for (int i = 0; i < 37; i++){
        initScreen(screen); //fills screenspace array with dots. can act as a blank to re-draw over.
        
        initPoints(points,basePoints); //resets points back to unity values

        // scaleMulti3DPoints(points); //take unity points and scale

        rotatePointsAroundX(points, (angle * (M_PI/180))); //takes output of "scalePoints", rotates by "angle"
        rotatePointsAroundY(points, (angle * (M_PI/180))); //takes output of "scalePoints", rotates by "angle"
        rotatePointsAroundZ(points, (angle * (M_PI/180))); //takes output of "scalePoints", rotates by "angle"
        
        projectPoints2d(points,p_points);

        scaleMulti2DPoints(p_points);

        drawCubeOnScreen(p_points,origin,ratio,screen); // take output of "rotate points", translate to screenspace(and adjust for aspect ratio) (without modifying points!) and draw lines between points in array

        plotMultiPointAbs(p_points,origin,ratio,screen); // take output of "rotate points", translate to screenspace(and adjust for aspect ratio) (without modifying points!) and plots a "DOT" on points in array

        displayScreen(screen);

        angle = angle + 10;
        sleep(1);
    }
    
    return 0;
}
