//
// Created by Liam Murphy on 2/09/23.
//

#include "drawSquareDoubles.c"

//Test
TEST_CASE("Test initPoints", "[initPoints]")
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
            REQUIRE(arr[i][j] == basePoints[i][j]);
        }
    }
}