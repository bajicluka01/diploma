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
            case 2: fb_LCS(str1, str2, row, column); break;
            case 3: sequentialLevenshtein(str1, str2, row, column); break;
            case 4: fb_levenshtein(str1, str2, row, column); break;
            default: return 0;
        }

        auto finish = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(finish - start);

        total += duration;
    }

    return total.count()/n;
}

int main (int argc, char* argv[]) {
    string str1 = "abcbcb";
    string str2 = "abccabb";

    //str1 = randomStringGenerator(30000);
    //str2 = randomStringGenerator(30000);

    
    //TODO: currently needs len+1 for LCS, but just len for Levenshtein
    int row = str1.length()+1;
    int column = str2.length()+1;
   
    auto start = high_resolution_clock::now();
    cout << "Sequential LCS solution: " << sequentialLevenshtein(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential LCS duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "Forward-backward LCS solution: " << fb_levenshtein(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Forward-backward LCS duration: " << duration.count() << "\n";

    return 0;
}