#include "lax_transport.h"
#include <math.h>

void lax_method(double *U, double *V, int N, double dt, double dx,
                double *U_trnsp) {
  if (N < 3)
    return;

  // left boundary: copy (zero-gradient / outflow)
  U_trnsp[0] = U[0];

  // interior points: classical Lax–Friedrichs
  for (int i = 1; i < N - 1; ++i) {
    double f_im = V[i - 1] * U[i - 1];
    double f_ip = V[i + 1] * U[i + 1];
    U_trnsp[i] =
        0.5 * (U[i + 1] + U[i - 1]) - (dt / (2.0 * dx)) * (f_ip - f_im);
  }

  // right boundary: copy (zero-gradient / outflow)
  U_trnsp[N - 1] = U[N - 1];
}
