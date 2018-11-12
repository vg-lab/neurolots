#version 400
out vec4 oColor;

in vec3 L;
// in vec3 color;
in vec3 normal;

uniform vec3 color;

#include("_functions/_illuminate.glsl")

void main( )
{

  oColor = shading( normal, L, color );
}
