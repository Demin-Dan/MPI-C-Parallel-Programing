
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
	
	int *buf, *rbuf; double stt, ent;
	
	int len = atoi(argv[1]);
	int st = atoi(argv[2]);
	int en = atoi(argv[3]);
	
	buf = (int*) malloc(len * sizeof(int));
	rbuf = (int*) malloc(len * sizeof(int));
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) {
		buffer_fill(buf, len , st, en);
		stt = MPI_Wtime();
		MPI_Send(buf, len, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(rbuf, len, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		ent = MPI_Wtime();
		printf("MPI_Send(thread(%d)): time(%lf)\n", MPI_COMM_RANK, ent - stt);
	} else if (MPI_COMM_RANK == 1) {
		MPI_Recv(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Send(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	
	free(buf); free(rbuf);
	
	buf = (int*) malloc(len * sizeof(int));
	rbuf = (int*) malloc(len * sizeof(int));
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) {
		buffer_fill(buf, len , st, en);
		stt = MPI_Wtime();
		MPI_Ssend(buf, len, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(rbuf, len, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		ent = MPI_Wtime();
		printf("MPI_Ssend(thread(%d)): time(%lf)\n", MPI_COMM_RANK, ent - stt);
	} else if (MPI_COMM_RANK == 1) {
		MPI_Recv(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Ssend(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	
	free(buf); free(rbuf);
	
	buf = (int*) malloc(len * sizeof(int));
	rbuf = (int*) malloc(len * sizeof(int));
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) {
		buffer_fill(buf, len , st, en);
		stt = MPI_Wtime();
		MPI_Rsend(buf, len, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(rbuf, len, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		ent = MPI_Wtime();
		printf("MPI_Rsend(thread(%d)): time(%lf)\n", MPI_COMM_RANK, ent - stt);
	} else if (MPI_COMM_RANK == 1) {
		MPI_Recv(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Rsend(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	
	free(buf); free(rbuf);
	
	buf = (int*) malloc(len * sizeof(int));
	rbuf = (int*) malloc(len * sizeof(int));
	int bbfs = (MPI_BSEND_OVERHEAD + len) * sizeof(int);
	int *bbuf; bbuf = (int*) malloc(bbfs);
	MPI_Buffer_attach(bbuf, bbfs);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) {
		buffer_fill(buf, len , st, en);
		stt = MPI_Wtime();
		MPI_Bsend(buf, len, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(rbuf, len, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		ent = MPI_Wtime();
		printf("MPI_Bsend(thread(%d)): time(%lf)\n", MPI_COMM_RANK, ent - stt);
	} else if (MPI_COMM_RANK == 1) {
		MPI_Recv(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Bsend(buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	
	MPI_Buffer_detach(bbuf, &bbfs);
	
	free(buf); free(rbuf); free(bbuf);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);	
}
