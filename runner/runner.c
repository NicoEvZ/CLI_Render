#include <math.h>
#include <time.h>
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
    screenStruct screen;
    mat4x4 projMat;
    vector camera = {0, 0, 0};
    vector lightDirection = {0, -1, 0};   

    const char jsonImportPath[] = "data/renderOptions.json";

    //failsafe that exits code if the importMeshFromOBJFile didn't succeed.
    if (importJSON(jsonImportPath, &importData))
    {
        printf("Import of JSON failed, exiting here");
        return 1;
    }

    screen.width = importData.screenWidthImprt;
    screen.height = importData.screenHeightImprt;

    initScreen(&screen);
    initProjectMat(importData, &projMat);

    double angle = 0;
    double lightAngle = 0;

    //Store OBJ data in mesh struct
    mesh baseMesh = importMeshFromOBJFile(importData.objPathBuffer); 
    if (baseMesh.numOfTris == 0)
    {
        printf("Error: Import failed, or OBJ file contains no vertex data\n");
        printf("Exiting...\n");
        return 0;
    }

    mat4x4 rotateX;
    mat4x4 rotateY;
    mat4x4 rotateZ;

    vector (*normalsVecArr) = malloc(baseMesh.numOfTris * sizeof(vector));
    triangle (*renderBufferArr) = malloc(baseMesh.numOfTris * sizeof(triangle));

    //display loop
    for (int i = 0; i < importData.i; i++)
    {
        int numOfTrisToRender = 0;
        clearScreen(&screen);
        initRotateXMat(&rotateX, angle);
        initRotateYMat(&rotateY, angle);
        initRotateZMat(&rotateZ, angle);
 
        for (int j = 0; j < baseMesh.numOfTris; j++)
        {
            triangle rotatedTri;
            triangle projectedTri;
            triangle translatedTri;

            copyTriangleData(baseMesh.tris[j], &rotatedTri);

            // rotate around axes
            if(importData.rotationX)
            {
                rotatedTri = matrixVectorMultiply(rotatedTri, rotateX);
            }

            if(importData.rotationY)
            {
                rotatedTri = matrixVectorMultiply(rotatedTri, rotateY);
            }

            if(importData.rotationZ)
            {
                rotatedTri = matrixVectorMultiply(rotatedTri, rotateZ);
            }

            copyTriangleData(rotatedTri, &translatedTri);

            //offest into screen
            translateTriangleZ(&translatedTri, importData.distance);

            //calculate face normals
            normalsVecArr[j] = calculateTriangleNormal(translatedTri);

            double dotProd = vecDotProduct(normalsVecArr[j], (subVec(translatedTri.p[0], camera)));
           
            if (dotProd < 0)
            {
                continue;    
            }

            //assign the "illumination" symbol based off normal
            illuminateTriangle(&translatedTri,normalsVecArr[j],lightDirection);

            for (int num = 0; num < 3; num++)
            {
                translatedTri.p[num].x *= CHARACHTER_RATIO; 
            }

            copyTriangleData(translatedTri, &projectedTri);

            //project 3D --> 2D
            projectedTri = matrixVectorMultiply(translatedTri, projMat);

            //scale points
            scaleTriangle(&projectedTri, screen);

            for (int triCopy = 0; triCopy < 3; triCopy++ )
            {
                projectedTri.p[triCopy].z = translatedTri.p[triCopy].z;
            }

            #ifdef DEBUG_POINTS_TRI_DATA
            printf("Triangle %d:\n",j+1);
            printf("\tp[0]: (%lf, %lf, %lf)\n", projectedTri.p[0].x, projectedTri.p[0].y, projectedTri.p[0].z);
            printf("\tp[1]: (%lf, %lf, %lf)\n", projectedTri.p[1].x, projectedTri.p[1].y, projectedTri.p[1].z);
            printf("\tp[2]: (%lf, %lf, %lf)\n", projectedTri.p[2].x, projectedTri.p[2].y, projectedTri.p[2].z);
            printf("\tnormal: (%lf, %lf, %lf)\n\n", normalsVecArr[j].x, normalsVecArr[j].y, normalsVecArr[j].z);
            #endif

            copyTriangleData(projectedTri,&renderBufferArr[numOfTrisToRender]);
            numOfTrisToRender++;
        }

        triangle (*trisToRender) = malloc(numOfTrisToRender*sizeof(triangle));
        
        for (int k = 0; k < numOfTrisToRender; k++)
        {
            copyTriangleData(renderBufferArr[k],&trisToRender[k]);
        }

        //select between vertex or rasterisation
        for (int tri = 0; tri < numOfTrisToRender; tri++)
        {   
            #ifdef DEBUG_POINTS_RENDER
            printf("Loading... %d%% complete:\n",(int)(((double)tri/(double)numOfTrisToRender)*100));
            printf("Drawing (%d/%d)\n",tri+1,numOfTrisToRender);
            #endif

            drawTriangleOnScreen(trisToRender[tri], screen, importData.rasteriseBool);

            #ifdef DEBUG_POINTS_RENDER_INDIVIDUAL
            displayScreen2(&screen);
            nanosleep((const struct timespec[]){{0, 125000000}}, NULL);
            #endif
        }

        #ifdef DEBUG_POINTS_RENDER
        printf("Angle: %lf\n",(angle*(180/PI)));
        printf("Final Output:\n");
        #endif

        angle = angle + 0.01745329;
        // lightAngle = lightAngle + 0.01745329;
        drawScreenBorder(&screen);
        
        displayScreen(&screen);
        #ifdef DEBUG_POINTS_ZBUFFER
        displayZBuffer(&screen);
        #endif
        nanosleep((const struct timespec[]){{0, 83333333L}}, NULL);
        free(trisToRender);
    }
    free(normalsVecArr);
    deleteScreen(&screen);
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

            //handle OBJs withe "f 00/00/00" format
            sscanf(a, "%d/%s", &p0, junk);
            sscanf(b, "%d/%s", &p1, junk);
            sscanf(c, "%d/%s", &p2, junk);

            newMesh.tris[fCount].p[0] = vectorArray[(p0-1)];
            newMesh.tris[fCount].p[1] = vectorArray[(p1-1)];
            newMesh.tris[fCount].p[2] = vectorArray[(p2-1)];

            fCount++;
        }
    }

    #ifdef DEBUG_POINTS_IMPORT
    //Handy debug for seeing if points were imported properly
    for (int i = 0; i < vCount; i++) 
    {
          printf("%d: %lf, %lf, %lf\n", i, vectorArray[i].x, vectorArray[i].y, vectorArray[i].z);
    }
    #endif


    //averages the coords so that the object appears in the center of the screen
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

