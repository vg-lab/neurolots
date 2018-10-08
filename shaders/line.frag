#version 400
subroutine vec3 colorFuncType( void );
subroutine uniform colorFuncType colorFunc;

subroutine void transparencyFuncType( void );
subroutine uniform transparencyFuncType transparencyFunc;

out vec4 accum;
out vec4 revealage;

in vec3 vColor;

uniform vec3 color;
uniform float alpha;

subroutine( colorFuncType )
vec3 globalColor( void ){ return color; }

subroutine( colorFuncType )
vec3 vertexColor( void ){ return vColor; }

subroutine( transparencyFuncType )
void transparencyEnable( void )
{
  vec3 colorI = colorFunc( ) * alpha;
  float w = alpha * max( 1e-2, 3e3 * ( 1 - pow( gl_FragCoord.z, 3 )));
  accum = vec4( colorI, alpha ) * w;
  revealage = vec4( alpha );
}

subroutine( transparencyFuncType )
void transparencyDisable( void )
{
  accum = vec4( colorFunc( ), 1.0 );
}

void main( )
{
  transparencyFunc( );
}
