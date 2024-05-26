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

int main(int argc, char* argv[]){
    string str1 = "abcde";
    string str2 = "axbxcx";

    str1 = randomStringGenerator(100000);
    str2 = randomStringGenerator(50000);

    int row = str1.length()+1;
    int column = str2.length()+1;

    //Longest Common Subsequence
    auto start = high_resolution_clock::now();
    cout << "Sequential LCS solution: " << sequentialLCS(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential LCS duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "Parallel LCS solution: " << parallelLCS(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Parallel LCS duration: " << duration.count() << "\n";


    str1 = "delete";
    str2 = "replace";
    row = str1.length()+1;
    column = str2.length()+1;

    //Levenshtein distance
    start = high_resolution_clock::now();
    cout << "Sequential Levenshtein solution: " << sequentialLevenshtein(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential Levenshtein duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "Parallel Levenshtein solution: " << parallelLevenshtein(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Parallel Levenshtein duration: " << duration.count() << "\n";
    
    return 0;
}