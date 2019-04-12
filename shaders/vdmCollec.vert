#version 400
subroutine float levelDistType( vec3 position );
subroutine uniform levelDistType levelDist;

in vec3 inVertex;
in vec2 inSpineCoord;
in mat4 inModel;

out vec3 vPosition;
out ivec2 vSpineCoord;
out float vlot;
out mat4 vModel;

uniform float lod;
uniform float maxTexel;
uniform float maxDist;
uniform mat4 viewModel;

subroutine( levelDistType )
float linear( vec3 position )
{
  return clamp( lod * clamp(( 1.0 - length( position ) / maxDist ), 0.0, 1.0 ),
    4.0, maxTexel );
}

subroutine( levelDistType )
float homogeneous( vec3 position )
{
  return clamp( lod, 4.0, maxTexel );
}

void main( void )
{
  vPosition = inVertex;
  vModel = inModel;
  vSpineCoord = ivec2( ceil( inSpineCoord ));
  vec3 position = ( viewModel * vModel * vec4( inVertex, 1.0 )).xyz;
  vlot = levelDist( position );
}
