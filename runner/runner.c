#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "cJson-w.h"
#include "draw.h"
#include "runner.h"

// #define DEBUG_POINTS_IMPORT
// #define DEBUG_POINTS_RENDER
// #define DEBUG_POINTS_FRAME_TIMER
// #define AVERAGE_COORDS

int main(void){
    cursesSetup();
    renderConfig importData;
    frameBuffer screen;
    frameBuffer oldScreen;
    
    #ifdef DEBUG_POINTS_FRAME_TIMER
    clock_t accumulatedDrawTime = 0;
    clock_t accumulatedCalcTime = 0;
    clock_t calcTimer;
    #endif

    const char jsonImportPath[] = "data/renderOptions.json";

    //failsafe that exits code if the importMeshFromOBJFile didn't succeed.
    if (importJSON(jsonImportPath, &importData))
    {
        printf("Import of JSON failed, exiting here");
        return 1;
    }

    initialiseFrameBuffer(&screen, importData);
    initialiseFrameBuffer(&oldScreen, importData);

    for(int i = 0; i < oldScreen.width; i++)
    {
        for(int j = 0; j < oldScreen.height; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                oldScreen.colourBuffer[i][j][k] = 0;
            }
        }
    }

    double angle = 0;
    double lightAngle = 0;

    //Store OBJ data in mesh struct
    mesh baseMesh = importMeshFromOBJFile(importData.objPathBuffer); 
    if (baseMesh.numberOfTriangles == 0)
    {
        printf("Error: Import failed, or OBJ file contains no vertex data\n");
        printf("Exiting...\n");
        return 0;
    }

    int meshColour[3] = {0, 255, 0};

    for (int i = 0; i < 3; i++)
    {
        baseMesh.colour[i] = meshColour[i];
    }
    
    matrix4x4 rotateX;
    matrix4x4 rotateY;
    matrix4x4 rotateZ;
    
    matrix4x4 rotateLightX;
    matrix4x4 rotateLightY;
    matrix4x4 rotateLightZ;

    matrix4x4 projectionMatrix;

    matrix4x4 translation;
    initialiseTranslationMatrix(&translation, 0, 0, (double)importData.distance);

    matrix4x4 world;

    vector vCamera;
    initialiseVector(&vCamera);
    vCamera = (vector){0, 0, 0, 1};

    vector vLookDirection = (vector){0, 0, 1, 1};
    vector vUp = (vector){0, 1, 0, 1};

    vector vTarget;
    matrix4x4 mCamera;
    matrix4x4 mView;

    vector (*normalsVectorArray) = malloc(baseMesh.numberOfTriangles * sizeof(vector));
    triangle (*renderBufferArray) = malloc(baseMesh.numberOfTriangles * sizeof(triangle));

    size_t sizeOfScreen = (sizeof(char) * ((screen.width) * (screen.height) * 40));
    char *a = malloc(sizeOfScreen);

    double *frameDrawTimes = malloc(sizeof(double) * (importData.iterations - importData.startFrame));

    if (setvbuf(stdout, a, _IOFBF, sizeOfScreen))
    {
        printf("Error setting buffer\n");
        fflush(stdout);
    }

    //escape code sequence for clearing the screen, and hiding cursor.
    printf("\e[H\e[J\e[?25l");
    fflush(stdout);
    
    int framesRendered = 0;
    int i = importData.startFrame;
    int buttonInput;
    int loopProgram = 1;

    do    
    {   
        buttonInput = getch();
        switch (buttonInput)
            {
            case KEY_DOWN:
                vCamera.y -= 0.05;
                break;

            case KEY_UP:
                vCamera.y += 0.05;
                break;

            case KEY_RIGHT:
                vCamera.x += 0.05;
                break;

            case KEY_LEFT:
                vCamera.x -= 0.05;
                break;
            
            case 'i':
                vCamera.z += 0.05;
                break;

            case 'k':
                vCamera.z -= 0.05;
                break;

            case 'a':
                if (i < importData.iterations)
                {
                    i++;
                }   
                break;

            case 's':
                if (i > importData.startFrame)
                {
                    i--;
                }   
                break;

            case 'q':
                loopProgram = -1;
                break;

            default:
                // printw("\"%c\"\n", buttonInput);
                break;
            }
        initialiseProjectionMatrix(importData, &projectionMatrix);
        angle = i * RAD;
        // lightAngle = i * RAD;
        #ifdef DEBUG_POINTS_FRAME_TIMER
        //grab current time i.e. "starts the timing"
        calcTimer = clock();
        #endif

        vTarget = addVector(vCamera, vLookDirection);

        InitialisePointAtMatrix(&mCamera, vCamera, vTarget, vUp);

        mView = quickMatrixInverse(mCamera);

        vector lightDirection = (vector){0, -0.6, 0.4, 1};   
        int numberOfTrianglessToRender = 0;
        clearFrameBuffer(&screen);
        initialiseRotateXMatrix(&rotateX, angle);
        initialiseRotateYMatrix(&rotateY, angle);
        initialiseRotateZMatrix(&rotateZ, angle);

        intialiseIdentityMatrix(&world);

        if(importData.rotationX)
        {
            world = matrixMatrixMultiply(rotateX, world);
        }

        if(importData.rotationY)
        {
            world = matrixMatrixMultiply(rotateY, world);
        }

        if(importData.rotationZ)
        {
            world = matrixMatrixMultiply(rotateZ, world);
        }
                
        world = matrixMatrixMultiply(world, translation);

        initialiseRotateXMatrix(&rotateLightX, lightAngle);
        initialiseRotateYMatrix(&rotateLightY, lightAngle);
        initialiseRotateZMatrix(&rotateLightZ, lightAngle);

        //3-phase thransform of r,g and b. Cycle through all colours once, regardless of max number of rotations
        cycleMeshColour(&baseMesh, i, importData.iterations);

        lightDirection = matrixVectorMultiply(lightDirection, rotateLightX);
        // lightDirection = matrixVectorMultiply(lightDirection, rotateLightY);
        // lightDirection = matrixVectorMultiply(lightDirection, rotateLightZ);

        for (int j = 0; j < baseMesh.numberOfTriangles; j++)
        {
            triangle projectedTriangle, transformedTriangle, viewedTriangle;

            copyTriangleData(baseMesh.trianglePointer[j], &transformedTriangle);

            inheritColourFromMesh(baseMesh.colour, &transformedTriangle);

            // rotate around axes and offest into screen
            transformedTriangle = matrixTriangleMultiply(transformedTriangle, world);

            #ifdef DEBUG_POINTS_TRI_DATA
            printf("Transformed Triangle %d:\n",j+1);
            printf("\tp[0]: (%lf, %lf, %lf)\n", transformedTriangle.point[0].x, transformedTriangle.point[0].y, transformedTriangle.point[0].z);
            printf("\tp[1]: (%lf, %lf, %lf)\n", transformedTriangle.point[1].x, transformedTriangle.point[1].y, transformedTriangle.point[1].z);
            printf("\tp[2]: (%lf, %lf, %lf)\n\n", transformedTriangle.point[2].x, transformedTriangle.point[2].y, transformedTriangle.point[2].z);
            #endif

            //calculate face normals
            normalsVectorArray[j] = calculateTriangleNormal(transformedTriangle);

            double dotProductResult = dotProduct(normalsVectorArray[j], (subtractVector(transformedTriangle.point[0], vCamera)));
           
            if (dotProductResult < 0)
            {
                continue;    
            }

            //assign the "illumination" symbol based off normal
            illuminateTriangle(&transformedTriangle, normalsVectorArray[j], lightDirection);

            for (int point = 0; point < 3; point++)
            {
                transformedTriangle.point[point].x *= importData.characterRatio; 
            }

            copyTriangleData(transformedTriangle, &viewedTriangle);

            viewedTriangle = matrixTriangleMultiply(transformedTriangle, mView);

            copyTriangleData(viewedTriangle, &projectedTriangle);

            //project 3D --> 2D
            projectedTriangle = matrixTriangleMultiply(viewedTriangle, projectionMatrix);

            #ifdef DEBUG_POINTS_TRI_DATA
            printf("Projected Triangle %d:\n",j+1);
            printf("\tp[0]: (%lf, %lf, %lf)\n", projectedTriangle.point[0].x, projectedTriangle.point[0].y, projectedTriangle.point[0].z);
            printf("\tp[1]: (%lf, %lf, %lf)\n", projectedTriangle.point[1].x, projectedTriangle.point[1].y, projectedTriangle.point[1].z);
            printf("\tp[2]: (%lf, %lf, %lf)\n", projectedTriangle.point[2].x, projectedTriangle.point[2].y, projectedTriangle.point[2].z);
            printf("\tnormal: (%lf, %lf, %lf)\n\n", normalsVectorArray[j].x, normalsVectorArray[j].y, normalsVectorArray[j].z);
            #endif

            //normailse the coordinates with the extra vector term
            for (int point = 0; point < 3; point++)
            {
                projectedTriangle.point[point] = divideVectorByScalar(projectedTriangle.point[point], projectedTriangle.point[point].w);
            }
            
            #ifdef DEBUG_POINTS_TRI_DATA
            printf("Projected and Normalised Triangle %d:\n",j+1);
            printf("\tp[0]: (%lf, %lf, %lf)\n", projectedTriangle.point[0].x, projectedTriangle.point[0].y, projectedTriangle.point[0].z);
            printf("\tp[1]: (%lf, %lf, %lf)\n", projectedTriangle.point[1].x, projectedTriangle.point[1].y, projectedTriangle.point[1].z);
            printf("\tp[2]: (%lf, %lf, %lf)\n", projectedTriangle.point[2].x, projectedTriangle.point[2].y, projectedTriangle.point[2].z);
            printf("\tnormal: (%lf, %lf, %lf)\n\n", normalsVectorArray[j].x, normalsVectorArray[j].y, normalsVectorArray[j].z);
            #endif
           
            //scale points
            scaleTriangle(&projectedTriangle, screen);

            // for (int triCopy = 0; triCopy < 3; triCopy++ ) 

            #ifdef DEBUG_POINTS_TRI_DATA
            printf("Scaled and Projected and Normalised Triangle (+z from transformed triangle) %d:\n",j+1);
            printf("\tp[0]: (%lf, %lf, %lf)\n", projectedTriangle.point[0].x, projectedTriangle.point[0].y, projectedTriangle.point[0].z);
            printf("\tp[1]: (%lf, %lf, %lf)\n", projectedTriangle.point[1].x, projectedTriangle.point[1].y, projectedTriangle.point[1].z);
            printf("\tp[2]: (%lf, %lf, %lf)\n", projectedTriangle.point[2].x, projectedTriangle.point[2].y, projectedTriangle.point[2].z);
            printf("\tnormal: (%lf, %lf, %lf)\n\n", normalsVectorArray[j].x, normalsVectorArray[j].y, normalsVectorArray[j].z);
            #endif

            copyTriangleData(projectedTriangle,&renderBufferArray[numberOfTrianglessToRender]);
            numberOfTrianglessToRender++;
        }

        triangle* trisToRender = malloc(numberOfTrianglessToRender * sizeof(*trisToRender));
        
        for (int k = 0; k < numberOfTrianglessToRender; k++)
        {
            copyTriangleData(renderBufferArray[k],&trisToRender[k]);
        }

        for (int tri = 0; tri < numberOfTrianglessToRender; tri++)
        {   
            #ifdef DEBUG_POINTS_RENDER
            printf("Loading... %d%% complete:\n",(int)(((double)tri/(double)numberOfTrianglessToRender)*100));
            printf("Drawing (%d/%d)\n",tri+1,numberOfTrianglessToRender);
            #endif

            drawTriangleOnScreen(trisToRender[tri], &screen, importData.rasteriseBool);

            // #ifdef DEBUG_POINTS_RENDER_INDIVIDUAL
            // printf("\e7\e[1;1H\e[48;5;255m\e[38;5;9m\e[4mTri(%d)\e8\e[m",tri);
            // fflush(stdout);
            // // frameDelay(12);
            // #endif
            
        }

        #ifdef DEBUG_POINTS_RENDER
        printf("Angle: %lf\n",(angle*(180/PI)));
        printf("Final Output:\n");
        #endif

        #ifdef DEBUG_POINTS_FRAME_TIMER
        //finds diff between previous time and current time, i.e. "measure time taken"
        calcTimer = clock() - calcTimer;
        accumulatedCalcTime += (calcTimer);

        clock_t drawTimer;
        drawTimer = clock();
        #endif
        drawScreenBorder(&screen);

        #ifdef DEBUG_POINTS_NO_CLEARSCREEN
        // for (int newlines = 0; newlines <= importData.screenHeightImport + 2; newlines++)
        for (int newlines = 0; newlines <= importData.screenHeightImport + 2; newlines++)
        {
            printf("\n");
        }
        fflush(stdout);
        #endif

        // displayFrameBuffer(&screen);
        displayFrameBufferFastColour(screen, oldScreen);
        // displayFrameBufferSlowColour(screen);
        // displayDepthBuffer(screen, oldScreen);
        fflush(stdout);
        #ifdef DEBUG_POINTS_FRAME_TIMER
        framesRendered++;
        drawTimer = clock() - drawTimer;
        accumulatedDrawTime += drawTimer;

        printf("\e7\e[1;1H\e[48;5;255m\e[38;5;9m\e[4m"
        "Frame: %3d / %3d\t"
        "Calc Time: %3.3lfms\t"
        "Running Avg: %3.3lfms\t"
        "Draw Time: %3.3lfms\t"
        "Running Avg: %3.3lfms"
        "\e8\e[m",
        i,
        importData.iterations -1, 
        ((double)calcTimer / CLOCKS_PER_SEC) * 1000,
        ((double)(accumulatedCalcTime / (framesRendered)) / CLOCKS_PER_SEC) * 1000,
        ((double)drawTimer / CLOCKS_PER_SEC) * 1000,
        ((double)(accumulatedDrawTime  / (framesRendered)) / CLOCKS_PER_SEC) * 1000);
        fflush(stdout);
        frameDrawTimes[i] = (((double)drawTimer / CLOCKS_PER_SEC) * 1000);
        #endif
        //screen becomes oldScreen
        copyFrameBufferData(screen, &oldScreen);
        clearFrameBuffer(&screen);

        #ifdef DEBUG_POINTS_ZBUFFER
        displayDepthBuffer(&screen);
        #endif
        frameDelay(importData.framesPerSecond);
        
        free(trisToRender);
    } while(loopProgram > 0);
    //escape code sequence for returning cursor below drawn frame, and replacing it to standard cursor.
    printf("\n\e[m\e[?25h\e[?12h");
    #ifdef DEBUG_POINTS_FRAME_TIMER
    printf("Total Calc Time: %3.3lfms\tAverage: %3.3lfms\tTotal Draw Time: %3.3lfms\tAverage: %3.3lfms\n",
    ((double)accumulatedCalcTime / CLOCKS_PER_SEC) * 1000,
    ((double)(accumulatedCalcTime / (framesRendered)) / CLOCKS_PER_SEC) * 1000,
    ((double)accumulatedDrawTime / CLOCKS_PER_SEC) * 1000,
    ((double)(accumulatedDrawTime  / (framesRendered)) / CLOCKS_PER_SEC) * 1000);
    for (int num = importData.startFrame; num < importData.iterations; num++)
    {
        if (frameDrawTimes[num] > 1.15)
        {
            continue;
        }
        printf("Frame: %d\tDraw Time: %3.3lf\n", num, frameDrawTimes[num]);
    }
    #endif
    fflush(stdout);
    free(a);
    free(normalsVectorArray);
    free(renderBufferArray);
    deleteFrameBuffer(&screen);
    deleteFrameBuffer(&oldScreen);
    cursesEnd();
    return 0;
}

