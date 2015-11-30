NeuroLOTs - Neuron Level Of Detail with TeSsellation
=====================================================

## Introduction

NeuroLOTs is a set of libraries and tools for generating neuronal meshes and 
visualize them using different levels of detail using tessellation.

## Dependencies

* Required dependencies:
    * nsol
    * vmmlib (required by nsol)
    * eigen3

* Optional dependencies:
    * Qt5.4: enables building qneurolots viewer
    * freeglut: enables building neurolots simple viewer
    * zeq: enables syncing selections and syncing camera
    * gmrvzeq: enables sending focus messages

Note: nsol and vmmlib are automatically download and build if not found by CMake.

In order to check which optional dependencies have been used the following 
command can be used:

```bash 
./qneurolots --version
```

## Building

NeuroLOTs has been succesfully built and used on Ubuntu 14.04, Mac OSX Yosemite
and Windows 7/8 Visual Studio 2013 Win64. Please ensure that Release version is
built in order to get the best performance possible.

```bash
git clone https://gitlab.gmrv.es/hbp/neurolots.git
mkdir neurolots/build && cd neurolots/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Running

All the features can be accesed using the GUI app qneurolots but some of them
are also available through CLI. Running the following can provide a list of
the CLI arguments qneurolots accepts:

```bash 
./qneurolots --help
```