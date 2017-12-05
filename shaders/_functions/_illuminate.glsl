vec4 shading( )
{
  vec3 N=normalize( normal );
  float dif = dot( N, L );
  dif = clamp( dif, 0.0, 1.0 );
  return vec4( color * dif, 1.0 ) + vec4( color * 0.3, 1.0 );
}