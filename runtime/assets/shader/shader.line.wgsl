// attribute/uniform decls
struct VertexIn {
  @location(0) aPosA : vec3<f32>,
                       @location(1) aPosB : vec3<f32>,
                                            @location(2) aCol
      : vec3<f32>,
        @location(3) aSide : vec2<f32>,
};

struct VertexOut {
  @builtin(position) Position : vec4<f32>,
                                @location(0) vCol : vec3<f32>,
                                                    @location(1) vUv : vec2<f32>
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

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  // var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  let a = input.aPosA; // anchor
  let b = input.aPosB; // opposite
  let side = input.aSide.x;
  let thickness = 0.3f; // input.aSide.y;

  let midpoint = (a + b) * 0.5f;
  var model_dir = normalize(a - b);

  // offset
  let world_pos = uMesh.model * vec4<f32>(a, 1.0f);
  let view_pos = uCamera.view * world_pos;

  // model space -> world space
  let world_dir : vec3<f32> =
                      normalize((uMesh.model * vec4<f32>(model_dir, 0.0f)).xyz);
  // world space -> view space
  let view_dir : vec3<f32> =
                     normalize((uCamera.view * vec4<f32>(world_dir, 0.0f)).xyz);

  let view_2_cam = normalize(-view_pos.xyz);
  var world_view_raw = cross(view_dir, view_2_cam);
  let epsilon = 0.0001;

  var world_view : vec3<f32>;

  if (length(world_view_raw) < epsilon) {

    let view_fwd = vec3<f32>(0.0f, 0.0f, -1.0f);
    world_view = normalize(cross(view_dir, view_fwd));

  } else {
    world_view = normalize(world_view_raw);
  }

  let perp_view_2D : vec2<f32> = world_view.xy;

  let abs_view_z = abs(view_pos.z);

  let view_space_offset_x =
      thickness * 0.5f * abs_view_z / uViewport.projection[0][0];
  let view_space_offset_y =
      thickness * 0.5f * abs_view_z / uViewport.projection[1][1];

  let offset_view_space = vec3<f32>(perp_view_2D.x * view_space_offset_x,
                                    perp_view_2D.y * view_space_offset_y, 0.0f);

  let final_view_pos =
      vec4<f32>(view_pos.xyz + offset_view_space * side, view_pos.w);

  var output : VertexOut;
  output.Position = uViewport.projection * final_view_pos;
  output.vCol = vec3<f32>(input.aCol);

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {

  return vec4<f32>(vCol, 1.0);
}
