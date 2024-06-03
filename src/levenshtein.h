#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <thread>
using namespace std;
using namespace std::chrono;

struct args {
    string s1;
    string s2;
    int row;
    int col;
};

int** arr;

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
    
    //allocate
    int** arr = new int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    int i = 0;
    int j = 0;
    while(i <= row-1 && j <= column-1) {

        int antidiag = min(j, row-i-1);

        #pragma omp parallel for
        for (int k = 0; k <= antidiag; ++k){
            int a = i + k;
            int b = j - k;

            if (a == 0)
                arr[a][b] = b;
            if (b == 0)
                arr[a][b] = a;
            
            if (a != 0 && b != 0){
                if (str1[a-1] == str2[b-1])
                    arr[a][b] = arr[a-1][b-1];
                else
                    arr[a][b] = 1 + min(arr[a][b-1], min(arr[a-1][b], arr[a-1][b-1]));
            }
        }
        
        if(j >= (column-1)) {
            j = column - 2  ;
            i++;
        }
        else
            j++;

    }

    int ret = arr[row-1][column-1];

    //deallocate
    for (int i = 0; i < row; i++)
        delete[] arr[i];

    return ret;

}

void topHalf(args& a) {
    for(int i = 0; i <= a.row; i++) {
        for(int j = 0; j < a.col; j++) {
            if (i == 0)
                arr[i][j] = j;
            if (j == 0)
                arr[i][j] = i;
            
            if (i != 0 && j != 0) {
                if(a.s1[i-1] == a.s2[j-1])
                    arr[i][j] = arr[i-1][j-1];
                else
                    arr[i][j] = 1 + min(arr[i][j-1], min(arr[i-1][j], arr[i-1][j-1]));
            }
        }
    }
}

//doesn't work yet
void bottomHalf(args& a) {
    int nrows = a.s1.length();
    for(int i = nrows; i > a.row; i--) {
        for(int j = a.col-1; j >= 0; j--) {
            if (i == nrows)
                arr[i][j] = j;
            if (j == a.col-1)
                arr[i][j] = i;
            
            if (i != nrows && j != a.col-1) {
                if(a.s1[i+1] == a.s2[j+1])
                    arr[i][j] = arr[i+1][j+1];
                else
                    arr[i][j] = 1 + min(arr[i][j+1], min(arr[i+1][j], arr[i+1][j+1]));
            }
        }
    }
}

//TODO: FIX - DOESN'T WORK IF FIRST TWO (OR LAST TWO) CHARS ARE EQUAL
int fb_levenshtein (string str1, string str2, int row, int column) {

    //allocate
    arr = new int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    int h = row / 2;
    struct args a; 
    a.s1 = str1;
    a.s2 = str2;
    a.row = h;
    a.col = column;

    thread t1(topHalf, ref(a));
    thread t2(bottomHalf, ref(a));
    
    t1.join();
    t2.join();

    /*for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            cout << arr[i][j] << " ";
        }
        cout << "\n";
    }*/


    //TODO: check if this is always correct!!
    return max(arr[h][0], arr[h][column]);

}
