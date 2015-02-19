/**
 * @author Juan José García Cantero
 * @version 0.1

 * @copyright Copyright (C) 2011-2015 GMRV - Human Brain Project
 * @license License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
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