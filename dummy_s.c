#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

int main(int argc, char **argv)
{
	int N = atoi(argv[1]);
	int i;
	//mem alloc
	int * a = (int*)malloc(N*sizeof(int));
	int * b = (int*)malloc(N*sizeof(int));
	int * c = (int*)malloc(N*sizeof(int));

	//array init
	for(i=0;i<N;i++)
	{
		a[i] = i;
		b[i] = i;
		c[i] = 0;
	}

	//loop to be vectorized
	double time0=gettime();
	for(i=0;i<N;i++)
	{
		c[i] = a[i] + b[i];

	}
	double time1=gettime();
	fprintf(stdout, "time:%f\n\n",time1-time0);
	return 0;
}