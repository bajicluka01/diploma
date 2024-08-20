#include <stdio.h>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <thread>
#include <barrier>
using namespace std;
using namespace std::chrono;

//structs used for passing data to thread functions
struct args {
    string s1;
    string s2;
    int row;
    int col;
};

struct diag {
    string s1;
    string s2;
    int row;
    int col;
    int id;
    int n_total;
};

//TODO fix bug: infinite loop (?) for n_thr = 1
int n_thr = 6;
barrier bar { n_thr };

unsigned short int** arr;

//for diagonal memory optimization
unsigned short int** arrMemory; 

//for diagonal space optimization
int arrDiag;

int* diagTemp1;
int* diagTemp2;
int* diagCurrent;
int* diagTemp1Pointer;
int* diagTemp2Pointer;
int* diagCurrentPointer;

//all of these variables could've been local, but for some reason I get segmentation fault for strLen = cca. 200000 or higher; making them global seems to fix the problem
int* temp1MS;
int* temp2MS;
int* currentMS;

int* temp1Top;
int* temp1Bottom;
int* currentTop;
int* currentBottom;

int* temp1;
int* current;

//basic dynamic programming solution
int forward_LCS (string str1, string str2, int row, int column) {
    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++) 
            arr[i][j] = 0;

    for(int i = 0; i < row; i++) 
        for (int j = 0; j < column; j++) 
            if (i == 0 || j == 0)
                arr[i][j] = 0;
            else if (str1[i-1] == str2[j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else
                arr[i][j] = max(arr[i-1][j], arr[i][j-1]);

    return arr[row-1][column-1];
}

//space optimization: we only keep current two rows in memory
int forward_LCS_space_optimization (string str1, string str2, int row, int column) {
    temp1 = new int[column];
    current = new int[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1[i] = 0;
        current[i] = 0;
    }

    int* temp1Pointer = temp1;
    int* currentPointer = current;
    bool b = false;
    int currentSolution = 0;

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
            if (i == 0 || j == 0)
                currentPointer[j] = 0;
            else if (str1[i-1] == str2[j-1])
                currentPointer[j] = 1 + temp1Pointer[j-1];
            else
                currentPointer[j] = max(currentPointer[j-1], temp1Pointer[j]);
        }
        currentSolution = currentPointer[column-1];

        //rewrite data
        b = !b;
        temp1Pointer = b ? temp1 : current;
        currentPointer = b ? current : temp1;
    }

    return currentSolution;
}

//basic dynamic programming solution backward
int backward_LCS (string str1, string str2, int row, int column) {
    //allocate
    arr = new unsigned short int*[row];
    for(int i = 0; i < row+1; i++)
        arr[i] = new unsigned short int[column];

    //initialize zeros
    for (int i = 0; i < row+1; i++)
        for (int j = 0; j < column+1; j++) 
            arr[i][j] = 0;

    for(int i = row-1; i >= 0; i--) 
        for(int j = column-1; j >= 0; j--) 
            if (i == row-1 || j == column-1)
                arr[i][j] = 0;
            else if(str1[i] == str2[j])
                arr[i][j] = 1 + arr[i+1][j+1];
            else
                arr[i][j] = max(arr[i][j+1], arr[i+1][j]);

    return arr[0][0];
}

//space optimization: we only keep current two rows in memory
int backward_LCS_space_optimization (string str1, string str2, int row, int column) {
    temp1 = new int[column];
    current = new int[column];

    //initialize zeros
    for (int i = 0; i < column; i++) {
        temp1[i] = 0;
        current[i] = 0;
    }

    int* temp1Pointer = temp1;
    int* currentPointer = current;
    bool b = false;
    int currentSolution = 0;

    for(int i = row-1; i >= 0; i--) {
        for(int j = column-1; j >= 0; j--) {
            if (i == row-1 || j == column-1)
                currentPointer[j] = 0;
            else if(str1[i] == str2[j])
                currentPointer[j] = 1 + temp1Pointer[j+1];
            else
                currentPointer[j] = max(currentPointer[j+1], temp1Pointer[j]);
        }
        currentSolution = currentPointer[0];

        //rewrite data
        b = !b;
        temp1Pointer = b ? temp1 : current;
        currentPointer = b ? current : temp1;
    }

    return currentSolution;
}

