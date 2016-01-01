#version 330 core
#extension GL_ARB_explicit_uniform_location : require

vec4 qconj(vec4 q)
{
  return vec4(-q.xyz, q.w);
}

vec3 qrot(vec3 v, vec4 q)
{
  return (cross(q.xyz, cross(q.xyz, v) + (v * q.w)) * 2.f) + v;
}

vec4 proj(vec3 mv, vec4 prj)
{
  return vec4(mv.xy * prj.xy, (mv.z * prj.z) + prj.w, -mv.z);
}

layout(location = 0) uniform vec3 cam_pos;
layout(location = 1) uniform vec4 cam_rot;
layout(location = 2) uniform vec4 cam_prj;

layout(location = 0) in vec3 vtx_pos;

void main()
{
  vec3 mv;
  mv = vtx_pos - cam_pos;
  mv = qrot(mv, qconj(cam_rot));
  gl_Position = proj(mv, cam_prj);
}
