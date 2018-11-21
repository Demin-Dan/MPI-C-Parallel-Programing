
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
	
	int *buf, *lbuf, *lens, *inds, gmx;
	
	int rws = atoi(argv[1]);
	int cls = atoi(argv[2]);
	int st = atoi(argv[3]);
	int en = atoi(argv[4]);
	
	buf = (int*) malloc(rws * cls * sizeof(int));
	lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	
	if (MPI_COMM_RANK == 0) {
		srand(time(NULL));
		buffer_fill(buf, rws * cls, st, en);
		for (int i = 0; i < rws; i ++) {
			for (int j = 0; j < cls; j ++) printf("%d ", buf[i * cls + j]); printf("\n");
		}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	buffer_distribute(rws, lens, inds, MPI_COMM_SIZE);
	for (int i = 0; i < MPI_COMM_SIZE; i ++) { lens[i] *= buffer_sum(lens, 0, i) < rws * cls ? cls : 0; inds[i] *= cls; }
	
	if (MPI_COMM_RANK == 0) {
		printf("distributed lengths "); buffer_print(lens, MPI_COMM_SIZE);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE);
	}
	
	lbuf = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	
	MPI_Scatterv(buf, lens, inds, MPI_INT, lbuf, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD);
	
	if (lens[MPI_COMM_RANK] / cls > 0) printf("thread(%d):\n", MPI_COMM_RANK);
	int lmx = INT_MIN, lmn;
	for (int i = 0; i < lens[MPI_COMM_RANK] / cls; i ++) {
		lmn = lbuf[i * cls];
		for (int j = 1; j < cls; j ++) { if (lbuf[i * cls + j] < lmn) lmn = lbuf[i * cls + j]; } printf("\tmin(%d) ", lmn);
		for (int j = 0; j < cls; j ++) printf("%d ", lbuf[i * cls + j]); printf("\n");
		if (lmn > lmx) lmx = lmn;
	}
	if (lens[MPI_COMM_RANK] / cls > 0) printf("\tmaxmin(%d)\n", lmx);
	
	free(buf); free(lbuf); free(lens); free(inds);
	
	MPI_Reduce(&lmx, &gmx, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) printf("result: maxmin(%d)\n", gmx);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);	
}
