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
                             @location(3) vUv : vec2<f32>
};

struct Mesh {
  model : mat4x4<f32>, position : vec4<f32>, _pad : array<u32, 44>,
}

struct Camera {
  view : mat4x4<f32>,
         position : vec4<f32>,
                    lookat : vec4<f32>,
                             mode : u32,
                                    _pad : array<u32, 39>,
};

struct Viewport {
  projection : mat4x4<f32>, width : u32, height : u32, _pad : array<u32, 46>,
};

struct Glass {
  roughness : f32,
              frost_scale : f32,
                            frost_strength : f32,
                                             _pad : f32,
                                                    color : vec4<f32>,
}

struct Projection {
  view : mat4x4<f32>, _padding : array<u32, 48>
};

struct PlaneReflection {
  position : vec3<f32>,
             near : f32,
                    normal : vec3<f32>,
                             far : f32,
                                   scale : vec3<f32>,
                                           distance : f32,
                                                      tangent : vec3<f32>,
                                                                signed_distance
      : f32,
        bitangent : vec3<f32>,
                    _pad1 : array<f32, 45>,
}

// === UBO ===
struct LightCount {
  point : u32, spot : u32, sun : u32, ambient : u32,
};

struct ProbeCount {
  reflection : u32, irradiance : u32,
};

struct UBO {
  light_count : LightCount, probe_count : ProbeCount,
};

@group(0) @binding(0) var<storage, read> uViewport : array<Viewport>;
@group(0) @binding(1) var<storage, read> uCamera : array<Camera>;
@group(0) @binding(2) var<storage, read> uMesh : array<Mesh>;

@group(1) @binding(0) var<uniform> uGlass : Glass;
@group(1) @binding(1) var<storage, read> uPlaneReflectionList
    : array<PlaneReflection>;
@group(1) @binding(2) var<storage, read> uProjections : array<Projection>;
@group(1) @binding(3) var<uniform> ubo : UBO;

@group(1) @binding(4) var probe_reflection_maps : texture_2d_array<f32>;
@group(1) @binding(5) var probe_reflection_sampler : sampler;

@group(1) @binding(6) var skybox_map : texture_cube<f32>;
@group(1) @binding(7) var skybox_sampler : sampler;

//
//
//
// ▗▖  ▗▖▗▄▄▄▖▗▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖
// ▐▌  ▐▌▐▌   ▐▌ ▐▌ █  ▐▌    ▝▚▞▘
// ▐▌  ▐▌▐▛▀▀▘▐▛▀▚▖ █  ▐▛▀▀▘  ▐▌
//  ▝▚▞▘ ▐▙▄▄▖▐▌ ▐▌ █  ▐▙▄▄▖▗▞▘▝▚▖
//
//
//
//

// vertex shader
@vertex fn vs_main(input : VertexIn) -> VertexOut {

  let mesh = uMesh[0];
  let camera = uCamera[0];
  let viewport = uViewport[0];

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = viewport.projection * camera.view;

  var output : VertexOut;
  output.Position = cam * mesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;
  output.vNorm = input.aNorm;
  output.vFrag = (mesh.model * vec4<f32>(input.aPos, 1.0f)).xyz;
  output.vUv = input.aUv;

  return output;
}

const MAX_MIP_LEVEL : u32 = 1u;

//
//
//
// ▗▖  ▗▖ ▗▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
// ▐▛▚▖▐▌▐▌ ▐▌  █  ▐▌   ▐▌
// ▐▌ ▝▜▌▐▌ ▐▌  █   ▝▀▚▖▐▛▀▀▘
// ▐▌  ▐▌▝▚▄▞▘▗▄█▄▖▗▄▄▞▘▐▙▄▄▖
//
//
//
//

fn fract_vec2(v : vec2<f32>) -> vec2<f32> { return v - floor(v); }

fn fract_vec3(v : vec3<f32>) -> vec3<f32> { return v - floor(v); }

fn n22(p : vec2<f32>) -> vec2<f32> {
  var a =
      fract_vec3(vec3<f32>(p.x, p.y, p.x) * vec3<f32>(123.34, 234.34, 345.65));
  a = a + vec3<f32>(dot(a, a + 34.45));
  return fract_vec2(vec2<f32>(a.x * a.y, a.y * a.z));
}

