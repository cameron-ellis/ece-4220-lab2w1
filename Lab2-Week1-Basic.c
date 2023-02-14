#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/timerfd.h>
#include <inttypes.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

// Helper Functions
// ================
//Function to print out the result matrix
void print_matrix_20x10(int arr[20][10], int numRows, int numCols){
    printf("\n");
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            printf("%3d ",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix_2x10(int arr[2][10], int numRows, int numCols){
    printf("\n");
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            printf("%3d ",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_kernel(int arr[1][3], int numRows, int numCols){
    printf("\n");
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            printf("%3d ",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//Declare Variables to store the given matrix and final results
int mat_20x10[20][10] = {0};
int mat_2x10[2][10] = {0};

int krnl_20x10[1][3] = {0};
int krnl_2x10[1][3] = {0};

int out_20x10[20][10] = {0}; // single thread output matrix for 20x10
int out_2x10[2][10] = {0}; // single thread output matrix for 2x10

int rt_out_20x10[20][10] = {0}; // thread per row output matrix for 20x10
int rt_out_2x10[2][10] = {0}; // thread per row output matrix for 2x10

int et_out_20x10[20][10] = {0}; // thread per element output matrix for 20x10
int et_out_2x10[2][10] = {0}; // thread per element output matrix for 2x10

//Declare structure to hold the convolution info (Like number of rows/cols)
typedef struct {
    int mat_rows;
    int mat_cols;
    int krnl_rows;
    int krnl_cols;
} conv_info;

conv_info conv_20x10;
conv_info conv_2x10;

// For thread per element, use this structure to pass the row and column info
typedef struct {
    int row;
    int column;
} element_info;


// 1. Single thread to evaluate the convolution 
void* ConvolutionPerMatrix20x10(){

	//get the Convolution info
	// Loop through the matrix based on the info
    for (int i = 0; i < conv_20x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_20x10.mat_cols; j++)
        {
            int l_krnl_idx = j - 1;
            int c_krnl_idx = j;
            int r_krnl_idx = j + 1;

            // Check to see if left most multiplication is out of bounds
            if (l_krnl_idx >= 0)
            {
                out_20x10[i][j] = out_20x10[i][j] + (mat_20x10[i][l_krnl_idx] * krnl_20x10[0][0]);
            }

            // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
            out_20x10[i][j] = out_20x10[i][j] + (mat_20x10[i][c_krnl_idx] * krnl_20x10[0][1]);

            // Check to see if right most multiplication is out of bounds
            if (r_krnl_idx < conv_20x10.mat_cols)
            {
                out_20x10[i][j] = out_20x10[i][j] + (mat_20x10[i][r_krnl_idx] * krnl_20x10[0][2]);
            }
            
        }
        
    }
}

void* ConvolutionPerMatrix2x10(){

	//get the Convolution info
	// Loop through the matrix based on the info
    for (int i = 0; i < conv_2x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_2x10.mat_cols; j++)
        {
            int l_krnl_idx = j - 1;
            int c_krnl_idx = j;
            int r_krnl_idx = j + 1;

            // Check to see if left most multiplication is out of bounds
            if (l_krnl_idx >= 0)
            {
                out_2x10[i][j] = out_2x10[i][j] + (mat_2x10[i][l_krnl_idx] * krnl_2x10[0][0]);
            }

            // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
            out_2x10[i][j] = out_2x10[i][j] + (mat_2x10[i][c_krnl_idx] * krnl_2x10[0][1]);

            // Check to see if right most multiplication is out of bounds
            if (r_krnl_idx < conv_2x10.mat_cols)
            {
                out_2x10[i][j] = out_2x10[i][j] + (mat_2x10[i][r_krnl_idx] * krnl_2x10[0][2]);
            }
            
        }
        
    }
}

// 2. Thread function to process one row of the given matrix
void* ConvolutionPerRow20x10(void* arg){
    int rowNum = *(int*)arg;

    for (int colNum = 0; colNum < conv_20x10.mat_cols; colNum++)
    {
        int l_krnl_idx = colNum - 1;
        int c_krnl_idx = colNum;
        int r_krnl_idx = colNum + 1;

        // Check to see if left most multiplication is out of bounds
        if (l_krnl_idx >= 0)
        {
            rt_out_20x10[rowNum][colNum] = rt_out_20x10[rowNum][colNum] + (mat_20x10[rowNum][l_krnl_idx] * krnl_20x10[0][0]);
        }

        // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
        rt_out_20x10[rowNum][colNum] = rt_out_20x10[rowNum][colNum] + (mat_20x10[rowNum][c_krnl_idx] * krnl_20x10[0][1]);

        // Check to see if right most multiplication is out of bounds
        if (r_krnl_idx < conv_20x10.mat_cols)
        {
            rt_out_20x10[rowNum][colNum] = rt_out_20x10[rowNum][colNum] + (mat_20x10[rowNum][r_krnl_idx] * krnl_20x10[0][2]);
        }
    }
    free(arg);
}

void* ConvolutionPerRow2x10(void* arg){
    int rowNum = *(int*)arg;
    for (int colNum = 0; colNum < conv_2x10.mat_cols; colNum++)
    {
        int l_krnl_idx = colNum - 1;
        int c_krnl_idx = colNum;
        int r_krnl_idx = colNum + 1;

        // Check to see if left most multiplication is out of bounds
        if (l_krnl_idx >= 0)
        {
            rt_out_2x10[rowNum][colNum] = rt_out_2x10[rowNum][colNum] + (mat_2x10[rowNum][l_krnl_idx] * krnl_2x10[0][0]);
        }

        // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
        rt_out_2x10[rowNum][colNum] = rt_out_2x10[rowNum][colNum] + (mat_2x10[rowNum][c_krnl_idx] * krnl_2x10[0][1]);

        // Check to see if right most multiplication is out of bounds
        if (r_krnl_idx < conv_2x10.mat_cols)
        {
            rt_out_2x10[rowNum][colNum] = rt_out_2x10[rowNum][colNum] + (mat_2x10[rowNum][r_krnl_idx] * krnl_2x10[0][2]);
        }
    }
    free(arg);
}

// 3. Thread to process each element of the matrix.
void* ConvolutionPerElement20x10(void* arg){
    element_info info = *(element_info*)arg;
    int rowNum = info.row;
    int colNum = info.column;

    int l_krnl_idx = colNum - 1;
    int c_krnl_idx = colNum;
    int r_krnl_idx = colNum + 1;

    // Check to see if left most multiplication is out of bounds
    if (l_krnl_idx >= 0)
    {
        et_out_20x10[rowNum][colNum] = et_out_20x10[rowNum][colNum] + (mat_20x10[rowNum][l_krnl_idx] * krnl_20x10[0][0]);
    }

    // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
    et_out_20x10[rowNum][colNum] = et_out_20x10[rowNum][colNum] + (mat_20x10[rowNum][c_krnl_idx] * krnl_20x10[0][1]);

    // Check to see if right most multiplication is out of bounds
    if (r_krnl_idx < conv_20x10.mat_cols)
    {
        et_out_20x10[rowNum][colNum] = et_out_20x10[rowNum][colNum] + (mat_20x10[rowNum][r_krnl_idx] * krnl_20x10[0][2]);
    }

    free(arg);
}

void* ConvolutionPerElement2x10(void* arg){
    element_info info = *(element_info*)arg;
    int rowNum = info.row;
    int colNum = info.column;

    int l_krnl_idx = colNum - 1;
    int c_krnl_idx = colNum;
    int r_krnl_idx = colNum + 1;

    // Check to see if left most multiplication is out of bounds
    if (l_krnl_idx >= 0)
    {
        et_out_2x10[rowNum][colNum] = et_out_2x10[rowNum][colNum] + (mat_2x10[rowNum][l_krnl_idx] * krnl_2x10[0][0]);
    }

    // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
    et_out_2x10[rowNum][colNum] = et_out_2x10[rowNum][colNum] + (mat_2x10[rowNum][c_krnl_idx] * krnl_2x10[0][1]);

    // Check to see if right most multiplication is out of bounds
    if (r_krnl_idx < conv_2x10.mat_cols)
    {
        et_out_2x10[rowNum][colNum] = et_out_2x10[rowNum][colNum] + (mat_2x10[rowNum][r_krnl_idx] * krnl_2x10[0][2]);
    }

    free(arg);
}

// Main function
int main(int argc, char *argv[]) {
    // Initialize file pointer to open text files
    FILE *f = fopen("20x10.txt", "r");

	// Step-1
    // ======
    // Read the file and load the data matrix and filter vector information
    // into a 2D and 1D array respectively.	
    fscanf(f, "%d %d", &(conv_20x10.mat_rows), &(conv_20x10.mat_cols));

	// Scan in matrix size using fscanf
    // Loop to initialize the original matrix
    // Print the original matrix
    for (int i = 0; i < conv_20x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_20x10.mat_cols; j++)
        {
            fscanf(f, "%d", &mat_20x10[i][j]);
        }
    }

    print_matrix_20x10(mat_20x10, conv_20x10.mat_rows, conv_20x10.mat_cols);

    // Scan in filter size
    // Loop to initialize the filter matrix
    // Print the filter matrix
    fscanf(f, "%d %d", &(conv_20x10.krnl_rows), &(conv_20x10.krnl_cols));

    for (int i = 0; i < conv_20x10.krnl_rows; i++)
    {
        for (int j = 0; j < conv_20x10.krnl_cols; j++)
        {
            fscanf(f, "%d", &krnl_20x10[i][j]);
        }
    }

    print_kernel(krnl_20x10, conv_20x10.krnl_rows, conv_20x10.krnl_cols);

    fclose(f);

    // Initialize file pointer to open text files
    f = fopen("2x10.txt", "r");

	// Step-1
    // ======
    // Read the file and load the data matrix and filter vector information
    // into a 2D and 1D array respectively.	
    fscanf(f, "%d %d", &(conv_2x10.mat_rows), &(conv_2x10.mat_cols));

	// Scan in matrix size using fscanf
    // Loop to initialize the original matrix
    // Print the original matrix
    for (int i = 0; i < conv_2x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_2x10.mat_cols; j++)
        {
            fscanf(f, "%d", &mat_2x10[i][j]);
        }
    }

    print_matrix_2x10(mat_2x10, conv_2x10.mat_rows, conv_2x10.mat_cols);

    // Scan in filter size
    // Loop to initialize the filter matrix
    // Print the filter matrix
    fscanf(f, "%d %d", &(conv_2x10.krnl_rows), &(conv_2x10.krnl_cols));

    for (int i = 0; i < conv_2x10.krnl_rows; i++)
    {
        for (int j = 0; j < conv_2x10.krnl_cols; j++)
        {
            fscanf(f, "%d", &krnl_2x10[i][j]);
        }
    }

    print_kernel(krnl_2x10, conv_2x10.krnl_rows, conv_2x10.krnl_cols);

    fclose(f);

	// Step-2
    // ======
    // Convolve using a single thread.
    pthread_t single_th_20x10; // thread for 20x10
    pthread_t single_th_2x10;  // thread for 2x10
    clock_t start;
    clock_t end;
    double time_20x10;
    double time_2x10;
    
    printf("\n\n**Case-1: Convolution using single thread**\n\n");
	// Start Timing
    start = clock();
    // Use pthread_create function to create threads
    pthread_create(&single_th_20x10, NULL, &ConvolutionPerMatrix20x10, NULL);
    // Use pthread_join the join the threads
    pthread_join(single_th_20x10, NULL);
	// End timing
    end = clock();
    time_20x10 = (double)(end - start) * ((double)1000/CLOCKS_PER_SEC);

    // Start Timing
    start = clock();
    // Use pthread_create function to create threads
    pthread_create(&single_th_2x10, NULL, &ConvolutionPerMatrix2x10, NULL);
    // Use pthread_join the join the threads
    pthread_join(single_th_2x10, NULL);
	// End timing
    end = clock();
    time_2x10 = (double)(end - start) * ((double)1000/CLOCKS_PER_SEC);

	// Print out search count with 1 thread and the time it took
    printf("Single Thread 20x10 Time: %fms\n", time_20x10);
    print_matrix_20x10(out_20x10, conv_20x10.mat_rows, conv_20x10.mat_cols);

    printf("Single Thread 2x10 Time: %fms\n", time_2x10);
    print_matrix_2x10(out_2x10, conv_2x10.mat_rows, conv_2x10.mat_cols);

	// Step-3
    // ======
    // Convolution using 1 thread per row
    printf("\n\n**Case-2: Convolution using single thread per row**\n");
    // Create thread array and variable to hold elapsed time
    pthread_t rowThread_20x10[20];
    double rt_time_20x10;
    // Start timer
    start = clock();
    // Create threads for each row, and pass to thread handler to do convolution
    for (int i = 0; i < conv_20x10.mat_rows; i++)
    {
        int* rowNum = malloc(sizeof(int));
        *rowNum = i;
        pthread_create(&rowThread_20x10[i], NULL, &ConvolutionPerRow20x10, rowNum);
    }
    // Join all threads
    for (int rowNum = 0; rowNum < conv_20x10.mat_rows; rowNum++)
    {
        pthread_join(rowThread_20x10[rowNum], NULL);
    }
    // Stop Timer
    end = clock();
    // Calculate and print elapsed time and convolution result
    rt_time_20x10 = (double)(end - start) * ((double)1000/CLOCKS_PER_SEC);
    printf("Thread Per Row 20x10 Time: %fms\n", rt_time_20x10);
    print_matrix_20x10(rt_out_20x10, conv_20x10.mat_rows, conv_20x10.mat_cols);

    // Create thread array and variable to hold elapsed time
    pthread_t rowThread_2x10[2];
    double rt_time_2x10;
    // Start timer
    start = clock();
    // Create threads for each row, and pass to thread handler to do convolution
    for (int i = 0; i < conv_2x10.mat_rows; i++)
    {
        int* rowNum = malloc(sizeof(int));
        *rowNum = i;
        pthread_create(&rowThread_2x10[i], NULL, &ConvolutionPerRow2x10, rowNum);
    }
    // Join all threads
    for (int i = 0; i < conv_2x10.mat_rows; i++)
    {
        pthread_join(rowThread_2x10[i], NULL);
    }
    // Stop Timer
    end = clock();
    // Calculate and print elapsed time and convolution result
    rt_time_2x10 = (double)(end - start) * ((double)1000/CLOCKS_PER_SEC);
    printf("Thread Per Row 2x10 Time: %fms\n", rt_time_2x10);
    print_matrix_2x10(rt_out_2x10, conv_2x10.mat_rows, conv_2x10.mat_cols);

    // Step-4
    // ======
    // Convolution with 1 thread per element
    printf("\n\n**Case 3: Convolution with 1 thread per element**\n");
    // Create thread matrix and variable to hold elapsed time
    pthread_t elementThread_20x10[20][10];
    double et_time_20x10;
    // Start timer
    start = clock();
    // Create theads for each element, and pass thread handler to do convolution
    for (int i = 0; i < conv_20x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_20x10.mat_cols; j++)
        {
            element_info* info = malloc(sizeof(element_info));
            info->row = i;
            info->column = j;
            pthread_create(&elementThread_20x10[i][j], NULL, &ConvolutionPerElement20x10, info);
        }
    }
    // Join all threads
    for (int i = 0; i < conv_20x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_20x10.mat_cols; j++)
        {
            pthread_join(elementThread_20x10[i][j], NULL);
        }
    }
    // Stop timer
    end = clock();
    // Calculate and print elapsed time and convolution result
    et_time_20x10 = (double)(end - start) * ((double)1000/CLOCKS_PER_SEC);
    printf("Thread Per Element 20x10 Time: %fms\n", et_time_20x10);
    print_matrix_20x10(et_out_20x10, conv_20x10.mat_rows, conv_20x10.mat_cols);

    // Create thread matrix and variable to hold elapsed time
    pthread_t elementThread_2x10[2][10];
    double et_time_2x10;
    // Start timer
    start = clock();
    // Create theads for each element, and pass thread handler to do convolution
    for (int i = 0; i < conv_2x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_2x10.mat_cols; j++)
        {
            element_info* info = malloc(sizeof(element_info));
            info->row = i;
            info->column = j;
            pthread_create(&elementThread_2x10[i][j], NULL, &ConvolutionPerElement2x10, info);
        }
    }
    // Join all threads
    for (int i = 0; i < conv_2x10.mat_rows; i++)
    {
        for (int j = 0; j < conv_2x10.mat_cols; j++)
        {
            pthread_join(elementThread_2x10[i][j], NULL);
        }
    }
    // Stop timer
    end = clock();
    // Calculate and print elapsed time and convolution result
    et_time_2x10 = (double)(end - start) * ((double)1000/CLOCKS_PER_SEC);
    printf("Thread Per Element 2x10 Time: %fms\n", et_time_2x10);
    print_matrix_2x10(et_out_2x10, conv_2x10.mat_rows, conv_2x10.mat_cols);
    
    return 0;
}



