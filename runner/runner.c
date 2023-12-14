#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "draw.h"
#include "cJSON.h"

typedef struct
{
    double distance;
    double scale;
    char objPathBuffer[64];
    int i;
}data;

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

    // printf("Distance: %lf\n", distance->valuedouble);
    // printf("Scale: %lf\n", scale->valuedouble);
    //printf("OBJ File Name: %s\n", objFile->valuestring);
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

    if(cJSON_IsString(objFileJOSN))
    {
        strncpy(importData_struct->objPathBuffer,objFileJOSN->valuestring,sizeof(importData_struct->objPathBuffer) -1);
        importData_struct->objPathBuffer[sizeof(importData_struct->objPathBuffer) -1] = '\0';
    }
    // Don't forget to free the cJSON object and the buffer when you're done with them
    cJSON_Delete(root);
    free(json_buffer);

    return 0;

}

int main(void){
    data importData;
    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    double angle = 0;

    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //origin is middle of screenspace

    int screen[MAX_X][MAX_Y];

    const char jsonImportPath[] = "data/inputData.json";
    // char importPath[64];

    if (importJSON(jsonImportPath, &importData))
    {
        printf("Import of JSON failed, exiting here");
        return 1;
    }
    int count = 0;

    // printf("Distance: %lf\n", distance);
    printf("importPath: %s\n", importData.objPathBuffer);


    //Store OBJ data in mesh struct
    mesh baseMesh = importMeshFromOBJFile(importData.objPathBuffer); 
    mesh rotatedMesh;
    mesh projectedMesh;
    
    //failsafe that exits code if the importMeshFromOBJFile didn't succeed.
    if (baseMesh.numOfTris == 0)
    {
        return 0;
    }
 
    //display 
    for (int i = 0; i < importData.i; i++)
    {
        //clear screen
        initScreen(screen);

        rotatedMesh = copyMeshData(baseMesh, rotatedMesh);

        // rotate around axes
        rotatedMesh = rotateMeshAroundX(rotatedMesh, (angle * (PI/180)));
        rotatedMesh = rotateMeshAroundY(rotatedMesh, (angle * (PI/180)));
        rotatedMesh = rotateMeshAroundZ(rotatedMesh, (angle * (PI/180)));

        projectedMesh = copyMeshData(rotatedMesh, projectedMesh);

        // distance =  30 * (sin(0.1 * i)+ 1.8);

        //project 3D --> 2D
        projectMeshTo2D(projectedMesh, importData.distance);
        
        //scale points
        scale2DPoints(projectedMesh,importData.scale);

        //draw lines
        drawMeshOnScreen(projectedMesh, origin, ratio, screen);

        displayScreen(screen);

        angle = angle + 1;
        nanosleep((const struct timespec[]){{0, 41600000L}}, NULL);
    }
    return 0;
}