
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "lib/buffer_lib.h"

/*
 *	Task:
 *		calculate scalar multiplication
 */
 
int main(int argc, char** argv) {
	
	int MPI_COMM_SIZE, MPI_COMM_RANK;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &MPI_COMM_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_COMM_RANK);
	
	if (argc != 6) {
		if (MPI_COMM_RANK == 0) printf("program arguments: <vectors length> <value range_1 start> <value range_1 end> <value range_2 start> <value range_2 end>\n");
		MPI_Finalize();
		return (EXIT_SUCCESS);
	}
	
	int *buf1, *buf2, *lens, *inds, *lbuf1, *lbuf2, lsm = 0, gsm = 0;
	
	int len = atoi(argv[1]);
	int st1 = atoi(argv[2]);
	int en1 = atoi(argv[3]);
	int st2 = atoi(argv[4]);
	int en2 = atoi(argv[5]);
	
	buf1 = (int*) malloc(len * sizeof(int));
	buf2 = (int*) malloc(len * sizeof(int));
	lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	
	if (MPI_COMM_RANK == 0) {
		srand(time(NULL));
		buffer_fill(buf1, len, st1, en1); buffer_print(buf1, len);
		srand(time(NULL));
		buffer_fill(buf2, len, st2, en2); buffer_print(buf2, len);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	buffer_distribute(buf1, len, lens, inds, MPI_COMM_SIZE);
	
	if (MPI_COMM_RANK == 0) {
		printf("distributed lengths "); buffer_print(lens, MPI_COMM_SIZE);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE);
	}
	
	lbuf1 = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	lbuf2 = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	
	MPI_Scatterv(buf1, lens, inds, MPI_INT, lbuf1, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(buf2, lens, inds, MPI_INT, lbuf2, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD);
	
	for (int i = 0; i < lens[MPI_COMM_RANK]; i ++) lbuf1[i] = lbuf1[i] * lbuf2[i];
	lsm = buffer_sum(lbuf1, 0, lens[MPI_COMM_RANK]);
	printf("thread(%d): sum(%d) array(%d) ", MPI_COMM_RANK, lsm, lens[MPI_COMM_RANK]); buffer_print(lbuf1, lens[MPI_COMM_RANK]);
	
	free(lbuf1); free(lbuf2);
	
	MPI_Reduce(&lsm, &gsm, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	free(buf1); free(buf2); free(lens); free(inds);
	
	if (MPI_COMM_RANK == 0) printf("result: sum(%d)\n", gsm);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}
