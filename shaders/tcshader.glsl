/**
 * @author Juan José García Cantero
 * @version 0.1

 * @copyright Copyright (C) 2011-2015 GMRV - Human Brain Project
 * @license License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
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
	//r[ID] = vRadius[ID];
	r[ID]=distance(vPosition[ID],vCenter[ID]);

	//calculo de la normal;
	m[ID]=tng*vTangent[ID];
	//m[ID]=3*normalize(cross(normalize(vPosition[(ID+1)%4]-vPosition[ID]),normalize(vCenter[ID]-vPosition[ID])));
	/*if(ID<2)
		m[ID]=normalize(vCenter[2]-vCenter[0]);
	else
		m[ID]=normalize(cross(normalize(vPosition[(ID+1)%4]-vPosition[ID]),normalize(vCenter[ID]-vPosition[ID])));*/
	float tcLod = (vlot[0]+vlot[1]+vlot[2]+vlot[3])/4;
	gl_TessLevelInner[0] = tcLod;
	gl_TessLevelInner[1] = tcLod;
	gl_TessLevelOuter[0] = (vlot[0]+vlot[2])/2;
	gl_TessLevelOuter[1] = (vlot[0]+vlot[1])/2;
	gl_TessLevelOuter[2] = (vlot[1]+vlot[3])/2;
	gl_TessLevelOuter[3] = (vlot[2]+vlot[3])/2;
}