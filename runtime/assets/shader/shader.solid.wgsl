// attribute/uniform decls
// WGSL Function references:
// https://webgpufundamentals.org/webgpu/lessons/webgpu-wgsl-function-reference.html#func-textureSampleCompareLevel

// constants
const PI : f32 = 3.14159265359;
const POINT_LIGHT_VIEWS : u32 = 6u;
const MAX_LIGHT : u32 = 16u;
const PCF_KERNEL_SIZE : u32 = 2u;

struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aCol : vec3<f32>,
                                                               @location(3) aUv
      : vec2<f32>,
};

struct VertexOut {
  @builtin(position) vPosition : vec4<f32>,
                                 @location(0) vNormal : vec3<f32>,
                                                        @location(1) vCol
      : vec3<f32>,
        @location(2) vUv : vec2<f32>,
                           @location(3) vFrag : vec3<f32>,
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
};

struct Material {
  albedo : vec3<f32>,
           metallic : f32,
                      roughness : f32,
                                  normal : vec3<f32>,
                                           occlusion : f32,
                                                       emissive : vec3<f32>,
};

// Lights
struct SunLight {
  position : vec3<f32>,
             intensity : f32,
                         color : vec3<f32>,
                                 _padding : f32,
                                            view : mat4x4<f32>,
};

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;
  var output : VertexOut;

  output.vPosition = cam * uMesh.model * vec4<f32>(input.aPos, 1.0f);
  output.vNormal = normalize(input.aNorm);
  output.vCol = input.aCol;
  output.vUv = input.aUv;

  // transform vertex position (object local position) to world space
  // used for lightning to know where the vertex is in the world space
  // if only used model, vertex position is "constriained" its own local space
  output.vFrag = (uMesh.model * vec4<f32>(input.aPos, 1.0f)).xyz;

  return output;
}

fn compute_point_light(fragment_position : vec3<f32>, vertex_normal : vec3<f32>,
                       camera_position : vec3<f32>, light_position : vec3<f32>,
                       light_color : vec3<f32>, light_intensity : f32)
    -> vec3<f32> {

  // 1. Normalize key vectors
  var V : vec3<f32> =
              normalize(camera_position - fragment_position); // view vector

  // transform normal to worldspace
  let world_normal = (uMesh.model * vec4<f32>(vertex_normal, 0.0f)).xyz;

  // make dot product to check if normal point towards light or not
  var L : vec3<f32> =
              vec3<f32>(dot(normalize(world_normal),
                            normalize(light_position - fragment_position)));

  return vec3<f32>(L);
}

// fragment shader
@fragment fn fs_main(@location(0) vNormal : vec3<f32>,
                     @location(1) vCol : vec3<f32>,
                     @location(2) vUv : vec2<f32>,
                     @location(3) vFrag : vec3<f32>) -> @location(0) vec4<f32> {

  let coord = vec2<i32>(vUv);
  var Lo : vec3<f32> = vec3<f32>(0.0f);

  var sun_light : SunLight;
  sun_light.position = vec3<f32>(20.0f, 20.0f, 20.0f);
  sun_light.intensity = 0.2f;
  sun_light.color = vec3<f32>(1.0f, 1.0f, 1.0f);

  // calculate sun lights
  Lo += compute_point_light(vFrag, vNormal, uCamera.position.xyz,
                            sun_light.position, sun_light.color,
                            sun_light.intensity);

  // calulate ambient lights
  let shadow = 0.4;
  let light = 0.7;
  Lo = mix(vec3<f32>(shadow), vec3<f32>(light), Lo);

  return vec4<f32>(Lo, 1.0f);
}
