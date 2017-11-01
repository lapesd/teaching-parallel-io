#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#define SIZE 8

static double **
array2d (int, int);

static void
fillarray2d (double **, int, int);

static void
printarray2d (double **, int, int);

int 
main (int argc, char **argv)
{
	double **bw;
	double **br;
	FILE *file;
	int i, j;
	size_t ret, total;

	assert (argc == 2);

	file = fopen (argv[1], "wb");
	assert (file);
	
	bw = array2d (SIZE, SIZE);
	fillarray2d (bw, SIZE, SIZE);
	total = 0;
	for (i = 0; i < SIZE; i++)
		//for (j = 0; j < SIZE; j++)
		{
			ret = fwrite (bw[i], sizeof (double), SIZE, file);
			assert (ret == SIZE);
			total += ret * sizeof (double);
		}
	fprintf (stdout, "%ld bytes written.\n", total);
	fclose (file);

	file = fopen (argv[1], "rb");
	assert (file);

	br = array2d (SIZE, SIZE);
	total = 0;
	for (i = 0; i < SIZE; i++)
		// for (j = 0; j < SIZE; j++)
		{
			ret = fread (br[i], sizeof (double), SIZE, file);
			assert (ret == SIZE);
			total += ret * sizeof (double);
		}
	fprintf (stdout, "%ld bytes read.\n", total);	

	printarray2d (br, SIZE, SIZE);

	fclose (file);

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
fillarray2d (double **d, int x, int y)
{
	int i, j;
	
	for (i = 0; i < x; i++)
		for (j = 0; j < y; j++)
			d[i][j] = j / 10.0 + i;

	return;
}

static void
printarray2d (double **d, int x, int y)
{
	int i, j;

	for (i = 0; i < x; i++)
	{
		for (j = 0; j < y; j++)
			fprintf (stdout, "%f ", d[i][j]);

		fprintf (stdout, "\n");
	}
}
