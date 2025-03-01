// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>, @location(1) aCol : vec3<f32>,
};

struct VertexOut {
  @location(0) vCol : vec3<f32>, @builtin(position) Position : vec4<f32>,
};

struct Mesh {
  model : mat4x4<f32>, position : vec4<f32>,
}

struct Camera {
  view : mat4x4<f32>, position : vec4<f32>,
};

struct Viewport {
  projection : mat4x4<f32>,
}

// NOTE:
// Need to add padding cause WebGPU align to memory based on 16-bytes
// alignment rule Meaning that data need to be a multiple of 16 bytes 1 float
// (4bytes) is automatically handled by WebGPU, 3 floats (12 bytes) as well:
// both are padded automatically to 16bytes. However for 2 floats (8 bytes) like
// the case below, provoke misalignment Each bind group has independent
// alignment.
// Maybe a trick to have better control on that is to use struct
// Also the 16 bytes alignment only matter in the GPU side, meaning the C struct
// doesn't require this padding.

struct GridData {
  division : f32, scale : f32, _padding : vec2<f32>
}

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

@group(1) @binding(0) var<uniform> uGrid : GridData;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  var output : VertexOut;
  output.Position = cam * uMesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {

  return vec4<f32>(vCol, 1.0);
}
