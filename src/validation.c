#include <stddef.h>

double total_mass(const double *U, size_t N) {
  double m = 0.0;
  for (size_t i = 0; i < N; ++i)
    m += U[i];
  return m;
}