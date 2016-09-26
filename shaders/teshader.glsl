/**
 * @file    teshader.glsl
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#version 440

layout(quads) in;
in vec3 tcPosition[];
in vec3 tcCenter[];
in float r[];
in vec3 tcNormal[];
in vec3 m[];
out vec3 normal;
out vec3 L;
//out vec3 tePosition;
//out vec3 teCenter;
//out vec3 teNormal;


uniform mat4 proy;
uniform mat4 view;


uniform vec3 cameraPos;


vec3 Slerp(vec3 p0, vec3 p1, float t)
{
  vec3 p0_n=normalize(p0);
  vec3 p1_n=normalize(p1);

  float alpha = acos(dot(p0_n,p1_n));
  vec3 P;
  if(alpha==0){
    P= p0_n;
  }else{
    P=normalize(((p0_n*sin((1-t)*alpha))+(p1_n*sin(t*alpha))));
  }
  return P;
}

vec3 Slerp2(vec3 p0, vec3 p1, float t)
{
  float alpha = acos(dot(normalize(p0),normalize(p1)));

  vec3 P=((p0*sin((1-t)*alpha))+(p1*sin(t*alpha)))/sin(alpha);
  
  return P;
}

vec4 make_quaternion(vec3 exe0,vec3 exe1)
{
  float cos_theta = dot(normalize(exe0), normalize(exe1));
    float half_cos = sqrt(0.5f * (1.f + cos_theta));
    float half_sin = sqrt(0.5f * (1.f - cos_theta));
  vec3 w;
    if(abs(cos_theta)>0.99999)
    w = vec3(0,0,0);
  else
    w = normalize(cross(normalize(exe0), normalize(exe1)));
    return vec4(half_sin * w.x, half_sin * w.y, half_sin * w.z, half_cos);
}

vec3 rotate_vector(vec4 quat, vec3 vec)
{
  return (vec + 2.0 * cross(quat.xyz,cross(quat.xyz,vec)+quat.w * vec));
}

void main()
{

  vec3 tePosition;
  vec3 teCenter;
  vec3 teNormal;
  
  //Cubic Interpolation parameters
  float u=gl_TessCoord.x;
  float u_2=u*u;
  float u_3=u_2*u;
  float v=gl_TessCoord.y;
  float v_2=v*v;
  float v_3=v_2*v;

  //Factor de tangente
  float factor_tangente= clamp(length(normalize(m[0])),0,1)*
                         clamp(length(normalize(m[2])),0,1);
    
  //Center Interpolation Compose Lineal Cubic
  vec3 a_c = mix(tcCenter[0],tcCenter[1],u);
  vec3 b_c = mix(tcCenter[2],tcCenter[3],u);
  vec3 teCenterLineal = mix(a_c,b_c,v);
  vec3 teCenterCubic = (2*v_3-3*v_2+1)*a_c+(v_3-2*v_2+v)*m[0]+(v_3-v_2)*m[2]
                       +(-2*v_3+3*v_2)*b_c;
  teCenter = mix(teCenterLineal,teCenterCubic,factor_tangente);
  
  //Tangent 
  vec3 tangent =normalize( (6*v_2-6*v)*a_c+(3*v_2-4*v+1)*m[0]+(3*v_2-2*v)*m[2]
                           +(-6*v_2+6*v)*b_c);
  
  //Normal interpoplation
  vec3 a_t=(Slerp(tcNormal[0],tcNormal[1],u));  
  vec3 b_t=(Slerp(tcNormal[2],tcNormal[3],u));

  vec3 a_t_n=normalize(rotate_vector( make_quaternion(normalize(m[0]),tangent),
                                      a_t));
  vec3 b_t_n=normalize(rotate_vector( make_quaternion(normalize(m[2]),tangent),
                                      b_t));
  
  vec3 teNormalRotate=mix(a_t_n,b_t_n,v);
  vec3 teNormalLineal=Slerp(a_t,b_t,v);
    
  if(factor_tangente==1)
    teNormal=teNormalRotate;
  else
    teNormal=teNormalLineal;

  //Radius Interpolation Lineal
  float a_r=mix(r[0],r[1],u);;
  float b_r=mix(r[2],r[3],u);
  float radius=mix(a_r,b_r,v);

  tePosition = (radius*teNormal+teCenter);
  normal = normalize(teNormal);
  L = normalize(cameraPos-tePosition);
  gl_Position = proy*view*vec4(tePosition,1);
}
