#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "mpi.h"

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

	double time0,time1;
	int nproc,id,count;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	//mem alloc
	float * a = (float*)malloc(N*sizeof(float));
	float * b = (float*)malloc(N*sizeof(float));
	float * c = (float*)malloc(N*sizeof(float));
	float * c_local = (float*)malloc(N/nproc*sizeof(float));
	float * partial_sum = (float*)malloc(N/nproc*sizeof(float));
	//array init
	for(i=0;i<N;i++)
	{
		a[i] = i;
		b[i] = i;
		c[i] = 0;
	}

	//loop to be vectorized
	time0=gettime();
	int k = 0;
	for(i=id;i<N;i+=nproc)
	{
		c_local[k]=a[i]+b[i];
		k++;
	}
	if(id != 0)
			MPI_Send(c_local,N/nproc,MPI_FLOAT,0,1,MPI_COMM_WORLD);
	else
	{	
		for(int k=0;k<N/nproc;k++)
			c[k] = c_local[k];
		for(int j=1;j<nproc;j++){
			MPI_Recv(partial_sum,N/nproc,MPI_FLOAT,j,1,MPI_COMM_WORLD,&status);
			// MPI_Get_count(&status, MPI_FLOAT, &count);
			// printf("Task %d: Received %d floats from task %d \n",
   //     				id, count, status.MPI_SOURCE);
			for(int k=0;k<N/nproc;k++)
				c[k+j*N/nproc] = partial_sum[k];
		}
	}	

	// HANDLE REMAINDER!!!
	
	if(id ==0){
    // for(int j=0;j<N;j++)
    //   fprintf(stdout, "c[%d]:%f\n",j,c[j]);
	    time1=gettime();
	    fprintf(stdout, "time:%f\n\n",time1-time0);
  	}

	MPI_Finalize();



	return 0;
}