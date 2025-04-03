// attribute/uniform decls
// WGSL Function references:
// https://webgpufundamentals.org/webgpu/lessons/webgpu-wgsl-function-reference.html#func-textureSampleCompareLevel

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
                           @location(3) vFrag : vec3<f32>
};

// constants
const PI : f32 = 3.14159265359;
const POINT_LIGHT_VIEWS : u32 = 6u;
const MAX_LIGHT : u32 = 16u;

struct Mesh { // 80B
  model : mat4x4<f32>, position : vec4<f32>,
}

struct Camera { // 112B
  view : mat4x4<f32>,
         position : vec4<f32>,
                    lookat : vec4<f32>,
                             mode : u32,
                                    _pad : vec3<u32>,
};

struct Viewport { // 64B
  projection : mat4x4<f32>,
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
struct PointLight {
  position : vec3<f32>,
             _padding : f32,
                        color : vec3<f32>,
                                intensity : f32,
                                            views : array<mat4x4<f32>, 6>
};

struct AmbientLight {
  color : vec3<f32>, intensity : f32,
};

struct DirectionalLight {
  position : vec3<f32>,
             _padding : f32,
                        lookat : vec3<f32>,
                                 _padding_1 : f32,
                                              color : vec3<f32>,
                                                      intensity : f32,
};

struct AmbientLightStorage {
  length : u32, items : array<AmbientLight, MAX_LIGHT>,
};

struct PointLightStorage {
  length : u32, items : array<PointLight, MAX_LIGHT>,
};

struct DirectionalLightStorage {
  length : u32, items : array<DirectionalLight, MAX_LIGHT>,
};

// material pbr
// 1. texture : The actual image data stored in VRAM (pixel colors, normal maps,
// etc.)
// 2. sampler : Defines how the GPU reads the texture (filtering, wrapping,
// mipmaps, etc.)
// TODO: combine textures in a texture_2d_array + only use 1 sampler ?

@group(0) @binding(0) var diffuse_texture : texture_2d<f32>;
@group(0) @binding(1) var diffuse_sampler : sampler;

@group(0) @binding(2) var metallic_texture : texture_2d<f32>;
@group(0) @binding(3) var metallic_sampler : sampler;

@group(0) @binding(4) var normal_texture : texture_2d<f32>;
@group(0) @binding(5) var normal_sampler : sampler;

@group(0) @binding(6) var occlusion_texture : texture_2d<f32>;
@group(0) @binding(7) var occlusion_sampler : sampler;

@group(0) @binding(8) var emissive_texture : texture_2d<f32>;
@group(0) @binding(9) var emissive_sampler : sampler;

// camera viewport
@group(1) @binding(0) var<uniform> uViewport : Viewport;
@group(1) @binding(1) var<uniform> uCamera : Camera;
@group(1) @binding(2) var<uniform> uMesh : Mesh;

// light
// TODO: using uniform for now, but check the storage type
@group(2) @binding(0) var<uniform> ambient_light_list : AmbientLightStorage;
@group(2) @binding(1) var<uniform> directional_light_list
    : DirectionalLightStorage;
@group(2) @binding(2) var<uniform> point_light_list : PointLightStorage;
//@group(2) @binding(3) var shadow_maps : texture_depth_2d_array;
//@group(2) @binding(4) var shadow_sampler : sampler_comparison;
@group(2) @binding(3) var shadow_maps : texture_2d_array<f32>;
@group(2) @binding(4) var shadow_sampler : sampler;

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
  output.vFrag = (uMesh.model * uMesh.position).xyz;

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
  material.metallic = metallic.b;
  material.roughness = metallic.g;                     // roughness in G channel
  material.normal = normalize(normal.rgb * 2.0 - 1.0); // convert to [-1,+1]
  material.occlusion = occlusion.b;
  material.emissive = emissive.rgb;

  return material;
}

fn compute_point_light(material : Material, fragment_position : vec3<f32>,
                       camera_position : vec3<f32>, light_position : vec3<f32>,
                       light_color : vec3<f32>, light_intensity : f32)
    -> vec3<f32> {

  // compute light base on Cook-Torrance BRDF Model

  // 1. Normalize key vectors
  var N : vec3<f32> = normalize(material.normal); // surface normal
  var V : vec3<f32> =
              normalize(camera_position - fragment_position); // view vector
  var L : vec3<f32> = normalize(light_position);              // light direction
  var H : vec3<f32> = normalize(V + L);                       // halfway vector

  // 2. Fresnel effect
  // Determines how much light reflects vs. refracts
  var HdotV : f32 = dot_max(H, V);
  var F0 : vec3<f32> = material.albedo;
  var F : vec3<f32> = f_schlick(material.albedo, HdotV);

  // 3. Normal Distribution Function - NDF (GGX/ Trowbridge-Reitz)
  // Determine how surface distribute microfacet (i.e. bumps)
  var NdotH : f32 = dot_max(N, H);
  var D : f32 = distribution_GGX(NdotH, material.roughness);

  // 4.Geometry Function (Schlick-GGX)
  //  Handles shadow and masking
  var NdotV : f32 = dot_max(N, V);
  var NdotL : f32 = dot_max(N, L);
  var G = geo_smith(NdotV, NdotL, material.roughness);

  // 5. Define Cook-Torrance Specular Reflection
  var specular : vec3<f32> = (D * F * G) / max(4.0f * NdotV * NdotL, 0.001);

  // 6. Compute Diffuse & Final lighting
  // Metallic surface only use the specular term
  // Non-Metallic surface use both diffuse + specular
  var kS = F;
  var kD = vec3(1.0f) - kS;
  kD *= 1.0f - material.metallic;

  var diffuse : vec3<f32> = (material.albedo / PI) * NdotL;
  var Lo : vec3<f32> = (kD * diffuse + specular) * light_color *
                       light_intensity * NdotL;

  // 7. Apply AO //0.03f
  // change first operator to alter ambient light

  return Lo;
}

