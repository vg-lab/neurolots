# neurolots

## git master

## version 0.2.0

* [!69] Improved the compute of the base mesh at the joint nodes. Updated ReTo tag. Updated Changelog and version. Release 0.2 preparation.
* [!67] Added support for vasculature mesh generation and visualization, added morphology extraction example.

## version 0.1.0

* [!66] nlrender and nlgenerator library refactored and commented, added tests and added examples.
* [!65] nlphysics library refactored and commented, added tests.
* [!63] nlgeometry library refactored and commented, added tests and added examples.
* [!61] Readme.md contributions.
* [!62] Fixed NeuroTessMesh.app Qt plugins Runtime error for the Mac OSX version.
* [!60] Added icons for osx bundle and linux.
* [!59] Adde notes to Documentation.
* [!57] Updated tags for Ubuntu 17.XX compilation.
* [!55] Added explicative tips for sliders.
* [!54] Added test to avoid bifurcation interception.
* [!53] ReTo integrated.
* [!51] Hide icons when Brion nor available.
* [!50] Added OSX Qt plug-in finding for app bundle.
* [!47] Fixes for El Capitan OSX.
* [!49] Adaptation for minimum required OpenGL 4.0. Added error window.
* [!48] Added method to get the dataset from NeuronsCollection.
* [!44] Updated to nsol xml loader API.
* [!42] Tessellation method changed to Phong Tessellation and added manifold bifurcation neuron mesh.
* [!43] Neurolots adaped to support the new load on-demand allowed by nsol using Brion.
* [!40] Adapted nlrender Camera class to ZeroEQ and Lexis. Adapted all classes to ZeroEQ, Lexis and GmrvLex.
* [!39] Adaption to the new cmake/common version. Changed tag to cmake/common on .gitexternals.
* [!38] Fixed error: changed the level of tessellation assing to each edge of triangles.
* [!37] Fixed zeq selection sync. Fixed error library export in Windows.
* [!36] Added functionallity to change background, neurons and selected neurons color. Added functionallity to select the piece of neuron to render. Fixed message in neuron color button.
* [!35] Fixed error: Usage message changed to context version param. Added minimum OpenGL context version 4.0.
* [!34] Some error checkings. Deleted default develop message. Changed nsol tag. Added macro EPSILON. Changed repo tags zeq gmrvzeq.
* [!33] Added file dialog to select file to save. Disable/enable show/hide some widgets. Fixed build error: deleted include <GL/freeglut.h>
* [!32] Updated README.md.
* [!31] Renamed qneurolots to NeuroTessMesh. Changed nlgeometry namespace. Changed nlfem namespace. Changed nlgenerator and nlrender namespaces. Renaming qneurolots.ico to neurotessmesh in CMakeSetup.rc. Changed install dir to independents library dirs.
* [!30] Added license.
* [!29] Added feature to dynamically change the tessellation methods between homogeneous and linear. Shaders inserted in libraries. Added visualization configuration interface.
* [!28] Added extraction functionallity. Added functionallity to delete duplicated vertices.
* [!27] Updated compilation to last nsol version.
* [!24] Added CLI and env variables to set OpenGl context related properties. Added about message.
* [!26] Updated README.md.
* [!25] Added regeneration of neuron based in alphas. Added neuron selection interface. Added feature to parameterized reconstruction of soma. Camera animation updated with qtimer.
* [!22] Added neurolots::nlrender::Config. First approach to a Qt-based interface to use nlrender/nlgenerator/nlfem.
* [!23] Fixes for OSX building. Added SYSTEM to Eigen3.
* [!21] Added .gitlab-ci.yml and .gitignore.
* [!20] Created nlrender library. Deleted not used dependencies from nlgenerator and nlrender.
* [!18] Fixed error returning when no shaders path in env. Improvements in parameters reading.
* [!17] Clean nlgenerator. Mesh generator change to nsol new structure. Added separate loaders to NeuronsCollection.
* [!16] Added gmrvzeq as optional dependency. Fixed cmake config link gmrvzeq.
* [!15] First changes to common_package. Fixed libraries generation. Changed *_WITH_* to *_USE_*. Fixed building when using zeq and/or deflect.
* [!14] Added deflect reconnection.
* [!13] Solved problems with deflect default connection and not connected check. Change error usage message. Switched deflect event zoom orientation.
* [!12] Created integration with deflect for image and basic interaction. Added command line argument for host address.
* [!11] Added camera focus and context functionallity.
* [!10] Camera movement changed to spheric. Updated nsol::Columns.
* [!9] Changes to generate common include files and install them. Fixed library compilation. Fixed Windows building.
* [!8] Changed lunchbox to servus. Fixed bugs removing completely lunchbox URI calls and types.
* [!7] Added integration with zeq selection. Added shaders location throught environment variable SHADERS_PATH.
* [!6] Fixes for OSX building.
* [!5] Added string include to enable vs10 building.
* [!3] Camera integrated with zeq. Added compilation option without zeq. Added .gitexternals for cmake common configuration. Added configuration file CPackConfig.cmake. Added rpath to nvidia gl detection.
* [!4] Added CI scripts. CMakeLists.txt clean-ups.
* [!2] NeuronsCollection interface unification: Set Params (lod, tng, dist, colors).
* [!1] Added view projection matrix composition and access funtion.
