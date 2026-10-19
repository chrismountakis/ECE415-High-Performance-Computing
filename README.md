![C](https://img.shields.io/badge/C-systems-blue)
![Clang](https://img.shields.io/badge/Apple%20clang-compiler-lightgrey)
![HPC](https://img.shields.io/badge/HPC-performance-purple)
![Coursework](https://img.shields.io/badge/UTH-ECE415-teal)

# ECE415 High-Performance Computing

Coursework for **ECE415 High-Performance Computing** at the **University of Thessaly**. The labs start with sequential CPU optimization and then move to parallel programming.

## Labs

| Path | Topic | Summary |
| --- | --- | --- |
| [`Lab1/`](Lab1/) | Sobel CPU optimization | Step-by-step optimization of a Sobel edge detector at `-O0` and `-O3 -ffast-math`. 20.4× and 20.0× over the baseline. |
| `Lab2/` | Upcoming | |
| `Lab3/` | Upcoming | |
| `Lab4/` | Upcoming | |
| `Lab5/` | Upcoming | |

Lab handouts are in [`docs/handouts/`](docs/handouts/) and our reports in [`docs/reports/`](docs/reports/).

## Repository structure

```
docs/
  handouts/     lab assignments
  reports/      submitted reports
LabN/
  README.md     what the lab does, how to build and run it, results
  src/          the code
  scripts/      measurement scripts
  results/      measured results
```
