# Square Wave Transport

A C-based simulation project for 1D advection of a square wave across a structured grid with three numerical transport schemes:
- Lax-Fredrichs 
- First Order Upwind 
- Flux Corrected Transport

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

