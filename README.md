# OLC 2024 Game Jam: The Running Man

This template provides a minimal working setup for the HaGameEngine. It includes the 
ability to run headless as well as in the web using Emscripten.

## Installation

To download the source and the required sub-repositories, run:
```bash
git clone --recurse-submodules https://github.com/henrywalters/olc-2024-the-runner
```

As this is meant to use a development version of HaGame, you must first clone the repository.

``git clone git@github.com:henrywalters/HaGameLite.git``

And then update the line in ``cmake/FindHaGame2.cmake`` with the path you cloned it to:

```cmake
set(
        FIND_HAGAME2_PATHS
        /home/dev/path-to-hagamelite
)
```

If you'd like to run the test server, run:

```bash
cd public && npm install && cd ..
```

## Building

To build for desktop, it is sufficient to run:

```bash
mkdir build && cd build
cmake ..
make
```

To build for web:

```bash
cd public
emcmake cmake .
make
```

To run the test server:
```bash
cd public && node server.js && cd ..
```