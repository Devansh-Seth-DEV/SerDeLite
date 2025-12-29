@echo off
setlocal enabledelayedexpansion
echo ============================================
echo   SerDeLite Benchmark Automation (Windows)
echo ============================================

:: Define paths relative to this script (tests/benchmarks/)
set BUILD_DIR=..\..\build
set BIN_DIR=..\..\bin
set INCLUDE_DIR=..\..\include

:: 1. Check if static library exists
if not exist "%BIN_DIR%\libserdelite.a" (
    echo [ERROR] libserdelite.a not found in %BIN_DIR%
    echo Please compile the library first.
    pause
    exit /b
)

:: 2. Create build directory in root if it doesn't exist
if not exist "%BUILD_DIR%" (
    echo Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

echo.
echo ============================================
echo            Compiling Benchmarks
echo ============================================

echo [1/4] Compiling: Simple Numeric...
g++ -O3 simple_numeric_benchmark.cpp -o "%BUILD_DIR%\num_bench.exe" -I"%INCLUDE_DIR%" -L"%BIN_DIR%" -lserdelite

echo [2/4] Compiling: Physics Data...
g++ -O3 physics_data_benchmark.cpp -o "%BUILD_DIR%\phys_bench.exe" -I"%INCLUDE_DIR%" -L"%BIN_DIR%" -lserdelite

echo [3/4] Compiling: Nested Objects...
g++ -O3 nested_object_benchmark.cpp -o "%BUILD_DIR%\nest_bench.exe" -I"%INCLUDE_DIR%" -L"%BIN_DIR%" -lserdelite

echo [4/4] Compiling: World State...
g++ -O3 world_state_benchmark.cpp -o "%BUILD_DIR%\world_bench.exe" -I"%INCLUDE_DIR%" -L"%BIN_DIR%" -lserdelite

echo.
echo ============================================
echo            Running All Benchmarks
echo ============================================
echo.

echo Running: Simple Numeric...
"%BUILD_DIR%\num_bench.exe"
echo --------------------------------------------
echo.

echo Running: Physics Data...
"%BUILD_DIR%\phys_bench.exe"
echo --------------------------------------------
echo.

echo Running: Nested Objects...
"%BUILD_DIR%\nest_bench.exe"
echo --------------------------------------------
echo.

echo Running: World State (Stress Test)...
"%BUILD_DIR%\world_bench.exe"
echo.

echo ============================================
echo          All benchmarks complete!
echo ============================================

pause