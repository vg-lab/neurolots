#version 400
subroutine vec3 colorFuncType( void );
subroutine uniform colorFuncType colorFunc;

in vec3 vColor;

out vec4 oColor;

uniform vec3 color;

subroutine( colorFuncType )
vec3 globalColor( void ){ return color; }

subroutine( colorFuncType )
vec3 vertexColor( void ){ return vColor; }

void main( )
{
  oColor = vec4( colorFunc( ), 1.0 );
}
