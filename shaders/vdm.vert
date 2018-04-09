#version 400

in vec3 inVertex;

out vec3 vPosition;
out float vlot;

uniform float lod;
uniform float maxTexel;

void main( void )
{
  vPosition = inVertex;
  // vlot = clamp( lod, 3.0, 64.0 );
  vlot = clamp( lod, 1.0, maxTexel );
}
