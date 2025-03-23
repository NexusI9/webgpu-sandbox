// attribute/uniform decls
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

const PI : f32 = 3.1415926535897931;

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
};

struct Material {
  albedo : vec3<f32>,
           metallic : f32,
                      roughness : f32,
                                  normal : vec3<f32>,
                                           occlusion : f32,
                                                       emissive : vec3<f32>,
};

// material pbr
// 1. texture : The actual image data stored in VRAM (pixel colors, normal maps,
// etc.)
// 2. sampler : Defines how the GPU reads the texture (filtering, wrapping,
// mipmaps, etc.)

@group(0) @binding(0) var diffuse_texture : texture_2d<f32>;
@group(0) @binding(1) var metallic_texture : texture_2d<f32>;
@group(0) @binding(2) var normal_texture : texture_2d<f32>;
@group(0) @binding(3) var occlusion_texture : texture_2d<f32>;
@group(0) @binding(4) var emissive_texture : texture_2d<f32>;

@group(1) @binding(0) var diffuse_sampler : sampler;
@group(1) @binding(1) var metallic_sampler : sampler;
@group(1) @binding(2) var normal_sampler : sampler;
@group(1) @binding(3) var occlusion_sampler : sampler;
@group(1) @binding(4) var emissive_sampler : sampler;

// camera viewport
@group(2) @binding(0) var<uniform> uViewport : Viewport;
@group(2) @binding(1) var<uniform> uCamera : Camera;
@group(2) @binding(2) var<uniform> uMesh : Mesh;

// light
//@group(3) @binding(0) var<uniform> light_direction : vec3<f32>;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;
  var output : VertexOut;

  output.vPosition = cam * uMesh.model * vec4<f32>(input.aPos, 1.0);
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
  material.metallic = metallic.r;
  material.roughness = metallic.g;                     // roughness in G channel
  material.normal = normalize(normal.rgb * 2.0 - 1.0); // convert to [-1,+1]
  material.occlusion = occlusion.r;
  material.emissive = emissive.rgb;

  return material;
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

  var light_pos = vec3<f32>(1.0f, 1.0f, 1.0f);
  var light_color = vec3<f32>(1.0f, 1.0f, 1.0f);

  // compute light base on Cook-Torrance BRDF Model

  // 1. Normalize key vectors
  var N : vec3<f32> = normalize(material.normal);              // surface normal
  var V : vec3<f32> = normalize(uCamera.position.xyz - vFrag); // view vector
  var L : vec3<f32> = normalize(light_pos - vFrag); // light direction
  var H : vec3<f32> = normalize(V + L);             // halfway vector

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

  var diffuse = (material.albedo / PI) * NdotL;
  var Lo = (kD * diffuse * specular) * light_color * NdotL;

  // 7. Apply AO
  var ambient : vec3<f32> =
                    vec3(0.03f) * material.albedo; //* material.occlusion
  var color = ambient * Lo;

  return vec4<f32>(ambient, 1.0f);
}
