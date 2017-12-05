void createGeom( )
{
  outValue0 = position[0];
  outValue1 = normal[0];
  gnormal = normal[0];
  gL = L[0];
  gl_Position = gl_in[0].gl_Position;
  EmitVertex( );

  outValue0 = position[1];
  outValue1 = normal[1];
  gnormal = normal[1];
  gL = L[1];
  gl_Position = gl_in[1].gl_Position;
  EmitVertex( );
  
  outValue0 = position[2];
  outValue1 = normal[2];
  gnormal = normal[2];
  gL = L[2];
  gl_Position = gl_in[2].gl_Position;
  EmitVertex( );
  EndPrimitive( );
}