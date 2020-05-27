#version 400

layout( location = 0 ) in vec3 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec3 l;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main( void )
{
  normal = ( view * model * vec4( inNormal, 0.0 )).xyz;
  l = -( view * model * vec4( inPosition, 1.0 )).xyz;
  gl_Position = proj * view * model * vec4( inPosition, 1.0 );
}