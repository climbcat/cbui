# cbui

[C-baselayer user interface]

Written in C-style C++:

- Vector & matrix 3D math
- Software 2D Renderer and immediate-mode UI
- Text font atlas generation
- Text layout for the renderer
- Software 3D Renderer which handles dots and lines
- Algorithms: Octree, Index sets, Tokenizer

## dependencies

This project depends on my baselayer repository; https://github.com/climbcat/baselayer

Also depends on two minimalist OpenGL and OS wrapper libraries; glew and glfw.

On Ubuntu and similar platforms:

<pre>
sudo apt install libglew-dev
sudo apt install libglfw3-dev
</pre>

## test

Having installed dependencies, do the following to run basic tests/examples:

<pre>
git clone https://github.com/climbcat/baselayer/
git clone https://github.com/climbcat/cbui/
cd cbui/build
cmake ..
make
./graphics
</pre>
