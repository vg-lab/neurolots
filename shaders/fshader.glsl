/**
 * @author Juan José García Cantero
 * @version 0.1

 * @copyright Copyright (C) 2011-2015 GMRV - Human Brain Project
 * @license License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#version 440

out vec4 oColor;
in vec3 normal;
in vec3 L;

uniform vec3 color;
void main()
{
	vec3 N=normalize(normal);
	float dif=dot(N,L);
	dif=clamp(dif,0.0,1.0);
	oColor = vec4(color*dif,1.0)+vec4(color*0.3,1.0);
}

