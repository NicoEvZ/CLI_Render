#include <math.h>
#include <time.h>
#include <stdio.h>
#include "draw.h"
#include "cJSON.h"

int importJSON(const char *file_path )
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
    cJSON *distance = cJSON_GetObjectItemCaseSensitive(root, "distance");
    cJSON *scale = cJSON_GetObjectItemCaseSensitive(root, "scale");
    cJSON *objFile = cJSON_GetObjectItemCaseSensitive(root, "objFile");

    printf("Distance: %lf\n", distance->valuedouble);
    printf("Scale: %lf\n", scale->valuedouble);
    printf("OBJ File Name: %s\n", objFile->valuestring);

    // Don't forget to free the cJSON object and the buffer when you're done with them
    cJSON_Delete(root);
    free(json_buffer);

    return 0;

}

int main(void){
    
    const double half_x = MAX_X/2;
    const double half_y = MAX_Y/2;
    double ratio = MAX_X/MAX_Y;
    double angle = 0;  
    double DISTANCE = 60;

    //screenspace center, not 3d space center
    double origin[]={half_x,half_y}; //origin is middle of screenspace

    int screen[MAX_X][MAX_Y];

    const char jsonImportPath[] = "data/inputData.json";
    char importPath[] = "data/videoShip.obj";

    importJSON(jsonImportPath);

    //Store OBJ data in mesh struct
    mesh baseMesh = importMeshFromOBJFile(importPath); 
    mesh rotatedMesh;
    mesh projectedMesh;
    
    //failsafe that exits code if the importMeshFromOBJFile didn't succeed.
    if (baseMesh.numOfTris == 0)
    {
        return 0;
    }
 
    //display 
    for (int i = 0; i < 1; i++)
    {
        //clear screen
        initScreen(screen);

        rotatedMesh = copyMeshData(baseMesh, rotatedMesh);

        // rotate around axes
        rotatedMesh = rotateMeshAroundX(rotatedMesh, (angle * (PI/180)));
        rotatedMesh = rotateMeshAroundY(rotatedMesh, (angle * (PI/180)));
        rotatedMesh = rotateMeshAroundZ(rotatedMesh, (angle * (PI/180)));

        projectedMesh = copyMeshData(rotatedMesh, projectedMesh);

        DISTANCE =  30 * (sin(0.1 * i)+ 1.8);

        //project 3D --> 2D
        projectMeshTo2D(projectedMesh, DISTANCE);
        
        //scale points
        scale2DPoints(projectedMesh);

        //draw lines
        drawMeshOnScreen(projectedMesh, origin, ratio, screen);

        displayScreen(screen);

        angle = angle + 1;
        nanosleep((const struct timespec[]){{0, 41600000L}}, NULL);
    }
    return 0;
}