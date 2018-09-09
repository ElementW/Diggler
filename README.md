# Diggler

A game of cubes and the like.

## Obtaining source

This repository uses git submodules, **do not download it using the "Download ZIP" option!**

Instead, install `git` and your machine, and
```sh
git clone https://github.com/ElementW/Diggler.git
cd Diggler
git submodule update --init
```

## Compiling

Diggler uses CMake to generate its build files. If you have an IDE that supports CMake, import the project.
If you want to compile it using the command line:
```sh
mkdir build
cd build
cmake ..
make -j$(nproc)
```

**Important note**: Diggler does **not** support in-source builds, i.e. running CMake in the top level directory, due to 1. the impossibility to have multiple builds that way, and 2. because of the frequent build structure fuckery it causes. Make sure to configure your IDE/run CMake to have the build directory elsewhere. Disregarding this will have CMake bail out instantly as well as remind you of this one more time.

### Dependencies

Diggler is written in C++14, so you need GCC 7+/clang 3.4+.

You need GLM, OpenAL, OpenGL (ES 2.0), libepoxy, glfw3, LuaJIT, sqlite3 and libsodium.

#### Linux

You should already have OpenGL available thanks to Mesa or proprietary drivers.

Arch: `pacman -S glm openal libepoxy glfw luajit sqlite libsodium`

Debian (Jessie **w/ testing**, or up) & derivatives: `apt-get install libglm-dev libopenal-dev libepoxy-dev libglfw3-dev libx{i,randr}-dev libluajit-5.1-dev libsqlite3-dev libsodium-dev`

Fedora (tested on F24): `dnf install glm-devel openal-soft-devel libepoxy-devel glfw-devel luajit-devel sqlite-devel libsodium-devel`

#### MinGW

MinGW64: `pacman -S cmake gdb git make mingw-w64-x86_64-{gcc,glfw,glm,lib{epoxy,sodium},luajit-git,openal,sqlite3} unzip`
