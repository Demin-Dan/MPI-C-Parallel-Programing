
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
	
	if (argc != 4) {
		if (MPI_COMM_RANK == 0) printf("program arguments: <vector length> <value range start> <value range end>\n");
		MPI_Finalize();
		return (EXIT_SUCCESS);
	}
	
	int *buf, *lens, *inds, *lbuf, *rbuf, *gbuf;
	
	int len = atoi(argv[1]);
	int st = atoi(argv[2]);
	int en = atoi(argv[3]);
	
	buf = (int*) malloc(len * sizeof(int));
	gbuf = (int*) malloc(len * sizeof(int));
	lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
	
	buffer_distribute(len, lens, inds, MPI_COMM_SIZE);
	
	if (MPI_COMM_RANK == 0) {
		srand(time(NULL)); buffer_fill(buf, len, st, en); buffer_print(buf, len);
		
		printf("distributed lengths "); buffer_print(lens, MPI_COMM_SIZE);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE);
	}
	
	lbuf = (int*) malloc(lens[MPI_COMM_RANK] * sizeof(int));
	
	MPI_Scatterv(buf, lens, inds, MPI_INT, lbuf, lens[MPI_COMM_RANK], MPI_INT, 0, MPI_COMM_WORLD);
	
	rbuf = buffer_reverse(lbuf, lens[MPI_COMM_RANK]);
	
	for (int i = 0; i < MPI_COMM_SIZE; i ++) inds[i] = lens[i] > 0 ? len - inds[i] - lens[i] : 0;	
	
	MPI_Gatherv(rbuf, lens[MPI_COMM_RANK], MPI_INT, gbuf, lens, inds, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) { printf("result: "); buffer_print(gbuf, len); } free(buf); free(gbuf); free(lens); free(inds);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);	
}
