//
// Created by Liam Murphy on 2/09/23.
//
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>

#include <draw.h>
#include <math.h>

double epsilon = 0.0001;

// static void test_example(void **state)
// {
//     (void) state;

// }

static void test_initialiseVector(void **state)
{
    (void) state;
    vector vector1;

    initialiseVector(&vector1);

    assert_float_equal(vector1.x, 0, epsilon);
    assert_float_equal(vector1.y, 0, epsilon);
    assert_float_equal(vector1.z, 0, epsilon);
    assert_float_equal(vector1.w, 1, epsilon);
}

static void test_addVector(void **state)
{   
    (void) state;
    vector vector1 = (vector){1, 2, 3, 1};
    vector vector2 = (vector){4, 5, 6, 1};
    vector expected = (vector){5, 7, 9, 1};
    
    assert_float_equal(expected.x, addVector(vector1, vector2).x, epsilon);
    assert_float_equal(expected.y, addVector(vector1, vector2).y, epsilon);
    assert_float_equal(expected.z, addVector(vector1, vector2).z, epsilon);
    assert_float_equal(expected.w, 1, epsilon);
}

static void test_subtractVector(void **state)
{   
    (void) state;
    vector vector1 = (vector){5, 7, 9, 1};
    vector vector2 = (vector){4, 5, 6, 1};
    vector expected = (vector){1, 2, 3, 1};
    
    assert_float_equal(expected.x, subtractVector(vector1, vector2).x, epsilon);
    assert_float_equal(expected.y, subtractVector(vector1, vector2).y, epsilon);
    assert_float_equal(expected.z, subtractVector(vector1, vector2).z, epsilon);
    assert_float_equal(expected.w, 1, epsilon);
}

static void test_divideVectorByScalar(void **state)
{
    (void) state;
    vector vector1 = (vector){4, 8, 16, 1};
    double scalar = 2;
    vector expected = (vector){2, 4, 8, 1};

    assert_float_equal(expected.x, divideVectorByScalar(vector1, scalar).x, epsilon);
    assert_float_equal(expected.y, divideVectorByScalar(vector1, scalar).y, epsilon);
    assert_float_equal(expected.z, divideVectorByScalar(vector1, scalar).z, epsilon);
    assert_float_equal(expected.w, 1, epsilon);

}

static void test_multiplyVectorByScalar(void **state)
{
    (void) state;
    vector vector1 = (vector){2, 4, 8, 1};
    double scalar = 2;
    vector expected = (vector){4, 8, 16, 1};

    assert_float_equal(expected.x, multiplyVectorByScalar(vector1, scalar).x, epsilon);
    assert_float_equal(expected.y, multiplyVectorByScalar(vector1, scalar).y, epsilon);
    assert_float_equal(expected.z, multiplyVectorByScalar(vector1, scalar).z, epsilon);
    assert_float_equal(expected.w, 1, epsilon);

}

static void test_CrossProduct(void **state)
{
    (void) state;
    vector vector1 = (vector){1, 2, 3, 1};
    vector vector2 = (vector){4, 5, 6, 1};
    vector expected = (vector){-3, 6, -3, 1};

    assert_float_equal(expected.x, CrossProduct(vector1,vector2).x, epsilon);
    assert_float_equal(expected.y, CrossProduct(vector1,vector2).y, epsilon);
    assert_float_equal(expected.z, CrossProduct(vector1,vector2).z, epsilon);
    assert_float_equal(expected.w, 1, epsilon);

}

static void test_vectorLength(void **state)
{
    (void) state;
    vector vector1 = (vector){2, 2, 2, 1};
    double expected = sqrtl(12);

    assert_float_equal(expected, vectorLength(vector1), epsilon);
}

static void test_normaliseVector(void **state)
{
    (void) state;
    vector vector1 = (vector){50, 100, 10, 1};
    double manual_dp = ((vector1.x * vector1.x) + (vector1.y * vector1.y) + (vector1.z*vector1.z));
    double length = sqrtl(manual_dp);
    vector expected = (vector){(vector1.x/length), (vector1.y/length), (vector1.z/length), 1};

    assert_float_equal(expected.x, normaliseVector(vector1).x, epsilon);
    assert_float_equal(expected.y, normaliseVector(vector1).y, epsilon);
    assert_float_equal(expected.z, normaliseVector(vector1).z, epsilon);
}

