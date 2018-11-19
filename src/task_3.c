
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

/*
 *	Task:
 *		calculate pi using Monte-Carlo algorithm
 */
 
int main(int argc, char** argv) {
	
	int MPI_COMM_SIZE, MPI_COMM_RANK;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &MPI_COMM_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_COMM_RANK);
	
	if (argc != 3) {
		if (MPI_COMM_RANK == 0) printf("program arguments: <cirlce radius> <number of points>\n");
		MPI_Finalize();
		return (EXIT_SUCCESS);
	}
	
	double rds = atof(argv[1]);
	int tps = atoi(argv[2]);
	int cps = 0, tcps = 0;
	double x, y;
	
	srand(time(NULL) + MPI_COMM_RANK);
	
	for (int i = 0; i < tps / MPI_COMM_SIZE; i ++) {
		x = rand() * rds / RAND_MAX;
		y = rand() * rds / RAND_MAX;
		if (x*x + y*y < rds*rds) cps++;
	}
	
	MPI_Reduce(&cps, &tcps, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if (MPI_COMM_RANK == 0) printf("result: pi(%lf)\n", 4 * tcps / (1.0 * tps));
	
	MPI_Finalize();
	
	return (EXIT_SUCCESS);
}