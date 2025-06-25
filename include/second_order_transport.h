#ifndef _SECOND_ORDER_TRANSPORT_H
#define _SECOND_ORDER_TRANSPORT_H

/**
 * TODO:
 *
 * @param U[in] Quantity to transport
 * @param V[in] Velocity values
 * @param arrlen[in] Size of U, V, and U_trnsp arrays
 * @param dt[in] Delta time step
 * @param dx[in] grid cell length
 * @param U_trnsp[out] Transported quantity
 */
void second_order_method(double *U, double *V, int arrlen, double dt, double dx,
                         double *U_trnsp);

#endif