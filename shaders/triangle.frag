#version 400
subroutine vec3 colorFuncType( void );
subroutine uniform colorFuncType colorFunc;

subroutine void transparencyFuncType( void );
subroutine uniform transparencyFuncType transparencyFunc;

out vec4 accum;
out vec4 revealage;

in vec3 normal;
in vec3 vColor;
in vec3 L;

uniform vec3 color;
uniform float alpha;

#include("_functions/_illuminate.glsl")

subroutine( colorFuncType )
vec3 globalColor( void ){ return color; }

subroutine( colorFuncType )
vec3 vertexColor( void ){ return vColor; }

subroutine( transparencyFuncType )
void transparencyEnable( void )
{
  vec3 colorI = shading( colorFunc( ), normal, L ).xyz * alpha;
  float w = alpha * max( 1e-2, 3e3 * ( 1 - pow( gl_FragCoord.z, 3 )));
  accum = vec4( colorI, alpha ) * w;
  revealage = vec4( alpha );
}

subroutine( transparencyFuncType )
void transparencyDisable( void )
{
  accum = shading( colorFunc( ), normal, L );
}

void main( )
{
  transparencyFunc( );
}
