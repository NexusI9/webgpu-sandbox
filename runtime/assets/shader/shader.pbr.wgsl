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
        @location(2) vUv : vec2<f32>
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
};

struct PBRMaterial {
  diffuse_factor : vec4<f32>,
                   metallic_factor : f32,
                                     roughness_factor : f32,
                                                        normalScale
      : f32,
        occlusion_factor : f32,
                           emissive_factor : vec3<f32>,
                                             _padding : f32,
};

// material pbr
// 1. texture : The actual image data stored in VRAM (pixel colors, normal maps,
// etc.)
// 2. sampler : Defines how the GPU reads the texture (filtering, wrapping,
// mipmaps, etc.)

@group(0) @binding(0) var<uniform> uMaterial : PBRMaterial;

//@group(1) @binding(1) var diffuse_texture : texture_2d<f32>;
//@group(1) @binding(2) var metallic_texture : texture_2d<f32>;
//@group(1) @binding(3) var normal_texture : texture_2d<f32>;
//@group(1) @binding(4) var occlusion_texture : texture_2d<f32>;
//@group(1) @binding(5) var emissive_texture : texture_2d<f32>;

//@group(1) @binding(6) var diffuse_sampler : sampler;
//@group(1) @binding(7) var metallic_sampler : sampler;
//@group(1) @binding(8) var normal_sampler : sampler;
//@group(1) @binding(9) var occlusion_sampler : sampler;
//@group(1) @binding(10) var emissive_sampler : sampler;

// light
//@group(2) @binding(0) var<uniform> light_direction : vec3<f32>;

// camera viewport
@group(1) @binding(0) var<uniform> uViewport : Viewport;
@group(1) @binding(1) var<uniform> uCamera : Camera;
@group(1) @binding(2) var<uniform> uMesh : Mesh;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;
  var output : VertexOut;

  output.vPosition = cam * uMesh.model * vec4<f32>(input.aPos, 1.0);
  output.vNormal = normalize(input.aNorm);
  output.vCol = input.aCol;
  output.vUv = input.aUv;

  return output;
}

// fragment shader
@fragment fn fs_main(@location(0) vNormal : vec3<f32>,
                     @location(1) vCol : vec3<f32>,
                     @location(2) vUv : vec2<f32>) -> @location(0) vec4<f32> {

  return vec4<f32>(1.0, 1.0, 1.0, 1.0);
}
