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
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++) 
            arr[i][j] = 0;

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
    temp1 = new int[column];
    current = new int[column];

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
    for (int i = 0; i < row+1; i++)
        for (int j = 0; j < column+1; j++) 
            arr[i][j] = 0;

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
    temp1 = new int[column];
    current = new int[column];

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

//anti-diagonal approach
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
            
            if (a != 0 && b != 0) 
                if (str1[a-1] == str2[b-1])
                    arr[a][b] = arr[a-1][b-1];
                else
                    arr[a][b] = 1 + min(arr[a][b-1], min(arr[a-1][b], arr[a-1][b-1]));
        }
        
        if(j >= (column-1)) {
            j = column - 2;
            i++;
        }
        else
            j++;
    }

    return arr[row-1][column-1];
}

void diagonal_levenshtein_thread (diag& a) {
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
            
            if (aa != 0 && b != 0) 
                if (a.s1[aa-1] == a.s2[b-1])
                    arr[aa][b] = arr[aa-1][b-1];
                else
                    arr[aa][b] = 1 + min(arr[aa][b-1], min(arr[aa-1][b], arr[aa-1][b-1]));
        }
        
        if(j >= (a.col-1)) {
            j = a.col - 2;
            i++;
        }
        else
            j++;

        //wait for all threads to finish working on current diagonal
        bar.arrive_and_wait();
    }
}

