/*
 * This is a C program that implements the following Litmus test
 * http://www.cprover.org/wmm/esop13/x86_litmus//safe031.litmus
 *

X86 safe031
"Wse MFencedWR Fre PodWW Wse PodWW"
Cycle=Fre PodWW Wse PodWW Wse MFencedWR
Relax=
Safe=Fre Wse PodWW MFencedWR
{
}
 P0          | P1         | P2         ;
 MOV [z],$2  | MOV [x],$1 | MOV [y],$2 ;
 MFENCE      | MOV [y],$1 | MOV [z],$1 ;
 MOV EAX,[x] |            |            ;
exists
(y=2 /\ z=2 /\ 0:EAX=0)

 * TSO violation not expected.
 *
 * PSO violation not expected.
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

long int x, y, z;


int dummy; void MFENCE(){ __sync_fetch_and_add (&dummy,0);}

void *P0(void *arg)
{
  long int EAX, EBX;
  z = 2;
  MFENCE();
  EAX = x;
  return (void*)(EAX==0);
}

void *P1(void *arg)
{
  long int EAX, EBX;
  x = 1;
  y = 1;
  return (void*)1;
}

void *P2(void *arg)
{
  long int EAX, EBX;
  y = 2;
  z = 1;
  return (void*)1;
}

int main(void) 
{
  pthread_t t0, t1, t2, t3;
  long int cond0, cond1, cond2, cond3;

  x = y = z = 0;

  pthread_create(&t0, 0, P0, 0);
  pthread_create(&t1, 0, P1, 0);
  pthread_create(&t2, 0, P2, 0);

  pthread_join(t0, (void**)&cond0);
  pthread_join(t1, (void**)&cond1);
  pthread_join(t2, (void**)&cond2);

  //assert( ! (y==2 && z==2 && cond0) );
  if ( y==2 && z==2 && cond0 ) {
    printf("\n@@@CLAP: There is a SC violation! \n");
    printf("\033[1;31m SC Violation!!! \033[0m\n");
  }

  return 0;
}
