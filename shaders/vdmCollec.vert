#version 400

in vec3 inVertex;
in vec2 inSpineCoord;
in mat4 inModel;

out vec3 vPosition;
out ivec2 vSpineCoord;
out float vlot;
out mat4 vModel;

uniform float lod;
uniform float maxTexel;

void main( void )
{
  vPosition = inVertex;
  vModel = inModel;
  vSpineCoord = ivec2( ceil( inSpineCoord ));
  // vlot = clamp( lod, 3.0, 64.0 );
  vlot = clamp( lod, 1.0, maxTexel );
}
