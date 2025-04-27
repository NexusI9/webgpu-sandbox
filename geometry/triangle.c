#include "triangle.h"
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

  for (int n = 0; n < amount; n++) {
    triangle_rand_dist_trilinear(r1[n], r2[n], r3[n], p1, p2, p3, dest[n]);
  }
}