int importJSON(const char *file_path, renderConfig *importData_struct)
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
    cJSON *fovJSON = cJSON_GetObjectItemCaseSensitive(root, "fov");
    cJSON *objFileJOSN = cJSON_GetObjectItemCaseSensitive(root, "objFile");
    cJSON *iterationsJSON = cJSON_GetObjectItemCaseSensitive(root, "iterations");
    cJSON *startFrameJSON = cJSON_GetObjectItemCaseSensitive(root, "startFrame");
    cJSON *rotateXJSON = cJSON_GetObjectItemCaseSensitive(root, "rotateX");
    cJSON *rotateYJSON = cJSON_GetObjectItemCaseSensitive(root, "rotateY");
    cJSON *rotateZJSON = cJSON_GetObjectItemCaseSensitive(root, "rotateZ");
    cJSON *screenWidthJSON = cJSON_GetObjectItemCaseSensitive(root, "screenWidth");
    cJSON *screenHeightJSON = cJSON_GetObjectItemCaseSensitive(root, "screenHeight");
    cJSON *rasteriseBoolJSON = cJSON_GetObjectItemCaseSensitive(root, "rasterise");
    cJSON *framesperSecondJSON = cJSON_GetObjectItemCaseSensitive(root, "FPSTarget");
    cJSON *characterRatioJSON = cJSON_GetObjectItemCaseSensitive(root, "characterRatio");

    if(cJSON_IsNumber(distanceJSON))
    {
        importData_struct->distance = distanceJSON->valuedouble;
    }

    if(cJSON_IsNumber(fovJSON))
    {
        importData_struct->fov = fovJSON->valuedouble;
    }

    if(cJSON_IsNumber(iterationsJSON) & cJSON_IsNumber(startFrameJSON))
    {
        importData_struct->iterations = iterationsJSON->valueint;
        importData_struct->startFrame = startFrameJSON->valueint;
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
        importData_struct->screenWidthImport = screenWidthJSON->valueint;
        importData_struct->screenHeightImport = screenHeightJSON->valueint;
    }

    if (cJSON_IsBool(rasteriseBoolJSON))
    {
        importData_struct->rasteriseBool = rasteriseBoolJSON->valueint;
    }

    if (cJSON_IsNumber(framesperSecondJSON))
    {
        importData_struct->framesPerSecond = framesperSecondJSON->valuedouble;
    }

    if (cJSON_IsNumber(characterRatioJSON))
    {
        importData_struct->characterRatio = characterRatioJSON->valuedouble;
    }
    
    // Don't forget to free the cJSON object and the buffer when you're done with them
    cJSON_Delete(root);
    free(json_buffer);

    return 0;
}

