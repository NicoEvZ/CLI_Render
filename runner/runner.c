#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cJson-w.h"
#include "draw.h"
#include "runner.h"

// #define DEBUG_POINTS_IMPORT
// #define DEBUG_POINTS_RENDER
// #define DEBUG_POINTS_RENDER_INDIVIDUAL
// #define DEBUG_POINTS_TRI_DATA

int main(void){
    renderConfig importData;
    frameBuffer screen;
    frameBuffer oldScreen;
    matrix4x4 projectionMatrix;
    vector camera = {0, 0, 0};
    vector lightDirection = {0, -0.60, 0.40 };   

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
    initialiseProjectionMatrix(importData, &projectionMatrix);

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

    int meshColour[3] = {0,255,0};

    for (int i = 0; i < 3; i++)
    {
        baseMesh.colour[i] = meshColour[i];
    }
    
    matrix4x4 rotateX;
    matrix4x4 rotateY;
    matrix4x4 rotateZ;

    vector (*normalsVectorArray) = malloc(baseMesh.numberOfTriangles * sizeof(vector));
    triangle (*renderBufferArray) = malloc(baseMesh.numberOfTriangles * sizeof(triangle));

    size_t sizeOfScreen = (sizeof(char)*((screen.width) * (screen.height) * 30));
    char *a = malloc(sizeOfScreen);

    if (setvbuf(stdout, a, _IOFBF, sizeOfScreen))
    {
        printf("Error setting buffer\n");
        fflush(stdout);
    }

    printf("\e[H\e[J");


    for (int i = 0; i < importData.iterations; i++)
    {
        int numberOfTrianglessToRender = 0;
        // clearFrameBuffer(&screen);
        initialiseRotateXMatrix(&rotateX, angle);
        initialiseRotateYMatrix(&rotateY, angle);
        initialiseRotateZMatrix(&rotateZ, angle);

        //3-phase thransform of r,g and b. Cycle through all colours once, regardless of max number of rotations
        baseMesh.colour[0] = 125.5*sin((i/((double)importData.iterations))*2*PI)+125.5;
        baseMesh.colour[1] = 125.5*sin(((i/((double)importData.iterations))*2*PI)+(1.33*PI))+125.5;
        baseMesh.colour[2] = 125.5*sin(((i/((double)importData.iterations))*2*PI)+(0.66*PI))+125.5;
 
        for (int j = 0; j < baseMesh.numberOfTriangles; j++)
        {
            triangle rotatedTriangle;
            triangle projectedTriangle;
            triangle translatedTriangle;

            copyTriangleData(baseMesh.trianglePointer[j], &rotatedTriangle);

            inheritColourFromMesh(baseMesh.colour, &rotatedTriangle);

            // rotate around axes
            if(importData.rotationX)
            {
                rotatedTriangle = matrixVectorMultiply(rotatedTriangle, rotateX);
            }

            if(importData.rotationY)
            {
                rotatedTriangle = matrixVectorMultiply(rotatedTriangle, rotateY);
            }

            if(importData.rotationZ)
            {
                rotatedTriangle = matrixVectorMultiply(rotatedTriangle, rotateZ);
            }

            copyTriangleData(rotatedTriangle, &translatedTriangle);

            //offest into screen
            translateTriangleZ(&translatedTriangle, importData.distance);

            // translateTriangleY(&translatedTriangle, -22);

            //calculate face normals
            normalsVectorArray[j] = calculateTriangleNormal(translatedTriangle);

            double dotProductResult = dotProduct(normalsVectorArray[j], (subtractVector(translatedTriangle.point[0], camera)));
           
            if (dotProductResult < 0)
            {
                continue;    
            }

            //assign the "illumination" symbol based off normal
            illuminateTriangle(&translatedTriangle,normalsVectorArray[j],lightDirection);

            for (int point = 0; point < 3; point++)
            {
                translatedTriangle.point[point].x *= importData.characterRatio; 
            }

            copyTriangleData(translatedTriangle, &projectedTriangle);

            //project 3D --> 2D
            projectedTriangle = matrixVectorMultiply(translatedTriangle, projectionMatrix);

            //scale points
            scaleTriangle(&projectedTriangle, screen);

            for (int triCopy = 0; triCopy < 3; triCopy++ )
            {
                projectedTriangle.point[triCopy].z = translatedTriangle.point[triCopy].z;
            }

            #ifdef DEBUG_POINTS_TRI_DATA
            printf("Triangle %d:\n",j+1);
            printf("\tp[0]: (%lf, %lf, %lf)\n", projectedTriangle.point[0].x, projectedTriangle.point[0].y, projectedTriangle.point[0].z);
            printf("\tp[1]: (%lf, %lf, %lf)\n", projectedTriangle.point[1].x, projectedTriangle.point[1].y, projectedTriangle.point[1].z);
            printf("\tp[2]: (%lf, %lf, %lf)\n", projectedTriangle.point[2].x, projectedTriangle.point[2].y, projectedTriangle.point[2].z);
            printf("\tnormal: (%lf, %lf, %lf)\n\n", normalsVectorArray[j].x, normalsVectorArray[j].y, normalsVectorArray[j].z);
            #endif

            copyTriangleData(projectedTriangle,&renderBufferArray[numberOfTrianglessToRender]);
            numberOfTrianglessToRender++;
        }

        triangle (*trisToRender) = malloc(numberOfTrianglessToRender*sizeof(triangle));
        
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

            #ifdef DEBUG_POINTS_RENDER_INDIVIDUAL
            displayFrameBuffer2(&screen, &oldScreen);
            frameDelay(60);
            #endif
        }

        #ifdef DEBUG_POINTS_RENDER
        printf("Angle: %lf\n",(angle*(180/PI)));
        printf("Final Output:\n");
        #endif

        angle += RAD;
        // lightAngle = lightAngle + 0.01745329;
        drawScreenBorder(&screen);
        
        // displayFrameBuffer(&screen);
        displayFrameBuffer2(screen, oldScreen);
        // displayDepthBuffer(screen, oldScreen);

        //screen becomes oldScreen
        copyFrameBufferData(screen, &oldScreen);
        clearFrameBuffer(&screen);

        #ifdef DEBUG_POINTS_ZBUFFER
        displayDepthBuffer(&screen);
        #endif
        frameDelay(importData.framesPerSecond);
        free(trisToRender);
    }
    printf("\n");
    free(a);
    free(normalsVectorArray);
    deleteFrameBuffer(&screen);
    deleteFrameBuffer(&oldScreen);
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

    if(cJSON_IsNumber(iterationsJSON))
    {
        importData_struct->iterations = iterationsJSON->valueint;
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
    vector average;
    vector runningTotal;
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

            runningTotal = addVector(runningTotal, pointCoordinatess);
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


    //averages the coords so that the object appears in the center of the screen
    average = divideVectorByScalar(runningTotal, vertexCount);

    for (int j = 0; j < faceCount; j++) 
    {
        for (int i = 0; i < 3; i++) 
        {
            newMesh.trianglePointer[j].point[i] = subtractVector(newMesh.trianglePointer[j].point[i], average);
        }
    }

    free(vectorArray);
    fclose(obj);

    return newMesh;
}
