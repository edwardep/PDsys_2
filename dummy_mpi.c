#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "xmmintrin.h"
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


	int chunk = ((N/4/nproc)+1)*4;
	//mem alloc
	float * a = (float*)malloc(N*sizeof(float));
	float * b = (float*)malloc(N*sizeof(float));
	float * c = (float*)malloc(N*sizeof(float));
	float * c_local = (float*)malloc(N*sizeof(float));
	float * partial_sum = (float*)malloc(N*sizeof(float));
	//array init
	for(i=0;i<N;i++)
	{
		a[i] = i;
		b[i] = i;
		c[i] = 0;
	}

	//loop to be vectorized
	time0=gettime();
	int unroll = (N/4)*4;
	int k = 0;
	for(i=id*4;i<unroll;i+=4*nproc)
	{
		//printf("this is id: %d, iters: [%d...%d] \n",id,i,i+3);
		__m128 av = _mm_load_ps(&a[i]);
		__m128 bv = _mm_load_ps(&b[i]);
		__m128 cv = _mm_add_ps(av,bv);

		_mm_store_ps(&c_local[i],cv);

		k+=4;
	}

	// if(id==1)
	// {
	// 	for(k=0;k< N;k++)	
	// 		printf("c[%d]_local:%f\n",k,c_local[k]);
	// }	


	if(id != 0){

		MPI_Send(c_local,N,MPI_FLOAT,0,1,MPI_COMM_WORLD);
	}		
	else
	{	
		for(int k=0;k<N;k++)
			c[k] = c_local[k];
		for(int j=1;j<nproc;j++){
			MPI_Recv(partial_sum,N,MPI_FLOAT,j,1,MPI_COMM_WORLD,&status);
			// printf("\n==============================\n");
			// MPI_Get_count(&status, MPI_FLOAT, &count);
			// printf("Task %d: Received %d floats from task %d \n",
   //     				id, count, status.MPI_SOURCE);
			// printf("\n==============================\n");
			for(int k = 0; k < N; k++)
				c[k] += partial_sum[k];
			// for(int k=0;k < chunk;k++)
			// 	printf("k+j*chunk: %d || partial_sum[%d]:%f\n", k+j*chunk,k,partial_sum[k]);
			// 	c[k+j*chunk] = partial_sum[k];
		}
	}	

	// HANDLE REMAINDER!!!
	if(id ==0){
		for(i=unroll;i<N;i++)
			c[i] = a[i] + b[i];
    // for(int j=0;j<N;j++)
    //   fprintf(stdout, "c[%d]:%f\n",j,c[j]);
	    time1=gettime();
	    fprintf(stdout, "time:%f\n\n",time1-time0);
  	}

	MPI_Finalize();



	return 0;
}