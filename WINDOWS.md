## Compiling the tests
### Windows
1. Install GCC on Windows with [this guide](https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingw-w64-toolchain)
2. Clone this repository with this command: `git clone https://github.com/FredRhys/libmontmul`
3. Modify the location of GCC in `CMakePresets.json` if you installed it in a non-default location
4. Open a terminal in `build/`
5. Run the command `cmake .. --preset windows` to initialise CMake
6. Run the command `cmake --build .` to build the executable
