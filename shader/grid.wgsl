// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aCol : vec3<f32>,
                                          @location(2) aUv : vec2<f32>,
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
  color : vec4<f32>,
          division : f32,
                     scale : f32,
                             thickness : f32,
                                         _padding : f32
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

  // Put the grid below the camera
  var translate_matrix
      : mat4x4<f32> = mat4x4<f32>(
            vec4<f32>(1.0, 0.0, 0.0, 0.0), vec4<f32>(0.0, 1.0, 0.0, 0.0),
            vec4<f32>(0.0, 0.0, 1.0, 0.0),
            vec4<f32>(uCamera.position.x, 0.0, uCamera.position.z, 1.0));

  output.Position =
      cam * translate_matrix * uMesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;
  output.vUv = input.aUv;

  return output;
}

// fragment shader

fn draw_grid(uv : vec2<f32>) -> vec4<f32> {

  // Setup grid
  var patternSize : f32 = 1.0 / uGrid.division;   // size of the tile
  var edge : f32 = patternSize / uGrid.thickness; // size of the edge
  var face_tone : f32 = 0.0; // 0.9 for the face of the tile
  var edge_tone : f32 = 1.0; // 0.5 for the edge

  // Move Uv to the opposite camera direction to compensate the grid translation
  var compensUv : vec2<f32> = vec2(uv.x - uCamera.position.x * patternSize,
                                   uv.y + uCamera.position.z * patternSize);

  var gridUv
      : vec2<f32> =
            sign(vec2(edge) - fract(compensUv / patternSize) * patternSize);
  var pattern : vec4<f32> = vec4(face_tone - sign(gridUv.x + gridUv.y + 1.0) *
                                                 (face_tone - edge_tone));

  var center : vec2<f32> = vec2(0.5f, 0.5f);
  var white : vec3<f32> = vec3(1.f);
  var black : vec3<f32> = vec3(0.f);
  var red : vec3<f32> = vec3(1.f, 0.f, 0.f);
  var green : vec3<f32> = vec3(0.f, 1.f, 0.f);

  // Add X & Y Axis
  var axisThickness : f32 = 0.04f / uGrid.scale;
  var xAxis : f32 = step(abs(compensUv.x - center.x), axisThickness);
  var yAxis : f32 = step(abs(compensUv.y - center.y), axisThickness);
  var xAxisColor : vec3<f32> = mix(black, red, vec3(xAxis));
  var yAxisColor : vec3<f32> = mix(black, green, vec3(yAxis));
  var axis : vec3<f32> = max(xAxisColor, yAxisColor); // combine axis

  var axisMask : vec4<f32> = vec4(min(1.0f - xAxis, 1.0f - yAxis));

  // Setup gradient
  var fadeFactor : f32 = max(100.0f / abs(uCamera.position.y), 50.0f);
  var ray : f32 = min(distance(uv, center) * uGrid.scale / fadeFactor, 1.0f);
  var grad : vec3<f32> = mix(white, black, ray);

  return (uGrid.color * pattern * axisMask + vec4(axis, 1.0f)) *
         vec4(grad, 0.0f);
}

@fragment fn fs_main(@location(0) vCol : vec3<f32>,
                     @location(1) vUv : vec2<f32>) -> @location(0) vec4<f32> {

  return draw_grid(vUv);
}
