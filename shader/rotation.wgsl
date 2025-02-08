// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>, @location(1) aCol : vec3<f32>,
};

struct VertexOut {
  @location(0) vCol : vec3<f32>, @builtin(position) Position : vec4<f32>,
};

struct Rotation {
  degs : f32,
};

struct Camera {
  view : mat4x4<f32>, position : vec3<f32>,
};

struct Viewport {
  projection : mat4x4<f32>,
}

struct Color {
  red : f32, blue : f32, green : f32
}

// Define Projection Matrix (Perspective)
fn perspective_matrix(fov : f32, aspect : f32, near : f32, far : f32)
    -> mat4x4<f32> {
  let f = 1.0 / tan(fov * 0.5);
  return mat4x4<f32>(vec4<f32>(f / aspect, 0.0, 0.0, 0.0),
                     vec4<f32>(0.0, f, 0.0, 0.0),
                     vec4<f32>(0.0, 0.0, far / (far - near), 1.0),
                     vec4<f32>(0.0, 0.0, -(near * far) / (far - near), 0.0));
}

// rot
@group(0) @binding(0) var<uniform> uRot : Rotation;

// camera viewport
@group(1) @binding(0) var<uniform> uCamera : Camera;
@group(1) @binding(1) var<uniform> uViewport : Viewport;

@group(2) @binding(0) var<uniform> uColor : Color;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Example usage
  var fov : f32 = radians(24.0);
  var aspect : f32 = 1920.0 / 1080.0;
  var near : f32 = 0.1;
  var far : f32 = 100.0;

  var projection : mat4x4<f32> = perspective_matrix(fov, aspect, near, far);

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = projection * uCamera.view;

  var output : VertexOut;
  output.Position = cam * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {
    
    return vec4<f32>(uColor.red, uColor.green, uColor.blue, 1.0);
}
