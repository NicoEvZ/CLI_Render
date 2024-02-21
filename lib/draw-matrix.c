#include <math.h>
#include "draw.h"

void initialiseVector(vector * input)
{
    input->x = 0;
    input->y = 0;
    input->z = 0;
    input->w = 1;
}

//vector1.elements plus vector2.elements
vector addVector(vector vector1, vector vector2) 
{
    vector returnVector;
    initialiseVector(&returnVector);

    returnVector.x = vector1.x + vector2.x;
    returnVector.y = vector1.y + vector2.y;
    returnVector.z = vector1.z + vector2.z;

    return returnVector;
}

//vector1.elements minus vector2.elements
vector subtractVector(vector vector1, vector vector2) 
{
    vector returnVector;
    initialiseVector(&returnVector);

    returnVector.x = vector1.x - vector2.x;
    returnVector.y = vector1.y - vector2.y;
    returnVector.z = vector1.z - vector2.z;

    return returnVector;
}

//divides each element of the vector by the value given
vector divideVectorByScalar(vector inputVector, double scalar) 
{
    vector returnVector;
    initialiseVector(&returnVector);

    returnVector.x = inputVector.x / scalar;
    returnVector.y = inputVector.y / scalar;
    returnVector.z = inputVector.z / scalar;

    return returnVector;
}

vector multiplyVectorByScalar(vector inputVector, double scalar)
{
    vector returnVector;
    initialiseVector(&returnVector);

    returnVector.x = inputVector.x * scalar;
    returnVector.y = inputVector.y * scalar;
    returnVector.z = inputVector.z * scalar;

    return returnVector;   
}

vector CrossProduct(vector vector1, vector vector2)
{
    vector output;
    initialiseVector(&output);

    output.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
    output.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
    output.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

    return output;
}

double vectorLength(vector inputVector)
{
    return (double)sqrtl(dotProduct(inputVector, inputVector));
}

vector normaliseVector(vector inputVector)
{
    if (inputVector.x == 0 && inputVector.y == 0 && inputVector.z == 0)
    {
        return inputVector;
    }

    vector returnVector;
    initialiseVector(&returnVector);
    returnVector = divideVectorByScalar(inputVector, vectorLength(inputVector));
    
    return returnVector;
}

double dotProduct(vector vector1, vector vector2)
{
    double output = (vector1.x * vector2.x) +
                    (vector1.y * vector2.y) +
                    (vector1.z * vector2.z);
    
    return output;
}

vector matrixVectorMultiply(vector input, matrix4x4 inputMatrix)
{
    vector output;
    initialiseVector(&output);
    
    output.x =  (input.x * inputMatrix.matrix[0][0]) + 
                (input.y * inputMatrix.matrix[1][0]) + 
                (input.z * inputMatrix.matrix[2][0]) + 
                (input.w * inputMatrix.matrix[3][0]);

    output.y =  (input.x * inputMatrix.matrix[0][1]) + 
                (input.y * inputMatrix.matrix[1][1]) + 
                (input.z * inputMatrix.matrix[2][1]) + 
                (input.w * inputMatrix.matrix[3][1]);

    output.z =  (input.x * inputMatrix.matrix[0][2]) + 
                (input.y * inputMatrix.matrix[1][2]) + 
                (input.z * inputMatrix.matrix[2][2]) + 
                (input.w * inputMatrix.matrix[3][2]);

    output.w =  (input.x * inputMatrix.matrix[0][3]) + 
                (input.y * inputMatrix.matrix[1][3]) + 
                (input.z * inputMatrix.matrix[2][3]) + 
                (input.w * inputMatrix.matrix[3][3]);

    #ifdef DEBUG_POINTS_TRI_DATA
    // printf("output.x (%lf) =\tinput.x (%lf) * inputMatrix.matrix[0][0](%lf) +\n", output.x, input.x, inputMatrix.matrix[0][0]);
    // printf("\t\t\tinput.y (%lf) * inputMatrix[1][0](%lf) +\n", input.y, inputMatrix.matrix[1][0]);
    // printf("\t\t\tinput.z (%lf) * inputMatrix[2][0](%lf) +\n", input.z, inputMatrix.matrix[2][0]);
    // printf("\t\t\tinput.w (%lf) * inputMatrix[3][0](%lf)\n\n", input.w, inputMatrix.matrix[3][0]);

    // printf("output.y (%lf) =\tinput.x (%lf) * inputMatrix.matrix[0][1](%lf) +\n", output.y, input.x, inputMatrix.matrix[0][1]);
    // printf("\t\t\tinput.y (%lf) * inputMatrix[1][1](%lf) +\n", input.y, inputMatrix.matrix[1][1]);
    // printf("\t\t\tinput.z (%lf) * inputMatrix[2][1](%lf) +\n", input.z, inputMatrix.matrix[2][1]);
    // printf("\t\t\tinput.w (%lf) * inputMatrix[3][1](%lf)\n\n", input.w, inputMatrix.matrix[3][1]);

    // printf("output.z (%lf) =\tinput.x (%lf) * inputMatrix.matrix[0][2](%lf) +\n", output.z, input.x, inputMatrix.matrix[0][2]);
    // printf("\t\t\tinput.y (%lf) * inputMatrix[1][2](%lf) +\n", input.y, inputMatrix.matrix[1][2]);
    // printf("\t\t\tinput.z (%lf) * inputMatrix[2][2](%lf) +\n", input.z, inputMatrix.matrix[2][2]);
    // printf("\t\t\tinput.w (%lf) * inputMatrix[3][2](%lf)\n\n", input.w, inputMatrix.matrix[3][2]);

    // printf("output.w (%lf) =\tinput.x (%lf) * inputMatrix.matrix[0][3](%lf) +\n", output.w, input.x, inputMatrix.matrix[0][3]);
    // printf("\t\t\tinput.y (%lf) * inputMatrix[1][3](%lf) +\n", input.y, inputMatrix.matrix[1][3]);
    // printf("\t\t\tinput.z (%lf) * inputMatrix[2][3](%lf) +\n", input.z, inputMatrix.matrix[2][3]);
    // printf("\t\t\tinput.w (%lf) * inputMatrix[3][3](%lf)\n\n", input.w, inputMatrix.matrix[3][3]);
    #endif

    return output;
}

