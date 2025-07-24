# Gacha Pull Simulator

A high-performance C++ application that simulates pull/drop rates for a gacha-style system. Results can be visualized using an accompanying Python script. Shoutout to SaiLus for the original Python scripting.

This project is designed for speed, capable of running millions of simulations in seconds. It has two modes:
* **Target Souls Mode**: Calculates the number of pulls required to obtain a specific number of "souls."
* **Fixed Pulls Mode**: Calculates the number of souls obtained from a fixed number of pulls.

---
## Core Files

* `main.cpp`: The core C++ simulation logic, optimized for performance with multithreading.
* `CMakeLists.txt`: Build instructions for CMake, configured for a portable, statically-linked executable.
* `visualize.py`: A Python script to plot the simulation results using Matplotlib.

---
## Requirements

To build and run this project, you will need:
* **C++ Compiler**: A modern C++ compiler that supports C++17 (e.g., GCC/MinGW on Windows, Clang on macOS, GCC on Linux).
* **CMake**: Version 3.16 or higher.
* **Python 3**: For running the visualization script.
* **Python Libraries**: `pandas`, `matplotlib`, and `scipy`. You can install them with pip:
    ```bash
    pip install pandas matplotlib scipy
    ```

---
## How to Build and Run

#### 1. Clone the Repository
```bash
git clone https://github.com/YourUsername/YourRepoName.git
cd YourRepoName
```
#### 2. Build the C++ Program
Use CMake to configure and build the project in **Release** mode for maximum performance.
# Create a build directory
```bash 
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```
# Compile the program
```bash
cmake --build build
```
This creates a single, standalone executable file in the `build` folder. The `CMakeLists.txt` is configured to **statically link** all libraries, so you won't need to worry about missing `.dll` files on Windows.

#### 3. Run the Simulation
Execute the program from your terminal.
# On Windows
```bash
.\\build\\GachaSimulator.exe
```
# On macOS or Linux
```bash
./build/GachaSimulator
```
Follow the on-screen prompts to run a simulation. This will generate a `.csv` results file (e.g., `soul_results.csv`).

#### 4. Visualize the Results
Use the Python script to generate a plot from the `.csv` file. You must specify which mode you ran (`1` for Target Souls, `2` for Fixed Pulls).
# Example for a Mode 2 simulation
```bash
python visualize.py 2
```
A plot window will appear showing a histogram of your results.
