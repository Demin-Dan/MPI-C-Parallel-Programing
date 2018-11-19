
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "lib/buffer_lib.h"

/*
 *	Task:
 *		find max value of vector
 */

int main(int argc, char** argv) {
	
	int MPI_COMM_SIZE, MPI_COMM_RANK;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &MPI_COMM_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_COMM_RANK);
	
	if (argc != 4) {
		if (MPI_COMM_RANK == 0) printf("program arguments: <vector length> <value range start> <value range end>\n");
		MPI_Finalize();
		return (EXIT_SUCCESS);
	}
	
	if (MPI_COMM_RANK == 0) {
		int len = atoi(argv[1]);
		int st = atoi(argv[2]);
		int en = atoi(argv[3]);
		
		int *buf; buf = (int*) malloc(len * sizeof(int));
		int *lens; lens = (int*) malloc((MPI_COMM_SIZE - 1) * sizeof(int));
		int *inds; inds = (int*) malloc((MPI_COMM_SIZE - 1) * sizeof(int));
		
		srand(time(NULL));
		
		buffer_fill(buf, len, st, en); buffer_print(buf, len);
		buffer_distribute(buf, len, lens, inds, MPI_COMM_SIZE - 1);
		
		printf("distributed lengths "); buffer_print(lens, MPI_COMM_SIZE - 1);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE - 1);
		
		int **bufs; bufs = (int**) malloc((MPI_COMM_SIZE - 1) * sizeof(int*));
		for (int i = 1; i < MPI_COMM_SIZE; i ++) bufs[i-1] = (int*) malloc(lens[i-1] * sizeof(int));
		
		for (int i = 1; i < MPI_COMM_SIZE; i ++) {
			MPI_Send(&lens[i-1], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			bufs[i-1] = buffer_slice(buf, len, inds[i-1], lens[i-1]);
			MPI_Send(bufs[i-1], lens[i-1], MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		
		free(inds);
		
		int lmx, mx = buf[0];
		MPI_Status *sts; sts = (MPI_Status*) malloc((MPI_COMM_SIZE - 1) * sizeof(MPI_Status));
		for (int i = 1; i < MPI_COMM_SIZE; i ++) if (lens[i-1] > 0) {
			MPI_Recv(&lmx, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &sts[i-1]);
			if (lmx > mx) mx = lmx;
		}
		
		for (int i = 1; i < MPI_COMM_SIZE; i ++) free(bufs[i-1]);
		free(buf); free(sts); free(bufs);
		
		printf("result: max(%d)\n", mx);
	} else {
		MPI_Status lst, bst;
		int len, *buf, mx;
		MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &lst);
		if (len > 0) {
			buf = (int*) malloc(len * sizeof(int));
			MPI_Recv(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD, &bst);
			mx = buffer_max(buf, len);
			printf("thread(%d): max(%d) array(%d) ", MPI_COMM_RANK, mx, len); buffer_print(buf, len);
			free(buf);
			MPI_Send(&mx, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}
