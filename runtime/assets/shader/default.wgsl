// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>, @location(1) aCol : vec3<f32>,
};

struct VertexOut {
  @location(0) vCol : vec3<f32>, @builtin(position) Position : vec4<f32>,
};

struct Rotation {
  @location(0) degs : f32,
};

struct Camera {
  view : mat4x4<f32>, position : vec3<f32>,
};

struct Viewport {
  projection : mat4x4<f32>,
}

// camera viewport
//@group(1) @binding(0) var<uniform> uViewport : Viewport;
//@group(1) @binding(1) var<uniform> uCamera : Camera;

// rot
@group(0) @binding(0) var<uniform> uRot : Rotation;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {
  var rads : f32 = radians(uRot.degs);
  var cosA : f32 = cos(rads);
  var sinA : f32 = sin(rads);
  var rot : mat3x3<f32> = mat3x3<f32>(vec3<f32>(cosA, sinA, 0.0),
                                      vec3<f32>(-sinA, cosA, 0.0),
                                      vec3<f32>(0.0, 0.0, 1.0));

  var cam : mat4x4<f32> = mat4x4<f32>(
                vec4<f32>(1.0, 0.0, 0.0, 0.0), vec4<f32>(0.0, 1.0, 0.0, 0.0),
                vec4<f32>(0.0, 0.0, 1.0, 0.0), vec4<f32>(0.0, 0.0, 0.0, 1.0));

  var output : VertexOut;
  output.Position = cam * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;
  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {
  return vec4<f32>(vCol, 1.0);
}
