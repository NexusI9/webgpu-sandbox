// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>, @location(1) aCol : vec3<f32>,
};

struct VertexOut {
  @location(0) vCol : vec3<f32>, @builtin(position) Position : vec4<f32>,
};

struct Camera {
  view : mat4x4<f32>, position : vec4<f32>,
};

struct Viewport {
  projection : mat4x4<f32>,
}

// camera viewport
@group(0) @binding(0) var<uniform> uCamera : Camera;
@group(0) @binding(1) var<uniform> uViewport : Viewport;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  var output : VertexOut;
  output.Position = cam * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {

  return vec4<f32>(vCol, 1.0);
}
