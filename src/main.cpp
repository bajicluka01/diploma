#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <math.h>
#include "LCS.h"
#include "levenshtein.h"
using namespace std;
using namespace std::chrono;

//generates random string of length len consisting of English alphabet characters
string randomStringGenerator (int len, int seed) {
    //srand(seed);
    string str = "";
    for(int i = 0; i < len; i++)
        str += 'a' + rand()%26;
    return str;
}

//measures average execution time of a function over n iterations
long long avgExecutionTime (int function, int n, string str1, string str2) {
    microseconds total = microseconds::zero();

    int row = str1.length()+1;
    int column = str2.length()+1;

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
            case 15: diagonal_LCS_parallel(str1, str2, row, column); break;
            case 16: diagonal_levenshtein_parallel(str1, str2, row, column); break;
            case 17: diagonal_levenshtein_memory_optimization(str1, str2, row, column); break;
            case 18: diagonal_levenshtein_memory_optimization_parallel(str1, str2, row, column); break;
            case 19: diagonal_levenshtein_memory_and_space_optimization(str1, str2, row, column); break;
            case 20: diagonal_levenshtein_memory_and_space_optimization_parallel(str1, str2, row, column); break;
            case 21: diagonal_LCS_memory_optimization(str1, str2, row, column); break;
            case 22: diagonal_LCS_memory_optimization_parallel(str1, str2, row, column); break;
            case 23: diagonal_LCS_memory_and_space_optimization(str1, str2, row, column); break;
            case 24: diagonal_LCS_memory_and_space_optimization_parallel(str1, str2, row, column); break;
            default: return 0;
        }

        auto finish = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(finish - start);

        total += duration;
    }

    return total.count()/n;
}

//testing for every permutation of dimensions for positive integers up to v1 and v2
//the idea is primarily to find potential bugs/edge cases for smaller arrays
void testLevenshteinForValues(int v1, int v2) {
    for(int i = 1; i <= v1; i++) {
        for(int j = 1; j <= v2; j++) {

            string str1 = randomStringGenerator(i, 42);
            string str2 = randomStringGenerator(j, 101);

            //requirement for diagonal
            if(str2.length() > str1.length()) {
                string tmp = str1;
                str1 = str2;
                str2 = tmp;
            }

            int row = str1.length()+1;
            int column = str2.length()+1;

            int tmp1 = forward_levenshtein(str1, str2, row, column);
            int tmp2 = forward_levenshtein_space_optimization(str1, str2, row, column);
            int tmp3 = backward_levenshtein(str1, str2, row, column);
            int tmp4 = backward_levenshtein_space_optimization(str1, str2, row, column);
            int tmp5 = diagonal_levenshtein(str1, str2, row, column);
            int tmp6 = fb_levenshtein(str1, str2, row, column);
            int tmp7 = fb_levenshtein_space_optimization(str1, str2, row, column);
            int tmp8 = diagonal_levenshtein_parallel(str1, str2, row, column);
            int tmp9 = diagonal_levenshtein_memory_optimization(str1, str2, row, column);

            if (tmp1 != tmp2) {
                cout<<"Error: forward space optimization ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp2<<"\n";
                //return;
            }
            if (tmp1 != tmp3) {
                cout<<"Error: backward ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp3<<"\n";
                //return;
            }
            if (tmp1 != tmp4) {
                cout<<"Error: backward space optimization ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp4<<"\n";
                //return;
            }
            if (tmp1 != tmp5) {
                cout<<"Error: diagonal ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp5<<"\n";
                //return;
            }
            if (tmp1 != tmp6) {
                cout<<"Error: forward-backward ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp6<<"\n";
                //return;
            }
            if (tmp1 != tmp7) {
                cout<<"Error: forward-backward space optimization ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp7<<"\n";
                //return;
            }
        }
    }

    cout<<"OK!";
}

//testing for every permutation of positive integers up to v1 and v2
void testLCSforValues(int v1, int v2) {
    for(int i = 1; i <= v1; i++) {
        for(int j = 1; j <= v2; j++) {

            string str1 = randomStringGenerator(i, 42);
            string str2 = randomStringGenerator(j, 555);

            int row = str1.length()+1;
            int column = str2.length()+1;

            int tmp1 = forward_LCS(str1, str2, row, column);
            int tmp2 = forward_LCS_space_optimization(str1, str2, row, column);
            int tmp3 = backward_LCS(str1, str2, row, column);
            int tmp4 = backward_LCS_space_optimization(str1, str2, row, column);
            int tmp5 = diagonal_LCS(str1, str2, row, column);
            int tmp6 = fb_LCS(str1, str2, row, column);
            int tmp7 = fb_LCS_space_optimization(str1, str2, row, column);

            if (tmp1 != tmp2) {
                cout<<"Error: forward space optimization ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp2<<"\n";
                return;
            }
            if (tmp1 != tmp3) {
                cout<<"Error: backward ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp3<<"\n";
                return;
            }
            if (tmp1 != tmp4) {
                cout<<"Error: backward space optimization ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp4<<"\n";
                return;
            }
            if (tmp1 != tmp5) {
                cout<<"Error: diagonal ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp5<<"\n";
                return;
            }
            if (tmp1 != tmp6) {
                cout<<"Error: forward-backward ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp6<<"\n";
                return;
            }
            if (tmp1 != tmp7) {
                cout<<"Error: forward-backward space optimization ("<<i<<" "<<j<<")\n";
                cout<<"Strings: "<<str1<<" "<<str2<<"\n";
                cout<<"Values: "<<tmp1<<" "<<tmp7<<"\n";
                return;
            }
        }
    }

    cout<<"OK!";
}

