#version 400
subroutine float levelDistType( vec3 position );
subroutine uniform levelDistType levelDist;

in vec3 inVertex;
in vec3 inCenter;
in vec3 inTangent;

out vec3 vPosition;
out vec3 vCenter;
out vec3 vTangent;
out float vlot;

uniform mat4 viewModel;
uniform float lod;
uniform float maxDist;

#include("_functions/_linearDist.glsl")
#include("_functions/_homogeneousDist.glsl")

void main( void )
{
  vPosition = ( viewModel * vec4(inVertex, 1.0 )).xyz;
  vCenter = ( viewModel * vec4( inCenter, 1.0 )).xyz;
  vTangent = ( viewModel * vec4( inTangent, 0.0 )).xyz;
  vlot = levelDist( vCenter );
}
