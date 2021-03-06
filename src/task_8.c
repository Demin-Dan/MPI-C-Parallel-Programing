
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
	
	int *buf, **bufs, *lens, *inds, *lbuf;
	
	int len = atoi(argv[1]);
	int st = atoi(argv[2]);
	int en = atoi(argv[3]);
	
	if (MPI_COMM_RANK == 0) {		
		buf = (int*) malloc(len * sizeof(int));
		lens = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
		inds = (int*) malloc(MPI_COMM_SIZE * sizeof(int));
		
		buffer_distribute(len, lens, inds, MPI_COMM_SIZE);
		
		printf("distributed lengths "); buffer_print(lens, MPI_COMM_SIZE);
		printf("distributed indexes "); buffer_print(inds, MPI_COMM_SIZE);
		
		srand(time(NULL)); buffer_fill(buf, len, st, en); buffer_print(buf, len);
		
		bufs = (int**) malloc(MPI_COMM_SIZE * sizeof(int*));
		
		for (int i = 0; i < MPI_COMM_SIZE; i ++) {
			bufs[i] = buffer_slice(buf, len, inds[i], lens[i]);
			MPI_Send(&lens[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(bufs[i], lens[i], MPI_INT, i, 1, MPI_COMM_WORLD); free(bufs[i]);
		} free(buf);
	}
	
	MPI_Status lstt, stt;
	int llen; MPI_Recv(&llen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &lstt);
	if (llen > 0) {
		lbuf = (int*) malloc(llen * sizeof(int));
		MPI_Recv(lbuf, llen, MPI_INT, 0, 1, MPI_COMM_WORLD, &stt);
		printf("thread(%d): ", MPI_COMM_RANK); buffer_print(lbuf, llen);
		MPI_Send(lbuf, llen, MPI_INT, 0, 0, MPI_COMM_WORLD); free(lbuf);
	} else {
		MPI_Send((void*) 0, 0, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	
	if (MPI_COMM_RANK == 0) {
		buf = (int*) malloc(len * sizeof(int));
		
		MPI_Status *sts; sts = (MPI_Status*) malloc(MPI_COMM_SIZE * sizeof(MPI_Status));
		
		for (int i = 0; i < MPI_COMM_SIZE; i ++) {
			bufs[i] = (int*) malloc(len * sizeof(int));
			MPI_Recv(bufs[i], len, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &sts[i]);
			if (lens[sts[i].MPI_SOURCE] > 0) printf("from thread(%d): ", sts[i].MPI_SOURCE); buffer_print(bufs[i], lens[sts[i].MPI_SOURCE]);
			buffer_copy(buf, bufs[i], lens[sts[i].MPI_SOURCE], inds[sts[i].MPI_SOURCE]);
		}
		
		buffer_print(buf, len); free(buf);
		
		for (int i = 0; i < MPI_COMM_SIZE; i ++) if (lens[i] > 0) free(bufs[i]);
		free(bufs); free(lens); free(inds); free(sts);
	}
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);	
}
