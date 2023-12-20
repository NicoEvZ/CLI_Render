#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"


#define BLANK ' ' //SPACE character ASCII code
#define LINE '#' //'#' character ASCII code
#define DOT '@'
#define BORDER '*'

//#define DEBUG_POINTS

#define CHAR_CONST 2.3

vector camera = {0,0,0};

mesh importMeshFromOBJFile (char * pathToFile) 
{
    FILE *obj = fopen(pathToFile, "r");

    mesh newMesh;
    newMesh.numOfTris = 0;

    if (NULL == obj) 
    {
        printf("Error: .OBJ file not found\n");
        printf("Reminder: run the program from top level \"Cube/\" dir\n");
        return newMesh;
    }

    char line[MAX_LINE_LENGTH];
    int verts = 0;
    int faces = 0;
    vector pointCoords;
    vector average;
    vector runningTotal;
    int p0, p1, p2;
    
    //counts faces and verticies.
    while (fgets(line, sizeof(line), obj) != NULL) 
    {
        if (line[0] == 'v' && line[1] == ' ') 
        {
            verts++;
        }
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            faces++;
        }
    }

    newMesh.numOfTris = faces;
    newMesh.numOfVerts = verts;

    vector (*vectorArray) = malloc(newMesh.numOfVerts * sizeof(vector)); //dynamically allocates an array of vectors, based off the number verticies.

    newMesh.tris = (triangle*) malloc(newMesh.numOfTris * sizeof(triangle)); //dynamically allocates memory for the number of triangles

    rewind(obj);
    char a[20];
    char b[20];
    char c[20];
    char junk[MAX_LINE_LENGTH];
    char buffChar[20];
    int vCount = 0;
    int fCount = 0;
    while (fgets(line, sizeof(line), obj) != NULL)
    {
        //reads all lines of obj that start with "v " into the dynamically assigned array of vectors.
        if (line[0] == 'v' && line[1] == ' ') 
        {
            sscanf(line,"v %lf %lf %lf", &pointCoords.x, &pointCoords.y, &pointCoords.z);

            vectorArray[vCount] = pointCoords;

            vCount++;

            runningTotal = addVec(runningTotal, pointCoords);
        }
        //reads all lines of obj that start with "f ".
        //each int after 'f ' represents an index (starting at 1), of the vector array of verticies.
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            sscanf(line,"f %s %s %s", a, b, c);

            // for (int i = 0; i < sizeof(buffChar); i++)
            // *a = stripString(a, '/');
            // b[0] = stripString(b, '/');
            // c[0] = stripString(c, '/');
            sscanf(a, "%d/%s", &p0, &junk);
            sscanf(b, "%d/%s", &p1, &junk);
            sscanf(c, "%d/%s", &p2, &junk);

            newMesh.tris[fCount].p[0] = vectorArray[(p0-1)];
            newMesh.tris[fCount].p[1] = vectorArray[(p1-1)];
            newMesh.tris[fCount].p[2] = vectorArray[(p2-1)];

            fCount++;
        }
    }

    #ifdef DEBUG_POINTS
    //Handy debug for seeing if points were imported properly
    for (int i = 0; i < vCount; i++) 
    {
          printf("%d: %lf, %lf, %lf\n", i, vectorArray[i].x, vectorArray[i].y, vectorArray[i].z);
    }
    #endif

    average = divVecByScalar(runningTotal, vCount);

    for (int j = 0; j < fCount; j++) 
    {
        for (int i = 0; i < 3; i++) 
        {
            newMesh.tris[j].p[i] = subVec(newMesh.tris[j].p[i], average);
        }
    }

    free(vectorArray);
    fclose(obj);

    return newMesh;
}

char * stripString(char *inputString, char stripChar)
{
    char buffChar[MAX_LINE_LENGTH]={0};
    for (int i = 0; i < MAX_LINE_LENGTH; i++)
            {   
                if (inputString[i] != stripChar)
                {
                    buffChar[i] = inputString[i];
                }
                else
                {
                    break;
                }
            }
    return *buffChar;
}

