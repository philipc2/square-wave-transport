#include "netcdf_output.h"
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NC_CHECK(status)                                                       \
  do {                                                                         \
    if ((status) != NC_NOERR) {                                                \
      fprintf(stderr, "NetCDF error: %s\n", nc_strerror(status));              \
      return (int)status;                                                      \
    }                                                                          \
  } while (0)

int nc_output_init(NetCDFOutput *out, const char *filename, size_t N) {
  out->N = N;
  int dimids[2];

  NC_CHECK(nc_create(filename, NC_NETCDF4 | NC_CLOBBER, &out->ncid));
  NC_CHECK(nc_def_dim(out->ncid, "time", NC_UNLIMITED, &out->time_dimid));
  NC_CHECK(nc_def_dim(out->ncid, "x", (int)N, &out->x_dimid));

  /* x coordinate */
  NC_CHECK(
      nc_def_var(out->ncid, "x", NC_DOUBLE, 1, &out->x_dimid, &out->var_x_id));
  NC_CHECK(nc_put_att_text(out->ncid, out->var_x_id, "units", strlen("meters"),
                           "meters"));

  /* time coordinate */
  NC_CHECK(nc_def_var(out->ncid, "time", NC_DOUBLE, 1, &out->time_dimid,
                      &out->var_t_id));

  /* density fields */
  dimids[0] = out->time_dimid;
  dimids[1] = out->x_dimid;
  NC_CHECK(nc_def_var(out->ncid, "density_lax", NC_DOUBLE, 2, dimids,
                      &out->var_lax_id));
  NC_CHECK(nc_def_var(out->ncid, "density_first_order", NC_DOUBLE, 2, dimids,
                      &out->var_fo_id));
  NC_CHECK(nc_def_var(out->ncid, "density_flux_corrected", NC_DOUBLE, 2, dimids,
                      &out->var_fc_id));

  NC_CHECK(nc_enddef(out->ncid));
  return 0;
}

int nc_output_write_x(NetCDFOutput *out, double dx) {
  double *coords = malloc(out->N * sizeof *coords);
  if (!coords)
    return NC_ENOMEM;
  for (size_t i = 0; i < out->N; ++i)
    coords[i] = (i + 0.5) * dx;
  NC_CHECK(nc_put_var_double(out->ncid, out->var_x_id, coords));
  free(coords);
  return 0;
}

int nc_output_write_step(NetCDFOutput *out, int step, double t,
                         const double *U_lax, const double *U_fo,
                         const double *U_fc) {
  size_t t_index = (size_t)step;
  NC_CHECK(nc_put_var1_double(out->ncid, out->var_t_id, &t_index, &t));

  size_t start[2] = {t_index, 0};
  size_t count[2] = {1, out->N};

  NC_CHECK(nc_put_vara_double(out->ncid, out->var_lax_id, start, count, U_lax));
  NC_CHECK(nc_put_vara_double(out->ncid, out->var_fo_id, start, count, U_fo));
  NC_CHECK(nc_put_vara_double(out->ncid, out->var_fc_id, start, count, U_fc));

  return 0;
}

int nc_output_close(NetCDFOutput *out) { return nc_close(out->ncid); }
