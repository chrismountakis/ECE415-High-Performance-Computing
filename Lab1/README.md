# Lab 1: Sobel Filter CPU Optimization

A naive Sobel edge detector on a 4096×4096 greyscale image is optimized one step at a time. Every step is measured and checked for an exact result (PSNR = inf against the golden output).

Assignment: [`docs/handouts/lab-1-sobel-cpu-optimization.pdf`](../docs/handouts/lab-1-sobel-cpu-optimization.pdf)

Report: [`docs/reports/lab-1-sobel-cpu-optimization-report.pdf`](../docs/reports/lab-1-sobel-cpu-optimization-report.pdf)

## Optimization chains

The common steps are measured at both `-O0` and `-O3 -ffast-math`. After step 08 the two flag sets continue with separate chains, because what helps without compiler optimizations often hurts the vectorizer.

```
common   00 orig → 01 loop interchange → 02 function inlining → 03 loop fusion
         → 04 conv flatten → 05 constant folding → 06 LICM + CSE
         → 07 strength reduction → 08 integer PSNR accumulator
-O0      → 09 unroll 1×16 → 10 4×4 block (memory reads) → 11 sqrt lookup table → 12 PSNR fusion
-O3      → 09 sqrtf → 10 compiler hints (restrict, register)
```

## Layout

```
src/
  common/    00–08, makefile defaults to -O3 -ffast-math (make OPT=-O0 for -O0)
  O0/        09–12, makefile defaults to -O0
  O3/        09–10, makefile defaults to -O3 -ffast-math
scripts/
  benchmark.py   builds and measures every version and writes the spreadsheet
results/
  HPC_Lab1_results.xlsx
data/        input.grey and golden.grey, the course's 4096×4096 test image and its correct Sobel output
```

## Build and run

Build and run one version by hand. `make` also symlinks `input.grey` and `golden.grey` from `data/` next to the binary, since the programs open them from the current directory.

```sh
cd src/common
make OPT=-O0
./00_sobel_orig
```

To measure everything and regenerate the spreadsheet (needs `openpyxl`):

```sh
python3 scripts/benchmark.py
python3 scripts/benchmark.py --rebuild  # rewrites the xlsx file
```

Each version gets 1 warm-up run and 12 measured runs. The fastest and slowest are dropped, and the mean and standard deviation are taken over the other 10.

## Results

Apple M2 Pro, macOS 27.0.1, Apple clang 21.0.0. Times in ms (from the report).

| Version | -O0 | Speedup | -O3 -ffast-math | Speedup |
| --- | ---: | ---: | ---: | ---: |
| 00 orig | 1282.5 | 1.00× | 173.9 | 1.00× |
| 08 integer PSNR (end of common steps) | 152.7 | 8.40× | 11.5 | 15.11× |
| 12 PSNR fusion (end of -O0 chain) | 62.7 | 20.44× | | |
| 10 compiler hints (end of -O3 chain) | | | 8.7 | 20.00× |

The full per-step tables, the rejected optimizations and the analysis are in the report.

Clang has no `-fast` flag, so `-O3 -ffast-math` is used for the optimized series.
