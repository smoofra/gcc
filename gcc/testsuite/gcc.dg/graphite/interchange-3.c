/* { dg-require-effective-target size32plus } */

/* Formerly known as ltrans-3.c */

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#endif

double u[1782225];

static void __attribute__((noinline))
foo (int N, int *res)
{
  int i, j;
  double sum = 0;
  for (i = 0; i < N; i++)
    {
      for (j = 0; j < N; j++)
	{
	  sum = sum + u[i + 1335 * j];
	}
    }

  *res = sum + N + u[1336 * 2] + u[1336];
}

extern void abort ();

int
main (void)
{
  int i, j, res;

  for (i = 0; i < 1782225; i++)
    u[i] = 2;

  foo (1335, &res);

#if DEBUG
  fprintf (stderr, "res = %d \n", res);
#endif

  if (res != 3565789)
    abort ();

  return 0;
}

/* { dg-final { scan-tree-dump-times "will be interchanged" 1 "graphite" } } */
