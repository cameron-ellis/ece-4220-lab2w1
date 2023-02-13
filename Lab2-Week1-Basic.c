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

int out_20x10[20][10] = {0};
int out_2x10[2][10] = {0};

//Declare structure to hold the convolution info (Like number of rows/cols)
typedef struct {
    int mat_rows;
    int mat_cols;
    int krnl_rows;
    int krnl_cols;
} conv_info;

conv_info conv_20x10;
conv_info conv_2x10;

// 1. Single thread to evaluate the convolution 
void ConvolutionPerMatrix20x10(){

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
                out_20x10[i][l_krnl_idx] = out_20x10[i][l_krnl_idx] + (mat_20x10[i][l_krnl_idx] * krnl_20x10[0][0]);
            }

            // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
            out_20x10[i][c_krnl_idx] = out_20x10[i][c_krnl_idx] + (mat_20x10[i][c_krnl_idx] * krnl_20x10[0][1]);

            // Check to see if right most multiplication is out of bounds
            if (r_krnl_idx < conv_20x10.mat_cols)
            {
                out_20x10[i][r_krnl_idx] = out_20x10[i][r_krnl_idx] + (mat_20x10[i][r_krnl_idx] * krnl_20x10[0][2]);
            }
            
        }
        
    }

	//pthread_exit(0);
}

void ConvolutionPerMatrix2x10(){

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
                out_2x10[i][l_krnl_idx] = out_2x10[i][l_krnl_idx] + (mat_2x10[i][l_krnl_idx] * krnl_2x10[0][0]);
            }

            // Perform center multiplication (no need to check, for loop is already checking to make sure it is not out of bounds)
            out_2x10[i][c_krnl_idx] = out_2x10[i][c_krnl_idx] + (mat_2x10[i][c_krnl_idx] * krnl_2x10[0][1]);

            // Check to see if right most multiplication is out of bounds
            if (r_krnl_idx < conv_2x10.mat_cols)
            {
                out_2x10[i][r_krnl_idx] = out_2x10[i][r_krnl_idx] + (mat_2x10[i][r_krnl_idx] * krnl_2x10[0][2]);
            }
            
        }
        
    }

	//pthread_exit(0);
}


// 2. Thread function to process one row of the given matrix

// 3. Thread to process each element of the matrix.

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
    ConvolutionPerMatrix20x10();

    print_matrix_20x10(out_20x10, conv_20x10.mat_rows, conv_20x10.mat_cols);

    ConvolutionPerMatrix2x10();

    print_matrix_2x10(out_2x10, conv_2x10.mat_rows, conv_2x10.mat_cols);

	// Start Timing
    // Use pthread_create function to create pthreads
    // Use pthread_join the join the threads
	// End timing

	// Print out search count with 1 thread and the time it took



	// Step-3
    // ======
    // Convolution using 1 thread per row
    printf("\n\n**Case-2: Convolution using single thread per row**\n");




    // Step-4
    // ======
    // Convolution with 1 thread per element
    printf("\n\n**Case 3: Convolution with 1 thread per row**\n");







    return 0;
}



