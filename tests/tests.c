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
    vector vec1 = {1,2,3};
    vector vec2 = {4,5,6};
    vector expected = {5,7,9};
    
    assert_float_equal(expected.x, addVec(vec1,vec2).x, epsilon);
    assert_float_equal(expected.y, addVec(vec1,vec2).y, epsilon);
    assert_float_equal(expected.z, addVec(vec1,vec2).z, epsilon);
}

static void test_subVec(void **state)
{   
    (void) state;
    vector vec1 = {5,7,9};
    vector vec2 = {4,5,6};
    vector expected = {1,2,3};
    
    assert_float_equal(expected.x, subVec(vec1,vec2).x, epsilon);
    assert_float_equal(expected.y, subVec(vec1,vec2).y, epsilon);
    assert_float_equal(expected.z, subVec(vec1,vec2).z, epsilon);
}

static void test_vecCrossProduct(void **state)
{
    (void) state;
    vector vec1 = {1,2,3};
    vector vec2 = {4,5,6};
    vector expected = {-3,6,-3};

    assert_float_equal(expected.x, vecCrossProduct(vec1,vec2).x, epsilon);
    assert_float_equal(expected.y, vecCrossProduct(vec1,vec2).y, epsilon);
    assert_float_equal(expected.z, vecCrossProduct(vec1,vec2).z, epsilon);
}

static void test_divVecByScalar(void **state)
{
    (void) state;
    vector vec1 = {4,8,16};
    double scalar = 2;
    vector expected = {2,4,8};

    assert_float_equal(expected.x, divVecByScalar(vec1, scalar).x, epsilon);
    assert_float_equal(expected.y, divVecByScalar(vec1, scalar).y, epsilon);
    assert_float_equal(expected.z, divVecByScalar(vec1, scalar).z, epsilon);
}

static void test_copyTriangleData(void **state)
{
    (void) state;
    triangle testTri = { {{1,2,3}, {4,5,6}, {7,8,9}}, 65 };
    triangle output = { {{0,0,0}, {0,0,0}, {0,0,0}}, 0 };

    copyTriangleData(testTri, &output);

    for (int i = 0; i < 3; i++)
    {
        assert_float_equal(output.p[i].x, testTri.p[i].x, epsilon);
        assert_float_equal(output.p[i].y, testTri.p[i].y, epsilon);
        assert_float_equal(output.p[i].z, testTri.p[i].z, epsilon);
    }
}

int main(void)
{
    mesh test_mesh;
    test_mesh.numOfTris = 2;
    test_mesh.numOfVerts = 4;
    test_mesh.tris = (triangle*) malloc(test_mesh.numOfTris * sizeof(triangle));
    triangle test_tri_A = {{{0, 1, 1}, {0, 0, 1}, {1, 0, 1}}, 65};
    triangle test_tri_B = {{{1, 0, 1}, {1, 1, 1}, {0, 1, 1}}, 66};
    test_mesh.tris[0] = test_tri_A;
    test_mesh.tris[1] = test_tri_B;

    printf("Test_Mesh:\n");
    for (int i = 0; i < test_mesh.numOfTris; i++)
    {
        printf("Triangle[%d of %d]:\n",i+1,test_mesh.numOfTris);
        for (int j = 0; j < 3; j++)
        {
            printf("\tp[%d]: (%lf, %lf, %lf)\n",j,test_mesh.tris[i].p[j].x, test_mesh.tris[i].p[j].y, test_mesh.tris[i].p[j].z);
        }
        printf("\tchar: %c\n\n",test_mesh.tris[i].symbol);
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