#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "xmmintrin.h"
#include "immintrin.h"
#include "mpi.h"

double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

float randpval()
{
	int vr = rand();
	int vm = rand()%vr;
	float r = ((float)vm)/(float)vr;
	assert(r>=0.0 && r<=1.00001);
	return r;
}

int main(int argc, char ** argv)
{
	int N = atoi(argv[1]);
	int iters = 1;

	//<<____MPI
	double time0,time1;
	int nproc,id,count;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	//MPI____>>

	srand(1);

	float * mVec = (float*)malloc(sizeof(float)*N);
	assert(mVec!=NULL);

	float * nVec = (float*)malloc(sizeof(float)*N);
	assert(nVec!=NULL);

	float * LVec = (float*)malloc(sizeof(float)*N);
	assert(LVec!=NULL);

	float * RVec = (float*)malloc(sizeof(float)*N);
	assert(RVec!=NULL);

	float * CVec = (float*)malloc(sizeof(float)*N);
	assert(CVec!=NULL);

	float * FVec = (float*)malloc(sizeof(float)*N);
	assert(FVec!=NULL);

	float * FVec_local = (float*)malloc(sizeof(float)*N/nproc);
	assert(FVec_local!=NULL);

	float * FVec_local2 = (float*)malloc(sizeof(float)*N/nproc);
	assert(FVec_local2!=NULL);

	for(int i=0;i<N;i++)
	{
		mVec[i] = (float)(2+rand()%10);
		nVec[i] = (float)(2+rand()%10);
		LVec[i] = 0.0;
		for(int j=0;j<mVec[i];j++)
		{
			LVec[i] += randpval();
		}
		RVec[i] = 0.0;
		for(int j=0;j<nVec[i];j++)
		{
			RVec[i] += randpval();
		}
		CVec[i] = 0.0;
		for(int j=0;j<mVec[i]*nVec[i];j++)
		{
			CVec[i] += randpval();
		}
		FVec[i] = 0.0;
	
		assert(mVec[i]>=2.0 && mVec[i]<=12.0);
		assert(nVec[i]>=2.0 && nVec[i]<=12.0);
		assert(LVec[i]>0.0 && LVec[i]<=1.0*mVec[i]);
		assert(RVec[i]>0.0 && RVec[i]<=1.0*nVec[i]);
		assert(CVec[i]>0.0 && CVec[i]<=1.0*mVec[i]*nVec[i]);
	}
	float maxF = 0.0f;
	double timeTotal = 0.0f;
	for(int j=0;j<iters;j++)
	{
		double time0=gettime();
		int unroll = (N/4)*4;
		int i,k = 0;
		for( i=id*4;i<unroll;i+=4*nproc)
		{
			//printf("this is id: %d, iters: [%d...%d] \n",id,i,i+3);

			/* num_0 = LVec[i]+RVec[i]; */
			__m128 _LVec = _mm_load_ps(&LVec[i]); 
			__m128 _RVec = _mm_load_ps(&RVec[i]);
			__m128 _num_0= _mm_add_ps(_LVec,_RVec);

			/* num_1 = mVec[i]*(mVec[i]-1.0)/2.0; */
			__m128 _mVec = _mm_load_ps(&mVec[i]);
			__m128 _con_1 = _mm_set1_ps(1.0);
			__m128 _con_2 = _mm_set1_ps(2.0);
			__m128 _sub_num1 = _mm_sub_ps(_mVec,_con_1);
			__m128 _div_num1 = _mm_div_ps(_sub_num1,_con_2);
			__m128 _num_1 = _mm_mul_ps(_mVec,_div_num1);

			/* num_2 = nVec[i]*(nVec[i]-1.0)/2.0; */
			__m128 _nVec = _mm_load_ps(&nVec[i]);
			__m128 _sub_num2 = _mm_sub_ps(_nVec,_con_1);
			__m128 _div_num2 = _mm_div_ps(_sub_num2,_con_2);
			__m128 _num_2 = _mm_mul_ps(_nVec,_div_num2);

			/* num = num_0/(num_1+num_2); */
			__m128 _add_nums = _mm_add_ps(_num_1,_num_2);
			__m128 _num = _mm_div_ps(_num_0,_add_nums);

			/* den_0 = CVec[i]-LVec[i]-RVec[i]; */
			__m128 _CVec = _mm_load_ps(&CVec[i]);
			__m128 _sub_CL = _mm_sub_ps(_CVec,_LVec);
			__m128 _den_0 = _mm_sub_ps(_sub_CL,_RVec);

			/* den_1 = mVec[i]*nVec[i]; */
			__m128 _den_1 = _mm_mul_ps(_mVec,_nVec);

			/* den = den_0/den_1; */
			__m128 _den = _mm_div_ps(_den_0,_den_1);

			/* FVec[i] = num/(den+0.01); */
			__m128 _con_01 = _mm_set1_ps(0.01);
			__m128 _add_den = _mm_add_ps(_den,_con_01);
			__m128 _FVec = _mm_div_ps(_num,_add_den);


			_mm_store_ps(&FVec_local[k],_FVec);
			k+=4;
			
		}

		if(id != 0)
			MPI_Send(FVec_local,N/nproc,MPI_FLOAT,0,0,MPI_COMM_WORLD);
		else
		{
			for(int m=0;m<N/nproc;m++)
					FVec[m] = FVec_local[m];
			for(int j=1;j<nproc;j++){
				MPI_Recv(FVec_local2,N/nproc,MPI_FLOAT,j,0,MPI_COMM_WORLD,&status);
				// MPI_Get_count(&status, MPI_FLOAT, &count);
				// printf("Task %d: Received %d floats from task %d \n",
    //    				id, count, status.MPI_SOURCE);
				for(int m=0;m<N/nproc;m++)
					FVec[m+j*N/nproc] = FVec_local2[m];
			}
		}
		if(id == 0){
			/* HANDLE REMAINDER */
			for(i = unroll; i<N ;i++)
			{
				float num_0 = LVec[i]+RVec[i];
				float num_1 = mVec[i]*(mVec[i]-1.0)/2.0;
				float num_2 = nVec[i]*(nVec[i]-1.0)/2.0;
				float num = num_0/(num_1+num_2);

				float den_0 = CVec[i]-LVec[i]-RVec[i];
				float den_1 = mVec[i]*nVec[i];
				float den = den_0/den_1;

				FVec[i] = num/(den+0.01);
			}
			for(int k = 0; k < N; k++)
				if(FVec[k] > maxF)
					maxF = FVec[k];
		}
		double time1=gettime();
		timeTotal += time1-time0;
	}
	/*
	 * IF (N % 4 != 0) remainder is NOT handled!!
	 * extra code needed(for loop)
	 */
	if(id ==0){

    // for(int j=0;j<N;j++)
    //   fprintf(stdout, "c[%d]:%f\n",j,c[j]);
	    //time1=gettime();
	    printf("Time %f Max %f\n", timeTotal/iters, maxF);
  	}
	
  	free(FVec_local);
  	free(FVec_local2);
	free(mVec);
	free(nVec);
	free(LVec);
	free(RVec);
	free(CVec);
	free(FVec);


	MPI_Finalize();
	exit(0);
}