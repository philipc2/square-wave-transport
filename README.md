# Square Wave Transport


A C-based simulation project for 1D advection of a square wave across a structured grid with three numerical transport schemes:
- Lax-Fredrichs 
- First Order Upwind 
- Flux Corrected Transport

![Animation](https://github.com/philipc2/square-wave-transport/blob/main/animations/density.gif)

## Requirements

- **C compiler** supporting C11 (e.g. GCC, Clang)  
- **CMake** (≥3.16) 
- **NetCDF C library**  


## Building

Using CMake:

```bash
git clone https://github.com/philipc2/square-wave-transport.git
cd square-wave-transport
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./square_transport [options]
```

**Options**:

| Flag            | Description                                           | Default     |
| --------------- | ----------------------------------------------------- | ----------- |
| `-N, --cells`   | Number of grid cells                                  | 1000        |
| `-x, --dx`      | Cell width (meters)                                   | 1.0         |
| `-v, --vel`     | Uniform advection velocity (m/s)                      | 10.0        |
| `-T, --time`    | Total simulation time (seconds)                       | 10.0        |
| `-d, --dt`      | Time step size (seconds)                              | 0.1         |
| `-t, --thresh`  | Density cutoff below which values are clamped to zero | 1e-6        |
| `-o, --output`  | Path to output NetCDF file                            | `output.nc` |

**Example**:
```bash
./square_transport -N 500 -x 0.5 -v 5.0 -T 8.0 -d 0.05  -o sim.nc
```

## Data Output

The simulation output is stored in a NetCDF file with the following structure:

```text
<xarray.Dataset>
Dimensions:                 (x: 1000, time: 501)
Coordinates:
  * x        (x) float64 0.5 1.5 2.5 ... 997.5 998.5 999.5
  * time     (time) float64 0.0 0.1 0.2 ... 49.8 49.9 50.0
Data variables:
    density_lax             (time, x) float64  ...
    density_first_order     (time, x) float64  ...
    density_flux_corrected  (time, x) float64  ...
```

## Analysis & Visualization

We recommend using Python with Xarray for exploration and plotting. For example:

```python
import xarray as xr

ds = xr.open_dataset("output.nc")
print(ds)
```

You can then access the different methods' results:

```python
# Plot flux-corrected density at a given time index
ds.density_flux_corrected.isel(time=0).plot()
```



