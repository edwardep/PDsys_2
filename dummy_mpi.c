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
	//mem alloc
	float * a = (float*)malloc(N*sizeof(float));
	float * b = (float*)malloc(N*sizeof(float));
	float * c = (float*)malloc(N*sizeof(float));
	float * accum = (float*)malloc(N*sizeof(float));

	int nproc,id,dest,source,rc,count,tag=1;
	char inmsg, outmsg='x';
	MPI_Status status;
	int startval,endval;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	 // MPI_Datatype float_array;
	 // MPI_Type_contiguous(N,MPI_FLOAT, &float_array);
	 // MPI_Type_commit(&float_array);

	//array init
	for(i=0;i<N;i++)
	{
		a[i] = i;
		b[i] = i;
		c[i] = 0;
	}

	double time0,time1;
	int x = -1;
	//loop to be vectorized
	if(id == 0)
		time0=gettime();

	startval = N*id/nproc+1;
	endval =   N*(id+1)/nproc;

	for(i=startval;i<=endval;++i)
	{
		c[i] = a[i] + b[i];

	}
	// printf("im the node: %d\n", id);
 //  	for(i=0;i<N;i++)	
 //    		printf("partial c[%d]: %f\n",i, c[i]);


  	if(id!=0) //the slaves sending back the partial sums
  	{
  		//printf("id != 0\n");
  		// printf("im the node: %d\n", id);
  		// for(i=0;i<N;i++)	
    // 		printf("partial c[%d]: %f\n",i, c[i]);
   		
   		MPI_Send(c,N,MPI_FLOAT,0,1,MPI_COMM_WORLD);
   		//MPI_Send(b,1,float_array,0,1,MPI_COMM_WORLD);
  	}
  	else //id==0! the master recieves the partial sums
	{
		//printf("id == 0\n");
	    for(int j=1;j<nproc;j++){
	      	MPI_Recv(accum,N,MPI_FLOAT, j, 1, MPI_COMM_WORLD, &status);
	    	// for(i=0;i<N;i++)	
    		// 	printf("pre - c[%d]: %f\n",i, c[i]);
	       	// for(int k=0;k<N;k++)
	       	// 	c[k] += accum[k];
	      //  	for(i=0;i<N;i++)	
    			// printf("post - c[%d]: %f\n",i, c[i]);
	    }
  	}
  	if(id == 0)
  	{
  		time1=gettime();
		fprintf(stdout, "time:%f\n\n",time1-time0);
	}	

	//loop to be vectorized
	// double time0=gettime();
	// int unroll = (N/4)*4;

	// for(i=0;i<unroll;i+=4)
	// {
	// 	//if (i%numtasks != id) continue;
	// 	if(id == 0)
	// 	{
	// 		rc = MPI_Send();
	// 	}
	// 	__m128 av = _mm_load_ps(&a[i]);
	// 	__m128 bv = _mm_load_ps(&b[i]);
	// 	__m128 cv = _mm_mul_ps(av,bv);

	// 	_mm_store_ps(&c[i],cv);

	// }
	// for(;i<N;i++)
	// {
	// 	c[i] = a[i] * b[i];
	// }
	// double time1=gettime();
	// fprintf(stdout, "time:%f\n\n",time1-time0);


	MPI_Finalize();

	return 0;
}