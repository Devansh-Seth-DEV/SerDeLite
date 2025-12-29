# SerDeLite Benchmarking Suite

This directory contains the performance testing suite for the SerDeLite serialization library. These tests are designed to measure raw throughput and latency across different data archetypes.

## 📈 Benchmarked Workloads

We categorize our performance into four distinct tiers to provide a transparent view of how complexity impacts serialization speed.

### 1. Simple Numeric (`PlayerStats`)
- **Focus:** Flat POD (Plain Old Data) structures.
- **Complexity:** 3 mixed integers (10 bytes total).
- **Technical Note:** Measures the peak efficiency of direct memory writes to the `ByteBuffer`. This test usually runs at the speed of the CPU's L1 cache.

### 2. Physics Data (`Vec3`)
- **Focus:** Floating-point arithmetic.
- **Complexity:** 3 x 32-bit floats (12 bytes total).
- **Technical Note:** Validates that the library handles floating-point conversion without significant overhead compared to integer operations.

### 3. Standard Profile (`Player`)
- **Focus:** Practical nesting and strings.
- **Complexity:** Nested objects + fixed-size character array.
- **Technical Note:** This is the most realistic "Game Entity" test. It involves string length calculations and recursive calls to child objects.

### 4. Stress Test (`ComplexPlayer`)
- **Focus:** Deep nesting and iteration.
- **Complexity:** A parent object containing an array of 10 nested `InventoryItem` objects.
- **Technical Note:** Measures the cost of loop-unrolling, multiple Virtual Table (vtable) lookups, and branch prediction efficiency during a "Heavy" serialization task.


## 🛠️ Execution (Windows)

A dedicated batch script is provided to automate the build and execution process. This script will automatically create a `/build` directory at the project root and place all executables there.

1. **Prerequisite**: Ensure you have compiled the static library (`libserdelite.a`) in the root `/bin` folder.

2. **Run the Script**:
   ```batch
   benchmark_win.bat
   ```

  The script performs the following:
  
  - Creates `../../build/` directory if missing.
  - Compiles all benchmarks with `-O3` optimization.
  - Links against the static library in `/bin`.
  - Executes all tests sequentially and displays results in the console

### 📂 Output Structure

After running the benchmark script, your project structure will appear as follows:

```txt
SerDeLite/
├── bin/           # Static library (libserdelite.a)
├── build/         # Generated benchmark executables (.exe)
├── include/       # Headers
└── tests/
    └── benchmarks/# Source files and scripts
```