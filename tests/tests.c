//
// Created by Liam Murphy on 2/09/23.
//
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <draw.h>

double epsilon = 0.0001;

static void test_initPoints(void /***state*/)
{
    double arr[8][3];
    double basePoints[8][3] = {
        {0, 0, 0},
        {0, 0, 1},
        {0, 1, 0},
        {0, 1, 1},
        {1, 0, 0},
        {1, 0, 1},
        {1, 1, 0},
        {1, 1, 1}
    };

    initPoints(arr, basePoints);
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++)
        {
            assert_float_equal(arr[i][j], basePoints[i][j], epsilon);
        }
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = 
    {
        cmocka_unit_test(test_initPoints),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}