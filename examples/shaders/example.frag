#version 400

in vec3 normal;
in vec3 l;

out vec4 oColor;

void main( void )
{
  vec3 color = vec3( 0.5, 0.8, 0.2 );
  vec3 N = normalize( normal );
  vec3 L = normalize( l );
  float diff = dot( N, L );
  diff = clamp( diff, 0.0, 1.0 );
  oColor = vec4( diff*color*0.8+color*0.2, 1.0 );
}
