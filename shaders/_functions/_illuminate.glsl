vec4 shading( vec3 color_, vec3 normal_, vec3 L_ )
{
  vec3 N=normalize( normal_ );
  float dif = dot( N, L_ );
  dif = clamp( dif, 0.0, 1.0 );
  return vec4( color_ * dif, 1.0 ) + vec4( color_ * 0.3, 1.0 );
}