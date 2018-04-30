#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
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
	int nproc,id,startval,endval;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	//int NP = N/nproc;
    float * a = (float*)malloc(N*sizeof(float));
    float * b = (float*)malloc(N*sizeof(float));
    float * c = (float*)malloc(N*sizeof(float));
     float * accum = (float*)malloc(N*sizeof(float));
    // int * a_local = (int*)malloc(NP*sizeof(int));
    // int * b_local = (int*)malloc(NP*sizeof(int));
    // int * c_local = (int*)malloc(NP*sizeof(int));
	
	//array init

		for(i=0;i<N;i++)
		{
			a[i] = i;
			b[i] = i;
			c[i] = 0;
		}
	
	
	if(id == 0)
		time0=gettime();

	//MPI________
		// startval = N*id/nproc+1;
		// endval =   N*(id+1)/nproc;
		//int chunk = endval-startval;
		float * a_local = (float*)malloc(sizeof(float)*N/nproc);
		float * c_local = (float*)malloc(sizeof(float)*N/nproc);
		int k = 0;
		//Wprintf("chunk: %d\n",chunk);
		for(int i=id;i<N;i+=nproc)
		{
			a_local[k] = a[i] + b[i];
			//c[i]= a[i] * b[i];
			k++;
		}
		if(id != 0)
			MPI_Send(a_local,N/nproc,MPI_FLOAT,0,1,MPI_COMM_WORLD);
		else
		{	
			//memcpy(c,a_local,N/nproc*sizeof(float));
			for(int k=0;k<N/nproc;k++)
					c[k] = a_local[k];
			for(int j=1;j<nproc;j++){
				MPI_Recv(c_local,N/nproc,MPI_FLOAT,j,1,MPI_COMM_WORLD,&status);
				//memcpy(c+(j*N/nproc*sizeof(float)),c_local,N/nproc*sizeof(float));
				for(int k=0;k<N/nproc;k++)
					c[k+j*N/nproc] = c_local[k];
			}
		}	


	// MPI_Scatter(a,NP,MPI_INT,a_local,NP,MPI_INT,0,MPI_COMM_WORLD);
	// MPI_Scatter(b,NP,MPI_INT,b_local,NP,MPI_INT,0,MPI_COMM_WORLD);

	// //work
	// for(i=0;i<NP;i++)
	// 	c_local[i] = a_local[i] + b_local[i];


	// MPI_Gather(c_local, NP,MPI_INT,c,NP,MPI_INT,0,MPI_COMM_WORLD);
  	if(id ==0){
    for(int j=0;j<N;j++)
      fprintf(stdout, "c[%d]:%f\n",j,c[j]);
	    time1=gettime();
	    fprintf(stdout, "time:%f\n\n",time1-time0);
  	}

	MPI_Finalize();

	return 0;
}