#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char ** argv){
  int N = atoi(argv[1]);
  int id, nproc;
  int sum,startval,endval,accum;
  MPI_Status status;
  MPI_Init(&argc,&argv);
  // get number of total nodes:
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  // get id of mynode:
  MPI_Comm_rank(MPI_COMM_WORLD, &id);     
  sum = 0; // zero sum for accumulation
  startval = N*id/nproc+1;
  endval =   N*(id+1)/nproc;
  for(int i=startval;i<=endval;++i)
    sum = sum + i;

  if(id!=0) //the slaves sending back the partial sums
    MPI_Send(&sum,1,MPI_INT,0,1,MPI_COMM_WORLD);
  else //id==0! the master recieves the partial sums
    for(int j=1;j<nproc;j=j+1){
      MPI_Recv(&accum, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &status);
      sum = sum + accum;

    }

  MPI_Finalize();
}