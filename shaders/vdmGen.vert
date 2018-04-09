#version 440

in vec3 inPosition;
in vec3 inDisplace;

out vec3 displace;

void main ( )
{
  displace = inDisplace;
  gl_Position = vec4( inPosition, 1.0 );
}