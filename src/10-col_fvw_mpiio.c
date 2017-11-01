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
	MPI_File file;
	MPI_Status status;
	int mode = MPI_MODE_CREATE | MPI_MODE_RDWR;
	int s;
	double **bw;
	double **br;
	//int i, j;
	int ret;
	ssize_t total;
	MPI_Datatype view;
	int ndims = 2;
	int sizes[] = { SIZE, SIZE };
	int subsizes[ndims];
	int starts[ndims];

	assert (argc == 2);

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	sqrnprocs = sqrt (nprocs);

	s = SIZE / sqrnprocs;
	subsizes[0] = s;
	subsizes[1] = s;
	
	bw = array2d (s, s);
	fillarray2d (bw, s, s, nprocs, rank);

	starts[0] = rank / sqrnprocs * s;
	starts[1] = rank % sqrnprocs * s;	

	MPI_Type_create_subarray (ndims, sizes, subsizes, starts, MPI_ORDER_C, MPI_DOUBLE, &view);
	MPI_Type_commit (&view);

	ret = MPI_File_open (MPI_COMM_WORLD, argv[1], mode, MPI_INFO_NULL, &file);
        assert (ret == MPI_SUCCESS);

	MPI_File_set_view (file, 0, MPI_DOUBLE, view, "native", MPI_INFO_NULL);

	ret = MPI_File_write_all (file, &(bw[0][0]), s * s, MPI_DOUBLE, &status); 
	assert (ret == MPI_SUCCESS);
        total = s * s * sizeof (double);
	
	fprintf (stdout, "Rank %d: %ld bytes written.\n", rank, total);
	MPI_Barrier (MPI_COMM_WORLD);

	br = array2d (s, s);

	MPI_File_seek (file, 0, MPI_SEEK_SET);
	ret = MPI_File_read_all (file, &(br[0][0]), s * s, MPI_DOUBLE, &status);
	assert (ret == MPI_SUCCESS);
        total = s * s * sizeof (double);
	
	fprintf (stdout, "Rank %d: %ld bytes read.\n", rank, total);	
	MPI_Barrier (MPI_COMM_WORLD);

	printarray2d (br, s, s, rank);
	MPI_Barrier (MPI_COMM_WORLD);

	MPI_File_close (&file);
	MPI_Type_free (&view);

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
