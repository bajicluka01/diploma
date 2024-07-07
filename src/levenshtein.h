#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <thread>
using namespace std;
using namespace std::chrono;

//dynamic programming solution
int forward_levenshtein (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {    
            if (i == 0)
                arr[i][j] = j;
            else if (j == 0)
                arr[i][j] = i;
            else if (str1[i-1] == str2[j-1])
                arr[i][j] = arr[i-1][j-1];
            else 
                arr[i][j] = 1 + min(arr[i][j-1], min(arr[i-1][j], arr[i-1][j-1]));
        }
    }

    return arr[row-1][column-1];
}

//space optimization: we only keep current two rows in memory
int forward_levenshtein_space_optimization (string str1, string str2, int row, int column) {

    int temp1[column]; 
    int current[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1[i] = 0;
        current[i] = 0;
    }

    for(int i = 0; i < row; i++) {

        for(int j = 0; j < column; j++) {
            if (i == 0)
                current[j] = j;
            else if (j == 0)
                current[j] = i;
            else if (str1[i-1] == str2[j-1])
                current[j] = temp1[j-1];
            else 
                current[j] = 1 + min(current[j-1], min(temp1[j], temp1[j-1]));
        }

        //rewrite data
        for(int j = 0; j < column; j++) 
            temp1[j] = current[j];
        
    }

    return current[column-1];
}

//dynamic programming solution backward
int backward_levenshtein (string str1, string str2, int row, int column) {

    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row+1; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row+1; i++){
        for (int j = 0; j < column+1; j++) {
            arr[i][j] = 0;
        }
    }

    for(int i = row-1; i >= 0; i--) {
        for (int j = column-1; j >= 0; j--) {    
            if (i == row-1)
                arr[i][j] = column-j-1;
            else if (j == column-1)
                arr[i][j] = row-i-1;
            else if (str1[i] == str2[j])
                arr[i][j] = arr[i+1][j+1];
            else 
                arr[i][j] = 1 + min(arr[i][j+1], min(arr[i+1][j], arr[i+1][j+1]));
        }
    }

    return arr[row-1][column-1];;
}

//anti-diagonal approach with parallelization
int diagonal_levenshtein (string str1, string str2, int row, int column) {
    
    //allocate
    arr = new unsigned short int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];

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
void topHalf_levenshtein(args& a) {
    for(int i = 0; i <= a.row; i++) 
        for (int j = 0; j < a.col; j++) {      
            if (i == 0)
                arr[i][j] = j;
            else if (j == 0)
                arr[i][j] = i;
            else if (a.s1[i-1] == a.s2[j-1])
                arr[i][j] = arr[i-1][j-1];
            else 
                arr[i][j] = 1 + min(arr[i][j-1], min(arr[i-1][j], arr[i-1][j-1]));
        }
}

//one thread calculates the bottom half of the table
void bottomHalf_levenshtein (args& a) {
    int nrows = a.s1.length()+1;

    for(int i = nrows-1; i >= a.row; i--) {
        for (int j = a.col-1; j >= 0; j--) {    
            if (i == nrows-1)
                arr[i+1][j+1] = a.col-j-1;
            else if (j == a.col-1)
                arr[i+1][j+1] = nrows-i-1;
            else if (a.s1[i] == a.s2[j])
                arr[i+1][j+1] = arr[i+2][j+2];
            else 
                arr[i+1][j+1] = 1 + min(arr[i+1][j+2], min(arr[i+2][j+1], arr[i+2][j+2]));
        }
    }
}

//merges last rows of topHalf and bottomHalf
int merge_levenshtein (int h, int row, int column) {
    int temp = 0;
    int temp2 = 0;
    int currentMin = INT_MAX;

    for(int i = 1; i<column; i++) {
        temp = arr[h][i] + arr[h+1][i];
        temp2 = arr[h][i] + arr[h+1][i+1];

        if(temp < currentMin || temp2 < currentMin)
            currentMin = min(temp, temp2);

    }

    return currentMin;
}

//forward-backward approach with 2 threads
int fb_levenshtein (string str1, string str2, int row, int column) {
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

    thread t1(topHalf_levenshtein, ref(a));
    thread t2(bottomHalf_levenshtein, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the distance
    int ret = merge_levenshtein(h, row, column);

    /*
    //TEMP
    int ret = 0;
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
    */

    return ret;

}


