#ifndef _LAX_TRANSPORT_H
#define _LAX_TRANSPORT_H

/**
 * Lax–Friedrichs method with simple outflow (zero‐gradient) boundary
 * conditions. The first and last cells are carried forward unchanged.
 *
 * @param U[in] Quantity to transport
 * @param V[in] Velocity values
 * @param arrlen[in] Size of U, V, and U_trnsp arrays
 * @param dt[in] Delta time step
 * @param dx[in] grid cell length
 * @param U_trnsp[out] Transported quantity
 */
void lax_method(double *U, double *V, int arrlen, double dt, double dx,
                double *U_trnsp);

#endif