#define MAX_LINE_LENGTH 256
#include "draw.h"

int importJSON(const char *file_path, renderConfig *importData_struct);

mesh importMeshFromOBJFile (char * pathToFile);