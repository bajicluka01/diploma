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

    for(int i = 0; i < n; i++) {
        auto start = high_resolution_clock::now();

        switch(function) {
            case 1: sequentialLCS(str1, str2, row, column); break;
            case 2: parallelLCS(str1, str2, row, column); break;
            case 3: sequentialLevenshtein(str1, str2, row, column); break;
            case 4: parallelLevenshtein(str1, str2, row, column); break;
            default: return 0;
        }

        auto finish = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(finish - start);

        total += duration;
    }

    return total.count()/n;
}

int main (int argc, char* argv[]) {
    string str1 = "abcde";
    string str2 = "axbxcx";

    str1 = randomStringGenerator(30000);
    str2 = randomStringGenerator(30000);

    int row = str1.length()+1;
    int column = str2.length()+1;

    /*
    //Longest Common Subsequence
    auto start = high_resolution_clock::now();
    cout << "Sequential LCS solution: " << sequentialLCS(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential LCS duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "Parallel LCS solution  : " << parallelLCS(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Parallel LCS duration  : " << duration.count() << "\n";

    str1 = "delete";
    str2 = "replace";

    str1 = randomStringGenerator(1000);
    str2 = randomStringGenerator(5000);

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
    */


   cout << "Sequential LCS average: " << avgExecutionTime(1, 10, str1, str2) << "\n";
   cout << "Parallel LCS average: " << avgExecutionTime(2, 10, str1, str2) << "\n";


    return 0;
}