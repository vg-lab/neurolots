/**
 * @file    vshader.glsl
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#version 440

in vec3 inVertex;
in vec3 inCenter;
in vec3 inTangent;
out vec3 vPosition;
out vec3 vCenter;
out vec3 vTangent;
out float vlot;

uniform mat4 proy;
uniform mat4 view;
uniform vec3 desp;

uniform vec3 cameraPos;

uniform float lod;
uniform float maxDist;

float levelDist( vec3 pos)
{
  return ( lod - 1 ) * clamp( ( 1.0 - length( pos - cameraPos ) / maxDist ), 
         0.0, 1.0 ) + 1;
}

void main( void )
{
  vPosition = inVertex + desp;
  vCenter = inCenter + desp;
  vTangent = inTangent;
  vlot = levelDist( vCenter );
}
