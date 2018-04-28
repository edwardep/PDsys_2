#include <stdio.h>
#include <stdlib.h>
double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}
int main(int argc, char ** argv){

  int sum;
  int N = atoi(argv[1]);
  sum = 0; // zero sum for accumulation
  double time0=gettime();
  for(int i=0;i<N;i++)
    sum = sum + i;

	double time1=gettime();
	fprintf(stdout, "time:%f\n\n",time1-time0);
}