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

unsigned short int** arr;

//dynamic programming solution
int forward_LCS (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++) 
            arr[i][j] = 0;

    for(int i = 0; i < row; i++) 
        for (int j = 0; j < column; j++) 
            if (i == 0 || j == 0 )
                arr[i][j] = 0;
            else if (str1[i-1] == str2 [j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else
                arr[i][j] = max(arr[i-1][j], arr[i][j-1]);

    return arr[row-1][column-1];
}

//dynamic programming solution backward
int backward_LCS (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row+1; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row+1; i++)
        for (int j = 0; j < column+1; j++) 
            arr[i][j] = 0;

    for(int i = row-1; i >= 0; i--) 
        for(int j = column-1; j >= 0; j--) 
            if (i == row-1 || j == column-1)
            if(str1[i-1] == str2[j-1])
                arr[i][j] = 1 + arr[i+1][j+1];
            else
                arr[i][j] = max(arr[i][j+1], arr[i+1][j]);

    return arr[0][0];
}

//anti-diagonal parallelization
int diagonal_LCS (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    int i = 0;
    int j = 0;
    //iterate through diagonals
    while(i <= row-1 && j <= column-1) {

        int antidiag = min(j, row-i-1);

        //parallel calculation of all elements on current diagonal
        #pragma omp parallel for
        for (int k = 0; k <= antidiag; ++k){
            int a = i + k;
            int b = j - k;

            if (a == 0 || b == 0)
                arr[a][b] = 0;
            else if (str1[a-1] == str2[b-1])
                arr[a][b] = 1 + arr[a-1][b-1];
            else
                arr[a][b] = max (arr[a-1][b], arr[a][b-1]);
        }
        
        if(j >= (column-1)) {
            j = column - 2  ;
            i++;
        }
        else
            j++;

    }

    return arr[row-1][column-1];

}

//one thread calculates top half of table
void topHalf_LCS (args& a) {
    
    //first row (half) and column
    for(int i = 0; i <= a.row; i++) 
        arr[i][0] = 0;
    for(int i = 0; i <= a.col; i++)
        arr[0][i] = 0;

    //rest of table
    for(int i = 1; i <= a.row; i++) 
        for(int j = 1; j < a.col; j++)        
            if(a.s1[i-1] == a.s2[j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else
                arr[i][j] = max(arr[i][j-1], arr[i-1][j]);
}

//one thread calculates bottom half of table
void bottomHalf_LCS (args& a) {
    int nrows = a.s1.length();

    //last row (half) and column
    for(int i = nrows; i > a.row; i--) 
        arr[i][a.col] = 0;
    for(int i = a.col+1; i >= 0; i--)
        arr[nrows+1][i] = 0;

    //rest of table
    for(int i = nrows; i > a.row; i--) 
        for(int j = a.col-1; j > 0; j--) 
            if(a.s1[i-1] == a.s2[j-1])
                arr[i][j] = 1 + arr[i+1][j+1];
            else
                arr[i][j] = max(arr[i][j+1], arr[i+1][j]);
}

//merges last rows of topHalf and bottomHalf
int merge_LCS(int h, int row, int column) {
    int temp = 0;
    int temp2 = 0;
    int currentMax = 0;

    for(int i = 1; i<column; i++) {
        temp = arr[h][i] + arr[h+1][i];
        temp2 = arr[h][i] + arr[h+1][i+1];

        if(temp > currentMax || temp2 > currentMax)
            currentMax = max(temp, temp2);

    }

    return currentMax;
}

//forward-backward approach with two threads
int fb_LCS (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row+1; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row+1; i++)
        for (int j = 0; j < column+1; j++) 
            arr[i][j] = 0;

    int h = row / 2;
    struct args a; 
    a.s1 = str1;
    a.s2 = str2;
    a.row = h;
    a.col = column;

    thread t1(topHalf_LCS, ref(a));
    thread t2(bottomHalf_LCS, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the actual distance
    int ret = merge_LCS(h, row, column);

    return ret;
}