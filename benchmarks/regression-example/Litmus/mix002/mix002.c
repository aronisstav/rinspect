/*
 * This is a C program that implements the following Litmus test
 * http://www.cprover.org/wmm/esop13/x86_litmus//mix002.litmus
 *

X86 mix002
"Fre PodWW Rfi PodRR Fre Rfi PodRR Fre Rfi PodRR"
Cycle=Rfi PodRR Fre Rfi PodRR Fre Rfi PodRR Fre PodWW
Relax=[Rfi,PodRR]
Safe=Fre PodWW
{
}
 P0          | P1          | P2          ;
 MOV [a],$1  | MOV [y],$1  | MOV [z],$1  ;
 MOV [x],$1  | MOV EAX,[y] | MOV EAX,[z] ;
 MOV EAX,[x] | MOV EBX,[z] | MOV EBX,[a] ;
 MOV EBX,[y] |             |             ;
exists
(0:EAX=1 /\ 0:EBX=0 /\ 1:EAX=1 /\ 1:EBX=0 /\ 2:EAX=1 /\ 2:EBX=0)


 * TSO violation expected.
 *
 * PSO violation expected.
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

long int a, x, y, z;

void *P0(void *arg)
{
  long int EAX, EBX;
  a = 1;
  x = 1;
  EAX = x;
  EBX = y;
  printf("\n %%%% (EAX0=%d, EBX0=%d) %%%%\n", EAX, EBX);
  return (void*)(EAX==1 && EBX==0);
}

void *P1(void *arg)
{
  long int EAX, EBX;
  y = 1;
  EAX = y;
  EBX = z;
  printf("\n %%%% (EAX1=%d, EBX1=%d) %%%%\n", EAX, EBX);
  return (void*)(EAX==1 && EBX==0);
}

void *P2(void *arg)
{
  long int EAX, EBX;
  z = 1;
  EAX = z;
  EBX = a;
  printf("\n %%%% (EAX2=%d, EBX2=%d) %%%%\n", EAX, EBX);
  return (void*)(EAX==1 && EBX==0);
}

int main(void) 
{
  pthread_t t0, t1, t2;
  long int cond0, cond1, cond2;

  pthread_create(&t0, 0, P0, 0);
  pthread_create(&t1, 0, P1, 0);
  pthread_create(&t2, 0, P2, 0);

  pthread_join(t0, (void**)&cond0);
  pthread_join(t1, (void**)&cond1);
  pthread_join(t2, (void**)&cond2);

  //assert( ! (cond0 && cond1 && cond2) );
  if ( cond0 && cond1 && cond2) {
    printf("\n@@@CLAP: There is a SC violation! \n");
    printf("\033[1;31m SC Violation!!! \033[0m\n");
  }

  return 0;
}
