// attribute/uniform decls
struct VertexIn {
  @location(0) aPos : vec3<f32>,
                      @location(1) aNorm : vec3<f32>,
                                           @location(2) aCol : vec3<f32>,
                                                               @location(3) aUv
      : vec2<f32>,
};

struct VertexOut {
  @builtin(position) Position : vec4<f32>,
                                @location(0) vCol : vec3<f32>,
                                                    @location(1) vNorm
      : vec3<f32>,
        @location(2) vFrag : vec3<f32>,
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

struct Glass {
  roughness : f32, _padding : vec3<f32>, color : vec4<f32>,
}

// camera viewport
@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;
@group(0) @binding(3) var<uniform> uGlass : Glass;

@group(1) @binding(0) var env_map : texture_cube<f32>;
@group(1) @binding(1) var env_sampler : sampler;

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  var output : VertexOut;
  output.Position = cam * uMesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;
  output.vNorm = input.aNorm;
  output.vFrag = (uMesh.model * vec4<f32>(input.aPos, 1.0f)).xyz;

  return output;
}

const MAX_MIP_LEVEL : u32 = 1u;

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>,
                     @location(1) vNorm : vec3<f32>,
                     @location(2) vFrag : vec3<f32>) -> @location(0) vec4<f32> {

  let N : vec3<f32> = normalize(vNorm);
  let V : vec3<f32> = normalize(uCamera.position.xyz - vFrag);

  // Fresnel
  let NdotV : f32 = max(dot(N, V), 0.0f);
  let f0 : vec3<f32> = vec3(0.04); // dielectric default reflectance
  let f : vec3<f32> = f0 + (1.0f - f0) * pow(1.0 - NdotV, 5.0f);

  // Reflection Direction
  let R : vec3<f32> = reflect(-V, N);

  let mip : f32 = uGlass.roughness * f32(MAX_MIP_LEVEL);
  let reflection : vec4<f32> = textureSample(env_map, env_sampler, R);

  let color : vec4<f32> = mix(uGlass.color, reflection, f.r);

  return color;
}