//searching for potential bugs in merge
void mergeTestLCS(int n, int size1, int size2) {
    for(int i = 0; i < n; i++) {
        string s1 = randomStringGenerator(size1, time(NULL));
        string s2 = randomStringGenerator(size2, time(NULL));
        int row = s1.length()+1;
        int column = s2.length()+1;

        int res1 = forward_LCS(s1, s2, row, column);
        int res2 = fb_LCS(s1, s2, row, column);
        
        if(res1 != res2)
            cout<<"Error for strings: "<<s1<<" "<<s2<<"\nResults: "<<res1<<" "<<res2<<"\n\n";
    }

    cout<<"OK!";
}

//searching for potential bugs in merge
void mergeTestLevenshtein(int n, int size1, int size2) {
    for(int i = 0; i < n; i++) {
        string s1 = randomStringGenerator(size1, time(NULL));
        string s2 = randomStringGenerator(size2, time(NULL));
        int row = s1.length()+1;
        int column = s2.length()+1;

        int res1 = forward_levenshtein(s1, s2, row, column);
        int res2 = fb_levenshtein(s1, s2, row, column);
        
        if(res1 != res2)
            cout<<"Error for strings: "<<s1<<" "<<s2<<"\nResults: "<<res1<<" "<<res2<<"\n\n";
    }

    cout<<"OK!";
}

int main (int argc, char* argv[]) {
    string str1 = "abcd";
    string str2 = "xyzw";

    srand(time(NULL));

    int strLen = 100000;

    str1 = randomStringGenerator(strLen, 42);
    str2 = randomStringGenerator(strLen, 101);

    //diagonal approach requires len(str1) >= len(str2)
    if(str2.length() > str1.length()) {
        string tmp = str1;
        str1 = str2;
        str2 = tmp;
    }

    int row = str1.length()+1;
    int column = str2.length()+1;

    //mergeTestLCS(8000, 6, 6);
    //mergeTestLevenshtein(8000, 6, 7);

    //example from excel
    //cout << "Sequential Levenshtein solution: " << fb_levenshtein("abcbcb", "abccabb", 7, 8);
    //cout << "Sequential Levenshtein solution: " << fb_LCS("abcbcb", "abccabb", 7, 8);

    //str1 = "dijreffdsjrw";
    //str2 = "ygwsiebhvuundivrspefpokuz";

    /*str1 = "aaabbb";
    str2 = "bbb";
    row = str1.length()+1;
    column = str2.length()+1;
    cout << "\nSequential Levenshtein solution: " << forward_levenshtein(str1, str2, row, column)<<"\n";
    cout << "Sequential Levenshtein solution: " << fb_levenshtein(str1, str2, row, column);*/

    //testLevenshteinForValues(55, 25);
    //testLCSforValues(55, 25);
   
    auto start = high_resolution_clock::now();
    cout << "Sequential Levenshtein solution: " << diagonal_LCS_memory_and_space_optimization(str1, str2, row, column) << "\n";
    auto finish = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(finish - start);

    cout << "Sequential Levenshtein duration: " << duration.count() << "\n";

    /*str1 = "xyz";
    str2 = "abcd";
    row = str1.length()+1;
    column = str2.length()+1;
    forward_levenshtein(str1, str2, row, column);*/

    /*start = high_resolution_clock::now();
    cout << "Forward-backward Levenshtein solution: " << fb_levenshtein_space_optimization(str1, str2, row, column) << "\n";
    finish = high_resolution_clock::now();
    duration = duration_cast<microseconds>(finish - start);

    cout << "Forward-backward Levenshtein duration: " << duration.count() << "\n";*/

    int n_iter = 1;

    //time testing
    //cout << "Forward LCS: "<< avgExecutionTime(1, n_iter, str1, str2) <<"\n";
    //cout << "Forward LCS space optimization: "<< avgExecutionTime(2, n_iter, str1, str2) <<"\n";
    //cout << "Backward LCS: "<< avgExecutionTime(3, n_iter, str1, str2) <<"\n";
    //cout << "Backward LCS space optimization: "<< avgExecutionTime(4, n_iter, str1, str2) <<"\n";
    //cout << "Diagonal LCS memory: "<< avgExecutionTime(24, n_iter, str1, str2) <<"\n";
    //cout << "Forward-backward LCS: "<< avgExecutionTime(6, n_iter, str1, str2) <<"\n";
    //cout << "Forward-backward LCS space optimization: "<< avgExecutionTime(7, n_iter, str1, str2) <<"\n";

    //cout << "Diagonal Levenshtein: "<< avgExecutionTime(17, n_iter, str1, str2) <<"\n";
    //cout << "Diagonal Levenshtein Parallel: "<< avgExecutionTime(19, n_iter, str1, str2) <<"\n";
    
    //cout << "Forward Levenshtein: "<< avgExecutionTime(8, n_iter, str1, str2) <<"\n";
    //cout << "Forward Levenshtein space optimization: "<< avgExecutionTime(9, n_iter, str1, str2) <<"\n";
    //cout << "Backward Levenshtein: "<< avgExecutionTime(10, n_iter, str1, str2) <<"\n";
    //cout << "Backward Levenshtein space optimization: "<< avgExecutionTime(11, n_iter, str1, str2) <<"\n";
    //cout << "Diagonal Levenshtein: "<< avgExecutionTime(12, n_iter, str1, str2) <<"\n";
    //cout << "Forward-backward Levenshtein: "<< avgExecutionTime(13, n_iter, str1, str2) <<"\n";
    //cout << "Forward-backward Levenshtein space optimization: "<< avgExecutionTime(14, n_iter, str1, str2) <<"\n";

    return 0;
}