triangle matrixTriangleMultiply(triangle input, matrix4x4 inputMatrix)
{
    triangle output;
    for (int i = 0; i < 3; i++)
    {
        #ifdef DEBUG_POINTS_TRI_DATA
        // printf("Point %d\n",i);
        #endif
        output.point[i] = matrixVectorMultiply(input.point[i], inputMatrix);
    }
    output.symbol = input.symbol;
    return output;
}

matrix4x4 matrixMatrixMultiply(matrix4x4 matrix1, matrix4x4 matrix2)
{
    matrix4x4 output;
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            output.matrix[row][column] = matrix1.matrix[row][0] * matrix2.matrix[0][column] +
                                         matrix1.matrix[row][1] * matrix2.matrix[1][column] +
                                         matrix1.matrix[row][2] * matrix2.matrix[2][column] +
                                         matrix1.matrix[row][3] * matrix2.matrix[3][column];                     
        }
    }
    return output;
}

void copyMatrix(matrix4x4 from, matrix4x4 *to)
{
    to->matrix[0][0] = from.matrix[0][0];   to->matrix[0][1] = from.matrix[0][1];   to->matrix[0][2] = from.matrix[0][2];   to->matrix[0][3] = from.matrix[0][3];
    to->matrix[1][0] = from.matrix[1][0];   to->matrix[1][1] = from.matrix[1][1];   to->matrix[1][2] = from.matrix[1][2];   to->matrix[1][3] = from.matrix[1][3];
    to->matrix[2][0] = from.matrix[2][0];   to->matrix[2][1] = from.matrix[2][1];   to->matrix[2][2] = from.matrix[2][2];   to->matrix[2][3] = from.matrix[2][3];
    to->matrix[3][0] = from.matrix[3][0];   to->matrix[3][1] = from.matrix[3][1];   to->matrix[3][2] = from.matrix[3][2];   to->matrix[3][3] = from.matrix[3][3];
}

void initialiseProjectionMatrix(int screenHeight, int screenWidth, double fov, matrix4x4 *ProjectionMatrix)
{
    double near = 0.1;
    double far = 1000;
    double aspectRatio = ((double)screenHeight / (double)screenWidth);
    double fovRadians = 1 / tanf(fov * 0.5 / 180 * PI);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ProjectionMatrix->matrix[i][j] = 0;
        }
    }

    ProjectionMatrix->matrix[0][0] = aspectRatio * fovRadians;
    ProjectionMatrix->matrix[1][1] = fovRadians;
    ProjectionMatrix->matrix[2][2] = far / (far - near);
    ProjectionMatrix->matrix[3][2] = (-far * near) / (far - near);
    ProjectionMatrix->matrix[2][3] = 1;
    ProjectionMatrix->matrix[3][3] = 0;
}

