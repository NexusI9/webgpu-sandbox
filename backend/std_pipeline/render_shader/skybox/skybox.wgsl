struct VertexOutput {
  @builtin(position) vPos : vec4<f32>, @location(0) vDir : vec3<f32>
};

struct Mesh {
  model : mat4x4<f32>,
          position : vec4<f32>,
                     probe_reflection_plane_count : u32,
                                                    probe_reflection_grid_count
      : u32,
}

struct Camera {
  view : mat4x4<f32>, position : vec4<f32>, lookat : vec4<f32>, mode : u32,
};

struct Viewport {
  projection : mat4x4<f32>, width : u32, height : u32
};

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

// skybox texture
@group(1) @binding(0) var skybox_texture : texture_cube<f32>;
@group(1) @binding(1) var skybox_sampler : sampler;
@group(1) @binding(2) var<uniform> skybox_blur : f32;

@vertex fn vs_main(@location(0) position : vec3<f32>) -> VertexOutput {

  let camera = uCamera;
  let viewport = uViewport;

  // follow camera but infinitelly far (remove translation/ rotation only)
  var rot_only_view = camera.view;
  rot_only_view[3] = vec4<f32>(0.0f, 0.0f, 0.0f, 1.0f);

  let new_position =
      viewport.projection * rot_only_view * vec4<f32>(position, 1.0f);

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
