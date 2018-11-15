
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

/*
 *	Task:
 *		print hello messages from all threads
 */

int main(int argc, char** argv) {
	
	int thread_rank, comm_size;
	
	MPI_Init(NULL, NULL);
	
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &thread_rank);
	
	printf("Thread (%d/%d) {\n\tHello World!\n}\n", thread_rank, comm_size);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}