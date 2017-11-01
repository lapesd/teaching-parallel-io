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
	int fd;
	int oflag = O_CREAT | O_RDWR;
	mode_t mode = S_IRUSR |S_IWUSR | S_IRGRP | S_IROTH;
	int i, j;
	ssize_t ret, total;

	assert (argc == 2);

	fd = open (argv[1], oflag, mode);
	assert (fd > -1);
	
	bw = array2d (SIZE, SIZE);
	fillarray2d (bw, SIZE, SIZE);
	total = 0;
	for (i = 0; i < SIZE; i++)
		//for (j = 0; j < SIZE; j++)
		{
			//ret = write (fd, &bw[i][j], sizeof (double));
			ret = write (fd, bw[i], SIZE * sizeof (double));
			assert (ret > -1);
			total += ret;
		}
	fprintf (stdout, "%ld bytes written.\n", total);

	br = array2d (SIZE, SIZE);
	total = 0;
	lseek (fd, 0, SEEK_SET);
	for (i = 0; i < SIZE; i++)
		//for (j = 0; j < SIZE; j++)
		{
			//ret = read (fd, &br[i][j], sizeof (double));
			ret = read (fd, br[i], SIZE * sizeof (double));
			assert (ret > -1);
			total += ret;
		}
	fprintf (stdout, "%ld bytes read.\n", total);	

	printarray2d (br, SIZE, SIZE);

	close (fd);

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
