#version 400

in vec2 inPosition;

out vec3 l;
out vec3 normal;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main( void )
{
  normal = ( view * model * vec4( 0.0, 0.0, 1.0, 0.0 )).xyz;
  l = -( view * model * vec4( inPosition, 0.0, 1.0 )).xyz;
  gl_Position = proj * view * model * vec4( inPosition, 0.0, 1.0 );
}