int importJSON(const char *file_path, data *importData_struct)
{
    // Open the file for reading
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the content of the file into a buffer
    char *json_buffer = (char *)malloc(file_size + 1);
    fread(json_buffer, 1, file_size, file);
    fclose(file);

    // Null-terminate the buffer
    json_buffer[file_size] = '\0';

    // Parse the JSON from the buffer
    cJSON *root = cJSON_Parse(json_buffer);

    if (root == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(json_buffer);
        return 1;
    }

    // Access values in the JSON object    
    cJSON *distanceJSON = cJSON_GetObjectItemCaseSensitive(root, "distance");
    cJSON *scaleJSON = cJSON_GetObjectItemCaseSensitive(root, "scale");
    cJSON *objFileJOSN = cJSON_GetObjectItemCaseSensitive(root, "objFile");
    cJSON *iterationsJSON = cJSON_GetObjectItemCaseSensitive(root, "iterations");
    cJSON *rotateXJSON = cJSON_GetObjectItemCaseSensitive(root, "rotateX");
    cJSON *rotateYJSON = cJSON_GetObjectItemCaseSensitive(root, "rotateY");
    cJSON *rotateZJSON = cJSON_GetObjectItemCaseSensitive(root, "rotateZ");
    cJSON *screenWidthJSON = cJSON_GetObjectItemCaseSensitive(root, "screenWidth");
    cJSON *screenHeightJSON = cJSON_GetObjectItemCaseSensitive(root, "screenHeight");
    cJSON *rasteriseBoolJSON = cJSON_GetObjectItemCaseSensitive(root, "rasterise");


    if(cJSON_IsNumber(distanceJSON))
    {
        importData_struct->distance = distanceJSON->valuedouble;
    }

    if(cJSON_IsNumber(scaleJSON))
    {
        importData_struct->scale = scaleJSON->valuedouble;
    }

    if(cJSON_IsNumber(iterationsJSON))
    {
        importData_struct->i = iterationsJSON->valueint;
    }

    if(cJSON_IsBool(rotateXJSON) & cJSON_IsBool(rotateYJSON) & cJSON_IsBool(rotateZJSON))
    {
        importData_struct->rotationX = rotateXJSON->valueint;
        importData_struct->rotationY = rotateYJSON->valueint;
        importData_struct->rotationZ = rotateZJSON->valueint;
    }

    if(cJSON_IsString(objFileJOSN))
    {
        strncpy(importData_struct->objPathBuffer,objFileJOSN->valuestring,sizeof(importData_struct->objPathBuffer) -1);
        importData_struct->objPathBuffer[sizeof(importData_struct->objPathBuffer) -1] = '\0';
    }

    if(cJSON_IsNumber(screenWidthJSON) & cJSON_IsNumber(screenHeightJSON))
    {
        importData_struct->screenWidthImprt = screenWidthJSON->valueint;
        importData_struct->screenHeightImprt = screenHeightJSON->valueint;
    }

    if (cJSON_IsBool(rasteriseBoolJSON))
    {
        importData_struct->rasteriseBool = rasteriseBoolJSON->valueint;
    }

    // Don't forget to free the cJSON object and the buffer when you're done with them
    cJSON_Delete(root);
    free(json_buffer);

    return 0;
}

mesh copyMeshData(mesh fromMesh, mesh toMesh)
{
    toMesh.numOfTris = fromMesh.numOfTris;
    toMesh.numOfVerts = fromMesh.numOfVerts;
    toMesh.tris = (triangle*) malloc(toMesh.numOfTris * sizeof(triangle));
    for (int i = 0; i < toMesh.numOfTris; i++)
    {
        toMesh.tris[i] = fromMesh.tris[i];
    }
    return toMesh;
}

//vec1.elements plus vec2.elements
vector addVec( vector vec1, vector vec2) 
{
    vector returnVec;

    returnVec.x = vec1.x + vec2.x;
    returnVec.y = vec1.y + vec2.y;
    returnVec.z = vec1.z + vec2.z;

    return returnVec;
}

//vec1.elements minus vec2.elements
vector subVec(vector vec1, vector vec2) 
{
    vector returnVec;

    returnVec.x = vec1.x - vec2.x;
    returnVec.y = vec1.y - vec2.y;
    returnVec.z = vec1.z - vec2.z;

    return returnVec;
}

//devides each element of the vector by the value given
vector divVecByScalar(vector vec, int scalar) 
{
    vector returnVec;

    returnVec.x = vec.x/(double)scalar;
    returnVec.y = vec.y/(double)scalar;
    returnVec.z = vec.z/(double)scalar;

    return returnVec;
}

