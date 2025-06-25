#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "first_order_transport.h"
#include "flux_corrected_transport.h"
#include "lax_transport.h"
#include "netcdf_output.h"
#include "validation.h"

/* Default Parameters */
#define DEFAULT_N 1000
#define DEFAULT_DX 1.0
#define DEFAULT_VEL 10.0
#define DEFAULT_T 10.0
#define DEFAULT_DT 0.1
#define DEFAULT_THRESH 1e-6
#define DEFAULT_PROFILE "square"
#define DEFAULT_OUTFILE "output.nc"
#define DEFAULT_PULSE_WIDTH 100
#define DEFAULT_GAUSS_SIGMA_FRAC 0.1

static void usage(const char *prog) {
  fprintf(
      stderr,
      "Usage: %s [options]\n"
      "  -N, --cells   <int>     number of cells            (default %d)\n"
      "  -x, --dx      <real>    cell width, metres         (default %.2f)\n"
      "  -v, --vel     <real>    velocity, m/s              (default %.2f)\n"
      "  -T, --time    <real>    total run time, s          (default %.2f)\n"
      "  -d, --dt      <real>    time step size, s          (default %.2f)\n"
      "  -t, --thresh  <real>    density cutoff             (default %.2e)\n"
      "  -p, --profile <string>  initial profile: square or gaussian (default "
      "%s)\n"
      "  -o, --output  <file>    output NetCDF file         (default %s)\n",
      prog, DEFAULT_N, DEFAULT_DX, DEFAULT_VEL, DEFAULT_T, DEFAULT_DT,
      DEFAULT_THRESH, DEFAULT_PROFILE, DEFAULT_OUTFILE);
}

int main(int argc, char **argv) {
  size_t N = DEFAULT_N;
  double dx = DEFAULT_DX;
  double vel = DEFAULT_VEL;
  double T = DEFAULT_T;
  double dt = DEFAULT_DT;
  double thresh = DEFAULT_THRESH;
  char profile_buf[16] = DEFAULT_PROFILE;
  const char *out_filename = DEFAULT_OUTFILE;

  int opt, idx;
  static const struct option long_opts[] = {
      {"cells", required_argument, 0, 'N'},
      {"dx", required_argument, 0, 'x'},
      {"vel", required_argument, 0, 'v'},
      {"time", required_argument, 0, 'T'},
      {"dt", required_argument, 0, 'd'},
      {"thresh", required_argument, 0, 't'},
      {"profile", required_argument, 0, 'p'},
      {"output", required_argument, 0, 'o'},
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "N:x:v:T:d:t:p:o:", long_opts, &idx)) !=
         -1) {
    switch (opt) {
    case 'N':
      N = strtoull(optarg, NULL, 10);
      break;
    case 'x':
      dx = strtod(optarg, NULL);
      break;
    case 'v':
      vel = strtod(optarg, NULL);
      break;
    case 'T':
      T = strtod(optarg, NULL);
      break;
    case 'd':
      dt = strtod(optarg, NULL);
      break;
    case 't':
      thresh = strtod(optarg, NULL);
      break;
    case 'p':
      if (strcmp(optarg, "square") == 0 || strcmp(optarg, "gaussian") == 0) {
        strncpy(profile_buf, optarg, sizeof(profile_buf) - 1);
      } else {
        fprintf(stderr, "Unknown profile '%s'\n", optarg);
        usage(argv[0]);
        return EXIT_FAILURE;
      }
      break;
    case 'o':
      out_filename = optarg;
      break;
    default:
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  double rho_amp = 25.0;
  double CFL = fabs(vel) * dt / dx;
  if (CFL > 1.0) {
    fprintf(stderr, "Warning: CFL=%.3f > 1\n", CFL);
  }

  /* allocate arrays */
  double *U_lax = malloc(N * sizeof *U_lax);
  double *U_fo = malloc(N * sizeof *U_fo);
  double *U_fc = malloc(N * sizeof *U_fc);
  double *Unew = malloc(N * sizeof *Unew);
  double *V = malloc(N * sizeof *V);
  if (!U_lax || !U_fo || !U_fc || !Unew || !V) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  /* initialize velocity */
  for (size_t i = 0; i < N; ++i) {
    V[i] = vel;
  }

  /* choose initial condition */
  if (strcmp(profile_buf, "square") == 0) {
    size_t w = DEFAULT_PULSE_WIDTH;
    size_t start = (N > w ? (N - w) / 2 : 0);
    size_t end = start + w;
    for (size_t i = 0; i < N; ++i) {
      double val = (i >= start && i < end) ? rho_amp : 0.0;
      U_lax[i] = U_fo[i] = U_fc[i] = val;
    }
  } else { /* gaussian */
    double mid = 0.5 * (double)(N - 1);
    double sigma = DEFAULT_GAUSS_SIGMA_FRAC * (double)N;
    for (size_t i = 0; i < N; ++i) {
      double d = ((double)i - mid) / sigma;
      double val = rho_amp * exp(-0.5 * d * d);
      U_lax[i] = U_fo[i] = U_fc[i] = val;
    }
  }

  /* NetCDF setup */
  NetCDFOutput out;
  if (nc_output_init(&out, out_filename, N))
    return EXIT_FAILURE;
  if (nc_output_write_x(&out, dx)) {
    nc_output_close(&out);
    return EXIT_FAILURE;
  }
  /* write initial condition */
  if (nc_output_write_step(&out, 0, 0.0, U_lax, U_fo, U_fc)) {
    nc_output_close(&out);
    return EXIT_FAILURE;
  }

  /* time-stepping */
  int n_steps = (int)round(T / dt);
  for (int step = 1; step <= n_steps; ++step) {
    double t_val = step * dt;

    /* Lax–Friedrichs */
    lax_method(U_lax, V, (int)N, dt, dx, Unew);
    for (size_t i = 0; i < N; ++i)
      U_lax[i] = (Unew[i] < thresh ? 0.0 : Unew[i]);

    /* First-order upwind */
    first_order_method(U_fo, V, (int)N, dt, dx, Unew);
    for (size_t i = 0; i < N; ++i)
      U_fo[i] = (Unew[i] < thresh ? 0.0 : Unew[i]);

    /* Flux-corrected */
    flux_corr_method(U_fc, V, (int)N, dt, dx, Unew);
    for (size_t i = 0; i < N; ++i)
      U_fc[i] = (Unew[i] < thresh ? 0.0 : Unew[i]);

    if (nc_output_write_step(&out, step, t_val, U_lax, U_fo, U_fc))
      break;
  }

  nc_output_close(&out);
  free(U_lax);
  free(U_fo);
  free(U_fc);
  free(Unew);
  free(V);
  return 0;
}
