
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "lib/buffer_lib.h"

/*
 *	Task:
 *		find average of positive numbers of vector
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
	
	int *buf, *lbuf, *rbuf, *lens, *inds, *gbuf;
	
	int len = atoi(argv[1]);
	int st = atoi(argv[2]);
	int en = atoi(argv[3]);
	
	buf = (int*) malloc(len * sizeof(int));
	lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	rbuf = (int*) calloc(2, sizeof(int));
	gbuf = (int*) calloc(2, sizeof(int));
	
	srand(time(NULL));
	
	if (MPI_COMM_RANK == 0) { buffer_fill(buf, len, st, en); buffer_print(buf, len); }
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	buffer_distribute(buf, len, lens, inds, MPI_COMM_SIZE);
	
	if (MPI_COMM_RANK == 0) {
		printf("distributed lengths "); buffer_print(lens, MPI_COMM_SIZE);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE);
	}
	
	lbuf = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	
	MPI_Scatterv(buf, lens, inds, MPI_INT, lbuf, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD);
	
	for (int i = 0; i < lens[MPI_COMM_RANK]; i ++) if (lbuf[i] > 0) { rbuf[0] ++; rbuf[1] += lbuf[i]; }
	printf("thread(%d): avg(%lf) array(%d) ", MPI_COMM_RANK, rbuf[0] > 0 ? rbuf[1] / (1.0 * rbuf[0]) : 0.0, lens[MPI_COMM_RANK]);
	buffer_print(lbuf, lens[MPI_COMM_RANK]);
	
	free(lbuf); free(lens); free(inds); free(buf);
	
	MPI_Reduce(rbuf, gbuf, 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) printf("result: avg(%lf)\n", gbuf[1] / (1.0 * gbuf[0]));
	
	free(gbuf); free(rbuf);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}
