#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include "LCS.h"
using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]){
    string str1 = "abcde";
    string str2 = "axbxcx";

    str1 = randomStringGenerator(1000);
    str2 = randomStringGenerator(500);

    int row = str1.length()+1;
    int column = str2.length()+1;

    auto start = high_resolution_clock::now();
    cout << "LCS: " << sequentialLCS(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "LCS: " << parallelLCS(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Parallel: " << duration.count() << "\n";
    
    return 0;
}