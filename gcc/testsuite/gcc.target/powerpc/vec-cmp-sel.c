/* { dg-do compile { target powerpc64*-*-* } } */
/* { dg-require-effective-target powerpc_p8vector_ok } */
/* { dg-options "-maltivec -O2" } */
/* { dg-final { scan-assembler "vcmpgtsd" } } */
/* { dg-final { scan-assembler-not "xxlnor" } } */

/* Test code in simplify-rtx.c that converts
     (!c) != {0,...,0} ? a : b
   into
     c != {0,...,0} ? b : a  */

#include <altivec.h>

vector signed long long foo () {
  vector signed long long x = { 25399, -12900 };
  vector signed long long y = { 12178, -9987 };
  vector bool long long b = vec_cmpge (x, y);
  vector signed long long z = vec_sel (y, x, b);
  return z;
}
