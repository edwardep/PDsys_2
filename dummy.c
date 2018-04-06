#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "xmmintrin.h"

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

	//array init
	for(i=0;i<N;i++)
	{
		a[i] = i;
		b[i] = i;
		c[i] = 0;
	}

	//loop to be vectorized
	double time0=gettime();
	int unroll = (N/4)*4;
	__m128 xVec = _mm_set1_ps(-1);
	__m128 c9 = _mm_set1_ps(9);
	__m128 c1 = _mm_set1_ps(2);
	for(i=0;i<unroll;i+=4)
	{
		__m128 av = _mm_load_ps(&a[i]);
		//__m128 bv = _mm_load_ps(&b[i]);
		//__m128 c_v = _mm_mul_ps(a_v,b_v);

		//_mm_store_ps(&c[i],c_v);


		//x = if(i<3)?0:1;
		//===============
		//if( a[i] < 3 )
		//	x = 1;
		//else
		//	x = 0;
		

		__m128 cmpv = _mm_cmplt_ps(av,c1);
		__m128 not_cmpv = _mm_cmpge_ps(av,c1);
		__m128 and1 = _mm_and_ps(c9,cmpv);
		__m128 and2 = _mm_and_ps(c1,not_cmpv);
		xVec = _mm_or_ps(and1,and2);

		_mm_store_ps(&c[i],xVec);

		//c[i] = a[i] * b[i];
	}
	for(;i<N;i++)
	{
		c[i] = a[i] * b[i];
	}
	double time1=gettime();
	fprintf(stdout, "time:%f\n\n",time1-time0);


	return 0;
}