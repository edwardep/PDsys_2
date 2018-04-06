#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "xmmintrin.h"
#include "immintrin.h"

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
	int iters = 1000;
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
	__m128 _maxF = _mm_set1_ps(maxF);

	double timeTotal = 0.0f;
	for(int j=0;j<iters;j++)
	{
		double time0=gettime();
		int unroll = (N/4)*4;
		for(int i=0;i<unroll;i+=4)
		{
			//float num_0 = LVec[i]+RVec[i];
			__m128 _LVec = _mm_load_ps(&LVec[i]); 
			__m128 _RVec = _mm_load_ps(&RVec[i]);
			__m128 _num_0= _mm_add_ps(_LVec,_RVec);
			//_mm_store_ps(_num_0);

			//float num_1 = mVec[i]*(mVec[i]-1.0)/2.0;
			__m128 _mVec = _mm_load_ps(&mVec[i]);
			__m128 _tmp;
			_tmp = _mm_sub_ps(_mVec,_mm_set1_ps(1.0));
			_tmp = _mm_div_ps(_tmp,_mm_set1_ps(2.0));

			__m128 _num_1 = _mm_mul_ps(_mVec,_tmp);

			//float num_2 = nVec[i]*(nVec[i]-1.0)/2.0;
			__m128 _nVec = _mm_load_ps(&nVec[i]);
			_tmp = _mm_sub_ps(_nVec,_mm_set1_ps(1.0));
			_tmp = _mm_div_ps(_tmp,_mm_set1_ps(2.0));

			__m128 _num_2 = _mm_mul_ps(_nVec,_tmp);

			//float num = num_0/(num_1+num_2);
			_tmp = _mm_add_ps(_num_1,_num_2);
			__m128 _num = _mm_div_ps(_num_0,_num);

			//float den_0 = CVec[i]-LVec[i]-RVec[i];
			__m128 _CVec = _mm_load_ps(&CVec[i]);
			_tmp = _mm_sub_ps(_CVec,_LVec);
			__m128 _den_0 = _mm_sub_ps(_tmp,_RVec);

			//float den_1 = mVec[i]*nVec[i];
			__m128 _den_1 = _mm_mul_ps(_mVec,_nVec);

			//float den = den_0/den_1;
			__m128 _den = _mm_div_ps(_den_0,_den_1);

			//FVec[i] = num/(den+0.01);
			__m128 _tmpd = _mm_add_ps(_den,_mm_set1_ps(0.01));
			__m128 _FVec = _mm_div_ps(_num,_tmpd);
			_mm_store_ps(&FVec[i],_FVec);

			//maxF = FVec[i]>maxF?FVec[i]:maxF;
			__m128 cmpv = _mm_cmpgt_ps(_FVec,_maxF);
			//__m128 not_cmpv = _mm_cmple_ps(_FVec,_maxF);
			__m128 and1 = _mm_and_ps(_FVec,cmpv);
			__m128 and2 = _mm_and_ps(_maxF,not_cmpv);
			__m128 res = _mm_or_ps(and1,and2);

			maxF  = _mm_set1_ps(res);

		}
		double time1=gettime();
		timeTotal += time1-time0;
	}
	printf("Time %f Max %f\n", timeTotal/iters, maxF);

	free(mVec);
	free(nVec);
	free(LVec);
	free(RVec);
	free(CVec);
	free(FVec);
}

//~~~~~~ || original for loop ||~~~~~~
	// for(int i=0;i<N;i++)
	// 	{
	// 		float num_0 = LVec[i]+RVec[i];
	// 		float num_1 = mVec[i]*(mVec[i]-1.0)/2.0;
	// 		float num_2 = nVec[i]*(nVec[i]-1.0)/2.0;
	// 		float num = num_0/(num_1+num_2);

	// 		float den_0 = CVec[i]-LVec[i]-RVec[i];
	// 		float den_1 = mVec[i]*nVec[i];
	// 		float den = den_0/den_1;

	// 		FVec[i] = num/(den+0.01);

	// 		maxF = FVec[i]>maxF?FVec[i]:maxF;
	// 	}