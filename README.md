# cbui

[C-baselayer user interface]

Written in C-style C++:

- Vectors & 4x4 matrix transforms
- Immediate-mode UI with offline auto-layout
- Font atlas & text layout
- Platform interface (depends on glew/glfw)

The header-only library <code>jg_cbui.h</code> and its dependency, <code>jg_baselayer.h</code>, is simply copied and included into your project.

The platform layer wraps mouse/keyboard input, and sets up a simple OpenGL buffer that covers the window as a texture.
This effectively makes a direct screen buffer available for drawing with your custom renderer, or by some of the available raster/blit functions.

To use this feature, set ENABLE_GLFW using e.g.

<pre>
#define ENABLE_GLFW
</pre>

## Linux

<pre>
sudo apt install cmake
sudo apt install libglew-dev
sudo apt install libglfw3-dev
</pre>

To check that everything works, do the following:

<pre>
git clone https://github.com/climbcat/cbui
cd cbui
mkdir build
cd build
cmake ..
make
cbui --test
</pre>


## Windows

If using the platform layer, e.g. <code>ENABLE_GLFW</code> is set, manually extract the provided 'lib_win.zip', or provide glew/glfw in a different way.

Install cmake for windows via https://cmake.org/download/.

If you are a linux person, I recommend the light weight tty tool 'w64devkit' is available via https://www.mingw-w64.org/downloads/.

To compile and run, use:

<pre>
cd cbui/build
cmake ..
cmake --build .
./Debug/cbui.exe --test
</pre>
