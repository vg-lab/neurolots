#version 400
out vec4 oColor;

in vec3 normal;
in vec3 L;

uniform vec3 color;

#include("_functions/_illuminate.glsl")

void main( )
{
  oColor = shading( );
}
