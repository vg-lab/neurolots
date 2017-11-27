subroutine( levelDistType )
float linear( vec3 position )
{
  return ( lod ) * clamp( ( 1.0 - length( position )
    / maxDist ), 0.0, 1.0 );
}
