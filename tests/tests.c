//
// Created by Liam Murphy on 2/09/23.
//
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <draw.h>

#include <quick_sort.h>

double epsilon = 0.0001;

static void test_addVec(void **state)
{   
    (void) state;
    vector vec1 = {1,2,3};
    vector vec2 = {4,5,6};
    vector output = {5,7,9};
    
    assert_float_equal(output.x, addVec(vec1,vec2).x, epsilon);
    assert_float_equal(output.y, addVec(vec1,vec2).y, epsilon);
    assert_float_equal(output.z, addVec(vec1,vec2).z, epsilon);
}

// static void test_quicksort(void **state)
// {
//     (void) state;
//     triangle triangleArry[] = {
//                                 {{25, -21, 0}, {-25, -21, 0}, {0, 22, 0}},
//                                 {{25, -21, 0}, {0, 22, 0}, {0, -7, 40}},
//                                 {{-25, -21, 0}, {0, -7, 40}, {0, 22, 0}},
//                                 {{25, -21, 0}, {0, -7, 40}, {-25, -21, 0}}
//                             };

//     double arrayLength = (sizeof(triangleArry)/sizeof(triangleArry[0]));
    
//     quickSort(triangleArry, 0, arrayLength - 1);

// }

int main(void)
{
    const struct CMUnitTest tests[] = 
    {
        cmocka_unit_test(test_addVec)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}