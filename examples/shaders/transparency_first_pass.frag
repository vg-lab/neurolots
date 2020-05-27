#version 400

subroutine void transparencyFuncType( vec3 color );
subroutine uniform transparencyFuncType transparencyFunc;

in vec3 normal;
in vec3 l;

layout( location = 0 ) out vec4 accum;
layout( location = 1 ) out vec4 revelage;

uniform float alpha;


subroutine( transparencyFuncType )
void transparencyEnable( vec3 color )
{
  vec3 colorI = color * alpha;
  float w = alpha * max( 1e-2, 3e3 * ( 1 - pow( gl_FragCoord.z, 3 )));
  accum = vec4( colorI, alpha ) * w;
  revelage = vec4( alpha );
}

subroutine( transparencyFuncType )
void transparencyDisable( vec3 color )
{
  accum = vec4( color, 1.0 );
}

void main( void )
{
  vec3 color = vec3( 0.5, 0.8, 0.2 );
  vec3 N = normalize( normal );
  vec3 L = normalize( l );
  float diff = dot( N, L );
  diff = clamp( diff, 0.0, 1.0 );
  vec3 outColor = diff * color * 0.8 + color * 0.2;
  transparencyFunc( outColor );
}