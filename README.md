Simple solar system OpenGL model.

Features
========

Right now the model has following features:

* Following planets are present:
    - Mercury
    - Venus
    - Earth with Moon
    - Mars
* Planets' orbit planes (ecliptic) are inclined to the Sun's equator plane at the correct angle
* Planets' axial tilts are correct
* Moon orbit plane is inclined to ecliptic at the correct angle
* Ascending nodes of orbits are placed correctly
* Arguments of perihelion of orbits are respected
* Time goes in real scale: 1 day per 1 second by default, configurable in sources
* All stellar bodies orbital and siderial periods of revolutions are correct
* All size ratios are correct

Controls
========

* w, a, s, d, z, x - camera movement along three axes
* left, right, up, down arrows, PgUp and PgDown - camera rotation along three axes
* t - move camera to the Earth
* r - return camera to initial position
* f - toggle fullscreen mode
* o - toggle orbits
* q - quit program

Compilation
===========

Linux
-----

You will need:

* Working C++ compiler (g++ or clang++)
* OpenGL headers and libraries
* SDL2 and SLD2_ttf headers and libraries
* GNU compatible make

Compilation is straightforward:

    make CXX=g++

or

    make CXX=clang++

Makefile supports CXXFLAGS and LDFLAGS variables.

Windows
-------

You will need:

* Visual Studio with C++ compiler or Windows SDK (cl.exe, link.exe and nmake.exe)
* OpenGL headers and .lib's. I guess they are installed with Visual Studio
* SDL2 development libraries from <http://libsdl.org/download-2.0.php> for Visual C++
* SDL2_ttf development libraries from <http://www.libsdl.org/projects/SDL_ttf/> for Visual C++
* `glext.h` header from <http://www.opengl.org/registry/oldspecs/glext.h>

First of all, unpack SDL2 and SDL2_ttf zip's to this directory. Then copy `glext.h` to `include\GL` and move all files from `lib\YOURARCH` to `lib`, where `YOURARCH` is either `x86` or `x64`. Finally launch "Visual Studio command prompt" from Start menu, `cd` into this directory and type:

    nmake -f Makefile.vc

If all goes well, you will get `solar.exe` file. In order to run it, copy all `*.dll` files from `lib` to this directory.

Licensing
=========

The program itself is licensed under 2-clause BSD license, see `LICENSE`. Textures are taken from <http://www.celestiamotherlode.net>, which claims they are free for non-commercial use. `Vera.ttf` font file is from `ttf-bitstream-vera` font package with compatible license.
