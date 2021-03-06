# Power Tab Editor

[![Build Status](https://travis-ci.org/powertab/powertabeditor.svg?branch=master)](https://travis-ci.org/powertab/powertabeditor)
[![GitHub release](https://img.shields.io/github/release/powertab/powertabeditor.svg?style=flat)](https://github.com/powertab/powertabeditor/releases)

### Users
* New user? Read the [intro page](https://github.com/powertab/powertabeditor/wiki/Power-Tab-Editor-2.0,-Here-at-last!) to learn more about the project!
* You can get the latest releases from our [downloads page](https://github.com/powertab/powertabeditor/releases).
* You can report bugs or suggest features on our [issue tracker](https://github.com/powertab/powertabeditor/issues).

### Developers
#### Getting the code:

If you're using [GitHub for Windows](https://windows.github.com/), just use the "Clone in Desktop" button.

Otherwise, make sure to use `git clone --recursive` to initialize submodules in the `external` folder:

`git clone --recursive git://github.com/powertab/powertabeditor.git`

If you've already cloned the repository, you can run `git submodule init && git submodule update`.

#### Dependencies:
* [CMake](http://www.cmake.org/) >= 2.8.9
* [Boost 1.55](http://www.boost.org/) or greater
  * Earlier versions may work, but are currently untested
* [Qt 5.x](http://qt-project.org/)
  * If you are compiling with Clang, Qt 5.2 or higher is required.
* [zlib](http://www.zlib.net/)
* (Linux only) - ALSA library (e.g. `libasound2-dev`)
* (Linux only) - MIDI sequencer (e.g. `timidity`)
* (Linux only) - libbfd (e.g. `binutils-dev`)
* A compiler with C++11 support (gcc 4.8+, Clang, VS 2013)

#### Windows:
* Install Git - see https://help.github.com/articles/set-up-git
* Building Boost:
  * Download and extract [Boost 1.56](http://www.boost.org/users/history/version_1_56_0.html) to `C:\Program Files\boost\boost_1_56_0`.
  * Download and extract [zlib](http://www.zlib.net/) to any directory. However, the path must *not* contain any spaces.
  * Open a command prompt (e.g. VS2013 x64 Native Tools Command Prompt) and navigate to the Boost directory.
  * Run `bootstrap` and then `b2 -s ZLIB_SOURCE=/path/to/zlib link=shared address-model=64 variant=debug,release`.
    * Use `address-model=32` for a 32-bit build.
    * If building both 32-bit and 64-bit versions, use `--stagedir=stage32` to place the 32-bit versions under a different directory (`stage32/lib` instead of `stage/lib`), and set `BOOST_LIBRARYDIR` accordingly when running CMake.
* Installing Qt:
  * Download the online installer from http://qt-project.org/downloads.
  * Install the `msvc2013 64-bit` component (or `msvc2013 32-bit` for a 32-bit build)
* Compiling Qt (optional):
  * You may want to do this instead if you intend on building the installer so that Qt can be built without dependencies on huge libraries such as `icu`.
  * [Download](http://qt-project.org/downloads) and extract the source code to a directory such as `C:\Qt\5.2.0`.
  * If necessary, install [Python 2.7.x](https://www.python.org/downloads/) and add it to your PATH.
  * Open a command prompt (e.g. VS2013 x64 Native Tools Command Prompt) and navigate to the Qt directory.
  * Run `configure -opensource -nomake examples -debug-and-release -mp -no-icu -c++11` and accept the license agreement.
  * Run `nmake` and grab a cup of coffee ...
* Install and open CMake, and browse to select the location of the root directory (e.g. `$HOME/Documents/GitHub/powertabeditor`).
* Set the build directory to `$HOME/Documents/GitHub/powertabeditor/build`.
* Use the "Add Entry" button to set the `STRING` `CMAKE_PREFIX_PATH` to the `cmake` directory inside Qt's installation directory (e.g. `C:\Qt\5.2.0\qtbase\lib\cmake`)
* For Windows XP support, set `CMAKE_GENERATOR_TOOLSET` to `v120_xp`.
* Press `Configure` and select your compiler version (e.g. `Visual Studio 12 Win64`, or `Visual Studio 12` for a 32-bit build) and then press `Generate`
* Open the resulting solution (`powertabeditor.sln`) and select `Build Solution` from the `Build` menu.
* Right-click on the `powertabeditor` project and select "Set as Startup Project" before running.

#### Linux:
* These instructions assume a recent Ubuntu/Debian-based system, but the package names should be similar for other package managers.
  * For older Ubuntu systems (such as Ubuntu 12.04) - you may need to [add some PPAs](https://github.com/powertab/powertabeditor/blob/master/.travis.yml) to get updated versions of the dependencies.
* Install dependencies:
  * `sudo apt-get update`
  * `sudo apt-get install cmake qtbase5-dev libboost1.55-dev libboost-date-time1.55-dev libboost-iostreams1.55-dev libboost-program-options1.55-dev libboost-regex1.55-dev libasound2-dev timidity`
  * Optionally, use [Ninja](http://martine.github.io/ninja/) instead of `make` (`sudo apt-get install ninja-build`)
* Build:
  * `mkdir build && cd build`
  * `cmake ..`
    * Add `-DCMAKE_BUILD_TYPE=Debug` for a debug build instead of a `Release` build.
    * Add `-DCMAKE_CXX_COMPILER=clang++` to compile with Clang.
    * Add `-G Ninja` to generate [Ninja](http://martine.github.io/ninja/) build files.
  * `make -j8` or `ninja`
* Run:
  * `./bin/powertabeditor`
  * `./bin/pte_tests` to run the unit tests.

#### OS X:
* Currently tested with Mac OS X 10.9.2 only.
* Install Xcode along with its Command Line Tools.
* Install CMake, Qt 5.2+, libbfd, and Boost
  * Install [Homebrew](http://brew.sh/) and run `./osx/setup.sh`.
  * Install [CMake](http://www.cmake.org/).
* Build (adapt paths to your setup) in `source/` directory
  * `export CC=/usr/bin/clang`
  * `export CXX=/usr/bin/clang++`
  * `export Qt5Widgets_DIR=<PATH_TO_Qt>/clang_64/lib/cmake/Qt5Widgets`
  * `export Qt5Core_DIR=<PATH_TO_Qt>/clang_64/lib/cmake/Qt5Core`
  * `export BOOST_ROOT=/usr/local/Cellar/boost/1.55.0_1`
  * `export BOOST_LIBRARY_DIR=/usr/local/Cellar/boost/1.55.0_1/lib`
  * `export CMAKE_LIBRARY_PATH="/usr/local/Cellar/boost/1.55.0_1/lib:$CMAKE_LIBRARY_PATH"` 
  * `cmake ..`
  * `make`
* Run:
  * `./bin/powertabeditor`
  * `./bin/pte_tests` to run the unit tests.
  
