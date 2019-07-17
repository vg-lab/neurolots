#version 400

in vec3 inVertex;
in vec3 inColor;

out vec3 vColor;

uniform mat4 viewModel;
uniform mat4 proy;

void main( void )
{
  vColor = inColor;
  gl_Position = proy * viewModel * vec4( inVertex , 1.0 );
}