//iterate through diagonals instead of rows
int diagonal_LCS (string str1, string str2, int row, int column) {
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
    //iterate through diagonals
    while(i <= row-1 && j <= column-1) {
        int antidiag = min(j, row-i-1);

        //calculate elements on the current diagonal
        for (int k = 0; k <= antidiag; k++) {
            int a = i + k;
            int b = j - k;

            if (a == 0 || b == 0)
                arr[a][b] = 0;
            else if (str1[a-1] == str2[b-1])
                arr[a][b] = 1 + arr[a-1][b-1];
            else
                arr[a][b] = max (arr[a-1][b], arr[a][b-1]);
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

//function for diagonal_LCS_parallel
void diagonal_LCS_thread (diag& a) {
    int i = 0;
    int j = 0;

    //iterating through diagonals
    while(i <= a.row-1 && j <= a.col-1) {
        int current_elements = min(j, a.row-i-1);

        //calculate elements on current diagonal
        for (int k = a.id-1; k <= current_elements; k+=a.n_total-1) {
            int aa = i + k;
            int b = j - k;

            if (aa == 0 || b == 0)
                arr[aa][b] = 0;
            else if (a.s1[aa-1] == a.s2[b-1])
                arr[aa][b] = 1 + arr[aa-1][b-1];
            else
                arr[aa][b] = max (arr[aa-1][b], arr[aa][b-1]);
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

//diagonal approach with parallelization
int diagonal_LCS_parallel (string str1, string str2, int row, int column) {
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

        threads[i] = thread(diagonal_LCS_thread, ref(diag_structs[i]));
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    return arr[row-1][column-1];
}

//memory optimization: diagonals are now represented as rows, so that we only need to access previous two rows in the memory, as opposed to every row
int diagonal_LCS_memory_optimization(string str1, string str2, int row, int column) {
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
        arrMemory[i][0] = 0;
    for (int j = 1; j < column; j++)
        arrMemory[j][j] = 0;

    //upper triangle
    for(int i = 2; i < column; i++) 
        for(int j = 1; j < i; j++) 
            if(str1[i-j-1] == str2[j-1])
                arrMemory[i][j] = 1 + arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = max(arrMemory[i-1][j-1], arrMemory[i-1][j]);

    //middle
    for(int i = column; i < row; i++) 
        for(int j = 1; j < column; j++) 
            if(str1[i-j-1] == str2[j-1])
                arrMemory[i][j] = 1 + arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = max(arrMemory[i-1][j], arrMemory[i-1][j-1]);
    for(int j = 0; j < column - 1; j++) 
        if(str1[row-j-2] == str2[j])
            arrMemory[row][j] = 1 + arrMemory[row-2][j];
        else
            arrMemory[row][j] = max(arrMemory[row-1][j+1], arrMemory[row-1][j]);
    
    //lower triangle
    for(int i = row+1; i < newRow; i++) 
        for(int j = 0; j < newRow - i; j++) 
            if(str1[row-j-2] == str2[j+i-row])
                arrMemory[i][j] = 1 + arrMemory[i-2][j+1];
            else
                arrMemory[i][j] = max(arrMemory[i-1][j+1], arrMemory[i-1][j]);

    return arrMemory[newRow-1][0]; 
}

//function for diagonal_LCS_memory_optimization_parallel
void diagonal_LCS_memory_optimization_thread (diag& a) {
    int newRow = a.row + a.col - 1;

    int chunkSize = ceil((double)(a.col)/a.n_total);
    int startIndex = a.id * chunkSize;

    //first row and column (row actually becomes a diagonal, because we tilt the array by 45degrees)
    for (int i = startIndex; i < a.row && i <= startIndex+chunkSize; i++)
        arrMemory[i][0] = 0;
    for (int j = startIndex+1; j < a.col && j <= startIndex+chunkSize; j++)
        arrMemory[j][j] = 0;

    //upper triangle
    for(int i = 2; i < a.col; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < i && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                arrMemory[i][j] = 1 + arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = max(arrMemory[i-1][j-1], arrMemory[i-1][j]);
    }

    //middle
    for(int i = a.col; i < a.row; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < a.col && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                arrMemory[i][j] = 1 + arrMemory[i-2][j-1];
            else
                arrMemory[i][j] = max(arrMemory[i-1][j], arrMemory[i-1][j-1]);
    }
    bar.arrive_and_wait();
    for(int j = startIndex; j < a.col - 1 && j < startIndex+chunkSize; j++) 
        if(a.s1[a.row-j-2] == a.s2[j])
            arrMemory[a.row][j] = 1 + arrMemory[a.row-2][j];
        else
            arrMemory[a.row][j] = max(arrMemory[a.row-1][j+1], arrMemory[a.row-1][j]);
    
    //lower triangle
    for(int i = a.row+1; i < newRow; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex; j < newRow - i && j < startIndex+chunkSize; j++) 
            if(a.s1[a.row-j-2] == a.s2[j+i-a.row])
                arrMemory[i][j] = 1 + arrMemory[i-2][j+1];
            else
                arrMemory[i][j] = max(arrMemory[i-1][j+1], arrMemory[i-1][j]);
    }
}

//memory optimization + parallelization
int diagonal_LCS_memory_optimization_parallel(string str1, string str2, int row, int column) {
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

        threads[i] = thread(diagonal_LCS_memory_optimization_thread, ref(diag_structs[i]));
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    return arrMemory[newRow-1][0]; 
}

//memory optimization: diagonals are now represented as rows, so that we only need to access previous two rows in the memory, as opposed to every row
int diagonal_LCS_memory_and_space_optimization(string str1, string str2, int row, int column) {
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
    for(int i = 2; i < column; i++) {
        temp1MS[0] = 0;
        temp1MS[i-2] = 0;
        temp2MS[0] = 0;
        temp2MS[i-1] = 0;
        currentMS[0] = 0;
        currentMS[i] = 0;

        for(int j = 1; j < i; j++) 
            if(str1[i-j-1] == str2[j-1])
                currentMSpointer[j] = 1 + temp1MSpointer[j-1];
            else
                currentMSpointer[j] = max(temp2MSpointer[j-1], temp2MSpointer[j]);

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
                currentMSpointer[j] = 1 + temp1MSpointer[j-1];
            else
                currentMSpointer[j] = max(temp2MSpointer[j], temp2MSpointer[j-1]);

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

    for(int j = 0; j < column - 1; j++) {
        if(str1[row-j-2] == str2[j])
            currentMSpointer[j] = 1 + temp1MSpointer[j];
        else
            currentMSpointer[j] = max(temp2MSpointer[j+1], temp2MSpointer[j]);
    }
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
                currentMSpointer[j] = 1 + temp1MSpointer[j+1];
            else
                currentMSpointer[j] = max(temp2MSpointer[j+1], temp2MSpointer[j]);

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

//function for diagonal_LCS_memory_and_space_optimization_parallel
void diagonal_LCS_memory_and_space_optimization_thread (diag& a) {
    int newRow = a.row + a.col - 1;

    int chunkSize = ceil((double)(a.col)/a.n_total);
    int startIndex = a.id * chunkSize;

    diagTemp1Pointer = diagTemp1;
    diagTemp2Pointer = diagTemp2;
    diagCurrentPointer = diagCurrent;
    int c = 0; 

    //upper triangle
    for(int i = 2; i < a.col; i++) {
        bar.arrive_and_wait();
        for(int j = startIndex+1; j < i && j <= startIndex+chunkSize; j++) 
            if(a.s1[i-j-1] == a.s2[j-1])
                diagCurrentPointer[j] = 1 + diagTemp1Pointer[j-1];
            else
                diagCurrentPointer[j] = max(diagTemp2Pointer[j-1], diagTemp2Pointer[j]);

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
                diagCurrentPointer[j] = 1 + diagTemp1Pointer[j-1];
            else
                diagCurrentPointer[j] = max(diagTemp2Pointer[j], diagTemp2Pointer[j-1]);
        
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
            diagCurrentPointer[j] = 1 + diagTemp1Pointer[j];
        else
            diagCurrentPointer[j] = max(diagTemp2Pointer[j+1], diagTemp2Pointer[j]);
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
                diagCurrentPointer[j] = 1 + diagTemp1Pointer[j+1];
            else
                diagCurrentPointer[j] = max(diagTemp2Pointer[j+1], diagTemp2Pointer[j]);

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
int diagonal_LCS_memory_and_space_optimization_parallel(string str1, string str2, int row, int column) {
    int newRow = row + column - 1;

    diagTemp1 = new int[column];
    diagTemp2 = new int[column];
    diagCurrent = new int[column];

    for(int i = 0; i < column; i++) {
        diagTemp1[i] = 0;
        diagTemp2[i] = 0;
        diagCurrent[i] = 0;
    }

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

        threads[i] = thread(diagonal_LCS_memory_and_space_optimization_thread, ref(diag_structs[i])); 
    }

    for(int i = 0; i < n_threads; i++) 
        threads[i].join();

    return arrDiag; 
}

//diagonal parallelization using the OpenMP library (doesn't achieve any speedup on 60000 x 60000 arrays)
int diagonal_LCS_parallel_openmp (string str1, string str2, int row, int column) {
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
    //iterate through diagonals
    while(i <= row-1 && j <= column-1) {
        int antidiag = min(j, row-i-1);

        //parallel calculation of all elements on current diagonal
        #pragma omp parallel for num_threads (n_thr)
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
        
        if(j >= (column-1)) {
            j = column - 2  ;
            i++;
        }
        else
            j++;

    }

    return arr[row-1][column-1];
}

//one thread calculates top half of table
void topHalf_LCS (args& a) {
    for(int i = 0; i <= a.row; i++) 
        for(int j = 0; j < a.col; j++)
            if(i == 0 || j == 0)
                arr[i][j] = 0;        
            else if(a.s1[i-1] == a.s2[j-1])
                arr[i][j] = 1 + arr[i-1][j-1];
            else 
                arr[i][j] = max(arr[i][j-1], arr[i-1][j]);
}

//one thread calculates bottom half of table
void bottomHalf_LCS (args& a) {
    int nrows = a.s1.length();

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    for(int i = nrows+1; i > a.row; i--) 
        for(int j = a.col; j > 0; j--) 
            if (i == nrows+1 || j == a.col) 
                arr[i][j] = 0;
            else if(a.s1[i-1] == a.s2[j-1])
                arr[i][j] = 1 + arr[i+1][j+1];
            else
                arr[i][j] = max(arr[i][j+1], arr[i+1][j]);
}

//merges last rows of topHalf and bottomHalf
int merge_LCS(int h, int row, int column) {
    int temp = 0;
    int currentMax = 0;

    for(int i = 1; i <= column; i++) {
        temp = arr[h][i-1] + arr[h+1][i];

        if(temp > currentMax)
            currentMax = temp;
    }

    return currentMax;
}

//space optimization: we only keep current two rows in memory
void topHalf_LCS_space_optimization (args& a) {
    //initialize zeros
    for (int i = 0; i < a.col+1; i++) {
        temp1Top[i] = 0;
        currentTop[i] = 0;
    }

    int* temp1TopPointer = temp1Top;
    int* currentTopPointer = currentTop;
    int c = 0;
    int* currentSolution; 

    for(int i = 0; i <= a.row; i++) {
        for(int j = 0; j < a.col; j++) 
            if (i == 0 || j == 0)
                currentTopPointer[j] = 0;
            else if (a.s1[i-1] == a.s2[j-1])
                currentTopPointer[j] = 1 + temp1TopPointer[j-1];
            else
                currentTopPointer[j] = max(currentTopPointer[j-1], temp1TopPointer[j]);
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
void bottomHalf_LCS_space_optimization (args& a) {
    //initialize zeros
    for (int i = 0; i <= a.col; i++) {
        temp1Bottom[i] = 0;
        currentBottom[i] = 0;
    }

    int nrows = a.s1.length();

    //in case there's nothing for this thread to do
    if(nrows == 1)
        return;

    int* temp1BottomPointer = temp1Bottom;
    int* currentBottomPointer = currentBottom;
    int c = 0;
    int* currentSolution;

    for(int i = nrows+1; i > a.row; i--) {
        for(int j = a.col; j > 0; j--)  
            if (i == nrows+1 || j == a.col)
                currentBottomPointer[j] = 0;
            else if (a.s1[i-1] == a.s2[j-1])
                currentBottomPointer[j] = 1 + temp1BottomPointer[j+1];
            else 
                currentBottomPointer[j] = max(currentBottomPointer[j+1], temp1BottomPointer[j]);
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

//merges last rows of topHalf_LCS_space_optimization and bottomHalf_LCS_space_optimization
int merge_LCS_space_optimization(int column) {
    int temp = 0;
    int currentMax = 0;

    for(int i = 1; i <= column; i++) {
        temp = currentTop[i-1] + currentBottom[i];

        if(temp > currentMax)
            currentMax = temp;
    }

    return currentMax;
}

//forward-backward approach with two threads
int fb_LCS (string str1, string str2, int row, int column) {
    //allocate
    arr = new unsigned short int*[row+1];
    for(int i = 0; i < row+1; i++)
        arr[i] = new unsigned short int[column+1];

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

    thread t1(topHalf_LCS, ref(a));
    thread t2(bottomHalf_LCS, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the actual distance
    int ret = merge_LCS(h, row, column);

    return ret;
}

//forward-backward approach with two threads and with space optimization
int fb_LCS_space_optimization (string str1, string str2, int row, int column) {
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

    thread t1(topHalf_LCS_space_optimization, ref(a));
    thread t2(bottomHalf_LCS_space_optimization, ref(a));
    
    t1.join();
    t2.join();

    //merge results to find the actual distance
    int ret = merge_LCS_space_optimization(column);

    return ret;
}
