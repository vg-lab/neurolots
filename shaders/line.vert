#version 400

in vec3 inVertex;

uniform mat4 viewModel;
uniform mat4 proy;

void main( void )
{
  gl_Position = proy * viewModel * vec4( inVertex , 1.0 );
}
