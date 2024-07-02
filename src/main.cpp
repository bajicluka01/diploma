#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include "LCS.h"
#include "levenshtein.h"
using namespace std;
using namespace std::chrono;

string randomStringGenerator (int len) {
    string str = "";
    for(int i = 0; i < len; i++)
        str += 'a' + rand()%26;
    return str;
}

long long avgExecutionTime (int function, int n, string str1, string str2) {
    microseconds total = microseconds::zero();

    int row = str1.length()+1;
    int column = str2.length()+1;

    /*arr = new int*[row];
    for(int i = 0; i < row+1; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row+1; i++)
        for (int j = 0; j < column+1; j++) 
            arr[i][j] = 0;

    int h = row / 2;
    struct args a; 
    a.s1 = str1;
    a.s2 = str2;
    a.row = h;
    a.col = column;*/

    for(int i = 0; i < n; i++) {
        auto start = high_resolution_clock::now();

        switch(function) {
            case 1: sequentialLCS(str1, str2, row, column); break;
            case 2: fb_LCS(str1, str2, row, column); break;
            case 3: sequentialLevenshtein(str1, str2, row, column); break;
            case 4: fb_levenshtein(str1, str2, row, column); break;
            case 5: parallelLevenshtein(str1, str2, row, column); break;
            //case 5: topHalf(ref(a)); break;
            //case 6: bottomHalf(ref(a)); break;
            default: return 0;
        }

        auto finish = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(finish - start);

        total += duration;
    }

    return total.count()/n;
}

int main (int argc, char* argv[]) {
    //string str1 = "abcbcb";
    //string str2 = "abccabb";

    string str1 = "replace";
    string str2 = "replace";

    str1 = randomStringGenerator(50000);
    str2 = randomStringGenerator(50000);

    int row = str1.length()+1;
    int column = str2.length()+1;
   
    /*auto start = high_resolution_clock::now();
    cout << "Sequential Levenshtein solution: " << sequentialLevenshtein(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential Levenshtein duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "Forward-backward Levenshtein solution: " << fb_levenshtein(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Forward-backward Levenshtein duration: " << duration.count() << "\n";*/

    
    //cout << "Average Levenshtein dp: "<< avgExecutionTime(3, 1, str1, str2) <<"\n";
    cout << "Average Levenshtein fb: "<< avgExecutionTime(4, 1, str1, str2) <<"\n";
    //cout << "Average Levenshtein diag: "<< avgExecutionTime(5, 3, str1, str2) <<"\n";

    return 0;
}