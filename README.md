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

