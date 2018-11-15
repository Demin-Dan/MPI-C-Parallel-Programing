
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <mpi.h>
#include <math.h>
#include "lib/buffer_lib.h"

/*
 *	Task:
 *		find max value of vector
 */

int main(int argc, char** argv) {
	
	int thread_rank, comm_size;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &thread_rank);
	
	int *buffer; int buffer_length;
	
	if (argc == 1) {
		
		if (thread_rank == 0) printf("Please, specify length of vector as program argument in command line.\n");
		MPI_Finalize();
		return (EXIT_SUCCESS);
		
	} else {
		
		char *p = argv[1];
		buffer_length = atoi(p);
		if (comm_size >= buffer_length) {
			if (thread_rank == 0) printf("The amount of threads can't be greater than or equal to vector length.\n");
			MPI_Finalize();
			return (EXIT_SUCCESS);
		}
		
		buffer = malloc(buffer_length * sizeof(int));
		buffer_fill(buffer, buffer_length, 0, 100);
		
		if (thread_rank == 0) { for (int i = 0; i < buffer_length; i ++) printf("%d ", buffer[i]); printf("\n"); }
		
		int global_max;
		int thread_buffer_length;
		int *sendcounts, *displs;
		int remain_buffer_length = buffer_length;
		
		if (buffer_length / comm_size > 1) {
			thread_buffer_length = ceil(buffer_length / (1.0 * comm_size));
		} else {
			thread_buffer_length = buffer_length % comm_size;
		}
		
		if (buffer_length - comm_size == 1) thread_buffer_length = comm_size;
		
		sendcounts = (int*) calloc(comm_size, sizeof(int));
		displs = (int*) malloc(comm_size * sizeof(int));
		
		for (int i = 0; i < comm_size; i ++) {
			remain_buffer_length = buffer_length - i * thread_buffer_length;
			if (remain_buffer_length >= thread_buffer_length) {
				sendcounts[i] += thread_buffer_length;
			} else {
				if (remain_buffer_length <= 0) remain_buffer_length = 1;
				sendcounts[i] += remain_buffer_length;
			}
			int offset = buffer_sum(sendcounts, 0, i);
			if (offset < buffer_length) {
				displs[i] = offset;
			} else {
				displs[i] = 0;
			}
		}
		
		if (thread_rank == 0) {
			printf("Sendcounts ["); for (int i = 0; i < comm_size; i ++) printf("%d ", sendcounts[i]); printf("]\n");
			printf("Displs ["); for (int i = 0; i < comm_size; i ++) printf("%d ", displs[i]); printf("]\n");
		}
		
		int *thread_buffer;
		thread_buffer = malloc(thread_buffer_length * sizeof(int));
		
		MPI_Scatterv(buffer, sendcounts, displs, MPI_INT, thread_buffer, thread_buffer_length, MPI_INT, 0, MPI_COMM_WORLD);
		
		free(buffer);
		
		int thread_max = thread_buffer[0];
		for (int i = 0; i < sendcounts[thread_rank]; i ++) {
			if (thread_buffer[i] > thread_max) thread_max = thread_buffer[i];
		}
		
		MPI_Reduce(&thread_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
		
		free(sendcounts);
		free(displs);
		free(thread_buffer);
		
		if (thread_rank == 0) printf("Maximum: %d\n", global_max);
		
	}
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}