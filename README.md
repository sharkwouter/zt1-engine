# ZT1-Engine

ZT1-Engine is an open source engine for Zoo Tycoon 1 that is still very much work in progress.

Currently it has the following features:
- Read `zoo.ini`.
- Map the resources found in ztd files.
- Load tga files from ztd files, like the menu background.
- Load the into music.
- Open a window.

Please do not use this unless you want to help make an open source engine for Zoo Tycoon a reality. This is very far from playable.

## Building

To build ZT1-Engine, first makes sure the following dependencies are installed:

- libzip-dev
- libsdl2-dev
- libsdl2-image-dev
- libsdl2-mixer-dev
- libsdl2-ttf-dev

Then run the following commands:

```
mkdir build
cd build
cmake ..
make
```

Now copy all the Zoo Tycoon files into the build directory and start it with:

```
./zt1-engine
```

## License

ZT1-Engine is available under the following MIT license:

```
Copyright (c) 2025 Wouter (sharkwouter) Wijsman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
