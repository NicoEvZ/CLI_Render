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

double epsilon = 0.0001;

static void test_addVec(void **state)
{   
    (void) state;
    vector vector1 = {1,2,3};
    vector vector2 = {4,5,6};
    vector expected = {5,7,9};
    
    assert_float_equal(expected.x, addVector(vector1,vector2).x, epsilon);
    assert_float_equal(expected.y, addVector(vector1,vector2).y, epsilon);
    assert_float_equal(expected.z, addVector(vector1,vector2).z, epsilon);
}

static void test_subVec(void **state)
{   
    (void) state;
    vector vector1 = {5,7,9};
    vector vector2 = {4,5,6};
    vector expected = {1,2,3};
    
    assert_float_equal(expected.x, subtractVector(vector1,vector2).x, epsilon);
    assert_float_equal(expected.y, subtractVector(vector1,vector2).y, epsilon);
    assert_float_equal(expected.z, subtractVector(vector1,vector2).z, epsilon);
}

static void test_vecCrossProduct(void **state)
{
    (void) state;
    vector vector1 = {1,2,3};
    vector vector2 = {4,5,6};
    vector expected = {-3,6,-3};

    assert_float_equal(expected.x, CrossProduct(vector1,vector2).x, epsilon);
    assert_float_equal(expected.y, CrossProduct(vector1,vector2).y, epsilon);
    assert_float_equal(expected.z, CrossProduct(vector1,vector2).z, epsilon);
}

static void test_divVecByScalar(void **state)
{
    (void) state;
    vector vector1 = {4,8,16};
    double scalar = 2;
    vector expected = {2,4,8};

    assert_float_equal(expected.x, divideVectorByScalar(vector1, scalar).x, epsilon);
    assert_float_equal(expected.y, divideVectorByScalar(vector1, scalar).y, epsilon);
    assert_float_equal(expected.z, divideVectorByScalar(vector1, scalar).z, epsilon);
}

static void test_copyTriangleData(void **state)
{
    (void) state;
    triangle testTriangle = { {{1,2,3}, {4,5,6}, {7,8,9}}, 65 };
    triangle output = { {{0,0,0}, {0,0,0}, {0,0,0}}, 0 };

    copyTriangleData(testTriangle, &output);

    for (int i = 0; i < 3; i++)
    {
        assert_float_equal(output.point[i].x, testTriangle.point[i].x, epsilon);
        assert_float_equal(output.point[i].y, testTriangle.point[i].y, epsilon);
        assert_float_equal(output.point[i].z, testTriangle.point[i].z, epsilon);
    }
}

int main(void)
{
    mesh test_mesh;
    test_mesh.numberOfTriangles = 2;
    test_mesh.numberOfVertices = 4;
    test_mesh.trianglePointer = (triangle*) malloc(test_mesh.numberOfTriangles * sizeof(triangle));
    triangle test_tri_A = {{{0, 1, 1}, {0, 0, 1}, {1, 0, 1}}, 65};
    triangle test_tri_B = {{{1, 0, 1}, {1, 1, 1}, {0, 1, 1}}, 66};
    test_mesh.trianglePointer[0] = test_tri_A;
    test_mesh.trianglePointer[1] = test_tri_B;

    printf("Test_Mesh:\n");
    for (int i = 0; i < test_mesh.numberOfTriangles; i++)
    {
        printf("Triangle[%d of %d]:\n",i+1,test_mesh.numberOfTriangles);
        for (int j = 0; j < 3; j++)
        {
            printf("\tp[%d]: (%lf, %lf, %lf)\n",j,test_mesh.trianglePointer[i].point[j].x, test_mesh.trianglePointer[i].point[j].y, test_mesh.trianglePointer[i].point[j].z);
        }
        printf("\tchar: %c\n\n",test_mesh.trianglePointer[i].symbol);
    }
    const struct CMUnitTest tests[] = 
    {
        cmocka_unit_test(test_addVec),
        cmocka_unit_test(test_subVec),
        cmocka_unit_test(test_vecCrossProduct),
        cmocka_unit_test(test_copyTriangleData),
        cmocka_unit_test(test_divVecByScalar)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}