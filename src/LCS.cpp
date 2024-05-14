#include <stdio.h>
#include <iostream>
using namespace std;


int sequentialLCS (int** arr, string str1, string str2, int row, int column) {

    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (i == 0 || j == 0 )
                arr[i][j] = 0;
            else if (str1[i-1] == str2 [j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else
                arr[i][j] = max(arr[i-1][j], arr[i][j-1]);
        }
    }

    return arr[row-1][column-1];
}


int main(int argc, char* argv[]){
    string str1 = "abcde";
    string str2 = "axbxcx";

    //allocate
    int row = str1.length()+1;
    int column = str2.length()+1;
    int** arr = new int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    cout << sequentialLCS(arr, str1, str2, row, column);

    //deallocate
    for(int i = 0; i < row; i++)
        delete[] arr[i];
    
    return 0;
}