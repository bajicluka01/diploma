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

    int* temp1Pointer = temp1;
    int* currentPointer = current;
    int c = 0; 
    int currentSolution = 0;

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
            if (i == 0)
                currentPointer[j] = j;
            else if (j == 0)
                currentPointer[j] = i;
            else if (str1[i-1] == str2[j-1])
                currentPointer[j] = temp1Pointer[j-1];
            else 
                currentPointer[j] = 1 + min(currentPointer[j-1], min(temp1Pointer[j], temp1Pointer[j-1]));
        }
        currentSolution = currentPointer[column-1];

        //rewrite data
        c = (c+1) % 2;
        if(c != 0) {
            temp1Pointer = current;
            currentPointer = temp1;
        }
        else {
            temp1Pointer = temp1;
            currentPointer = current;
        }
    }

    return currentSolution;
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

    int* temp1Pointer = temp1;
    int* currentPointer = current;
    int c = 0;
    int currentSolution = 0;

    for(int i = row-1; i >= 0; i--) {
        for(int j = column-1; j >= 0; j--) {
            if (i == row-1)
                currentPointer[j] = column-j-1;
            else if (j == column-1)
                currentPointer[j] = row-i-1;
            else if(str1[i] == str2[j])
                currentPointer[j] = temp1Pointer[j+1];
            else
                currentPointer[j] = 1 + min(currentPointer[j+1], min(temp1Pointer[j], temp1Pointer[j+1]));
        }
        currentSolution = currentPointer[0];

        //rewrite data
        c = (c+1) % 2;
        if(c == 0) {
            temp1Pointer = temp1;
            currentPointer = current;
        }
        else {
            temp1Pointer = current;
            currentPointer = temp1;
        }
    }

    return currentSolution;
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

    int* temp1MSpointer = temp1MS;
    int* temp2MSpointer = temp2MS;
    int* currentMSpointer = currentMS;
    int c = 0; 

    //upper triangle
    for (int i = 2; i < column; i++) {
        temp1MSpointer[0] = i-2;
        temp1MSpointer[i-2] = i-2;
        temp2MSpointer[0] = i-1;
        temp2MSpointer[i-1] = i-1;
        currentMSpointer[0] = i;
        currentMSpointer[i] = i;

        for(int j = 1; j < i; j++) 
            if(str1[i-j-1] == str2[j-1])
                currentMSpointer[j] = temp1MSpointer[j-1];
            else
                currentMSpointer[j] = 1 + min(temp1MSpointer[j-1], min(temp2MSpointer[j-1], temp2MSpointer[j]));

        //rewrite data
        c = (c+1) % 3;
        if(c == 0) {
            temp1MSpointer = temp1MS;
            temp2MSpointer = temp2MS;
            currentMSpointer = currentMS;
        } else if (c == 1) {
            temp1MSpointer = temp2MS;
            temp2MSpointer = currentMS;
            currentMSpointer = temp1MS;
        } else {
            temp1MSpointer = currentMS;
            temp2MSpointer = temp1MS;
            currentMSpointer = temp2MS;
        }
    }

    //middle
    for(int i = column; i < row; i++) {
        for(int j = 1; j < column; j++) 
            if(str1[i-j-1] == str2[j-1])
                currentMSpointer[j] = temp1MSpointer[j-1];
            else
                currentMSpointer[j] = 1 + min(temp2MSpointer[j], min(temp2MSpointer[j-1], temp1MSpointer[j-1]));

        //rewrite data
        c = (c+1) % 3;
        if(c == 0) {
            temp1MSpointer = temp1MS;
            temp2MSpointer = temp2MS;
            currentMSpointer = currentMS;
        } else if (c == 1) {
            temp1MSpointer = temp2MS;
            temp2MSpointer = currentMS;
            currentMSpointer = temp1MS;
        } else {
            temp1MSpointer = currentMS;
            temp2MSpointer = temp1MS;
            currentMSpointer = temp2MS;
        }
    }

    for(int j = 0; j < column - 1; j++) 
        if(str1[row-j-2] == str2[j])
            currentMSpointer[j] = temp1MSpointer[j];
        else
            currentMSpointer[j] = 1 + min(temp1MSpointer[j], min(temp2MSpointer[j+1], temp2MSpointer[j]));
    //rewrite data
    c = (c+1) % 3;
    if(c == 0) {
        temp1MSpointer = temp1MS;
        temp2MSpointer = temp2MS;
        currentMSpointer = currentMS;
    } else if (c == 1) {
        temp1MSpointer = temp2MS;
        temp2MSpointer = currentMS;
        currentMSpointer = temp1MS;
    } else {
        temp1MSpointer = currentMS;
        temp2MSpointer = temp1MS;
        currentMSpointer = temp2MS;
    }
    
    //lower triangle
    for(int i = row+1; i < newRow; i++) {
        for(int j = 0; j < newRow - i; j++) 
            if(str1[row-j-2] == str2[j+i-row])
                currentMSpointer[j] = temp1MSpointer[j+1];
            else
                currentMSpointer[j] = 1 + min(temp1MSpointer[j+1], min(temp2MSpointer[j+1], temp2MSpointer[j]));

        //rewrite data
        c = (c+1) % 3;
        if(c == 0) {
            temp1MSpointer = temp1MS;
            temp2MSpointer = temp2MS;
            currentMSpointer = currentMS;
        } else if (c == 1) {
            temp1MSpointer = temp2MS;
            temp2MSpointer = currentMS;
            currentMSpointer = temp1MS;
        } else {
            temp1MSpointer = currentMS;
            temp2MSpointer = temp1MS;
            currentMSpointer = temp2MS;
        }
    }

    if(c == 0)
        return currentMSpointer[0]; 
    else if (c == 1)
        return temp2MSpointer[0];
    else
        return temp1MSpointer[0]; 
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

    diagTemp1Pointer = diagTemp1;
    diagTemp2Pointer = diagTemp2;
    diagCurrentPointer = diagCurrent;
    int c = 0; 

    //upper triangle
    for(int i = 2; i < a.col; i++) {
        if(a.id == 1) {
            diagTemp1Pointer[0] = i-2;
            diagTemp1Pointer[i-2] = i-2;
            diagTemp2Pointer[0] = i-1;
            diagTemp2Pointer[i-1] = i-1;
            diagCurrentPointer[0] = i;
            diagCurrentPointer[i] = i;
        }

        bar.arrive_and_wait();
        for(int j = startIndex+1; j < i && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                diagCurrentPointer[j] = diagTemp1Pointer[j-1];
            else
                diagCurrentPointer[j] = 1 + min(diagTemp1Pointer[j-1], min(diagTemp2Pointer[j-1], diagTemp2Pointer[j]));
        
        bar.arrive_and_wait();
        //rewrite data
        if(a.id == 0) {
            c = (c+1) % 3;
            if(c == 0) {
                diagTemp1Pointer = diagTemp1;
                diagTemp2Pointer = diagTemp2;
                diagCurrentPointer = diagCurrent;
            } else if (c == 1) {
                diagTemp1Pointer = diagTemp2;
                diagTemp2Pointer = diagCurrent;
                diagCurrentPointer = diagTemp1;
            } else {
                diagTemp1Pointer = diagCurrent;
                diagTemp2Pointer = diagTemp1;
                diagCurrentPointer = diagTemp2;
            }
        }
    }

    //middle
    for(int i = a.col; i < a.row; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < a.col && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                diagCurrentPointer[j] = diagTemp1Pointer[j-1];
            else
                diagCurrentPointer[j] = 1 + min(diagTemp2Pointer[j], min(diagTemp2Pointer[j-1], diagTemp1Pointer[j-1]));

        bar.arrive_and_wait();
        //rewrite data
        if(a.id == 0) {
            c = (c+1) % 3;
            if(c == 0) {
                diagTemp1Pointer = diagTemp1;
                diagTemp2Pointer = diagTemp2;
                diagCurrentPointer = diagCurrent;
            } else if (c == 1) {
                diagTemp1Pointer = diagTemp2;
                diagTemp2Pointer = diagCurrent;
                diagCurrentPointer = diagTemp1;
            } else {
                diagTemp1Pointer = diagCurrent;
                diagTemp2Pointer = diagTemp1;
                diagCurrentPointer = diagTemp2;
            }
        }
    }

    bar.arrive_and_wait();
    for(int j = startIndex; j < a.col - 1 && j < startIndex+chunkSize; j++) 
        if(a.s1[a.row-j-2] == a.s2[j])
            diagCurrentPointer[j] = diagTemp1Pointer[j];
        else
            diagCurrentPointer[j] = 1 + min(diagTemp1Pointer[j], min(diagTemp2Pointer[j+1], diagTemp2Pointer[j]));
    bar.arrive_and_wait();
    //rewrite data
    if(a.id == 0) {
        c = (c+1) % 3;
        if(c == 0) {
            diagTemp1Pointer = diagTemp1;
            diagTemp2Pointer = diagTemp2;
            diagCurrentPointer = diagCurrent;
        } else if (c == 1) {
            diagTemp1Pointer = diagTemp2;
            diagTemp2Pointer = diagCurrent;
            diagCurrentPointer = diagTemp1;
        } else {
            diagTemp1Pointer = diagCurrent;
            diagTemp2Pointer = diagTemp1;
            diagCurrentPointer = diagTemp2;
        }
    }
    
    //lower triangle
    for(int i = a.row+1; i < newRow; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex; j < newRow - i && j < startIndex+chunkSize; j++) 
            if(a.s1[a.row-j-2] == a.s2[j+i-a.row])
                diagCurrentPointer[j] = diagTemp1Pointer[j+1];
            else
                diagCurrentPointer[j] = 1 + min(diagTemp1Pointer[j+1], min(diagTemp2Pointer[j+1], diagTemp2Pointer[j]));

        bar.arrive_and_wait();
        //rewrite data
        if(a.id == 0) {
        c = (c+1) % 3;
            if(c == 0) {
                diagTemp1Pointer = diagTemp1;
                diagTemp2Pointer = diagTemp2;
                diagCurrentPointer = diagCurrent;
            } else if (c == 1) {
                diagTemp1Pointer = diagTemp2;
                diagTemp2Pointer = diagCurrent;
                diagCurrentPointer = diagTemp1;
            } else {
                diagTemp1Pointer = diagCurrent;
                diagTemp2Pointer = diagTemp1;
                diagCurrentPointer = diagTemp2;
            }
        }
    }

    bar.arrive_and_wait();
    if(a.id == 0)
        if(c == 0)
            arrDiag = diagCurrentPointer[0]; 
        else if (c == 1)
            arrDiag = diagTemp2Pointer[0];
        else
            arrDiag = diagTemp1Pointer[0];
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

    int* temp1TopPointer = temp1Top;
    int* currentTopPointer = currentTop;
    int c = 0;
    int* currentSolution; 

    for(int i = 0; i <= a.row; i++) {
        for(int j = 0; j < a.col; j++) 
            if (i == 0)
                currentTopPointer[j] = j;
            else if (j == 0)
                currentTopPointer[j] = i;
            else if (a.s1[i-1] == a.s2[j-1])
                currentTopPointer[j] = temp1TopPointer[j-1];
            else 
                currentTopPointer[j] = 1 + min(currentTopPointer[j-1], min(temp1TopPointer[j], temp1TopPointer[j-1]));
        currentSolution = currentTopPointer;

        //rewrite data
        c = (c+1) % 2;
        if(c != 0) {
            temp1TopPointer = currentTop;
            currentTopPointer = temp1Top;
        }
        else {
            temp1TopPointer = temp1Top;
            currentTopPointer = currentTop;
        }
    }

    currentTop = currentSolution;
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

    int* temp1BottomPointer = temp1Bottom;
    int* currentBottomPointer = currentBottom;
    int c = 0;
    int* currentSolution;

    for(int i = nrows-1; i >= a.row; i--) {
        for (int j = a.col-1; j >= 0; j--) 
            if (i == nrows-1)
                currentBottomPointer[j+1] = a.col-j-1;
            else if (j == a.col-1)
                currentBottomPointer[j+1] = nrows-i-1;
            else if (a.s1[i] == a.s2[j])
                currentBottomPointer[j+1] = temp1BottomPointer[j+2];
            else 
                currentBottomPointer[j+1] = 1 + min(currentBottomPointer[j+2], min(temp1BottomPointer[j+1], temp1BottomPointer[j+2])); 
        currentSolution = currentBottomPointer;

        //rewrite data
        c = (c+1) % 2;
        if(c != 0) {
            temp1BottomPointer = currentBottom;
            currentBottomPointer = temp1Bottom;
        }
        else {
            temp1BottomPointer = temp1Bottom;
            currentBottomPointer = currentBottom;
        }
    }

    currentBottom = currentSolution;
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
