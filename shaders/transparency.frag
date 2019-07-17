#version 400

out vec4 oColor;

uniform sampler2D opaqueTexture;
uniform sampler2D accumTexture;
uniform sampler2D revealageTexture;

void main( void )
{
  // vec4 C0 = texelFetch( opaqueTexture, ivec2( gl_FragCoord.xy)*2 , 0 );
  // vec4 accum = texelFetch( accumTexture, ivec2( gl_FragCoord.xy)*2 + ivec2( -600, 0 ) , 0 );
  // float r = texelFetch( revealageTexture, ivec2( gl_FragCoord.xy)*2 + ivec2( 0,-600 ), 0 ).r;
  // // vec4 sumC1 = vec4( accum.rgb / clamp( accum.a, 1e-4, 5e4), r );
  // // oColor = sumC1 *(1-r) + C0 * r;
  // oColor = vec4( r ) + accum + C0;
  // // oColor = vec4( gl_FragCoord.xy / 600.0f, 0, 0 );


  vec4 C0 = texelFetch( opaqueTexture, ivec2( gl_FragCoord.xy), 0 );
  vec4 accum = texelFetch( accumTexture, ivec2( gl_FragCoord.xy), 0 );
  float r = texelFetch( revealageTexture, ivec2( gl_FragCoord.xy), 0 ).r;
  vec4 sumC1 = vec4( accum.rgb / clamp( accum.a, 1e-4, 5e4), r );
  oColor = sumC1 *(1.0 - r) + C0 * r;
  // oColor = vec4( r ) + accum + C0;
}
