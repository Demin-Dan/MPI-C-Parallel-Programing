
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

/*
 *	Task:
 *		print hello messages from all processes
 */

int main(int argc, char** argv) {
	
	int MPI_COMM_SIZE, MPI_COMM_RANK;
	
	MPI_Init(NULL, NULL);
	
	MPI_Comm_size(MPI_COMM_WORLD, &MPI_COMM_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_COMM_RANK);
	
	printf("thread(%d): Hello World!\n", MPI_COMM_RANK);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}
