#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv){

  int sum;
  int N = atoi(argv[1]);
  sum = 0; // zero sum for accumulation
  for(int i=0;i<N;i++)
    sum = sum + i;

}