subroutine( levelDistType )
float linear( vec3 position )
{
  return ( lod - 1.0 ) * clamp( ( 1.0 - length( position
    - cameraPos ) / maxDist ), 0.0, 1.0 );
}