/**
 * @file    tcsahder.glsl
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#version 440

layout(vertices=4) out;
in vec3 vPosition[];
in vec3 vCenter[];
in vec3 vTangent[];
in float vlot[];
out vec3 tcCenter[];
out vec3 tcPosition[];
out float r[];
out vec3 tcNormal[];
out vec3 m[];

uniform float lod;
uniform float tng;


#define ID gl_InvocationID

void main()
{
  tcPosition[ID] = vPosition[ID];
  tcCenter[ID] = vCenter[ID];
  tcNormal[ID] = (vPosition[ID]-vCenter[ID]);
  r[ID]=distance(vPosition[ID],vCenter[ID]);

  //calculo de la normal;
  m[ID]=tng*vTangent[ID];

  float tcLod = (vlot[0]+vlot[1]+vlot[2]+vlot[3])/4;
  gl_TessLevelInner[0] = tcLod;
  gl_TessLevelInner[1] = tcLod;
  gl_TessLevelOuter[0] = (vlot[0]+vlot[2])/2;
  gl_TessLevelOuter[1] = (vlot[0]+vlot[1])/2;
  gl_TessLevelOuter[2] = (vlot[1]+vlot[3])/2;
  gl_TessLevelOuter[3] = (vlot[2]+vlot[3])/2;
}