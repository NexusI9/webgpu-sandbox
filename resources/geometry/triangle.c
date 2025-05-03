#include "triangle.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static void triangle_rand_dist_trilinear(float, float, float, vec3, vec3, vec3,
                                         vec3);

static float triangle_rand_dist_trilinear_dimension(float, float, float, vec3,
                                                    vec3, vec3, int);

static void triangle_rand_gen_list(int, float *);
static void triangle_rand_list_div(float *, int, float);

float triangle_rand_dist_trilinear_dimension(float x1, float y1, float z1,
                                             vec3 p1, vec3 p2, vec3 p3,
                                             int dim) {

  float dist_p2_p3 = glm_vec3_distance(p2, p3);
  float dist_p1_p3 = glm_vec3_distance(p1, p3);
  float dist_p1_p2 = glm_vec3_distance(p1, p2);

  return (p1[dim] * dist_p2_p3 * x1 + p2[dim] * dist_p1_p3 * y1 +
          p3[dim] * dist_p1_p2 * z1) /
         (dist_p2_p3 * x1 + dist_p1_p3 * y1 + dist_p1_p2 * z1);
}

void triangle_rand_dist_trilinear(float x1, float y1, float z1, vec3 p1,
                                  vec3 p2, vec3 p3, vec3 dest) {

  float x = triangle_rand_dist_trilinear_dimension(x1, y1, z1, p1, p2, p3, 0);
  float y = triangle_rand_dist_trilinear_dimension(x1, y1, z1, p1, p2, p3, 1);
  float z = triangle_rand_dist_trilinear_dimension(x1, y1, z1, p1, p2, p3, 2);

  glm_vec3_copy((vec3){x, y, z}, dest);
}

void triangle_rand_list_div(float *list, int length, float k) {
  for (int i = 0; i < length; i++)
    list[i] /= k;
}

void triangle_rand_gen_list(int length, float *dest) {

  for (int i = 0; i < length; i++) {
    float r = rand();
    while (r == 0)
      r = rand();
    dest[i] = log((float)r / (float)RAND_MAX);
  }
}

/**
   Place a given amount of points evenly randomly within a triangle using
   baycentric coodinates:
   https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html
 */
void triangle_random_points(triangle *surface, uint16_t amount, vec3 *dest) {

  // store points
  vec3 p1;
  glm_vec3_copy(surface->a.position, p1);

  vec3 p2;
  glm_vec3_copy(surface->b.position, p2);

  vec3 p3;
  glm_vec3_copy(surface->c.position, p3);

  // generate random number list
  float r1[amount];
  float r2[amount];
  float r3[amount];

  triangle_rand_gen_list(amount, r1);
  triangle_rand_gen_list(amount, r2);
  triangle_rand_gen_list(amount, r3);

  triangle_rand_list_div(r1, amount, glm_vec3_distance(p2, p3));
  triangle_rand_list_div(r2, amount, glm_vec3_distance(p1, p3));
  triangle_rand_list_div(r3, amount, glm_vec3_distance(p1, p2));

  for (int n = 0; n < amount; n++)
    triangle_rand_dist_trilinear(r1[n], r2[n], r3[n], p1, p2, p3, dest[n]);
}

/**
   Retrieve the face surface from the triangle.
 */
void triangle_normal(triangle *surface, vec3 dest) {
  vec3 edge1, edge2;

  glm_vec3_sub(surface->b.position, surface->a.position, edge1);
  glm_vec3_sub(surface->c.position, surface->a.position, edge2);

  glm_vec3_cross(edge1, edge2, dest);
  glm_vec3_normalize(dest);
}

/**
   Use Moller Trumbore intersection algorithm to check if a given ray traverse a
   triangle.
   https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */
void triangle_raycast(triangle *surface, vec3 ray_origin, vec3 ray_direction,
                      float max_distance, vec3 hit) {

  // glm_vec3_copy(ray_origin, hit);
  // return;
  float epsilon = FLT_EPSILON;

  vec3 edge1, edge2, ray_cross_e2;

  glm_vec3_sub(surface->b.position, surface->a.position, edge1);
  glm_vec3_sub(surface->c.position, surface->a.position, edge2);
  glm_vec3_cross(ray_direction, edge2, ray_cross_e2);

  float det = glm_vec3_dot(edge1, ray_cross_e2);

  if (det > -epsilon && det < epsilon) {
    // ray parallel to triangle
    glm_vec3_zero(hit);
    return;
  }

  float inv_det = 1.0 / det;
  vec3 s;
  glm_vec3_sub(ray_origin, surface->a.position, s);
  float u = inv_det * glm_vec3_dot(s, ray_cross_e2);

  if (u < 0.0f || u > 1.0f) {
    glm_vec3_zero(hit);
    return;
  }

  vec3 s_cross_e1;
  glm_vec3_cross(s, edge1, s_cross_e1);
  float v = inv_det * glm_vec3_dot(ray_direction, s_cross_e1);

  if (v < 0.0f || u + v > 1.0f) {
    glm_vec3_zero(hit);
    return;
  }

  // compute t to find where interesction is on the line
  float t = inv_det * glm_vec3_dot(edge2, s_cross_e1);
  //&&t < max_distance
  if (t > epsilon) {
    // intersection
    vec3 distance;
    glm_vec3_scale(ray_direction, t, distance);
    glm_vec3_add(ray_origin, distance, hit);
  } else {
    // line interesction but no Ray intersection
    glm_vec3_zero(hit);
  }
}

/**
   Read a point position in the vertex 3D space and return the equivalent
   in the UV space
 */
void triangle_point_to_uv(triangle *surface, vec3 point, vec2 dest) {

  vec3 p0, p1, p2;

  glm_vec3_sub(surface->b.position, surface->a.position, p0);
  glm_vec3_sub(surface->c.position, surface->a.position, p1);
  glm_vec3_sub(point, surface->a.position, p2);

  float d00 = glm_vec3_dot(p0, p0);
  float d01 = glm_vec3_dot(p0, p1);
  float d11 = glm_vec3_dot(p1, p1);
  float d20 = glm_vec3_dot(p2, p0);
  float d21 = glm_vec3_dot(p2, p1);

  float denom = d00 * d11 - d01 * d01;

  float v = (d11 * d20 - d01 * d21) / denom;
  float w = (d00 * d21 - d01 * d20) / denom;
  float u = 1 - v - w;

  vec2 uv0, uv1, uv2;
  glm_vec2_scale(surface->a.uv, u, uv0);
  glm_vec2_scale(surface->b.uv, v, uv1);
  glm_vec2_scale(surface->c.uv, w, uv2);

  vec2 uv01;
  glm_vec2_add(uv0, uv1, uv01);
  glm_vec2_add(uv01, uv2, dest);
}
