#version 400
subroutine float levelDistType( vec3 position );
subroutine uniform levelDistType levelDist;

in vec3 inVertex;

out vec3 vPosition;
out float vlot;

uniform float lod;
uniform float maxTexel;
uniform float maxDist;
uniform mat4 viewModel;

subroutine( levelDistType )
float linear( vec3 position )
{
  return clamp( lod * clamp( 1.0 - length( position ) / maxDist, 0.0, 1.0 ),
    3.0, maxTexel );
}

subroutine( levelDistType )
float homogeneous( vec3 position )
{
  return clamp( lod, 3.0, maxTexel );
}


void main( void )
{
  vPosition = inVertex;
  vec3 position = ( viewModel * vec4( inVertex, 1.0)).xyz;
  vlot = levelDist( position );
}
