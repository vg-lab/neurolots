#version 400
subroutine vec3 colorFuncType( void );
subroutine uniform colorFuncType colorFunc;

out vec4 oColor;

in vec3 normal;
in vec3 vColor;
in vec3 L;

uniform vec3 color;

subroutine( colorFuncType )
vec3 globalColor( void ){ return color; }

subroutine( colorFuncType )
vec3 vertexColor( void ){ return vColor; }

#include("_functions/_illuminate.glsl")

void main( )
{
  oColor = shading( colorFunc( ), normal, L );
}
