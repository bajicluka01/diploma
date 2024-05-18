#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
using namespace std;
using namespace std::chrono;

//dynamic programming solution
int sequentialLCS (string str1, string str2, int row, int column) {

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

    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (i == 0 || j == 0 )
                arr[i][j] = 0;
            else if (str1[i-1] == str2 [j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else
                arr[i][j] = max(arr[i-1][j], arr[i][j-1]);
        }
    }

    int ret = arr[row-1][column-1];

    //deallocate
    for(int i = 0; i < row; i++)
        delete[] arr[i];

    return ret;
}

//anti-diagonal parallelization
int parallelLCS (string str1, string str2, int row, int column) {

    //allocate
    int** arr = new int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new int[column];

    //initialize zeros
    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++) {
            arr[i][j] = 0;
        }
    }

    for (int i = 0, j = 0; i <= str2.length() && j <= str2.length(); j++) {

        int antidiag = min(j, (int)str1.length()-i);

        #pragma omp parallel for
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
        
        if(j >= str2.length()) {
            j = str2.length() - 1;
            i++;
        }

    }

    int ret = arr[row-1][column-1];

    //deallocate
    for (int i = 0; i < row; i++)
        delete[] arr[i];

    return ret;

}


string randomStringGenerator (int len) {
    string str = "";
    for(int i = 0; i < len; i++)
        str += 'a' + rand()%26;
    return str;
}


int main(int argc, char* argv[]){
    string str1 = "abcde";
    string str2 = "axbxcx";

    str1 = randomStringGenerator(10000);
    str2 = randomStringGenerator(10000);

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