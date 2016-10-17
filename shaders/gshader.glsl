/**
 * @file    gshader.glsl
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
in vec3 teCenter[3];
out vec3 normal;


uniform mat4 proy;
uniform mat4 view;


void main()
{
  normal = normalize(tePosition[0]-teCenter[0]);
  gl_Position = gl_in[0].gl_Position; EmitVertex();

  normal = normalize(tePosition[1]-teCenter[1]);
  gl_Position = gl_in[1].gl_Position; EmitVertex();

  normal = normalize(tePosition[2]-teCenter[2]);
  gl_Position = gl_in[2].gl_Position; EmitVertex();

  EndPrimitive();
}