//anti-diagonal approach with parallelization
int diagonal_levenshtein_parallel (string str1, string str2, int row, int column) {
    //allocate
    arr = new unsigned short int*[row];
    for (int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];
        
    int n_threads = n_thr;

    thread threads[n_threads];
    diag diag_structs[n_threads];

    for(int i = 0; i < n_threads; i++) {
        diag_structs[i].s1 = str1;
        diag_structs[i].s2 = str2;
        diag_structs[i].row = row;
        diag_structs[i].col = column;
        diag_structs[i].id = i+1;
        diag_structs[i].n_total = n_threads;

        threads[i] = thread(diagonal_levenshtein_thread, ref(diag_structs[i]));
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    return arr[row-1][column-1];
}

//memory optimization: diagonals are now represented as rows, so that we only need to access previous two rows in the memory, as opposed to every row
int diagonal_levenshtein_memory_optimization(string str1, string str2, int row, int column) {
    int newRow = row + column - 1;

    //allocate
    arrMemory = new unsigned short int*[newRow];
    for (int i = 0; i < newRow; i++)
        if(i < row)
            arrMemory[i] = new unsigned short int[i+1];
        else
            arrMemory[i] = new unsigned short int[row + column - i - 1];

    //first row and column (row becomes a diagonal)
    for (int i = 0; i < row; i++)
        arrMemory[i][0] = i;
    for (int j = 1; j < column; j++)
        arrMemory[j][j] = j;

    //upper triangle
    for(int i = 2; i < column; i++) 
        for(int j = 1; j < i; j++) 
            if(str1[i-j-1] == str2[j-1])
                arrMemory[i][j] = arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = 1 + min(arrMemory[i-2][j-1], min(arrMemory[i-1][j-1], arrMemory[i-1][j]));

    //middle
    for(int i = column; i < row; i++) 
        for(int j = 1; j < column; j++) 
            if(str1[i-j-1] == str2[j-1])
                arrMemory[i][j] = arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = 1 + min(arrMemory[i-1][j], min(arrMemory[i-1][j-1], arrMemory[i-2][j-1]));

    for(int j = 0; j < column - 1; j++) 
        if(str1[row-j-2] == str2[j])
            arrMemory[row][j] = arrMemory[row-2][j];
        else
            arrMemory[row][j] = 1 + min(arrMemory[row-2][j], min(arrMemory[row-1][j+1], arrMemory[row-1][j]));
    
    //lower triangle
    for(int i = row+1; i < newRow; i++) 
        for(int j = 0; j < newRow - i; j++) 
            if(str1[row-j-2] == str2[j+i-row])
                arrMemory[i][j] = arrMemory[i-2][j+1];
            else
                arrMemory[i][j] = 1 + min(arrMemory[i-2][j+1], min(arrMemory[i-1][j+1], arrMemory[i-1][j]));

    return arrMemory[newRow-1][0]; 
}

//memory optimization: diagonals are now represented as rows, so that we only need to access previous two rows in the memory, as opposed to every row
int diagonal_levenshtein_memory_and_space_optimization(string str1, string str2, int row, int column) {
    int newRow = row + column - 1;

    temp1MS = new int[column];
    temp2MS = new int[column];
    currentMS = new int[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1MS[i] = 0;
        temp2MS[i] = 0;
        currentMS[i] = 0;
    }

    //upper triangle
    for (int i = 2; i < column; i++) {
        temp1MS[0] = i-2;
        temp1MS[i-2] = i-2;
        temp2MS[0] = i-1;
        temp2MS[i-1] = i-1;
        currentMS[0] = i;
        currentMS[i] = i;

        for(int j = 1; j < i; j++) 
            if(str1[i-j-1] == str2[j-1])
                currentMS[j] = temp1MS[j-1];
            else
                currentMS[j] = 1 + min(temp1MS[j-1], min(temp2MS[j-1], temp2MS[j]));

        //rewrite data
        for(int j = 0; j < column; j++) {
            temp1MS[j] = temp2MS[j];
            temp2MS[j] = currentMS[j];
        }
    }

    //middle
    for(int i = column; i < row; i++) {
        for(int j = 1; j < column; j++) 
            if(str1[i-j-1] == str2[j-1])
                currentMS[j] = temp1MS[j-1];
            else
                currentMS[j] = 1 + min(temp2MS[j], min(temp2MS[j-1], temp1MS[j-1]));

        //rewrite data
        for(int j = 0; j < column; j++) {
            temp1MS[j] = temp2MS[j];
            temp2MS[j] = currentMS[j];
        }
    }

    for(int j = 0; j < column - 1; j++) 
        if(str1[row-j-2] == str2[j])
            currentMS[j] = temp1MS[j];
        else
            currentMS[j] = 1 + min(temp1MS[j], min(temp2MS[j+1], temp2MS[j]));
    //rewrite data
    for(int j = 0; j < column; j++) {
        temp1MS[j] = temp2MS[j];
        temp2MS[j] = currentMS[j];
    }
    
    //lower triangle
    for(int i = row+1; i < newRow; i++) {
        for(int j = 0; j < newRow - i; j++) 
            if(str1[row-j-2] == str2[j+i-row])
                currentMS[j] = temp1MS[j+1];
            else
                currentMS[j] = 1 + min(temp1MS[j+1], min(temp2MS[j+1], temp2MS[j]));

        //rewrite data
        for(int j = 0; j < column; j++) {
            temp1MS[j] = temp2MS[j];
            temp2MS[j] = currentMS[j];
        }
    }

    return currentMS[0]; 
}

void diagonal_levenshtein_memory_optimization_thread (diag& a) {
    int newRow = a.row + a.col - 1;

    int chunkSize = ceil((double)(a.col)/a.n_total);
    int startIndex = a.id * chunkSize;

    //first row and column (row actually becomes a diagonal, because we tilt the array by 45degrees)
    for (int i = startIndex; i < a.row && i < startIndex+chunkSize; i++)
        arrMemory[i][0] = i;
    for (int j = startIndex+1; j < a.col && j <= startIndex+chunkSize; j++)
        arrMemory[j][j] = j;

    //upper triangle
    for(int i = 2; i < a.col; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < i && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                arrMemory[i][j] = arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = 1 + min(arrMemory[i-2][j-1], min(arrMemory[i-1][j-1], arrMemory[i-1][j]));
    }

    //middle
    for(int i = a.col; i < a.row; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < a.col && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                arrMemory[i][j] = arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = 1 + min(arrMemory[i-1][j], min(arrMemory[i-1][j-1], arrMemory[i-2][j-1]));
    }

    bar.arrive_and_wait();
    for(int j = startIndex; j < a.col - 1 && j < startIndex+chunkSize; j++) 
        if(a.s1[a.row-j-2] == a.s2[j])
            arrMemory[a.row][j] = arrMemory[a.row-2][j];
        else
            arrMemory[a.row][j] = 1 + min(arrMemory[a.row-2][j], min(arrMemory[a.row-1][j+1], arrMemory[a.row-1][j]));

    //lower triangle
    for(int i = a.row+1; i < newRow; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex; j < newRow - i && j < startIndex+chunkSize; j++) 
            if(a.s1[a.row-j-2] == a.s2[j+i-a.row])
                arrMemory[i][j] = arrMemory[i-2][j+1];
            else
                arrMemory[i][j] = 1 + min(arrMemory[i-2][j+1], min(arrMemory[i-1][j+1], arrMemory[i-1][j]));
    }
}

