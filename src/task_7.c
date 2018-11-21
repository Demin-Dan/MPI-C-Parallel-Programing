
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
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
	
	int *buf, *tbuf, *lbuf, *tlbf, *vec, *lvc, *rbuf, *grbf, *lens, *inds;
	
	int rws = atoi(argv[1]);
	int cls = atoi(argv[2]);
	int st = atoi(argv[3]);
	int en = atoi(argv[4]);
	
	buf = (int*) malloc(rws * cls * sizeof(int));
	vec = (int*) malloc(cls * sizeof(int));
	grbf = (int*) calloc(rws, sizeof(int));
	tbuf = (int*) malloc(rws * cls * sizeof(int));
	lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	
	if (MPI_COMM_RANK == 0) {
		srand(time(NULL));
		buffer_fill(vec, cls, st, en);
		srand(time(NULL) + 1);
		buffer_fill(buf, rws * cls, st, en);
		
		buffer_print(vec, cls); printf("\n");
		
		for (int i = 0; i < rws; i ++) {
			for (int j = 0; j < cls; j ++) printf("%d ", buf[i * cls + j]); printf("\n");
		} printf("\n");
		
		buffer_2d_transpose(buf, tbuf, rws, cls); free(buf);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	buffer_distribute(cls, lens, inds, MPI_COMM_SIZE);
	
	lvc = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	rbuf = (int*) calloc(rws, sizeof(int));
	
	MPI_Scatterv(vec, lens, inds, MPI_INT, lvc, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD); free(vec);
	
	for (int i = 0; i < MPI_COMM_SIZE; i ++) { lens[i] *= buffer_sum(lens, 0, i) < rws * cls ? rws : 0; inds[i] *= rws; }
	
	lbuf = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	tlbf = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	
	MPI_Scatterv(tbuf, lens, inds, MPI_INT, lbuf, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD); free(inds);
	
	if (lens[MPI_COMM_RANK] / rws > 0) printf("thread(%d):\n", MPI_COMM_RANK);
	
	buffer_2d_transpose(lbuf, tlbf, lens[MPI_COMM_RANK] / rws, rws); free(lbuf);
	
	for (int i = 0; i < rws; i ++) {
		printf("\t"); for (int j = 0; j < lens[MPI_COMM_RANK] / rws; j ++) {
			rbuf[i] += lvc[j] * tlbf[i * lens[MPI_COMM_RANK] / rws + j];
			printf("%d * %d\t", tlbf[i * lens[MPI_COMM_RANK] / rws + j], lvc[j]);
		} printf("= %d\n", rbuf[i]);
	}
	
	MPI_Reduce(rbuf, grbf, rws, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	free(tbuf); free(rbuf); free(lvc); free(tlbf); free(lens);
	
	if (MPI_COMM_RANK == 0) {
		printf("result: [\n"); for (int i = 0; i < rws; i ++) printf("\t%d\n", grbf[i]); printf("]\n");
	} free(grbf);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}
