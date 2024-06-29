#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <thread>
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

    //test if first characters match for the purposes of calculating first row and column more efficiently
    bool firstChar = str1[0] == str2[0];

    //first row and column (can probably be further optimized)
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            if (i == 0 && j == 0)
                arr[i][j] = firstChar ? 0 : 1;
            else {
                if(i == 0) 
                    arr[i][j] = firstChar ? 0 : j+1;
                if(j == 0) 
                    arr[i][j] = firstChar ? 0 : i+1;
            }
        }
    }

    //rest of table
    for(int i = 1; i < row; i++) {
        for (int j = 1; j < column; j++) {          
            if (str1[i] == str2[j])
                arr[i][j] = arr[i-1][j-1];
            else 
                arr[i][j] = 1 + min(arr[i][j-1], min(arr[i-1][j], arr[i-1][j-1]));
        }
    }

    /*cout << "\n";
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            cout << arr[i][j] << " ";
        }
        cout << "\n";
    }*/

    int ret = arr[row-1][column-1];

    return ret;
}

//anti-diagonal approach with parallelization
int parallelLevenshtein (string str1, string str2, int row, int column) {
    
    //allocate
    int** arr = new int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++) 
            arr[i][j] = 0;
        
    int i = 0;
    int j = 0;
    //iterating through diagonals
    while(i <= row-1 && j <= column-1) {

        int antidiag = min(j, row-i-1);

        //parallell calculations of elements on current diagonal
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

    return arr[row-1][column-1];

}

//one thread calculates the top half of the table
void topHalf(args& a) {
    //test if first characters match for the purposes of calculating first row and column more efficiently
    bool firstChar = a.s1[0] == a.s2[0];

    //first row and column (can probably be further optimized)
    for (int i = 0; i < a.row; i++){
        for (int j = 0; j < a.col; j++) {
            if (i == 0 && j == 0)
                arr[i][j] = firstChar ? 0 : 1;
            else {
                if(i == 0) 
                    arr[i][j] = firstChar ? arr[i][j-1]+1 : j+1;
                if(j == 0) 
                    arr[i][j] = firstChar ? arr[i-1][j]+1 : i+1;
            }
        }
    }

    //rest of table
    for(int i = 1; i < a.row; i++) 
        for (int j = 1; j < a.col; j++)       
            if (a.s1[i] == a.s2[j])
                arr[i][j] = arr[i-1][j-1];
            else 
                arr[i][j] = 1 + min(arr[i][j-1], min(arr[i-1][j], arr[i-1][j-1]));
}

//one thread calculates the bottom half of the table
//TODO fix odd/even h
void bottomHalf(args& a) {
    int nrows = a.s1.length()-1;

    bool lastChar = a.s1[a.s1.length()-1] == a.s2[a.s2.length()-1];

    //first row and column (can probably be further optimized)
    for (int i = nrows; i >= a.row; i--) {
        for (int j = a.col-1; j >= 0; j--) {
            if (i == nrows && j == a.col-1) 
                arr[i][j] = lastChar ? 0 : 1;
            else {
                if(i == nrows) 
                    arr[i][j] = lastChar ? arr[i][j+1]+1 : a.col-j;
                if(j == a.col-1) 
                    arr[i][j] = lastChar ? arr[i+1][j]+1 : nrows-i+1;
            }
        }
    }

    //rest of table
    for(int i = nrows-1; i >= a.row; i--) 
        for(int j = a.col-2; j >= 0; j--) 
            if(a.s1[i] == a.s2[j])
                arr[i][j] = arr[i+1][j+1];
            else
                arr[i][j] = 1 + min(arr[i][j+1], min(arr[i+1][j], arr[i+1][j+1]));
}

//merges last rows of topHalf and bottomHalf
int merge(int h, int row, int column) {
    int* temp = new int[column-1];
    int currentMin = 0;

    for(int i = 1; i<column; i++) {
        temp[i] = arr[h][i] + arr[h+1][i];

        if(temp[i] > currentMin)
            currentMin = temp[i];

    }

    return currentMin;
}

//forward-backward approach with 2 threads
int fb_levenshtein (string str1, string str2, int row, int column) {
    //allocate
    arr = new int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++) 
            arr[i][j] = 0;
        
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

    //merge results to find the distance
    int ret = merge(h, row, column);

    //TEMP
    auto start = high_resolution_clock::now();
    topHalf(ref(a));
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);
    cout << "\n\nTop half duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    bottomHalf(ref(a));
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);
    cout << "Bottom half duration: " << duration.count() << "\n\n\n";
    //TEMP

    cout << "\n";
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            cout << arr[i][j] << " ";
        }
        cout << "\n";
    }

    return ret;

}
