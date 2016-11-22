# FileInsight

FileInsight is a graphical file type identifier using libmagic (i.e. the Unix `file` command), Qt 5, and TrID as its backends. It is written in C++ and has been tested to work on both Windows and Linux.

## Compilation / Installation

### Linux

Linux installation is fairly straightforward, because the toolchain and dependencies can easily be fetched from most system repositories. For icon display, FileInsight will simply use the icon theme configured on your desktop.

1) Install Qt 5's development suite, Qt Creator, and libmagic with its development files. On a Debian / Ubuntu system, this corresponds to `apt-get install qt5-default libmagic-dev qtcreator`.

2) Compile the app with Qt Creator.

3) Optionally, you can download TrID from http://mark0.net/soft-trid-e.html for the TrID backend. TrID is free for personal and non-commercial use, but is *NOT* under a FOSS license.

### Windows

Windows builds are a pain (no, really!). Instead, you may want to simply grab a Windows build from [the releases page](https://github.com/GLolol/FileInsight/releases).

#### Getting all the required components

1) Download and install MinGW and MSYS using this guide: http://www.mingw.org/wiki/Getting_Started

2) Install Qt with MinGW from https://www.qt.io/download/..

3) Fetch the sources for [libmagic/file](https://github.com/file/file) and its dependency [libgnurx](https://github.com/glolol/libgnurx)

#### Building libmagic/file

4) Create an empty folder (prefix) to use as the prefix for building libgnurx and libmagic/file.

5) Compile libgnurx into the empty prefix you created: in a MinGW shell, navigate to the source folder and run `./configure --prefix=THE FOLDER YOU CHOSE && make && make install`

6) Compile libmagic/file into the empty prefix you created: in a MinGW shell, navigate to the source folder and run:

- `export TARGET=THE FOLDER YOU CHOSE`
- `autoreconf -f -i`
- `LDFLAGS="-L$TARGET/lib -static-libgcc" CFLAGS="-I$TARGET/include" ./configure --prefix=$TARGET`
- `make && make install`

7) Now that libmagic and libgnurx have been built, copy the contents of the libmagic build folder into the `thirdparty/` directory of FileInsight's source. On Windows, this is where the toolchain will try to load its dependency libraries from.

#### Building FileInsight (MinGW)

8) You should be able to build FileInsight now using Qt Creator + MinGW. However, in order for the resulting binary to run, you must copy the following into the directory of your resultant build:
    - `bin/libgnurx-0.dll` from the libmagic build
    - `bin/libmagic-1.dll` from the libmagic build
    - `share/misc/magic.mgc` (libmagic's data file) from the libmagic build
    - `libgcc_s_dw2-1.dll` from your MinGW root (e.g. `C:\MinGW\bin`)

#### Making a Redistributable Build

9) Copy the `windows-build-skeleton/` folder in FileInsight's source to somewhere else (this will be your "Windows build folder"). This folder includes the relevant copyright information that makes the build legally distributable.

10) Use [windeployqt](https://doc.qt.io/qt-5/windows-deployment.html) to copy the required Qt libraries to the Windows build folder: in a command line, run `windeploy PATH-TO-FILEINSIGHT.EXE`.

11) Copy `libgnurx-0.dll`, `libmagic-1.dll`, `libgcc_s_dw2-1.dll`, and `magic.mgc` into the Windows build folder.

12) Optional: for TrID backend support, extract TrID from http://mark0.net/soft-trid-e.html it into the `thirdparty` directory in the Windows build folder.

13) Optional: for icon display, extract a prebuilt copy of the Oxygen icon theme (from a source like [Debian](https://packages.debian.org/sid/all/oxygen-icon-theme/download)) into the `icons/` folder. Since simple tarball extraction may create (broken) Unix symlinks, they must be expanded so that FileInsight can actually load them.
    - You can use the bundled `expand-symlinks.py` in the `scripts/` folder for this purpose: `scripts/expand-symlinks.py PATH_TO_OXYGEN_ICONS`
