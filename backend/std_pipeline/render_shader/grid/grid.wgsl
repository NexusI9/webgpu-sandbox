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

// NOTE:
// Need to add padding cause WebGPU align to memory based on 16-bytes
// alignment rule Meaning that data need to be a multiple of 16 bytes 1 float
// (4bytes) is automatically handled by WebGPU, 3 floats (12 bytes) as well:
// both are padded automatically to 16bytes. However for 2 floats (8 bytes) like
// the case below, provoke misalignment Each bind group has independent
// alignment.
// Maybe a trick to have better control on that is to use struct
// Important to align both C and WGPU struct, especially if the struct isn't the
// last of the bind group

// Can check:
// https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/
// However requires inverted matrix/vertex which is expensive...

struct GridData {
  color : vec4<f32>,
          division : f32,
                     scale : f32,
                             thickness : f32,
                                         _padding : f32
}

@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

@group(1) @binding(0) var<uniform> uGrid : GridData;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  let mesh = uMesh;
  let camera = uCamera;
  let viewport = uViewport;

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = viewport.projection * camera.view;
  var output : VertexOut;
  var offset : vec2<f32> = vec2<f32>(camera.position.x, camera.position.z);

  offset.x = camera.position.x;
  offset.y = camera.position.z;

  // Put the grid below the camera
  var translate_matrix
      : mat4x4<f32> = mat4x4<f32>(vec4<f32>(1.0, 0.0, 0.0, 0.0),
                                  vec4<f32>(0.0, 1.0, 0.0, 0.0),
                                  vec4<f32>(0.0, 0.0, 1.0, 0.0),
                                  vec4<f32>(offset.x, 0.0, offset.y, 1.0));

  output.Position =
      cam * translate_matrix * mesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;
  output.vUv = input.aUv;

  return output;
}

// fragment shader

@fragment fn fs_main(@location(0) vCol : vec3<f32>,
                     @location(1) vUv : vec2<f32>) -> @location(0) vec4<f32> {

  let mesh = uMesh;
  let camera = uCamera;
  let viewport = uViewport;

  var offset : vec2<f32> = vec2<f32>(camera.position.x, camera.position.z);

  // Setup grid
  var patternSize : f32 = 1.0 / uGrid.division;   // size of the tile
  var edge : f32 = patternSize / uGrid.thickness; // size of the edge
  var face_tone : f32 = 0.0; // 0.9 for the face of the tile
  var edge_tone : f32 = 1.0; // 0.5 for the edge

  // Move Uv to the opposite camera direction to compensate the grid translation
  var compensUv : vec2<f32> = vec2(vUv.x + offset.x * patternSize,
                                   vUv.y + offset.y * patternSize);
  var gridUv
      : vec2<f32> =
            sign(vec2(edge) - fract(compensUv / patternSize) * patternSize);

  var pattern : vec4<f32> = vec4(face_tone - sign(gridUv.x + gridUv.y + 1.0) *
                                                 (face_tone - edge_tone));

  var center : vec2<f32> = vec2(0.5f, 0.5f);
  var white : vec3<f32> = vec3(1.f);
  var black : vec3<f32> = vec3(0.f);
  var x_color : vec3<f32> = vec3(0.91f, 0.29f, 0.23f);
  var z_color : vec3<f32> = vec3(0.23f, 0.40f, 0.91f);

  // Add X & Y Axis
  var axisThickness : f32 = 0.0005f;
  var yAxis
      : f32 = step(abs(vUv.x - 0.5f + offset.x / uGrid.scale), axisThickness);
  var xAxis
      : f32 = step(abs(vUv.y - 0.5f + offset.y / uGrid.scale), axisThickness);
  var yAxisColor : vec3<f32> = mix(black, z_color, vec3(yAxis));
  var xAxisColor : vec3<f32> = mix(black, x_color, vec3(xAxis));
  var axis : vec3<f32> = max(xAxisColor, yAxisColor); // combine axis

  var axisMask : vec4<f32> = vec4(min(1.0f - xAxis, 1.0f - yAxis));

  // Setup gradient
  var fade_factor : f32 = max(abs(camera.position.y), 50.0f);
  var ray : f32 = min(pow(distance(vUv, center), 4.0f) * 11.0f, 1.0f);
  var grad : vec3<f32> = mix(white, black, ray);
  var avg : f32 = (grad.r + grad.g + grad.b) / 3.0f;

  return (uGrid.color * pattern * axisMask + vec4(axis, 1.0f)) *
         vec4(grad, avg);
}
