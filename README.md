# Diggler

A game of cubes and the like.

## Compiling

This repository uses git submodules, **do not download it using the "Download ZIP" option!**

Instead, install `git` and your machine, and
```sh
git clone https://github.com/ElementW/Diggler.git
cd Diggler
git submodule update --init
```

### Dependencies

Diggler is written in C++14, so you need GCC 5 or more.

You need GLM, OpenAL, OpenGL (ES 2.0), libepoxy, glfw3 and LuaJIT.

#### Linux

You should already have OpenGL available thanks to Mesa or proprietary drivers.

Arch: `pacman -S glm openal libepoxy glfw luajit`

Debian (Jessie **w/ testing**, or up) & derivatives: `apt-get install libglm-dev libopenal-dev libepoxy-dev libglfw3-dev libx{i,randr}-dev libluajit-5.1-dev`

Fedora (tested on F24): `dnf install glm-devel openal-soft-devel libepoxy-devel glfw-devel luajit-devel`