mesh importMeshFromOBJFile (char * pathToFile) 
{
    FILE *obj = fopen(pathToFile, "r");

    mesh newMesh;
    newMesh.numberOfTriangles = 0;

    if (NULL == obj) 
    {
        printf("Error: .OBJ file not found\n");
        printf("Reminder: run the program from top level \"Cube/\" dir\n");
        return newMesh;
    }

    char line[MAX_LINE_LENGTH];
    int vertices = 0;
    int faces = 0;
    vector pointCoordinatess;
    initialiseVector(&pointCoordinatess);
    #ifdef AVERAGE_COORDS
    vector average;
    initialiseVector(&average);
    vector runningTotal;
    initialiseVector(&runningTotal);
    #endif
    int p0, p1, p2;
    
    //counts faces and verticies.
    while (fgets(line, sizeof(line), obj) != NULL) 
    {
        if (line[0] == 'v' && line[1] == ' ') 
        {
            vertices++;
        }
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            faces++;
        }
    }

    newMesh.numberOfTriangles = faces;
    newMesh.numberOfVertices = vertices;

    vector (*vectorArray) = malloc(newMesh.numberOfVertices * sizeof(vector)); //dynamically allocates an array of vectors, based off the number verticies.

    newMesh.trianglePointer = (triangle*) malloc(newMesh.numberOfTriangles * sizeof(triangle)); //dynamically allocates memory for the number of triangles

    rewind(obj);
    char a[20];
    char b[20];
    char c[20];
    char junk[MAX_LINE_LENGTH];
    int vertexCount = 0;
    int faceCount = 0;
    while (fgets(line, sizeof(line), obj) != NULL)
    {
        //reads all lines of obj that start with "v " into the dynamically assigned array of vectors.
        if (line[0] == 'v' && line[1] == ' ') 
        {
            sscanf(line,"v %lf %lf %lf", &pointCoordinatess.x, &pointCoordinatess.y, &pointCoordinatess.z);

            vectorArray[vertexCount] = pointCoordinatess;

            vertexCount++;
            #ifdef AVERAGE_COORDS
            runningTotal = addVector(runningTotal, pointCoordinatess);
            #endif
        }
        //reads all lines of obj that start with "f ".
        //each int after 'f ' represents an index (starting at 1), of the vector array of verticies.
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            sscanf(line,"f %s %s %s", a, b, c);

            //handle OBJs withe "f 00/00/00" format
            sscanf(a, "%d/%s", &p0, junk);
            sscanf(b, "%d/%s", &p1, junk);
            sscanf(c, "%d/%s", &p2, junk);

            newMesh.trianglePointer[faceCount].point[0] = vectorArray[(p0-1)];
            newMesh.trianglePointer[faceCount].point[1] = vectorArray[(p1-1)];
            newMesh.trianglePointer[faceCount].point[2] = vectorArray[(p2-1)];

            faceCount++;
        }
    }

    #ifdef DEBUG_POINTS_IMPORT
    //Handy debug for seeing if points were imported properly
    for (int i = 0; i < vertexCount; i++) 
    {
          printf("%d: %lf, %lf, %lf\n", i, vectorArray[i].x, vectorArray[i].y, vectorArray[i].z);
    }
    #endif

    #ifdef AVERAGE_COORDS
    //averages the coords so that the object appears in the center of the screen
    average = divideVectorByScalar(runningTotal, vertexCount);

    for (int j = 0; j < faceCount; j++) 
    {
        for (int i = 0; i < 3; i++) 
        {
            newMesh.trianglePointer[j].point[i] = subtractVector(newMesh.trianglePointer[j].point[i], average);
        }
    }
    #endif

    free(vectorArray);
    fclose(obj);

    return newMesh;
}
