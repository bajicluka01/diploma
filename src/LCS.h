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

//for forward-backward with space optimization
int* arrTop;
int* arrBottom;

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
            if (i == 0 || j == 0)
                arr[i][j] = 0;
            else if (str1[i-1] == str2[j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else
                arr[i][j] = max(arr[i-1][j], arr[i][j-1]);

    return arr[row-1][column-1];
}

//space optimization: we only keep current two rows in memory
int forward_LCS_space_optimization (string str1, string str2, int row, int column) {

    int temp1[column]; 
    int current[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1[i] = 0;
        current[i] = 0;
    }

    for(int i = 0; i < row; i++) {

        for(int j = 0; j < column; j++) {
            if (i == 0 || j == 0)
                current[j] = 0;
            else if (str1[i-1] == str2[j-1])
                current[j] = 1 + temp1[j-1];
            else
                current[j] = max(current[j-1], temp1[j]);
        }

        //rewrite data
        for(int j = 0; j < column; j++) 
            temp1[j] = current[j];

    }

    return current[column-1];
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
                arr[i][j] = 0;
            else if(str1[i] == str2[j])
                arr[i][j] = 1 + arr[i+1][j+1];
            else
                arr[i][j] = max(arr[i][j+1], arr[i+1][j]);

    return arr[0][0];
}

//space optimization: we only keep current two rows in memory
int backward_LCS_space_optimization (string str1, string str2, int row, int column) {

    int temp1[column]; 
    int current[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1[i] = 0;
        current[i] = 0;
    }

    for(int i = row-1; i >= 0; i--) {
        for(int j = column-1; j >= 0; j--) {
            if (i == row-1 || j == column-1)
                current[j] = 0;
            else if(str1[i] == str2[j])
                current[j] = 1 + temp1[j+1];
            else
                current[j] = max(current[j+1], temp1[j]);
        }

        //rewrite data
        for(int j = 0; j < column; j++) 
            temp1[j] = current[j];

    }

    return current[0];
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
        //#pragma omp parallel for
        //#pragma omp target teams distribute parallel for
        //#pragma omp target teams loop
        #pragma omp parallel for num_threads (6)
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
    for(int i = 0; i <= a.row; i++) 
        for(int j = 0; j < a.col; j++)
            if(i == 0 || j == 0)
                arr[i][j] = 0;        
            else if(a.s1[i-1] == a.s2[j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else 
                arr[i][j] = max(arr[i][j-1], arr[i-1][j]);
}

//one thread calculates bottom half of table
void bottomHalf_LCS (args& a) {
    int nrows = a.s1.length();

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    for(int i = nrows+1; i > a.row; i--) 
        for(int j = a.col; j > 0; j--) 
            if (i == nrows+1 || j == a.col) 
                arr[i][j] = 0;
            else if(a.s1[i-1] == a.s2[j-1])
                arr[i][j] = 1 + arr[i+1][j+1];
            else
                arr[i][j] = max(arr[i][j+1], arr[i+1][j]);
}

//merges last rows of topHalf and bottomHalf
int merge_LCS(int h, int row, int column) {
    int temp = 0;
    int currentMax = 0;

    for(int i = 1; i <= column; i++) {
        temp = arr[h][i-1] + arr[h+1][i];

        if(temp > currentMax)
            currentMax = temp;

    }

    return currentMax;
}

//space optimization: we only keep current two rows in memory
void topHalf_LCS_space_optimization (args& a) {
    int temp1[a.col+1]; 
    int current[a.col+1];

    //initialize zeros
    for (int i = 0; i < a.col+1; i++) {
        temp1[i] = 0;
        current[i] = 0;
        arrTop[i] = 0;
    }

    for(int i = 0; i <= a.row; i++) {
        for(int j = 0; j < a.col; j++) {
            if (i == 0 || j == 0)
                current[j] = 0;
            else if (a.s1[i-1] == a.s2[j-1])
                current[j] = 1 + temp1[j-1];
            else
                current[j] = max(current[j-1], temp1[j]);
        }

        //rewrite data
        for(int j = 0; j < a.col+1; j++) 
            temp1[j] = current[j];

    }

    for(int i = 0; i < a.col+1; i++) 
        arrTop[i] = current[i];
    
}

//space optimization: we only keep current two rows in memory
void bottomHalf_LCS_space_optimization (args& a) {
    int temp1[a.col+1]; 
    int current[a.col+1];
    

    //initialize zeros
    for (int i = 0; i <= a.col; i++) {
        temp1[i] = 0;
        current[i] = 0;
        arrBottom[i] = 0;
    }

    int nrows = a.s1.length();

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    for(int i = nrows+1; i > a.row; i--) {
        for(int j = a.col; j > 0; j--)  
            if (i == nrows+1 || j == a.col)
                current[j] = 0;
            else if (a.s1[i-1] == a.s2[j-1])
                current[j] = 1 + temp1[j+1];
            else 
                current[j] = max(current[j+1], temp1[j]);

        //rewrite data
        for(int j = 0; j < a.col; j++) 
            temp1[j] = current[j];
        
    }

    for(int i = 0; i < a.col; i++) 
        arrBottom[i] = current[i];

}



//merges last rows of topHalf_LCS_space_optimization and bottomHalf_LCS_space_optimization
int merge_LCS_space_optimization(int column) {
    int temp = 0;
    int currentMax = 0;

    for(int i = 1; i <= column; i++) {
        temp = arrTop[i-1] + arrBottom[i];

        if(temp > currentMax)
            currentMax = temp;

    }

    return currentMax;
}

//forward-backward approach with two threads
int fb_LCS (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row+1];
    for(int i = 0; i < row+1; i++)
        arr[i] = new unsigned short int[column+1];

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

//forward-backward approach with two threads and with space optimization
int fb_LCS_space_optimization (string str1, string str2, int row, int column) {

    int h = row / 2;
    struct args a; 
    a.s1 = str1;
    a.s2 = str2;
    a.row = h;
    a.col = column;

    arrTop = new int[column+1];
    arrBottom = new int[column+1];

    thread t1(topHalf_LCS_space_optimization, ref(a));
    thread t2(bottomHalf_LCS_space_optimization, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the actual distance
    int ret = merge_LCS_space_optimization(column);

    return ret;
}
