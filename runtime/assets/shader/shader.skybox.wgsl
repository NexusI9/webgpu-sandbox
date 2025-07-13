struct VertexOutput {
  @builtin(position) vPos : vec4<f32>, @location(0) vDir : vec3<f32>
};

struct Mesh {
  model : mat4x4<f32>, position : vec4<f32>,
}

struct Camera {
  view : mat4x4<f32>,
         position : vec4<f32>,
                    lookat : vec4<f32>,
                             mode : u32,
                                    _pad : vec3<u32>,
};

struct Viewport {
  projection : mat4x4<f32>, width : u32, height : u32,
}

// skybox texture
@group(0) @binding(0) var skybox_texture : texture_cube<f32>;
@group(0) @binding(1) var skybox_sampler : sampler;
@group(0) @binding(2) var<uniform> skybox_blur : f32;

// camera viewport
@group(1) @binding(0) var<uniform> uViewport : Viewport;
@group(1) @binding(1) var<uniform> uCamera : Camera;
@group(1) @binding(2) var<uniform> uMesh : Mesh;

@vertex fn vs_main(@location(0) position : vec3<f32>) -> VertexOutput {

  // follow camera but infinitelly far (remove translation/ rotation only)
  var rot_only_view = uCamera.view;
  rot_only_view[3] = vec4<f32>(0.0f, 0.0f, 0.0f, 1.0f);

  let new_position =
      uViewport.projection * rot_only_view * vec4<f32>(position, 1.0f);

  var output : VertexOutput;
  output.vDir = position;
  output.vPos = new_position;

  return output;
}

@fragment fn fs_main(@location(0) vDir : vec3<f32>) -> @location(0) vec4<f32> {

  if (skybox_blur == 0.0f) {
    return textureSample(skybox_texture, skybox_sampler, normalize(vDir));
  } else {

    var color = vec4<f32>(0.0f);
    let blur_sample = 8i;

    var offsets = array<vec3<f32>, 8>(
        vec3<f32>(0.0, 0.4, 0.1), vec3<f32>(0.3, -0.3, 0.2),
        vec3<f32>(-0.4, -0.1, 0.0), vec3<f32>(-0.2, 0.5, -0.3),
        vec3<f32>(0.5, 0.2, -0.1), vec3<f32>(-0.1, -0.5, 0.3),
        vec3<f32>(0.2, -0.2, -0.4), vec3<f32>(-0.3, 0.3, 0.4));

    for (var i = 0; i < blur_sample; i++) {
      let offset = offsets[i % 8];
      let sample_dir = normalize(vDir + offset * skybox_blur);
      color += textureSample(skybox_texture, skybox_sampler, sample_dir);
    }

    return color / f32(blur_sample);
  }
}
