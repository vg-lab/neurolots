NeuroTessMesh 
=====================================================

## Introduction
NeuroLOTs is a set of libraries and tools for generating neuronal meshes and for 
visualizing them at different levels of detail using GPU-based tessellation.
As a part of NeuroLOTS, NeuroTessMesh provides a visual environment for the 
generation of 3D polygonal meshes that approximate the membrane of neuronal 
cells, from the morphological tracings that describe the morphology of the 
neurons. The 3D models can be tesselated at different levels of detail, 
providing either homogeneus or adaptive resolution along the model. The soma 
shape is recovered from the incomplete information of the tracings, applying a 
physical deformation model that can be interactively adjusted.

## Dependencies

* Required dependencies:
    * nsol
    * vmmlib (required by nsol)
    * eigen3

* Optional dependencies:
    * Qt5.4: enables building neurotessmesh viewer
    * ZeroEQ: enables syncing selections and syncing camera
    * Lexis: provides de base vocabulary for ZeroEQ for sync operations
    * gmrvLex: enables sending focus messages

Note: nsol and vmmlib are automatically downloaded and built if not found by 
CMake.

In order to check which optional dependencies have been used, the following 
command can be used:

```bash 
./neurotessmesh --version
```

## Building

NeuroTessMesh has been successfully built and used on Ubuntu 14.04, Mac OSX Yosemite
and Windows 7/8 Visual Studio 2013 Win64. Please ensure that Release version is
built in order to get the best performance possible.

```bash
git clone https://gitlab.gmrv.es/hbp/neurolots.git
mkdir neurolots/build && cd neurolots/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Running

All the features can be accesed using the neurotessmesh GUI but some of them
are also available through CLI. Running the following can provide a list of
the CLI arguments neurotessmesh accepts:

```bash 
./neurotessmesh --help
```