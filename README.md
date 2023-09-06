NeuroLOTs
=====================================================

## Introduction
NeuroLOTs is a set of libraries and tools for generating neuronal meshes and for
visualizing them at different levels of detail using GPU-based tessellation.
Providing tools for the generation of 3D polygonal meshes that approximate the
membrane of neuronal cells, from the morphological tracings that describe the
morphology of the neurons. The 3D models can be tessellated at different levels
of detail, providing either homogeneous or adaptive resolution along the model.
The soma shape is recovered from the incomplete information of the tracings,
applying a physical deformation model that can be interactively adjusted.

See [NeuroTessMesh web page](http://vg-lab.es/neurotessmesh/) and
[NeuroTessMesh  manual](http://vg-lab.es/neurotessmesh/NeuroTessMeshUserManual.pdf)
for a complete description and sample data tests.

## Dependencies

* Required dependencies:
    * Boost
    * OpenGL
    * glew
    * nsol
    * ReTo
    * eigen3

Note: nsol library is automatically downloaded and built if not found by cmake.

## Building

neurolots has been successfully built and used on Ubuntu 16.04 LTS, Mac OSX High
Sierra and Windows 10 64-bits with Visual Studio 2013. Please ensure that you
build the Release version in order to get the best performance possible.

```bash
git clone https://github.com/vg-lab/neurolots
mkdir neurolots/build && cd neurolots/build
git submodule update --init
cmake .. -DCLONE_SUBPROJECTS=ON
make
```

## Documentation

You can access the online API documentation generated from the source
(using [Doxygen](http://doxygen.org/) ) in
https://vg-lab.github.io/doc/neurolots/
