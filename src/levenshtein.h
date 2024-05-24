#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
using namespace std;
using namespace std::chrono;

//dynamic programming solution
int sequentialLevenshtein (string str1, string str2, int row, int column) {
    //allocate
    int** arr = new int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            //first row and column (basically empty string)
            if (i == 0)
                arr[i][j] = j;
            if (j == 0)
                arr[i][j] = i;
            
            //the rest of the table
            if(i != 0 && j != 0) {
                if (str1[i-1] == str2[j-1])
                    arr[i][j] = arr[i-1][j-1];
                else
                    arr[i][j] = 1 + min(arr[i][j-1], min(arr[i-1][j], arr[i-1][j-1]));
            }
        }
    }

    int ret = arr[row-1][column-1];

    //deallocate
    for(int i = 0; i < row; i++)
        delete[] arr[i];

    return ret;
}

int parallelLevenshtein (string str1, string str2, int row, int column) {
    //TODO
    return 0;
}
