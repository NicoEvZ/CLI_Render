#include <stdio.h>
#include "quick_sort.h"
#include "draw.h"

// Function to swap two elements in an array
void swap(triangle* a, triangle* b) {
    triangle *temp = a;
    a = b;
    b = temp;
}

double zAvg(triangle inputTri)
{
    double output = 0;
    for (int i = 0; i < 3; i++)
    {
        output += inputTri.p[i].z;
    }
    output = output / 3;
    return output;
}

// Function to partition the array and return the pivot index
int partition(triangle triArr[], int low, int high) {
    triangle pivot = triArr[high]; // Choose the last element as the pivot
    int i = (low - 1);     // Index of the smaller element

    for (int j = low; j <= high - 1; j++) {
        // If the current element is smaller than or equal to the pivot
        if (zAvg(triArr[j]) <= zAvg(pivot)) {
            i++;
            swap(&triArr[i], &triArr[j]);
        }
    }

    swap(&triArr[i + 1], &triArr[high]); // Swap the pivot element with the element at (i + 1)
    return (i + 1);
}

// Function to implement QuickSort
void quickSort(triangle triArr[], int low, int high) {
    if (low < high) {
        // Find the pivot index such that elements smaller than the pivot are on the left,
        // and elements greater than the pivot are on the right
        int pivotIndex = partition(triArr, low, high);

        // Recursively sort the sub-arrays
        quickSort(triArr, low, pivotIndex - 1);
        quickSort(triArr, pivotIndex + 1, high);
    }
}
