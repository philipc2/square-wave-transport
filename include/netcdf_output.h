#ifndef NETCDF_OUTPUT_H
#define NETCDF_OUTPUT_H

#include <netcdf.h>
#include <stdlib.h>

/**
 * Handle for NetCDF output
 */
typedef struct {
  int ncid;
  int time_dimid;
  int x_dimid;
  size_t N;

  int var_x_id;
  int var_t_id;
  int var_lax_id;
  int var_fo_id;
  int var_fc_id;
} NetCDFOutput;

/**
 * Initialize NetCDF file for output of three density methods vs x over time.
 */
int nc_output_init(NetCDFOutput *out, const char *filename, size_t N);

/**
 * Write the spatial coordinate array (x) into the file.
 */
int nc_output_write_x(NetCDFOutput *out, double dx);

/**
 * Write a single time step of all three density data arrays.
 */
int nc_output_write_step(NetCDFOutput *out, int step, double t,
                         const double *U_lax, const double *U_fo,
                         const double *U_fc);

/**
 * Close the NetCDF file and release resources.
 */
int nc_output_close(NetCDFOutput *out);

#endif // NETCDF_OUTPUT_H
