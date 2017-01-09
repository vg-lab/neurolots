#version 400
layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in vec3 position[3];
in vec3 normal[3];
in vec3 L[3];

out vec3 gnormal;
out vec3 gL;
out vec3 outValue0;
out vec3 outValue1;

#include("_partials/_geom.glsl")

void main( void )
{
  createGeom( );
}
