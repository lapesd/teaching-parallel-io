#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>

#define SIZE 8

static double **
array2d (int, int);

static void
fillarray2d (double **, int, int, int, int);

static void
printarray2d (double **, int, int, int);

int 
main (int argc, char **argv)
{
	int nprocs, rank;
	int sqrnprocs;
	int fd;
	int oflag = O_CREAT | O_RDWR;
	mode_t mode = S_IRUSR |S_IWUSR | S_IRGRP | S_IROTH;
	int s;
	double **bw;
	double **br;
	int i, j;
	off_t off;
	ssize_t ret, total;

	assert (argc == 2);

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	sqrnprocs = sqrt (nprocs);

	fd = open (argv[1], oflag, mode);
	assert (fd > -1);

	s = SIZE / sqrnprocs;
	
	bw = array2d (s, s);
	fillarray2d (bw, s, s, nprocs, rank);
	total = 0;
	for (i = 0; i < s; i++)
	{
		off = ((rank / sqrnprocs * s + i) * SIZE * sizeof (double)) + ((rank % sqrnprocs * s) * sizeof (double));
		lseek (fd, off, SEEK_SET);
		for (j = 0; j < s; j++)
		{
			ret = write (fd, &bw[i][j], sizeof (double));
			assert (ret > -1);
			total += ret;
		}
	}
	fprintf (stdout, "Rank %d: %ld bytes written.\n", rank, total);
	MPI_Barrier (MPI_COMM_WORLD);

	br = array2d (s, s);
	total = 0;
	for (i = 0; i < s; i++)
	{
		off = ((rank / sqrnprocs * s + i) * SIZE * sizeof (double)) + ((rank % sqrnprocs * s) * sizeof (double));
		lseek (fd, off, SEEK_SET);
		for (j = 0; j < s; j++)
		{
			ret = read (fd, &br[i][j], sizeof (double));
			assert (ret > -1);
			total += ret;
		}
	}
	fprintf (stdout, "Rank %d: %ld bytes read.\n", rank, total);	
	MPI_Barrier (MPI_COMM_WORLD);

	printarray2d (br, s, s, rank);
	MPI_Barrier (MPI_COMM_WORLD);

	close (fd);

	MPI_Finalize ();

	exit (EXIT_FAILURE);
}

static double **
array2d (int x, int y)
{
	int i, j;
	double *d = malloc (x * y * sizeof (double));
	double **p = malloc (x * sizeof (double *));

	assert (d);
	assert (p);

	for (i = 0; i < x; i++)
		p[i] = &(d[y * i]);

	return p;	
}

static void
fillarray2d (double **d, int x, int y, int nprocs, int rank)
{
	int i, j;
	int base = sqrt (nprocs);
	
	for (i = 0; i < x; i++)
		for (j = 0; j < y; j++)
			d[i][j] = ((rank % base) * y + j) / 10.0 + ((rank / base) * x + i);

	return;
}

static void
printarray2d (double **d, int x, int y, int rank)
{
	int i, j;

	for (i = 0; i < x; i++)
	{
		for (j = 0; j < y; j++)
			fprintf (stdout, "%d:%f ", rank, d[i][j]);

		fprintf (stdout, "\n");
		fflush (stdout);
	}
}
