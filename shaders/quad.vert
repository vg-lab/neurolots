#version 400
subroutine float levelDistType( vec3 position );
subroutine uniform levelDistType levelDist;

layout( location = 0 ) in vec3 inVertex;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec3 inCenter;

out vec3 vPosition;
out vec3 vCenter;
out vec3 vColor;
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
  vColor = inColor;
  vlot = levelDist( vCenter );
}
