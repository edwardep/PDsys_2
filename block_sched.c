#include <stdio.h>
#include "mpi.h"

int main(int argc, char ** argv){
  int id, nproc;
  int sum,startval,endval,accum;
  MPI_Status status;
  MPI_Init(&argc,&argv);
  // get number of total nodes:
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  // get id of mynode:
  MPI_Comm_rank(MPI_COMM_WORLD, &id);     
  sum = 0; // zero sum for accumulation
  startval = 10*id/nproc+1;
  endval =   10*(id+1)/nproc;
  for(int i=startval;i<=endval;++i)
    sum = sum + i;
  printf("im the node: %d\n", id);
  printf("; the partial sum is: %d\n",sum);

  if(id!=0) //the slaves sending back the partial sums
    MPI_Send(&sum,1,MPI_INT,0,1,MPI_COMM_WORLD);
  else //id==0! the master recieves the partial sums
    for(int j=1;j<nproc;j=j+1){
      MPI_Recv(&accum, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &status);
      sum = sum + accum;
      printf("The sum yet is: %d\n", sum);
    }
  if(id == 0)
    printf("The sum from 1 to 10000 is %d\n", sum);
  MPI_Finalize();
}