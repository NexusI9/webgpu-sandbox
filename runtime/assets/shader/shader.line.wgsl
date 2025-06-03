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
  projection : mat4x4<f32>,
}

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  let thickness = input.aSide.y;
  let side = input.aSide.x;
  let a = input.aPosA;
  let b = input.aPosB;

  let dir = normalize(b - a);

  let midpoint = (a + b) * 0.5f;

  let camPos = uCamera.position.xyz;
  let camDir = normalize(camPos - midpoint);

  // get extrusion
  var right : vec3<f32> = normalize(cross(dir, camDir));

  // offset
  let extrude_pos = a + (right * side * thickness);

  var output : VertexOut;
  output.Position = cam * uMesh.model * vec4<f32>(extrude_pos, 1.0f);
  output.vCol = vec3<f32>(input.aCol);

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {

  return vec4<f32>(1.0f);
  // return vec4<f32>(vCol, 1.0);
}
