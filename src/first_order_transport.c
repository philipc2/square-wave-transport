// first_order_transport.c

#include "first_order_transport.h"
#include <math.h>

static inline double upwind_flux(double vel_face, double U_left,
                                 double U_right) {
  return (vel_face >= 0.0)
             ? vel_face * U_left   /* flow to right, upwind = left  */
             : vel_face * U_right; /* flow to left,  upwind = right */
}

void first_order_method(double *U, double *V, int N, double dt, double dx,
                        double *U_trnsp) {
  if (N < 3)
    return;

  // left boundary: copy (zero-gradient / outflow)
  U_trnsp[0] = U[0];

  // interior cells
  for (int i = 1; i < N - 1; ++i) {
    int im = i - 1;
    int ip = i + 1;

    // compute face velocities
    double V_left = 0.5 * (V[im] + V[i]);
    double V_right = 0.5 * (V[i] + V[ip]);

    // donor‐cell upwind fluxes
    double F_left = upwind_flux(V_left, U[im], U[i]);
    double F_right = upwind_flux(V_right, U[i], U[ip]);

    // conservative update
    U_trnsp[i] = U[i] - (dt / dx) * (F_right - F_left);
  }

  // right boundary: copy (zero-gradient / outflow)
  U_trnsp[N - 1] = U[N - 1];
}
