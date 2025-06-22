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
struct AmbientLight {
  color : vec3<f32>, intensity : f32,
};

struct SunLight {
  position : vec3<f32>,
             intensity : f32,
                         color : vec3<f32>,
                                 _padding : f32,
                                            view : mat4x4<f32>,
};

// material pbr
// 1. texture : The actual image data stored in VRAM (pixel colors, normal maps,
// etc.)
// 2. sampler : Defines how the GPU reads the texture (filtering, wrapping,
// mipmaps, etc.)
// TODO: combine textures in a texture_2d_array + only use 1 sampler ?

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

// Fresnel usin Schlick Approximation
fn f_schlick(F0 : vec3<f32>, HdotV : f32) -> vec3<f32> {
  // Metallic surface : F0 = albedo
  // Dielectric surface : F0 = vec3(0.4) (plastic/ wood...)
  return F0 + (1.0f - F0) * pow(1.0f - HdotV, 5.0);
}

fn distribution_GGX(NdotH : f32, roughness : f32) -> f32 {
  var a = roughness * roughness;
  var a2 = a * a;
  var NdotH2 = NdotH * NdotH;

  var num = a2;
  var denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
  denom = PI * denom * denom;
  return num / denom;
}

fn geo_schlick_ggx(NdotV : f32, roughness : f32) -> f32 {
  var r = (roughness + 1.0f);
  var k = (r * r) / 8.0f;
  return NdotV / (NdotV * (1.0f - k) + k);
}

fn geo_smith(NdotV : f32, NdotL : f32, roughness : f32) -> f32 {
  return geo_schlick_ggx(NdotV, roughness) * geo_schlick_ggx(NdotL, roughness);
}

fn dot_max(A : vec3<f32>, B : vec3<f32>) -> f32 { return max(dot(A, B), 0.0f); }

fn create_material(diffuse : vec4<f32>, metallic : vec4<f32>,
                   normal : vec4<f32>, occlusion : vec4<f32>,
                   emissive : vec4<f32>) -> Material {

  var material : Material;
  material.albedo = diffuse.rgb;
  material.metallic = metallic.g;
  material.roughness = metallic.g;                     // roughness in G channel
  material.normal = normalize(normal.rgb * 2.0 - 1.0); // convert to [-1,+1]
  material.occlusion = occlusion.r;
  material.emissive = emissive.rgb;

  return material;
}

fn compute_point_light(fragment_position : vec3<f32>, vertex_normal : vec3<f32>,
                       camera_position : vec3<f32>, light_position : vec3<f32>,
                       light_color : vec3<f32>, light_intensity : f32)
    -> vec3<f32> {

  // compute light base on Cook-Torrance BRDF Model

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

  var ambient_light : AmbientLight;
  ambient_light.color = vec3<f32>(1.0f, 1.0f, 1.0f);
  ambient_light.intensity = 0.3f;

  var sun_light : SunLight;
  sun_light.position = vec3<f32>(20.0f, 20.0f, 20.0f);
  sun_light.intensity = 1.0f;
  sun_light.color = vec3<f32>(1.0f, 1.0f, 1.0f);

  // calculate sun lights
  Lo += compute_point_light(vFrag, vNormal, uCamera.position.xyz,
                            sun_light.position, sun_light.color,
                            sun_light.intensity);

  // calulate ambient lights
  Lo += ambient_light.intensity;

  return vec4<f32>(Lo, 1.0f);
}
