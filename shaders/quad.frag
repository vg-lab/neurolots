#version 400
out vec4 oColor;

in vec3 normal;
in vec3 L;

uniform vec3 color;

void main( )
{
  vec3 N=normalize( normal );
  float dif = dot( N, L );
  dif = clamp( dif, 0.0, 1.0 );
  oColor = vec4( color * dif, 1.0 ) + vec4( color * 0.3, 1.0 );
}
