#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include "LCS.h"
#include "levenshtein.h"
using namespace std;
using namespace std::chrono;

//generates random string of length len consisting of English alphabet characters
string randomStringGenerator (int len) {
    string str = "";
    for(int i = 0; i < len; i++)
        str += 'a' + rand()%26;
    return str;
}

//measure average execution time over n iterations
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
            case 1: forward_LCS(str1, str2, row, column); break;
            case 2: forward_LCS_space_optimization(str1, str2, row, column); break;
            case 3: backward_LCS(str1, str2, row, column); break;
            case 4: backward_LCS_space_optimization(str1, str2, row, column); break;
            case 5: diagonal_LCS(str1, str2, row, column); break;
            case 6: fb_LCS(str1, str2, row, column); break;
            case 7: fb_LCS_space_optimization(str1, str2, row, column); break;
            case 8: forward_levenshtein(str1, str2, row, column); break;
            case 9: forward_levenshtein_space_optimization(str1, str2, row, column); break;
            case 10: backward_levenshtein(str1, str2, row, column); break;
            case 11: backward_levenshtein_space_optimization(str1, str2, row, column); break;
            case 12: diagonal_levenshtein(str1, str2, row, column); break;
            case 13: fb_levenshtein(str1, str2, row, column); break;
            case 14: fb_levenshtein_space_optimization(str1, str2, row, column); break;
            //case 15: topHalf_LCS(ref(a)); break;
            //case 16: bottomHalf_LCS(ref(a)); break;
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

    //string str1 = "replace";
    //string str2 = "replace";


    //interesting examples: (3001, 4006), (555, 4006), (555, 333)
    str1 = randomStringGenerator(3001);
    str2 = randomStringGenerator(4006);

    int row = str1.length()+1;
    int column = str2.length()+1;
   
    /*auto start = high_resolution_clock::now();
    cout << "Sequential Levenshtein solution: " << backward_levenshtein(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential Levenshtein duration: " << duration.count() << "\n";

    start = high_resolution_clock::now();
    cout << "Forward-backward Levenshtein solution: " << backward_levenshtein_space_optimization(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Forward-backward Levenshtein duration: " << duration.count() << "\n";*/

    int n_iter = 1;

    //time testing
    //cout << "Forward LCS: "<< avgExecutionTime(1, n_iter, str1, str2) <<"\n";
    //cout << "Backward LCS: "<< avgExecutionTime(2, n_iter, str1, str2) <<"\n";
    //cout << "Diagonal LCS: "<< avgExecutionTime(3, n_iter, str1, str2) <<"\n";
    //cout << "Forward-backward LCS: "<< avgExecutionTime(4, n_iter, str1, str2) <<"\n";
    
    //cout << "Backward Levenshtein: "<< avgExecutionTime(6, n_iter, str1, str2) <<"\n";
    //cout << "Diagonal Levenshtein: "<< avgExecutionTime(7, n_iter, str1, str2) <<"\n";
    //cout << "Forward-backward Levenshtein: "<< avgExecutionTime(8, n_iter, str1, str2) <<"\n";
    //cout << "Forward Levenshtein: "<< avgExecutionTime(5, n_iter, str1, str2) <<"\n";


    //test if all algorithms return same values
    cout<<forward_levenshtein(str1, str2, row, column)<<"\n";
    cout<<forward_levenshtein_space_optimization(str1, str2, row, column)<<"\n";
    cout<<backward_levenshtein(str1, str2, row, column)<<"\n";
    cout<<backward_levenshtein_space_optimization(str1, str2, row, column)<<"\n";
    cout<<diagonal_levenshtein(str1, str2, row, column)<<"\n";
    cout<<fb_levenshtein(str1, str2, row, column)<<"\n";
    cout<<fb_levenshtein_space_optimization(str1, str2, row, column)<<"\n\n";

    cout<<forward_LCS(str1, str2, row, column)<<"\n";
    cout<<forward_LCS_space_optimization(str1, str2, row, column)<<"\n";
    cout<<backward_LCS(str1, str2, row, column)<<"\n";
    cout<<backward_LCS_space_optimization(str1, str2, row, column)<<"\n";
    cout<<diagonal_LCS(str1, str2, row, column)<<"\n";
    cout<<fb_LCS(str1, str2, row, column)<<"\n";
    cout<<fb_LCS_space_optimization(str1, str2, row, column)<<"\n";

    return 0;
}