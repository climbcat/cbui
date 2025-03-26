# cbui

[C-baselayer user interface]

Written in C-style C++:

- Vector & matrix 3D math
- Software 2D Renderer and immediate-mode UI
- Font atlas generation & text layout
- Software 3D Renderer which handles dots and lines
- Algorithms: Octree, Index sets

## Linux

This project depends on my baselayer; https://github.com/climbcat/baselayer 
It and cbui must occupy the same root folder.

Also depends on two minimalist OpenGL and OS wrapper libraries; glew and glfw.

<pre>
sudo apt install cmake
sudo apt install libglew-dev
sudo apt install libglfw3-dev
</pre>

Having installed dependencies, do the following to run basic tests/examples:

<pre>
git clone https://github.com/climbcat/baselayer/
git clone https://github.com/climbcat/cbui/
cd cbui/build
cmake ..
make
./cbui
</pre>

## Windows

On Windows, 'glew' and 'glfw' are distributed as a zip file to be extracted locally:

Extract the file 'lib_win.zip' (right click -> "Extract All ..." -> press Enter).

CMake for windows is downloaded from https://cmake.org/download/.

The windows c++ libraries are installed using the Visual Studio Installer, e.g. "Desktop development with C++".

If you are natively a linux enjoyer, I recommend the alternative terminal 'w64devkit'
available via https://www.mingw-w64.org/downloads/.

Clone both of the repositories https://github.com/climbcat/baselayer/ and
https://github.com/climbcat/cbui/ into the same root folder.

To compile and run, use:

<pre>
cd cbui/build
cmake ..
cmake --build .
./Debug/cbui.exe
</pre>

(Use the cmake flag "-- config Release" for release builds.)
