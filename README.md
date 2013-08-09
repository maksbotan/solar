Simple solar system OpenGL model.

Features
========

Right now the model has following features:

* Earth's orbit plane (ecliptic) is inclined to the Sun's equator plane at the real angle
* Earth's axis is inclined to ecliptic at the real angle
* Moon orbit plane is inclined to ecliptic at the real angle
* Time goes in real scale: 1 day per 1 second by default, configurable in sources
* Earth's, Moon's and Sun's periods of revolutions are real

Controls
========

* w, a, s, d, z, x - camera movement along three axes
* left, right, up, down arrows, PgUp and PgDown - camera rotation along three axes
* -, + - zooming
* r - return camera to initial position
* q - quit program

Compilation
===========

Linux
-----

You will need:

* Working C compiler (gcc or clang)
* OpenGL headers and libraries
* FreeGLUT headers and libraries
* GNU compatible make

Compilation is straightforward:

    make CC=gcc

or

    make CC=clang # default

Makefile supports CFLAGS and LDFLAGS variables.

Windows
-------

You will need:

* Visual Studio with C++ compiler or Windows SDK (cl.exe, link.exe and nmake.exe)
* OpenGL headers and .lib's. I guess they are installed with Visual Studio
* FreeGLUT library from <http://files.transmissionzero.co.uk/software/development/GLUT/freeglut-MSVC.zip>
* `glext.h` header from <http://www.opengl.org/registry/oldspecs/glext.h>

First of all, unpack `freeglut-MSVC.zip` to this directory. Then copy `glext.h` to `freeglut\include\GL`. Finally launch "Visual Studio command prompt" from Start menu, `cd` into this directory and type:

    nmake -f Makefile.vc

If all goes well, you will get `solar.exe` file. In order to run it, copy `freeglut.dll` from `freeglut\bin` to this directory.

*Note*: if you have 64-bit Windows, you'll probably need to add `\x64` to freeglut paths in `Makefile.vc`