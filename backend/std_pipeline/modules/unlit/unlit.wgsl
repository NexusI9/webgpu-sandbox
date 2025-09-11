// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aTan : vec4<f32>,
                                                               @location(3) aCol
      : vec3<f32>,
        @location(4) aUv : vec2<f32>,
};

struct VertexOut {
  @builtin(position) Position : vec4<f32>,
                                @location(0) vCol : vec3<f32>,
                                                    @location(1) vUv : vec2<f32>
};

struct Mesh {
  model : mat4x4<f32>,
          position : vec4<f32>,
                     probe_reflection_plane_id : u32,
                                                 probe_reflection_plane_count
      : u32,
        probe_reflection_grid_id : u32,
                                   probe_reflection_grid_count : u32,
}

struct Camera {
  view : mat4x4<f32>, position : vec4<f32>, lookat : vec4<f32>, mode : u32,
};

struct Viewport {
  projection : mat4x4<f32>, width : u32, height : u32,
};

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

@group(1) @binding(0) var<uniform> uColor : vec4<f32>;
@group(1) @binding(1) var<uniform> uFixedScale : f32;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  let mesh = uMesh;
  let camera = uCamera;
  let viewport = uViewport;

  let local_pos = vec4<f32>(input.aPos, 1.0);
  let world_pos = mesh.model * local_pos;

  let cam_to_mesh = normalize(mesh.position - camera.position);
  let fixed_origin = camera.position + cam_to_mesh * uFixedScale;

  let offset = world_pos - mesh.position;

  // 0 = world position; 1 = fixed position
  let blend_pos =
      mix(world_pos, fixed_origin + offset, clamp(uFixedScale, 0.0f, 1.0f));

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = viewport.projection * camera.view;

  var output : VertexOut;
  output.Position = cam * blend_pos;
  output.vCol = input.aCol;

  return output;
}

// fragment shader
@fragment fn fs_main() -> @location(0) vec4<f32> {
  return uColor;
}
