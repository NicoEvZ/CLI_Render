#define MAX_LINE_LENGTH 256
#include "draw.h"

typedef struct
{
    double distance;
    double fov;
    char objPathBuffer[64];
    int i;
    int rotationX;
    int rotationY;
    int rotationZ;
    int screenWidthImprt;
    int screenHeightImprt;
    int rasteriseBool;
} renderConfig;

int importJSON(const char *file_path, renderConfig *importData_struct);

mesh importMeshFromOBJFile (char * pathToFile);