vector vecCrossProduct(vector vec1, vector vec2)
{
    vector output;

    output.x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
    output.y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
    output.z = (vec1.x * vec2.y) - (vec1.y * vec2.x);

    return output;
}

void projectMeshTo2D(mesh inputMesh, const double distance) 
{
    vector tempVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++)
        {
        tempVec = inputMesh.tris[i].p[j];

        tempVec.x = tempVec.x * CHAR_CONST;

        double zPerspective = 1/(distance - tempVec.z);

        double p_Mat[2][3] = {{zPerspective,0,0},{0,zPerspective,0}};
        
        double x_p = (p_Mat[0][0]*tempVec.x)+(p_Mat[0][1]*tempVec.y)+(p_Mat[0][2]*tempVec.z);
        double y_p = (p_Mat[1][0]*tempVec.x)+(p_Mat[1][1]*tempVec.y)+(p_Mat[1][2]*tempVec.z);
        
        inputMesh.tris[i].p[j].x = x_p;
        inputMesh.tris[i].p[j].y = y_p;

        }
    }
}

void drawMeshOnScreen(mesh inputMesh, vector origin, screenStruct screen, vector *inputVecArr) 
{
    printf("\033[H\033[J"); //clears the screen

    triangle output = {{{0,0,0}}};
    vector normal = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        normal = inputVecArr[i];
        // double normalCheck = normal.x * (inputMesh.tris[i].p[0].x - camera.x) +
        //                      normal.y * (inputMesh.tris[i].p[0].y - camera.y) +
        //                      normal.z * (inputMesh.tris[i].p[0].z - camera.z);

        double normalCheck = normal.z;
        // printf("Triangle: %d\n",i);
        // printf("normal.z: %lf\n", normalCheck);

        if (normalCheck > 0)
        {
            for (int j = 0; j < 3; j++)
            {
                //translates from unity to screenspace, and does aspect ratio adustment
                // output.p[j].x = origin[0] + (inputMesh.tris[i].p[j].x); 
                output.p[j].x = origin.x + (inputMesh.tris[i].p[j].x); 
                output.p[j].y = origin.y + inputMesh.tris[i].p[j].y;
            
            }
            BresenhamPlotLine(output.p[0],output.p[1],screen);
            BresenhamPlotLine(output.p[1],output.p[2],screen);
            BresenhamPlotLine(output.p[2],output.p[0],screen);
        }
        // displayScreen(&screen);
    }
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

