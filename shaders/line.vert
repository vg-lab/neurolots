#version 400

layout( location = 0 ) in vec3 inVertex;
layout( location = 1 ) in vec3 inColor;

out vec3 vColor;

uniform mat4 view;
uniform mat4 model;
uniform mat4 proj;

void main( void )
{
  vColor = inColor;
  gl_Position = proj * view * model * vec4( inVertex , 1.0 );
}