fn get_gradient(pos : vec2<f32>) -> vec2<f32> {
  let two_pi : f32 = 6.283185;
  let angle : f32 = n22(pos).x * two_pi;
  return vec2<f32>(cos(angle), sin(angle));
}

fn perlin_noise(uv : vec2<f32>, cells_count : f32) -> f32 {
  let pos_in_grid = uv * cells_count;
  let cell_pos_in_grid = floor(pos_in_grid);
  let local_pos_in_cell = pos_in_grid - cell_pos_in_grid;
  let blend =
      local_pos_in_cell * local_pos_in_cell * (3.0 - 2.0 * local_pos_in_cell);

  let left_top = cell_pos_in_grid + vec2<f32>(0.0, 1.0);
  let right_top = cell_pos_in_grid + vec2<f32>(1.0, 1.0);
  let left_bottom = cell_pos_in_grid + vec2<f32>(0.0, 0.0);
  let right_bottom = cell_pos_in_grid + vec2<f32>(1.0, 0.0);

  let left_top_dot = dot(pos_in_grid - left_top, get_gradient(left_top));
  let right_top_dot = dot(pos_in_grid - right_top, get_gradient(right_top));
  let left_bottom_dot =
      dot(pos_in_grid - left_bottom, get_gradient(left_bottom));
  let right_bottom_dot =
      dot(pos_in_grid - right_bottom, get_gradient(right_bottom));

  let noise_value = mix(mix(left_bottom_dot, right_bottom_dot, blend.x),
                        mix(left_top_dot, right_top_dot, blend.x), blend.y);

  return 0.5 + 0.5 * (noise_value / 0.7);
}

fn compute_reflection_uv(frag_pos : vec3<f32>, r_view : mat4x4<f32>)
    -> vec2<f32> {
  let clip = r_view * vec4<f32>(frag_pos, 1.0);
  let ndc = clip.xyz / clip.w;                                // [-1, 1] space
  let uv = ndc.xy * vec2<f32>(0.5f, -0.5f) + vec2<f32>(0.5f); // [0, 1] space
  return uv;
}

//
//
//
//
// ▗▄▄▄▖▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖
// ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▛▚▞▜▌▐▌   ▐▛▚▖▐▌  █
// ▐▛▀▀▘▐▛▀▚▖▐▛▀▜▌▐▌▝▜▌▐▌  ▐▌▐▛▀▀▘▐▌ ▝▜▌  █
// ▐▌   ▐▌ ▐▌▐▌ ▐▌▝▚▄▞▘▐▌  ▐▌▐▙▄▄▖▐▌  ▐▌  █
//
//
//
//
//

// fragment shader
@fragment fn fs_main(@location(0) vCol : vec3<f32>,
                     @location(1) vNorm : vec3<f32>,
                     @location(2) vFrag : vec3<f32>,
                     @location(3) vUv : vec2<f32>) -> @location(0) vec4<f32> {

  let camera = uCamera[0];

  let N : vec3<f32> = normalize(vNorm);

  // frost effect
  let n : f32 = perlin_noise(vUv, uGlass.frost_scale);
  let perturbed_N : vec3<f32> = normalize(N + n * uGlass.frost_strength);

  let V : vec3<f32> = normalize(camera.position.xyz - vFrag);

  // Fresnel
  let NdotV : f32 = max(dot(perturbed_N, V), 0.0f);
  let f0 : vec3<f32> = vec3(0.04); // dielectric default reflectance
  let f : vec3<f32> = f0 + (1.0f - f0) * pow(1.0 - NdotV, 5.0f);

  let plane_index = 0u;
  let plane = uPlaneReflectionList[plane_index];

  let ro = camera.position.xyz;
  let viewDir = normalize(vFrag - ro);

  let R = reflect(viewDir, normalize(plane.normal));

  let local = vFrag - plane.position;

  let reflUV = compute_reflection_uv(vFrag, uProjections[0].view);

  let skybox : vec4<f32> = textureSample(skybox_map, skybox_sampler, R);

  let reflection : vec4<f32> = textureSample(probe_reflection_maps,
                                             probe_reflection_sampler, reflUV,
                                             plane_index);

  let composite = mix(skybox, reflection, reflection.a);

  let out_color : vec4<f32> = mix(uGlass.color * composite, composite, f.r);

  return out_color;
}
