# NeuroLOTs

Libraries and tools for generating neuronal meshes

NeuroLOTs is a set of libraries and tools for generating neuronal meshes and for visualizing them at different levels of detail using GPU-based tessellation. It provides tools for generating 3D polygonal meshes that approximate the membrane of neuronal cells, starting from the morphological tracings that describe the morphology of the neurons. The 3D models can be tessellated at different levels of detail, providing either homogeneous or adaptive resolution along the model. The soma shape is recovered from the incomplete information of the tracings, by applying a physical deformation model that can be interactively adjusted.

The project is organized in four libraries:

- `nlgeometry`
- `nlphysics`
- `nlgenerator`
- `nlrender`

## Building

```bash
git clone https://github.com/vg-lab/neurolots.git
cd neurolots

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

In-source builds are not allowed, so always configure into a separate build directory. If `CMAKE_BUILD_TYPE` is not specified, it defaults to `Debug`, so make sure to build the `Release` version to get the best performance possible.

## CMake options

| Option | Default | Description |
| ------ | ------- | ----------- |
| `BUILD_SHARED_LIBS` | `ON` | Build the libraries as shared libraries (set to `OFF` for static) |
| `BUILD_DOCS` | `ON` | Generate Doxygen documentation when Doxygen is found |
| `NEUROLOTS_WITH_EXAMPLES` | `ON` | Build the programs in `examples/` (only when neurolots is the top-level project) |
| `NEUROLOTS_OPTIONALS_AS_REQUIRED` | `OFF` | Treat optional dependencies as required |

Other options are set automatically depending on whether extra libraries are found, and can be overridden:

| Option | Default | Description |
| ------ | ------- | ----------- |
| `NEUROLOTS_USE_GLUT` | `ON` if GLUT is found | Enable GLUT support (used by some examples) |

Example:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DNEUROLOTS_WITH_EXAMPLES=OFF -DBUILD_DOCS=OFF
```

## Dependencies

neurolots resolves its dependencies through CMake. Required dependencies are looked up with `find_package()` and must be installed on your system or made discoverable by adding their install location to `CMAKE_PREFIX_PATH`.

For some dependencies, CMake first looks for an installed copy with `find_package()`. Only if none is found is the dependency downloaded and built using `FetchContent`. This supports two workflows:

- System packages: install the dependencies with your package manager, or point CMake at them with `CMAKE_PREFIX_PATH`.
- Automatic fetching: on a system without them installed, the missing dependencies are fetched automatically during configuration.

## Using neurolots in your project

neurolots provides one CMake library target per library, all under the `neurolots::` namespace. Link against the ones you need:
 
| Target | Library |
| ------ | ------- |
| `nlgeometry` | Geometry data structures and utilities |
| `nlphysics` | Physical deformation model (used for the soma shape) |
| `nlgenerator` | Generation of neuronal meshes from morphological tracings |
| `nlrender` | Rendering with GPU-based tessellation at different levels of detail |
 
The namespaced form is recommended for downstream projects.


### With `find_package`

Install neurolots, then consume it:

```bash
cmake --install build --prefix /your/prefix
```

```cmake
find_package(neurolots REQUIRED)
target_link_libraries(my_app PRIVATE neurolots::nlrender)
```

Add `-DCMAKE_PREFIX_PATH=/your/prefix` when configuring your project if the prefix is not a default search location.

### With `FetchContent`

```cmake
include(FetchContent)
FetchContent_Declare(
  neurolots
  GIT_REPOSITORY https://github.com/vg-lab/neurolots.git
  GIT_TAG        master  # pin to a tag or commit
)
FetchContent_MakeAvailable(neurolots)

target_link_libraries(my_app PRIVATE neurolots::nlrender)
```

## Documentation

The API documentation is generated from the source with Doxygen. To build it locally (requires Doxygen and `BUILD_DOCS=ON`):

```bash
cmake --build build --target neurolots_doxygen
```

The HTML output is written to `build/docs_doxygen/html`.

## License

neurolots is released under the [GNU General Public License v3.0](LICENSE.txt).

Developed at the [Visualization and Graphics Lab (VG-Lab)](https://github.com/vg-lab).
