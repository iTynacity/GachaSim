# Gacha Pull Simulator

Copied and modified from: `https://github.com/aesoopIT/afk_companion` Please reference readme.md file there for further detailed program use instructions. 

A high-performance C++ application that simulates pull/drop rates for a gacha-style system. Results can be visualized using an accompanying Python script—Shoutout to SaiLus for the original Python scripting.

This project is designed for speed, capable of running millions of simulations in seconds. It has two modes:
* **Target Souls Mode**: Calculates the number of pulls required to obtain a specific number of "souls."
* **Fixed Pulls Mode**: Calculates the number of souls obtained from a fixed number of pulls.

---
## Core Files

* `main.cpp`: The core C++ simulation logic, optimized for performance with multithreading.
* `CMakeLists.txt`: Build instructions for CMake, configured for a portable, statically-linked executable.
* `visualize.py`: A Python script to plot the simulation results using Matplotlib.

---
## How to Build and Run

## 1. Clone the Repository
```bash
git clone [https://github.com/iTynacity/GachaSim.git](https://github.com/iTynacity/GachaSim.git)
cd GachaSim
```
## 2. Build the Program
The build commands depend on your operating system and toolchain.

### Windows (Visual Studio Method - Recommended)
This is the simplest method for Windows.

1. Install Build Tools for Visual Studio.

    * During installation, select the "Desktop development with C++" workload. This installs the compiler and CMake automatically.

3. Install Python for visualization.

    * Install Python from the official Python website.

    * Important: Check the box that says "Add python.exe to PATH" during installation.

    * Open a new terminal and install the required packages:
      ```bash
      pip install pandas matplotlib scipy
      ```
4. Open a terminal (PowerShell or Command Prompt) and run the following:
    ```bash 
    # Configure the build
     cmake -S . -B build

    # Compile the program in Release mode for performance
     cmake --build build --config Release
    ```
### Windows (MinGW/MSYS2 Method - What I Use)
For users who prefer the GCC compiler on Windows.

1. Install MSYS2.

2. Open the MSYS2 MINGW64 terminal from your Start Menu.

3. Install all required tools (C++ compiler, CMake, Python, and libraries) with this single command. Press `Y` when prompted.
    ```bash
    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-python-pip mingw-w64-x86_64-python-pandas mingw-w64-x86_64-python-matplotlib mingw-w64-x86_64-python-scipy
    ```
4. From the same MSYS2 MINGW64 terminal, run the build commands:
   ```bash
   # Configure the build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

   # Compile the program
    cmake --build build
    ```
### MacOS or Linux (Untested)
1. Install the necessary build tools.

    * **On macOS**: `xcode-select --install and brew install cmake`

    * **On Debian/Ubuntu**: `sudo apt install build-essential cmake`

2. Install Python and libraries for visualization.

    * **On macOS**: `brew install python`

    * **On Debian/Ubuntu**: `sudo apt install python3 python3-pip`

Then, install the required packages: `pip3 install pandas matplotlib scipy`

3. Run the build commands:
   ```bash
   # Configure and build in Release mode
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
   ```
   
## 3. Run the Simulation
Execute the program from your terminal inside the build path.
### For Visual Studio builds
```bash
.\build\Release\GachaSimulator.exe
```
### For MinGW, macOS, or Linux builds
```bash
./build/GachaSimulator
```
Follow the on-screen prompts to run a simulation. This will generate a `.csv` results file (e.g., `soul_results.csv`).

## 4. Visualize the Results
Use the Python script to generate a plot from the `.csv` file. You must specify which mode you ran (`1` for Target Souls, `2` for Fixed Pulls).
### Example for a Mode 2 simulation
```bash
# Windows
python visualize.py 2

# MacOS/Linux
python3 visualize.py 2
```
A plot window will appear showing a histogram of your results.