static void test_dotProduct(void **state)
{
    (void) state;
    vector vector1 = (vector){1, 2, 3, 1};
    vector vector2 = (vector){4, 5, 6, 1};
    double expected = 32;

    assert_float_equal(expected, dotProduct(vector1, vector2), epsilon);
}

static void test_copyTriangleData(void **state)
{
    (void) state;
    triangle testTriangle;
    testTriangle = (triangle){  .point[0] = (vector){1, 2, 3, 1},
                                .point[1] = (vector){4, 5, 6, 1},
                                .point[2] = (vector){7, 8, 9, 1},
                                .symbol.character = ' ',
                                .symbol.colour[0] = 255,
                                .symbol.colour[1] = 255,
                                .symbol.colour[2] = 255,
                                .symbol.brightness = 1
                            };

    triangle output;
    output = (triangle){    .point[0] = (vector){0, 0, 0, 0},
                            .point[1] = (vector){0, 0, 0, 0},
                            .point[2] = (vector){0, 0, 0, 0},
                            .symbol.character = 'A',
                            .symbol.colour[0] = 0,
                            .symbol.colour[1] = 0,
                            .symbol.colour[2] = 0,
                            .symbol.brightness = 0
                        };

    copyTriangleData(testTriangle, &output);

    for (int i = 0; i < 3; i++)
    {
        assert_float_equal(output.point[i].x, testTriangle.point[i].x, epsilon);
        assert_float_equal(output.point[i].y, testTriangle.point[i].y, epsilon);
        assert_float_equal(output.point[i].z, testTriangle.point[i].z, epsilon);
        assert_float_equal(output.point[i].w, testTriangle.point[i].w, epsilon);

        assert_string_equal(&output.symbol.character, &testTriangle.symbol.character);

        assert_float_equal(output.symbol.colour[i], testTriangle.symbol.colour[i], epsilon);

    }
}

static void test_intialiseIdentityMatrix(void **state)
{
    (void) state;
    matrix4x4 matrix1;
    intialiseIdentityMatrix(&matrix1);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(j == i)
            {
                assert_float_equal(matrix1.matrix[i][j], 1, epsilon);
            }
            else
            {
                assert_float_equal(matrix1.matrix[i][j], 0, epsilon);
            }
        }
    }
}

static void test_matrixVectorMultiply(void **state)
{
    (void) state;
    matrix4x4 matrix1;
    matrix1 = (matrix4x4){
                            .matrix[0][0] = 1, .matrix[0][1] = 2, .matrix[0][2] = 3, .matrix[0][3] = 4,
                            .matrix[1][0] = 2, .matrix[1][1] = 2, .matrix[1][2] = 3, .matrix[1][3] = 4, 
                            .matrix[2][0] = 3, .matrix[2][1] = 3, .matrix[2][2] = 3, .matrix[2][3] = 4, 
                            .matrix[3][0] = 4, .matrix[3][1] = 4, .matrix[3][2] = 4, .matrix[3][3] = 4, 
                        };
    vector vector1;
    vector1 = (vector){1, 2, 3, 4};

    assert_float_equal(matrixVectorMultiply(vector1, matrix1).x, 30, epsilon);
    assert_float_equal(matrixVectorMultiply(vector1, matrix1).y, 31, epsilon);
    assert_float_equal(matrixVectorMultiply(vector1, matrix1).z, 34, epsilon);
    assert_float_equal(matrixVectorMultiply(vector1, matrix1).w, 40, epsilon);
    
}