void initialiseRotateXMatrix(matrix4x4 * rotateX, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotateX->matrix[i][j] = 0;
        }
    }

    rotateX->matrix[0][0] = 1;
    rotateX->matrix[1][1] = cosf(angle);
    rotateX->matrix[1][2] = sinf(angle);
    rotateX->matrix[2][1] = -sinf(angle);
    rotateX->matrix[2][2] = cosf(angle);
    rotateX->matrix[3][3] = 1;
}

void initialiseRotateYMatrix(matrix4x4 * rotateY, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotateY->matrix[i][j] = 0;
        }
    }

    rotateY->matrix[0][0] = cosf(angle);;
    rotateY->matrix[0][2] = -sinf(angle);
    rotateY->matrix[1][1] = 1;
    rotateY->matrix[2][0] = sinf(angle);
    rotateY->matrix[2][2] = cosf(angle);
    rotateY->matrix[3][3] = 1;
}

void initialiseRotateZMatrix(matrix4x4 * rotateZ, double angle)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rotateZ->matrix[i][j] = 0;
        }
    }

    rotateZ->matrix[0][0] = cosf(angle);
    rotateZ->matrix[0][1] = sinf(angle);
    rotateZ->matrix[1][0] = -sinf(angle);
    rotateZ->matrix[1][1] = cosf(angle);
    rotateZ->matrix[2][2] = 1;
    rotateZ->matrix[3][3] = 1;
}

void initialiseTranslationMatrix(matrix4x4 * translate, double x, double y, double z)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            translate->matrix[i][j] = 0;
        }
    }

    translate->matrix[0][0] = 1;
    translate->matrix[1][1] = 1;
    translate->matrix[2][2] = 1;
    translate->matrix[3][3] = 1;
    translate->matrix[3][0] = x;
    translate->matrix[3][1] = y;
    translate->matrix[3][2] = z;
}

void intialiseIdentityMatrix(matrix4x4 * input)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            input->matrix[i][j] = 0;
        }
    }

    input->matrix[0][0] = 1;
    input->matrix[1][1] = 1;
    input->matrix[2][2] = 1;
    input->matrix[3][3] = 1;
}

void InitialisePointAtMatrix(matrix4x4 *input, vector position, vector target, vector up)
{
    // Calculate new forward directon
    vector newForward = subtractVector(target, position);
    newForward = normaliseVector(newForward);

    // Calculate new up direction
    vector a = multiplyVectorByScalar(newForward, dotProduct(up, newForward));
    vector newUp = subtractVector(up, a);
    newUp = normaliseVector(newUp);

    // New right direction is cross product
    vector newRight = CrossProduct(newUp, newForward);

    input->matrix[0][0] = newRight.x;   input->matrix[0][1] = newRight.y;   input->matrix[0][2] = newRight.z;   input->matrix[0][3] = 0;
    input->matrix[1][0] = newUp.x;      input->matrix[1][1] = newUp.y;      input->matrix[1][2] = newUp.z;      input->matrix[1][3] = 0;
    input->matrix[2][0] = newForward.x; input->matrix[2][1] = newForward.y; input->matrix[2][2] = newForward.z; input->matrix[2][3] = 0;
    input->matrix[3][0] = position.x;   input->matrix[3][1] = position.y;   input->matrix[3][2] = position.z;   input->matrix[3][3] = 1;

}

matrix4x4 quickMatrixInverse(matrix4x4 input)
{
    matrix4x4 output;
    output.matrix[0][0] = input.matrix[0][0];  output.matrix[0][1] = input.matrix[1][0];  output.matrix[0][2] = input.matrix[2][0];  output.matrix[0][3] = 0;
    output.matrix[1][0] = input.matrix[0][1];  output.matrix[1][1] = input.matrix[1][1];  output.matrix[1][2] = input.matrix[2][1];  output.matrix[1][3] = 0;
    output.matrix[2][0] = input.matrix[0][2];  output.matrix[2][1] = input.matrix[1][2];  output.matrix[2][2] = input.matrix[2][2];  output.matrix[2][3] = 0;

    output.matrix[3][0] = -(input.matrix[3][0] * output.matrix[0][0] + input.matrix[3][1] * output.matrix[1][0] + input.matrix[3][2] * output.matrix[2][0]);
    output.matrix[3][1] = -(input.matrix[3][0] * output.matrix[0][1] + input.matrix[3][1] * output.matrix[1][1] + input.matrix[3][2] * output.matrix[2][1]);
    output.matrix[3][2] = -(input.matrix[3][0] * output.matrix[0][2] + input.matrix[3][1] * output.matrix[1][2] + input.matrix[3][2] * output.matrix[2][2]);
    output.matrix[3][3] = 1;

    return output;
}