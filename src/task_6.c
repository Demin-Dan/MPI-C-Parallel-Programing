
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <limits.h>
#include "lib/buffer_lib.h"

int main(int argc, char** argv) {
	
	int MPI_COMM_SIZE, MPI_COMM_RANK;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &MPI_COMM_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_COMM_RANK);
	
	if (argc != 5) {
		if (MPI_COMM_RANK == 0) printf("program arguments: <number of rows> <number of columns> <value range start> <value range end>\n");
		MPI_Finalize();
		return (EXIT_SUCCESS);
	}
	
	int *lens, *inds, gmx;
	
	int rws = atoi(argv[1]);
	int cls = atoi(argv[2]);
	int st = atoi(argv[3]);
	int en = atoi(argv[4]);
	
	int (*buf)[cls] = malloc(sizeof(*buf) * rws);
	lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	
	if (MPI_COMM_RANK == 0) {
		srand(time(NULL));
		for (int i = 0; i < rws; i ++) { buffer_fill(buf[i], cls, st, en); buffer_print(buf[i], cls); }
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	buffer_distribute(rws, lens, inds, MPI_COMM_SIZE);
	
	int *slns; slns = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	for (int i = 0; i < MPI_COMM_SIZE; i ++) { slns[i] = lens[i] * cls; inds[i] = i > 0 ? buffer_sum(slns, 0, i) : 0; }
	
	if (MPI_COMM_RANK == 0) {
		printf("distributed rows "); buffer_print(lens, MPI_COMM_SIZE);
		printf("distributed lengths "); buffer_print(slns, MPI_COMM_SIZE);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE);
	}
	
	int (*lbuf)[cls] = malloc(sizeof(*lbuf) * lens[MPI_COMM_RANK]);
	
	MPI_Scatterv(buf, slns, inds, MPI_INT, lbuf, slns[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD);
	
	if (lens[MPI_COMM_RANK] > 0) printf("thread(%d):\n", MPI_COMM_RANK);
	int lmx = INT_MIN, lmn;
	for (int i = 0; i < lens[MPI_COMM_RANK]; i ++) {
		lmn = buffer_min(lbuf[i], cls);
		if (lmn > lmx) lmx = lmn;
		printf("\tmin(%d) ", lmn); buffer_print(lbuf[i], cls);
	}
	if (lens[MPI_COMM_RANK] > 0) printf("\tmaxmin(%d)\n", lmx);
	
	free(buf); free(lbuf); free(lens); free(inds); free(slns);
	
	MPI_Reduce(&lmx, &gmx, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) printf("result: maxmin(%d)\n", gmx);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);	
}
