#include "math.h"
#include "../utils/system.h"

/**
   Return a number between 0 and 1 (0 excluded)
 */
float randf() {
  float r = rand();
  while (r == 0)
    r = rand();
  return (float)r / (float)RAND_MAX;
}

/**
   Generate a list of random points within an hemisphere
 */
void hemisphere_random_points(vec3 orientation, int amount, vec3 *dest) {


  for (int i = 0; i < amount; i++) {
    float u1, u2, r, theta, x, y, z;
    u1 = randf();
    u2 = randf();

    r = sqrt(u1);
    theta = 2 * PI * u2;

    x = r * cos(theta);
    y = r * sin(theta);
    z = sqrt(1 - u1);

    vec3 T, B;
    vec3 up = (vec3){0.0f, 1.0f, 0.0f};

    if (fabsf(glm_vec3_dot(orientation, up)) > 0.99f)
      glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, up);

    glm_vec3_cross(orientation, up, T);
    glm_normalize(T);

    glm_vec3_cross(orientation, T, B);

    vec3 xT, yB, zN;

    glm_vec3_scale(T, x, xT);
    glm_vec3_scale(B, y, yB);
    glm_vec3_scale(orientation, z, zN);

    vec3 *point = &dest[i];
    glm_vec3_add(xT, yB, *point);
    glm_vec3_add(zN, *point, *point);
  }
}
