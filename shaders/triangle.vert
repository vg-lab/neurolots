#version 400
subroutine float levelDistType( vec3 position );
subroutine uniform levelDistType levelDist;

in vec3 inVertex;
in vec3 inCenter;
in vec3 inTangent;

out vec3 vPosition;
out vec3 vCenter;
out float vlot;

uniform mat4 model;
uniform vec3 cameraPos;
uniform float lod;
uniform float maxDist;

#include("_partials/_linearDist.glsl")
#include("_partials/_homogeneousDist.glsl")

void main( void )
{
  vPosition = ( model * vec4(inVertex, 1.0 )).xyz;
  vCenter = ( model * vec4( inCenter, 1.0 )).xyz;
  vlot = levelDist( vCenter ) * length( inTangent ) + 1.0;
}