int pixelInTriangle(triangle inputTri, int x, int y)
{
    vector A = inputTri.p[0];
    vector B = inputTri.p[1];
    vector C = inputTri.p[2];
    vector P;
    P.x = x;
    P.y = y;
    // Calculate the barycentric coordinates
    // of point P with respect to triangle ABC
    double denominator = ((B.y- C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    double a = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
    double b = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;
    double c = 1 - a - b;
 
    // Check if all barycentric coordinates
    // are non-negative
    if (a >= 0 && b >= 0 && c >= 0) 
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

void rasteriseMeshOnScreen(mesh inputMesh, vector origin, screenStruct screen, vector *inputVecArr)
{
    printf("\033[H\033[J"); //clears the screen

    triangle output = {{{0,0,0}}};
    double trisToRasterCounter = 0;


    vector lightDirection = {0, 1, 0};
    double l = sqrtl(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);
    lightDirection = divVecByScalar(lightDirection, l);

    vector normal = {0,0,0};
    
    for (int i = 0; i < inputMesh.numOfTris; i++)
    {
        normal = inputVecArr[i];
        double dp = normal.x * lightDirection.x + normal.y * lightDirection.y + normal.z * lightDirection.z;
        char c = getGrad(dp);
        // double normalCheck = normal.x * (inputMesh.tris[i].p[0].x - camera.x) +
        //                      normal.y * (inputMesh.tris[i].p[0].y - camera.y) +
        //                      normal.z * (inputMesh.tris[i].p[0].z - camera.z);

        double normalCheck = normal.z;
        // printf("Triangle: %d\n",i);
        // printf("normal.z: %lf\n", normalCheck);

        if (normalCheck > 0)
        {
            trisToRasterCounter++;
            for (int j = 0; j < 3; j++)
            {
                //translates from unity to screenspace, and does aspect ratio adustment
                // output.p[j].x = origin[0] + (inputMesh.tris[i].p[j].x); 
                output.p[j].x = origin.x + (inputMesh.tris[i].p[j].x); 
                output.p[j].y = origin.y + inputMesh.tris[i].p[j].y;
            }

            for (int x = 0; x < screen.width; x++)
            {
                for (int y = 0; y < screen.height; y++)
                {
                    if (pixelInTriangle(output,x,y))
                    {
                        screen.screen[x][y] = c;
                    }
                }
            }
            // fillTriangle(output.p[0], output.p[1], output.p[2], screen);
            // BresenhamPlotLine(output.p[0],output.p[1],screen);
            // BresenhamPlotLine(output.p[1],output.p[2],screen);
            // BresenhamPlotLine(output.p[2],output.p[0],screen);
        }
        // printf("\033[H\033[J"); //clears the screen
        // displayScreen(&screen);

    }
}

char  getGrad(double lum)
{
    // `````` = 96
    // '''''' = 39
    // ****** = 42
    // !!!!!! = 33
    // $$$$$$ = 36
    // &&&&&& = 38
    // %%%%%% = 37
    // ###### = 35

    char outputChar;
    int grad = (int)7 * lum;
    switch (grad)
    {
    case 0:
        outputChar = '`';
        break;
    case 1:
        outputChar = '\'';
        break;
    case 2:
        outputChar = '*';
        break;
    case 3:
        outputChar = '!';
        break;
    case 4: 
        outputChar = '$';
        break;
    case 5:
        outputChar = '&';
        break;
    case 6:
        outputChar = '%';
        break;
    case 7:
        outputChar = '#';
        break;
    default:
        outputChar = ' ';
        break;
    }
    return outputChar;
}

void scale2DPoints(mesh inputMesh, const double scaleFactor) 
{
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j = 0; j < 3; j++)
        {
            //extract x and y from array.
            double x = inputMesh.tris[i].p[j].x;
            double y = inputMesh.tris[i].p[j].y;

            x = x * scaleFactor;
            y = y * scaleFactor;

            inputMesh.tris[i].p[j].x = x;
            inputMesh.tris[i].p[j].y = y;
        }
    }
}

mesh rotateMeshAroundX(mesh inputMesh, const double angle) 
{
    vector rotatedVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j =0; j < 3; j++)
        {
            rotatedVec.x = inputMesh.tris[i].p[j].x;

            rotatedVec.y = ((inputMesh.tris[i].p[j].y * cos(angle)) - (inputMesh.tris[i].p[j].z * sin(angle)));

            rotatedVec.z = ((inputMesh.tris[i].p[j].y * sin(angle)) + (inputMesh.tris[i].p[j].z * cos(angle)));

            inputMesh.tris[i].p[j] = rotatedVec;
        }
    }
    return inputMesh;
}

mesh rotateMeshAroundY(mesh inputMesh, const double angle) 
{
    vector rotatedVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j =0; j < 3; j++)
        {
            rotatedVec.x = ((inputMesh.tris[i].p[j].x * cos(angle)) + (inputMesh.tris[i].p[j].z * sin(angle)));

            rotatedVec.y = inputMesh.tris[i].p[j].y;

            rotatedVec.z = ((inputMesh.tris[i].p[j].x * -1 * sin(angle)) + (inputMesh.tris[i].p[j].z * cos(angle)));

            inputMesh.tris[i].p[j] = rotatedVec;
        }
    }
    return inputMesh;
}

mesh rotateMeshAroundZ(mesh inputMesh, const double angle) 
{
    vector rotatedVec = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++) 
    {
        for (int j =0; j < 3; j++)
        {
            rotatedVec.x = ((inputMesh.tris[i].p[j].x * cos(angle)) - (inputMesh.tris[i].p[j].y * sin(angle)));

            rotatedVec.y = ((inputMesh.tris[i].p[j].x * sin(angle)) + (inputMesh.tris[i].p[j].y * cos(angle)));

            rotatedVec.z = inputMesh.tris[i].p[j].z;

            inputMesh.tris[i].p[j] = rotatedVec;
        }
    }
    return inputMesh;
}

