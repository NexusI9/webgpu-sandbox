// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aCol : vec3<f32>,
                                                               @location(3) aUv
      : vec2<f32>,
};

struct VertexOut {
  @builtin(position) Position : vec4<f32>,
                                @location(0) vCol : vec3<f32>,
                                                    @location(1) vUv : vec2<f32>
};

struct Mesh {
  model : mat4x4<f32>, position : vec4<f32>,
}

struct Viewport {
  projection : mat4x4<f32>, width : u32, height : u32,
};

struct Camera {
  view : mat4x4<f32>,
         position : vec4<f32>,
                    lookat : vec4<f32>,
                             mode : u32,
                                    _pad : vec3<u32>,
};

@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

@group(1) @binding(0) var texture : texture_2d<f32>;
@group(1) @binding(1) var texture_sampler : sampler;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var output : VertexOut;

  let look = normalize(uCamera.position.xyz - uMesh.position.xyz);
  let worldUp = vec3<f32>(0.0f, 1.0f, 0.0f);

  let scale_x = length(vec3<f32>(uMesh.model[0].xyz));
  let scale_y = length(vec3<f32>(uMesh.model[1].xyz));
  let scale_z = length(vec3<f32>(uMesh.model[2].xyz));

  let flatToCamera = normalize(vec3<f32>(look.x, 0.0f, look.z));
  let right = normalize(cross(worldUp, flatToCamera));
  let up = cross(look, right);

  let local_position =
      input.aPos.x * right * scale_x + input.aPos.z * up * scale_z;
  let world_position = uMesh.position.xyz + local_position;

  output.Position =
      uViewport.projection * uCamera.view * vec4<f32>(world_position, 1.0f);
  output.vCol = input.aCol;
  output.vUv = input.aUv;
  return output;
}

// fragment shader
@fragment fn fs_main(@location(1) vUv : vec2<f32>) -> @location(0) vec4<f32> {

  return textureSample(texture, texture_sampler, vUv);
}
