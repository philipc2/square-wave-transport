
#ifndef _FLUX_CORRECTED_TRANSPORT_H
#define _FLUX_CORRECTED_TRANSPORT_H

/**
 * Transports quantities using the flux-corrected method, using the Boris & Book
 * limiter function.  This is technique for solving a hyperbolic partial
 * differential equation, that advects quantity 'U' using velocity 'V'.
 *
 * The algorithm here is described in:
 *    Boris, J. P., & Book, D. L. (1976). Solution of continuity equations
 *    by the method of flux-corrected transport. Controlled Fusion, 85-129.
 *
 * @param U[in] Quantity to transport
 * @param V[in] Velocity values
 * @param arrlen[in] Size of U, V, and U_trnsp arrays
 * @param dt[in] Delta time step
 * @param dx[in] grid cell length
 * @param U_trnsp[out] Transported quantity
 */
void flux_corr_method(double *U, double *V, int arrlen, double dt, double dx,
                      double *U_trnsp);

#endif