vector calculateTriangleNormal(triangle inputTri)
{   
    vector U,V,normal;
    
    U = (subVec(inputTri.p[1],inputTri.p[0]));
    V = (subVec(inputTri.p[2],inputTri.p[1]));

    normal = vecCrossProduct(U, V);

    //its normally normal to normalise the normal

    double l = sqrtl(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    normal = divVecByScalar(normal, l);
    
    return normal;
}

void calculateMeshNormals(mesh inputMesh, vector *inputArray)
{
    vector normal = {0,0,0};
    for (int i = 0; i < inputMesh.numOfTris; i++)
    {
        normal = calculateTriangleNormal(inputMesh.tris[i]);
        inputArray[i] = normal;
    }
}

void clearScreen(screenStruct *screen) 
{
    for (int x = 0; x < screen->width; x++) 
    {
        for (int y = 0; y < screen->height; y++) 
        {
            screen->screen[x][y]=BLANK;
            if ((x == 0) | (y == 0) | (x == (screen->width-1)) | (y == (screen->height-1))) 
            {   
                screen->screen[x][y]=BORDER;
            }
        }
    }
}

void initScreen(screenStruct *screen)
{
    screen->screen = malloc(screen->width * sizeof(int *));
    for (int i = 0; i < screen->width; i++)
    {
        screen->screen[i] = malloc(screen->height * sizeof(int));
    }
}

void deleteScreen(screenStruct *screen)
{
    for (int i = 0; i < screen->width; i++)
    {
        free(screen->screen[i]);
    }
    free(screen->screen);
}

void drawInScreen(screenStruct screen, int x, int y, const char ASCII) 
{
    if (x < 0) 
    {

        x = 0;
    }

    if (x > (screen.width-1)) 
    {

        x = screen.width-1;
    }

    if (y < 0) 
    {

        y = 0;
    }

    if (y > (screen.height-1)) 
    {

        y = screen.height-1;
    }

    screen.screen[x][y] = ASCII;
}

void displayScreen(screenStruct *screen)
{
    // Iterate over y axis
    char outputString[screen->width+1];
    for (int y = 0; y < screen->height; y++)
    {
        for (int x = 0; x < screen->width; x++)
        {
            // Store current value in array at point(x,y), as char in string
            // String is length of screen.width
            outputString[x]=screen->screen[x][y];
        }
        // Display filled string, and new line character, before moving onto the next value of y
        outputString[screen->width]='\0';
        printf("%s\n",outputString);
    }
}

void plotLineLow(int x0, int y0, int x1, int y1, screenStruct screen)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if (dy < 0) 
    {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        if ((x <= 0) | (y <= 0) | (x >= (screen.width-1)) | (y >= (screen.height-1))) 
        {
            drawInScreen(screen,x,y,BORDER);
        }
        else
        {
            drawInScreen(screen,x,y,LINE);
        }
            
        if (D > 0) 
        {
            y = y + yi;
            D = D + (2 * (dy - dx));
        }
        else
        {
            D = D + 2 * dy;
        }
    }
}

void plotLineHigh(int x0, int y0, int x1, int y1, screenStruct screen)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) 
    {
        xi = -1;
        dx = -dx;
    }   
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++) 
    {
        if ((x <= 0) | (y <= 0) | (x >= (screen.width-1)) | (y >= (screen.height-1))) 
        {
            drawInScreen(screen,x,y,BORDER);
        }
        else
        {
            drawInScreen(screen,x,y,LINE);
        }
        
        if (D > 0) 
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
        {
            D = D + 2 * dx;
        }
    }
}

void BresenhamPlotLine(vector pointA, vector pointB, screenStruct screen)
{
    // Initialise points as doubles to do maths nicer
    int x0 = (int)pointA.x;
    int y0 = (int)pointA.y;
    int x1 = (int)pointB.x;
    int y1 = (int)pointB.y;

    if (abs(y1 - y0) < abs(x1 - x0)) 
    {

        if (x0 > x1) 
        {
            plotLineLow(x1, y1, x0, y0, screen);
        }
        else
        {
            plotLineLow(x0, y0, x1, y1, screen);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            plotLineHigh(x1, y1, x0, y0, screen);
        }
        else
        {
            plotLineHigh(x0, y0, x1, y1, screen);
        }
    }
}