
#include "flux_corr_method.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

  // Configuration
  size_t N = 1000;
  double dt = 0.1;
  double *U = malloc(N * sizeof(*U));
  double *V = malloc(N * sizeof(*V));
  double *Unew = malloc(N * sizeof(*Unew));

  flux_corr_method(U, V, N, dt, dx, Unew);

  // TODO: Save output

  free(U);
  free(V);
  free(Unew);
  return 0;
}
