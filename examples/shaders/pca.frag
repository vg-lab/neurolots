#version 400

in vec3 normal;
in vec3 l;
in vec2 coord;

out vec4 oColor;

uniform sampler2D mainTex;

void main( void )
{
  vec3 color = texture( mainTex, coord ).xyz;
  vec3 N = normalize( normal );
  vec3 L = normalize( l );
  float diff = dot( N, L );
  diff = clamp( diff, 0.0, 1.0 );
  oColor = vec4( diff*color*0.8+color*0.2, 1.0 );
}