//memory optimization + parallelization
int diagonal_levenshtein_memory_optimization_parallel(string str1, string str2, int row, int column) {
    int newRow = row + column - 1;

    //allocate
    arrMemory = new unsigned short int*[newRow];

    //still needs to be checked for edge cases and whatnot
    for (int i = 0; i < newRow; i++) 
        if(i < row)
            arrMemory[i] = new unsigned short int[i+1];
        else
            arrMemory[i] = new unsigned short int[row + column - i - 1];

    int n_threads = n_thr;

    thread threads[n_threads];
    diag diag_structs[n_threads];

    for(int i = 0; i < n_threads; i++) {
        diag_structs[i].s1 = str1;
        diag_structs[i].s2 = str2;
        diag_structs[i].row = row;
        diag_structs[i].col = column;
        diag_structs[i].id = i;
        diag_structs[i].n_total = n_threads;

        threads[i] = thread(diagonal_levenshtein_memory_optimization_thread, ref(diag_structs[i]));
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    return arrMemory[newRow-1][0]; 
}

void diagonal_levenshtein_memory_and_space_optimization_thread (diag& a) {
    int newRow = a.row + a.col - 1;
    int chunkSize = ceil((double)(a.col)/a.n_total);
    int startIndex = a.id * chunkSize;

    //upper triangle
    for(int i = 2; i < a.col; i++) {
        if(a.id == 1) {
            diagTemp1[0] = i-2;
            diagTemp1[i-2] = i-2;
            diagTemp2[0] = i-1;
            diagTemp2[i-1] = i-1;
            diagCurrent[0] = i;
            diagCurrent[i] = i;
        }

        bar.arrive_and_wait();
        for(int j = startIndex+1; j < i && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                diagCurrent[j] = diagTemp1[j-1];
            else
                diagCurrent[j] = 1 + min(diagTemp1[j-1], min(diagTemp2[j-1], diagTemp2[j]));
        
        bar.arrive_and_wait();
        //rewrite data
        for(int j = startIndex; j < a.col && j < startIndex+chunkSize; j++) {
            diagTemp1[j] = diagTemp2[j];
            diagTemp2[j] = diagCurrent[j];
        }
    }

    //middle
    for(int i = a.col; i < a.row; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < a.col && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                diagCurrent[j] = diagTemp1[j-1];
            else
                diagCurrent[j] = 1 + min(diagTemp2[j], min(diagTemp2[j-1], diagTemp1[j-1]));

        bar.arrive_and_wait();
        //rewrite data
        for(int j = startIndex; j < a.col && j < startIndex+chunkSize; j++) {
            diagTemp1[j] = diagTemp2[j];
            diagTemp2[j] = diagCurrent[j];
        }
    }

    bar.arrive_and_wait();
    for(int j = startIndex; j < a.col - 1 && j < startIndex+chunkSize; j++) 
        if(a.s1[a.row-j-2] == a.s2[j])
            diagCurrent[j] = diagTemp1[j];
        else
            diagCurrent[j] = 1 + min(diagTemp1[j], min(diagTemp2[j+1], diagTemp2[j]));
    bar.arrive_and_wait();
    //rewrite data
    for(int j = startIndex; j < a.col && j < startIndex+chunkSize; j++) {
        diagTemp1[j] = diagTemp2[j];
        diagTemp2[j] = diagCurrent[j];
    }
    
    //lower triangle
    for(int i = a.row+1; i < newRow; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex; j < newRow - i && j < startIndex+chunkSize; j++) 
            if(a.s1[a.row-j-2] == a.s2[j+i-a.row])
                diagCurrent[j] = diagTemp1[j+1];
            else
                diagCurrent[j] = 1 + min(diagTemp1[j+1], min(diagTemp2[j+1], diagTemp2[j]));

        bar.arrive_and_wait();
        //rewrite data
        for(int j = startIndex; j < a.col && j < startIndex+chunkSize; j++) {
            diagTemp1[j] = diagTemp2[j];
            diagTemp2[j] = diagCurrent[j];
        }
    }

    bar.arrive_and_wait();
    arrDiag = diagCurrent[0];
}

//memory optimization + parallelization
int diagonal_levenshtein_memory_and_space_optimization_parallel(string str1, string str2, int row, int column) {
    int newRow = row + column - 1;

    int n_threads = n_thr;

    diagTemp1 = new int[column];
    diagTemp2 = new int[column];
    diagCurrent = new int[column];

    for(int i = 0; i < column; i++) {
        diagTemp1[i] = 0;
        diagTemp2[i] = 0;
        diagCurrent[i] = 0;
    }

    thread threads[n_threads];
    diag diag_structs[n_threads];

    for(int i = 0; i < n_threads; i++) {
        diag_structs[i].s1 = str1;
        diag_structs[i].s2 = str2;
        diag_structs[i].row = row;
        diag_structs[i].col = column;
        diag_structs[i].id = i;
        diag_structs[i].n_total = n_threads;

        threads[i] = thread(diagonal_levenshtein_memory_and_space_optimization_thread, ref(diag_structs[i]));
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    return arrDiag; 
}

//diagonal parallelization using the OpenMP library
int diagonal_levenshtein_parallel_openmp (string str1, string str2, int row, int column) {
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
        #pragma omp parallel for num_threads (n_thr)
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

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    for(int i = nrows-1; i >= a.row; i--) 
        for (int j = a.col-1; j >= 0; j--)     
            if (i == nrows-1)
                arr[i+1][j+1] = a.col-j-1;
            else if (j == a.col-1)
                arr[i+1][j+1] = nrows-i-1;
            else if (a.s1[i] == a.s2[j])
                arr[i+1][j+1] = arr[i+2][j+2]; 
            else 
                arr[i+1][j+1] = 1 + min(arr[i+1][j+2], min(arr[i+2][j+1], arr[i+2][j+2]));
}

//merges last rows of topHalf_levenshtein and bottomHalf_levenshtein
int merge_levenshtein (int h, int row, int column, string str1, string str2) {
    int temp = 0;
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
    //initialize zeros
    for (int i = 0; i <= a.col; i++) {
        temp1Top[i] = 0;
        currentTop[i] = 0;
    }

    for(int i = 0; i <= a.row; i++) {
        for(int j = 0; j < a.col; j++) 
            if (i == 0)
                currentTop[j] = j;
            else if (j == 0)
                currentTop[j] = i;
            else if (a.s1[i-1] == a.s2[j-1])
                currentTop[j] = temp1Top[j-1];
            else 
                currentTop[j] = 1 + min(currentTop[j-1], min(temp1Top[j], temp1Top[j-1]));

        //rewrite data
        for(int j = 0; j < a.col+1; j++) 
            temp1Top[j] = currentTop[j];
    }
}

//space optimization: we only keep current two rows in memory
void bottomHalf_levenshtein_space_optimization(args& a) {
    //initialize zeros
    for (int i = 0; i < a.col+1; i++) {
        temp1Bottom[i] = 0;
        currentBottom[i] = 0;
    }

    int nrows = a.s1.length()+1;

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    for(int i = nrows-1; i >= a.row; i--) {
        for (int j = a.col-1; j >= 0; j--) 
            if (i == nrows-1)
                currentBottom[j+1] = a.col-j-1;
            else if (j == a.col-1)
                currentBottom[j+1] = nrows-i-1;
            else if (a.s1[i] == a.s2[j])
                currentBottom[j+1] = temp1Bottom[j+2];
            else 
                currentBottom[j+1] = 1 + min(currentBottom[j+2], min(temp1Bottom[j+1], temp1Bottom[j+2])); 

        //rewrite data
        for(int j = 0; j < a.col+1; j++) 
            temp1Bottom[j] = currentBottom[j]; 
    }
}

//merges topHalf_levenshtein_space_optimization and bottomHalf_levenshtein_space_optimization
int merge_levenshtein_space_optimization (int column) {
    int temp = 0;
    int temp2 = 0;
    int currentMin = INT_MAX;

    for(int i = 1; i <= column; i++) {
        temp = currentTop[i-1] + currentBottom[i];

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
    int ret = merge_levenshtein(h, row, column, str1, str2);

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

    temp1Top = new int[column+1];
    temp1Bottom = new int[column+1];
    currentTop = new int[column+1];
    currentBottom = new int[column+1];

    thread t1(topHalf_levenshtein_space_optimization, ref(a));
    thread t2(bottomHalf_levenshtein_space_optimization, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the distance
    int ret = merge_levenshtein_space_optimization(column);

    return ret;
}
