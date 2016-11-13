#include <stdio.h>
#include "strsplit.h"

int main(void){
  char str[] = "$GPRMC,104337.000,A,2823.0783,N,12121.3476,E,0.000,316.13,311016,,,A*5E\r\n";
  char *parts[2];

  size_t size = strsplit(str,parts,",");
  int i;
  for(i=0;i<size;i++)
  {
  	printf("%s\n",parts[i]);
  }

}