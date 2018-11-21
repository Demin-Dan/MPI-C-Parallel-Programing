
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "lib/buffer_lib.h"

int main(int argc, char** argv) {
	
	int MPI_COMM_SIZE, MPI_COMM_RANK;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &MPI_COMM_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_COMM_RANK);
	
	int data = 11;
	MPI_Status status;
	
	if (MPI_COMM_RANK != 0) {
		MPI_Recv(&data, 1, MPI_INT, MPI_COMM_RANK > 0 ? MPI_COMM_RANK - 1 : MPI_COMM_SIZE - 1, 0, MPI_COMM_WORLD, &status);
		printf("thread(%d): received(%d) sending(%d)\n", MPI_COMM_RANK,  data, data + 1); data ++;
		MPI_Send(&data, 1, MPI_INT, MPI_COMM_RANK < MPI_COMM_SIZE - 1 ? MPI_COMM_RANK + 1 : 0, 0, MPI_COMM_WORLD);
	} else {
		printf("thread(%d): sending(%d)\n", MPI_COMM_RANK,  data);
		MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&data, 1, MPI_INT, MPI_COMM_SIZE - 1, 0, MPI_COMM_WORLD, &status);
		printf("thread(%d): received(%d)\n", MPI_COMM_RANK,  data);
	}
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);	
}
