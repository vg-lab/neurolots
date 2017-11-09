#include <nlgeometry/nlgeometry.h>

class Cube
  : public nlgeometry::Mesh
{

public:

  Cube( void )
    : nlgeometry::Mesh( )
  {
    init( );
  }

  ~Cube( void )
  {

  }

  virtual void init( void )
  {
    std::cout << "hello" << std::endl;
    nlgeometry::VertexPtr vertex0 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( -1.0f, 1.0f, 1.0f ));
    nlgeometry::VertexPtr vertex1 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( -1.0f, -1.0f, 1.0f ));
    nlgeometry::VertexPtr vertex2 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( 1.0f, -1.0f, 1.0f ));
    nlgeometry::VertexPtr vertex3 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( 1.0f, 1.0f, 1.0f ));
    nlgeometry::VertexPtr vertex4 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( -1.0f, 1.0f, -1.0f ));
    nlgeometry::VertexPtr vertex5 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( -1.0f, -1.0f, -1.0f ));
    nlgeometry::VertexPtr vertex6 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( 1.0f, -1.0f, -1.0f ));
    nlgeometry::VertexPtr vertex7 =
      new nlgeometry::OrbitalVertex( Eigen::Vector3f( 1.0f, 1.0f, -1.0f ));

    _vertices.resize( 8 );
    _vertices[0] = vertex0; _vertices[1] = vertex1;
    _vertices[2] = vertex2; _vertices[3] = vertex3;
    _vertices[4] = vertex4; _vertices[5] = vertex5;
    _vertices[6] = vertex6; _vertices[7] = vertex7;

    _quads.resize( 6 );
    _quads[0] = new nlgeometry::Facet( vertex0, vertex1, vertex3, vertex2 );
    _quads[1] = new nlgeometry::Facet( vertex3, vertex2, vertex7, vertex6 );
    _quads[2] = new nlgeometry::Facet( vertex7, vertex6, vertex4, vertex5 );
    _quads[3] = new nlgeometry::Facet( vertex4, vertex5, vertex0, vertex1 );
    _quads[4] = new nlgeometry::Facet( vertex4, vertex0, vertex7, vertex3 );
    _quads[5] = new nlgeometry::Facet( vertex1, vertex5, vertex2, vertex6 );
  }

};
