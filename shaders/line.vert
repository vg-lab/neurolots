#version 400

layout( location = 0 ) in vec3 inVertex;
layout( location = 1 ) in vec3 inColor;

out vec3 vColor;

uniform mat4 viewModel;
uniform mat4 proy;

void main( void )
{
  vColor = inColor;
  gl_Position = proy * viewModel * vec4( inVertex , 1.0 );
}
