/**
 * @file    NeuronMeshGenerator.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_NEURON_MESH_GENERATOR__
#define __NEUROLOTS_NEURON_MESH_GENERATOR__

#include <nsol/nsol.h>

#include <iostream>
#include <vector>

namespace neurolots {

class NeuronMeshGenerator {

  public:

    NeuronMeshGenerator( const char * file_name );
    ~NeuronMeshGenerator( void );

    void GenerateStructure( void );
    void GenerateMesh( void );

    std::vector< float > Vertices( void );
    std::vector< unsigned int > Mesh( void );

  private:

    void _GenerateStructure( nsol::SectionPtr section );
    void _GenerateMesh( nsol::SectionPtr section );

    unsigned int _NumNodes( void );
    unsigned int _NumNodes( nsol::SectionPtr section );

    unsigned int _MaxId( void );
    unsigned int _MaxId( nsol::SectionPtr section );



    nsol::NeuronMorphologyPtr _morfo;

    std::vector< int > _ids;
    int _numIds;

    std::vector< float > _vertices;
    std::vector< unsigned int > _mesh;

};

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH_GENERATOR__

//EOF
