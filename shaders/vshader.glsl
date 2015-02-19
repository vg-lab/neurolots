/**
 * @author Juan Jos� Garc�a Cantero
 * @version 0.1

 * @copyright Copyright (C) 2011-2015 GMRV - Human Brain Project
 * @license License: http://www.gnu.org/licenses/gpl.html GPL version 2 or 
 * higher
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
	return ( lod - 1 ) * clamp( ( 1.0 - length( pos - cameraPos ) / maxDist ), 0.0,
	         1.0 ) + 1;
}

void main( void )
{
	vPosition = inVertex + desp;
	vCenter = inCenter + desp;
	vTangent = inTangent;
	vlot = levelDist( vCenter );
}
