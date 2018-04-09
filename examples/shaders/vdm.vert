#version 400

in vec3 inPosition;
in vec3 inNormal;

out vec3 l;
out vec3 normal;
out vec2 coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main( void )
{
  coord = vec2( inPosition.x + 1, 1 - inPosition.y ) / 2.0f;
  normal = ( view * model * vec4( inNormal, 0.0 )).xyz;
  l = -( view * model * vec4( inPosition, 1.0 )).xyz;
  gl_Position = proj * view * model * vec4( inPosition, 1.0 );
}