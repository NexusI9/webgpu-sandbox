#include "viewport.h"


viewport viewport_create(float fov, float near_clip, float far_clip){
    //set viewport default values
    viewport v = (viewport){
	.fov = fov,
	.near_clip = near_clip,
	.far_clip = far_clip
    };

    //init projection matrix
    glm_mat4_identity(v.projection);

    return v;
}
