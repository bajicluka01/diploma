#include <barrier>
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

    return arr[0][0];
}

//space optimization: we only keep current two rows in memory
int backward_levenshtein_space_optimization (string str1, string str2, int row, int column) {
    int temp1[column]; 
    int current[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1[i] = 0;
        current[i] = 0;
    }

    for(int i = row-1; i >= 0; i--) {
        for(int j = column-1; j >= 0; j--) {
            if (i == row-1)
                current[j] = column-j-1;
            else if (j == column-1)
                current[j] = row-i-1;
            else if(str1[i] == str2[j])
                current[j] = temp1[j+1];
            else
                current[j] = 1 + min(current[j+1], min(temp1[j], temp1[j+1]));
        }

        //rewrite data
        for(int j = 0; j < column; j++) 
            temp1[j] = current[j];

    }

    return current[0];
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

        for (int k = 0; k <= antidiag; ++k){
            int a = i + k;
            int b = j - k;

            if (a == 0)
                arr[a][b] = b;
            if (b == 0)
                arr[a][b] = a;
            
            if (a != 0 && b != 0) {
                if (str1[a-1] == str2[b-1])
                    arr[a][b] = arr[a-1][b-1];
                else
                    arr[a][b] = 1 + min(arr[a][b-1], min(arr[a-1][b], arr[a-1][b-1]));
            }
        }
        
        if(j >= (column-1)) {
            j = column - 2;
            i++;
        }
        else
            j++;

    }

    /*cout<<"\n";
    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) 
            cout<<arr[i][j]<<" ";
        cout<<"\n";
    }*/

    return arr[row-1][column-1];

}

void diagonal_thread (diag& a) {
    int i = 0;
    int j = 0;

    //iterating through diagonals
    while(i <= a.row-1 && j <= a.col-1) {

        int current_elements = min(j, a.row-i-1);

        for (int k = a.id-1; k <= current_elements; k+=a.n_total-1) {
            int aa = i + k;
            int b = j - k;

            if (aa == 0)
                arr[aa][b] = b;
            if (b == 0)
                arr[aa][b] = aa;
            
            if (aa != 0 && b != 0) {
                if (a.s1[aa-1] == a.s2[b-1])
                    arr[aa][b] = arr[aa-1][b-1];
                else
                    arr[aa][b] = 1 + min(arr[aa][b-1], min(arr[aa-1][b], arr[aa-1][b-1]));
            }
        }
        
        if(j >= (a.col-1)) {
            j = a.col - 2;
            i++;
        }
        else
            j++;

        //wait for all threads to finish woring on current diagonal
        bar.arrive_and_wait();

    }
}

//anti-diagonal approach with parallelization
int diagonal_levenshtein_test (string str1, string str2, int row, int column) {
    
    //allocate
    arr = new unsigned short int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];

     //initialize zeros
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++) 
            arr[i][j] = 0;
        
    int n_threads = n_thr;

    thread threads[n_threads];
    diag diag_structs[n_threads];

    //bar = barrier(n_threads);

    for(int i = 0; i < n_threads; i++) {

        diag_structs[i].s1 = str1;
        diag_structs[i].s2 = str2;
        diag_structs[i].row = row;
        diag_structs[i].col = column;
        diag_structs[i].id = i+1;
        diag_structs[i].n_total = n_threads;

        threads[i] = thread(diagonal_thread, ref(diag_structs[i]));
        
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    /*cout<<"\n";
    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) 
            cout<<arr[i][j]<<" ";
        cout<<"\n";
    }*/

    return arr[row-1][column-1];

}

//anti-diagonal approach with parallelization
int diagonal_levenshtein_parallel (string str1, string str2, int row, int column) {
    
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
        #pragma omp parallel for num_threads (6)
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

    /*cout<<"\n";
    for(int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) 
            cout<<arr[i][j]<<" ";
        cout<<"\n";
    }*/

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

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

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

//merges last rows of topHalf_levenshtein and bottomHalf_levenshtein
int merge_levenshtein (int h, int row, int column) {
    int temp = 0;
    int temp2 = 0;
    int currentMin = INT_MAX;

    for(int i = 1; i <= column; i++) {
        temp = arr[h][i-1] + arr[h+1][i];

        if(temp < currentMin)
            currentMin = temp;

    }

    return currentMin;
}

//space optimization: we only keep current two rows in memory
void topHalf_levenshtein_space_optimization(args& a) {
    int temp1[a.col+1]; 
    int current[a.col+1];
    

    //initialize zeros
    for (int i = 0; i <= a.col; i++) {
        temp1[i] = 0;
        current[i] = 0;
        arrTop[i] = 0;
    }

    for(int i = 0; i <= a.row; i++) {
        for(int j = 0; j < a.col; j++) {
            if (i == 0)
                current[j] = j;
            else if (j == 0)
                current[j] = i;
            else if (a.s1[i-1] == a.s2[j-1])
                current[j] = temp1[j-1];
            else 
                current[j] = 1 + min(current[j-1], min(temp1[j], temp1[j-1]));
        }

        //rewrite data
        for(int j = 0; j < a.col+1; j++) 
            temp1[j] = current[j];
        
    }

    for(int i = 0; i < a.col+1; i++) 
        arrTop[i] = current[i];
}

//space optimization: we only keep current two rows in memory
void bottomHalf_levenshtein_space_optimization(args& a) {
    int temp1[a.col+1]; 
    int current[a.col+1];
    

    //initialize zeros
    for (int i = 0; i < a.col+1; i++) {
        temp1[i] = 0;
        current[i] = 0;
        arrBottom[i] = 0;
    }

    int nrows = a.s1.length()+1;

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    for(int i = nrows-1; i >= a.row; i--) {
        for (int j = a.col-1; j >= 0; j--) {   
            if (i == nrows-1)
                current[j+1] = a.col-j-1;
            else if (j == a.col-1)
                current[j+1] = nrows-i-1;
            else if (a.s1[i] == a.s2[j])
                current[j+1] = temp1[j+2];
            else 
                current[j+1] = 1 + min(current[j+2], min(temp1[j+1], temp1[j+2])); 
        }

        //rewrite data
        for(int j = 0; j < a.col+1; j++) 
            temp1[j] = current[j];
        
    }

    for(int i = 0; i < a.col+1; i++) 
        arrBottom[i] = current[i];
    
}

//merges topHalf_levenshtein_space_optimization and bottomHalf_levenshtein_space_optimization
int merge_levenshtein_space_optimization (int column) {
    int temp = 0;
    int temp2 = 0;
    int currentMin = INT_MAX;

    for(int i = 1; i <= column; i++) {
        temp = arrTop[i-1] + arrBottom[i];

        if(temp < currentMin)
            currentMin = temp;

    }

    return currentMin;
}

//forward-backward approach with two threads
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

//forward-backward approach with two threads and space optimization
int fb_levenshtein_space_optimization (string str1, string str2, int row, int column) {
        
    int h = row / 2;
    struct args a; 
    a.s1 = str1;
    a.s2 = str2;
    a.row = h;
    a.col = column;

    arrTop = new int[column+1];
    arrBottom = new int[column+1];

    thread t1(topHalf_levenshtein_space_optimization, ref(a));
    thread t2(bottomHalf_levenshtein_space_optimization, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the distance
    int ret = merge_levenshtein_space_optimization(column);

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
