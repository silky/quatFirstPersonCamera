// Public domain single-file C implementation by Constantine Tarasenkov
//
// LICENSE
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#ifndef QFPC_H_
#define QFPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef QFPC_SIN
#define QFPC_SIN sin
#endif

#ifndef QFPC_COS
#define QFPC_COS cos
#endif

#ifndef QFPC_TO_RAD
#define QFPC_TO_RAD 0.0174532925199432957f
#endif

void qfpcQAxisAngle(float * q, const float * axis, float angle_rad);
void qfpcQMulq(float * result, const float * q1, const float * q2);
void qfpcV3Mulf(float * result, const float * v, float f);  
void qfpcV3Addv(float * result, const float * v1, const float * v2);
void qfpcV3Cross(float * result, const float * v1, const float * v2);
void qfpcV3Rotq(float * result, const float * v, const float * q);

void quatFirstPersonCamera(
  float * camera_vec3_position,
  float * camera_quat_rotation,
  int mouse_x,
  int mouse_y,
  int window_middle_x,
  int window_middle_y,
  float look_multiplier,
  float move_multiplier,
  int key_forward,
  int key_left,
  int key_back,
  int key_right,
  int key_up,
  int key_down
);

#ifdef __cplusplus
}
#endif
#endif /* QFPC_H_ */

#ifdef QFPC_IMPLEMENTATION

void qfpcQAxisAngle(
  float * q,
  const float * axis,
  float angle_rad)
{
  float s = (float)QFPC_SIN(angle_rad / 2.f);
  float c = (float)QFPC_COS(angle_rad / 2.f);
  
  q[0] = axis[0] * s;
  q[1] = axis[1] * s;
  q[2] = axis[2] * s;
  q[3] = c;
}

void qfpcQMulq(
  float * result,
  const float * q1,
  const float * q2)
{
  float q1x = q1[0];
  float q1y = q1[1];
  float q1z = q1[2];
  float q1w = q1[3];
  
  float q2x = q2[0];
  float q2y = q2[1];
  float q2z = q2[2];
  float q2w = q2[3];
  
  result[0] = q1w * q2x + q1x * q2w + q1y * q2z - q1z * q2y;
  result[1] = q1w * q2y - q1x * q2z + q1y * q2w + q1z * q2x;
  result[2] = q1w * q2z + q1x * q2y - q1y * q2x + q1z * q2w;
  result[3] = q1w * q2w - q1x * q2x - q1y * q2y - q1z * q2z;
}

void qfpcV3Mulf(
  float * result,
  const float * v,
  float f)
{
  result[0] = v[0] * f;
  result[1] = v[1] * f;
  result[2] = v[2] * f;
}

void qfpcV3Addv(
  float * result,
  const float * v1,
  const float * v2)
{
  result[0] = v1[0] + v2[0];
  result[1] = v1[1] + v2[1];
  result[2] = v1[2] + v2[2];
}

void qfpcV3Cross(
  float * result,
  const float * v1,
  const float * v2)
{
  float v1x = v1[0];
  float v1y = v1[1];
  float v1z = v1[2];
  
  float v2x = v2[0];
  float v2y = v2[1];
  float v2z = v2[2];
  
  result[0] = (v1y * v2z) - (v1z * v2y);
  result[1] = (v1z * v2x) - (v1x * v2z);
  result[2] = (v1x * v2y) - (v1y * v2x);
}

void qfpcV3Rotq(
  float * result,
  const float * v,
  const float * q)
{
  qfpcV3Cross(result, q, v);
  float temp[3];
  qfpcV3Mulf(temp, v, q[3]);
  qfpcV3Addv(result, result, temp);
  qfpcV3Cross(result, q, result);
  qfpcV3Mulf(result, result, 2.f);
  qfpcV3Addv(result, result, v);
}

void quatFirstPersonCamera(
  float * cam_vec3_pos,
  float * cam_quat_rot,
  int mouse_x,
  int mouse_y,
  int win_mid_x,
  int win_mid_y,
  float look_mult,
  float move_mult,
  int key_f,
  int key_l,
  int key_b,
  int key_r,
  int key_u,
  int key_d)
{
  int mouse_diff_x = win_mid_x - mouse_x;
  int mouse_diff_y = win_mid_y - mouse_y;
  
  float cam_rot_x = mouse_diff_y * look_mult;
  float cam_rot_y = mouse_diff_x * look_mult;
  
  float rot_x[4];
  float rot_y[4];
  
  qfpcQAxisAngle(rot_x, (float[]){ 1, 0, 0 }, cam_rot_x * QFPC_TO_RAD);
  qfpcQAxisAngle(rot_y, (float[]){ 0, 1, 0 }, cam_rot_y * QFPC_TO_RAD);
  
  qfpcQMulq(cam_quat_rot, cam_quat_rot, rot_x);
  qfpcQMulq(cam_quat_rot, rot_y, cam_quat_rot);
  
  float side_vec[3];
  float   up_vec[3];
  float  dir_vec[3];
  
  qfpcV3Rotq(side_vec, (float[]){ -1.f,  0.f,  0.f }, cam_quat_rot);
  qfpcV3Rotq(  up_vec, (float[]){  0.f,  1.f,  0.f }, cam_quat_rot);
  qfpcV3Rotq( dir_vec, (float[]){  0.f,  0.f, -1.f }, cam_quat_rot);
  
  qfpcV3Mulf(side_vec, side_vec, (key_l - key_r) * move_mult);
  qfpcV3Mulf(  up_vec,   up_vec, (key_u - key_d) * move_mult);
  qfpcV3Mulf( dir_vec,  dir_vec, (key_f - key_b) * move_mult);
  
  qfpcV3Addv(cam_vec3_pos, cam_vec3_pos, side_vec);
  qfpcV3Addv(cam_vec3_pos, cam_vec3_pos,   up_vec);
  qfpcV3Addv(cam_vec3_pos, cam_vec3_pos,  dir_vec);
}

#endif /* QFPC_IMPLEMENTATION */