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


//Declare Variables to store the given matrix and final results

//Declare structure to hold the convolution info (Like number of rows/cols)

// 1. Single thread to evaluate the convolution 
void ConvolutionPerMatrix(void *info){

	//get the Convolution info
	// Loop through the matrix based on the info

	pthread_exit(0);
}


// 2. Thread function to process one row of the given matrix

// 3. Thread to process each element of the matrix.

// Main function
int main(int argc, char *argv[]) {
    
    // Define variables
    


	// Step-1
    // ======
    // Read the file and load the data matrix and filter vector information
    // into a 2D and 1D array respectively.	

	// Scan in matrix size using fscanf
    // Loop to initialize the original matrix
    // Print the original matrix
    
    // Scan in filter size
    // Loop to initialize the filter matrix
    // Print the filter matrix



	// Step-2
    // ======
    // Convolve using a single thread.


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



// Helper Functions
// ================
//Function to print out the result matrix
void print_matrix(int arr[100][100], int numRows, int numCols){
    printf("\n");
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            printf("%3d ",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}