static void test_copyMatrix(void **state)
{
    (void) state;
    matrix4x4 matrix1;
    matrix4x4 matrix2;
    matrix1 = (matrix4x4){
                            .matrix[0][0] = 1, .matrix[0][1] = 2, .matrix[0][2] = 3, .matrix[0][3] = 4,
                            .matrix[1][0] = 2, .matrix[1][1] = 2, .matrix[1][2] = 3, .matrix[1][3] = 4, 
                            .matrix[2][0] = 3, .matrix[2][1] = 3, .matrix[2][2] = 3, .matrix[2][3] = 4, 
                            .matrix[3][0] = 4, .matrix[3][1] = 4, .matrix[3][2] = 4, .matrix[3][3] = 4, 
                        };

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matrix2.matrix[i][j] = 0;
        }
    }
    
    copyMatrix(matrix1, &matrix2);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            assert_float_equal(matrix1.matrix[i][j], matrix2.matrix[i][j], epsilon);
        }
    }
}

int main(void)
{
    mesh test_mesh;
    test_mesh = (mesh){ .colour[0] = 255,
                        .colour[1] = 255,
                        .colour[2] = 255,
                        .numberOfTriangles = 2,
                        .numberOfVertices = 4
    };
    test_mesh.trianglePointer = (triangle*) malloc(test_mesh.numberOfTriangles * sizeof(triangle));
    triangle test_triangle_A = (triangle){  .point[0] = (vector){0, 1, 1, 1},
                                            .point[1] = (vector){0, 0, 1, 1},
                                            .point[2] = (vector){1, 0, 1, 1},
                                            .symbol.character = 'A',
                                            .symbol.colour[0] = 255,
                                            .symbol.colour[1] = 255,
                                            .symbol.colour[2] = 255,
                                            .symbol.brightness = 1.0
                                        };
    triangle test_triangle_B = (triangle){  .point[0] = (vector){1, 0, 1, 1},
                                            .point[1] = (vector){1, 1, 1, 1},
                                            .point[2] = (vector){0, 1, 1, 1},
                                            .symbol.character = 'B',
                                            .symbol.colour[0] = 255,
                                            .symbol.colour[1] = 255,
                                            .symbol.colour[2] = 255,
                                            .symbol.brightness = 1.0
                                        };
    test_mesh.trianglePointer[0] = test_triangle_A;
    test_mesh.trianglePointer[1] = test_triangle_B;

    printf("Test_Mesh:\n");
    for (int i = 0; i < test_mesh.numberOfTriangles; i++)
    {
        printf("Triangle[%d of %d]:\n",i+1,test_mesh.numberOfTriangles);
        for (int j = 0; j < 3; j++)
        {
            printf("\tpoint[%d]: (%lf, %lf, %lf, %lf)\n", j, 
                                                    test_mesh.trianglePointer[i].point[j].x, 
                                                    test_mesh.trianglePointer[i].point[j].y, 
                                                    test_mesh.trianglePointer[i].point[j].z,
                                                    test_mesh.trianglePointer[i].point[j].w);
        }
        char c;
        for (int j = 0; j < 3; j++)
        {
            printf("\tcolour[%c]: %d\n", c = (j == 0) ? 'r' : (j == 1) ? 'g' : 'b', test_mesh.trianglePointer[i].symbol.colour[j]);
        }
        printf("\tbrightnes: %lf\n", test_mesh.trianglePointer[i].symbol.brightness);
        printf("\tchar: %c\n\n", test_mesh.trianglePointer[i].symbol.character);
    }
    const struct CMUnitTest tests[] = 
    {
        cmocka_unit_test(test_initialiseVector),
        cmocka_unit_test(test_addVector),
        cmocka_unit_test(test_subtractVector),
        cmocka_unit_test(test_divideVectorByScalar),
        cmocka_unit_test(test_multiplyVectorByScalar),
        cmocka_unit_test(test_CrossProduct),
        cmocka_unit_test(test_vectorLength),
        cmocka_unit_test(test_normaliseVector),
        cmocka_unit_test(test_dotProduct),
        cmocka_unit_test(test_copyTriangleData),
        cmocka_unit_test(test_intialiseIdentityMatrix),
        cmocka_unit_test(test_matrixVectorMultiply),
        cmocka_unit_test(test_copyMatrix)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}