fn compute_ambient_light(material : Material, light_color : vec3<f32>,
                         light_intensity : f32) -> vec3<f32> {
  return vec3(light_intensity) * material.albedo * material.occlusion;
}

fn point_shadow_position(world_position : vec3<f32>, light_view : mat4x4<f32>)
    -> vec3<f32> {

  // transform world position into clip space
  let shadow_clip = light_view * vec4<f32>(world_position, 1.0f);

  // convert to NDC coordinates
  let shadow_ndc = shadow_clip.xyz / shadow_clip.w;

  // convert to UV (0-1 range)
  let shadow_uv = shadow_ndc.xy * 0.5 + 0.5;

  // returns depth to compare
  return vec3<f32>(shadow_uv, shadow_ndc.z);
}

fn point_shadow_factor(frag_position : vec3<f32>) -> f32 {
  var factor : f32 = 1.0f; // show by default
  for (var l : u32 = 0u; l < point_light_list.length; l++) {
    for (var v : u32 = 0u; v < POINT_LIGHT_VIEWS; v++) {
      let shadow_position = point_shadow_position(
          frag_position, point_light_list.items[l].views[v]);
      let layer = l * POINT_LIGHT_VIEWS + v;
      // factor *=
      //     textureSampleCompare(shadow_maps, shadow_sampler,
      //     shadow_position.xy,
      //   layer, shadow_position.z);
    }
  }

  return factor;
}

// fragment shader
@fragment fn fs_main(@location(0) vNormal : vec3<f32>,
                     @location(1) vCol : vec3<f32>,
                     @location(2) vUv : vec2<f32>,
                     @location(3) vFrag : vec3<f32>) -> @location(0) vec4<f32> {

  var albedo = textureSample(diffuse_texture, diffuse_sampler, vUv);
  var metallic = textureSample(metallic_texture, metallic_sampler, vUv);
  var normal = textureSample(normal_texture, normal_sampler, vUv);
  var occlusion = textureSample(occlusion_texture, occlusion_sampler, vUv);
  var emissive = textureSample(emissive_texture, emissive_sampler, vUv);

  let material = create_material(albedo, metallic, normal, occlusion, emissive);

  // let shadow_factor = point_shadow_factor(vFrag);
  // let shadow_color = mix(vec3<f32>(0.2f), vec3<f32>(1.0f), shadow_factor);

  let coord = vec2<i32>(vUv);
  var shadow_combined = vec4<f32>(0.0);
  for (var l : u32 = 0u; l < 6u; l++) {
    shadow_combined += textureLoad(shadow_maps, coord, l, 0i);
  }

  var light_pos = vec3<f32>(0.0f);
  var light_color = vec3<f32>(0.0f);
  var Lo : vec3<f32> = vec3<f32>(0.0f);
  var ambient : vec3<f32> = vec3<f32>(0.0f);

  // calculate point lights
  for (var i = 0u; i < point_light_list.length; i = i + 1u) {
    let light = point_light_list.items[i];
    Lo += compute_point_light(material, vFrag, uCamera.position.xyz,
                              light.position - vFrag, light.color,
                              light.intensity);
  }

  // calculate direction lights
  // basically the same a point light, but use a fixed light direction instead
  // of a one per fragment like a point light
  for (var i = 0u; i < directional_light_list.length; i = i + 1u) {
    let light = directional_light_list.items[i];
    let light_direction = -1.0f * light.lookat - light.position;
    Lo += compute_point_light(material, vFrag, uCamera.position.xyz,
                              light_direction, light.color, light.intensity);
  }

  // calulate ambient lights
  for (var i = 0u; i < ambient_light_list.length; i = i + 1u) {
    let light = ambient_light_list.items[i];
    ambient += compute_ambient_light(material, light.color, light.intensity);
  }

  var color = ambient + Lo;

  // let shadow_position =
  // point_shadow_position(vFrag, point_light_list.items[0].views[1]);
  // return vec4<f32>(color, 1.0f);
  // return vec4<f32>(color * shadow_color, 1.0f);

  // let point = abs(point_light_list.items[0].views[1][3][3]) - 2.5f;
  // return vec4<f32>(vec3<f32>(point), 1.0f);
  return vec4<f32>(shadow_combined.rrr, 1.0f);
}
