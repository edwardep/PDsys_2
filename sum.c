#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
double gettime(void)
{
  struct timeval ttime;
  gettimeofday(&ttime , NULL);
  return ttime.tv_sec + ttime.tv_usec * 0.000001;
}
int main(int argc, char ** argv){
  int N = atoi(argv[1]);
  int id, nproc;
  double time0,time1;
  int sum,startval,endval,accum;
  MPI_Status status;
  MPI_Init(&argc,&argv);
  // get number of total nodes:
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  // get id of mynode:
  MPI_Comm_rank(MPI_COMM_WORLD, &id);     
  sum = 0; // zero sum for accumulation
  // startval = N*id/nproc+1;
  // endval =   N*(id+1)/nproc;
  if(id ==0 )
    time0=gettime();
  for(int i=1+id;i<=N;i+=nproc)
    sum = sum + i;

  if(id!=0) //the slaves sending back the partial sums
    MPI_Send(&sum,1,MPI_INT,0,1,MPI_COMM_WORLD);
  else //id==0! the master recieves the partial sums
    for(int j=1;j<nproc;j=j+1){
      MPI_Recv(&accum, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &status);
      sum = sum + accum;

    }
  if(id ==0){
    time1=gettime();
    fprintf(stdout, "time:%f\n\n",time1-time0);
  }
  MPI_Finalize();
}