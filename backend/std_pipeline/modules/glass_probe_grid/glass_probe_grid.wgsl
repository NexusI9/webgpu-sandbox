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
                                                    @location(1) vNorm
      : vec3<f32>,
        @location(2) vFrag : vec3<f32>,
                             @location(3) vUv : vec2<f32>
};

struct Mesh {
  model : mat4x4<f32>,
          position : vec4<f32>,
                     probe_reflection_plane_id : u32,
                                                 probe_reflection_plane_count
      : u32,
        probe_reflection_grid_id : u32,
                                   probe_reflection_grid_count : u32,
                                                                 _pad
      : array<u32, 40>,
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

struct ProbeReflection {
  position : vec3<f32>, radius : f32, _pad : array<f32, 60>
}

// === UBO ===
struct LightCount {
  point : u32, spot : u32, sun : u32, ambient : u32,
};

struct ProbeCount {
  reflection : u32, plane : u32, irradiance : u32, _pad : u32,
};

struct Fog {
  color : vec4<f32>, start_distance : f32, density : f32, _pad : vec2<f32>,
};

struct UBO {
  light_count : LightCount, probe_count : ProbeCount, fog : Fog,
};

@group(0) @binding(0) var<storage, read> uViewport : array<Viewport>;
@group(0) @binding(1) var<storage, read> uCamera : array<Camera>;
@group(0) @binding(2) var<storage, read> uMesh : array<Mesh>;

@group(1) @binding(0) var<uniform> uGlass : Glass;
@group(1) @binding(1) var<storage, read> uProbeReflectionList
    : array<ProbeReflection>;
@group(1) @binding(2) var<uniform> ubo : UBO;

@group(1) @binding(3) var probe_reflection_maps : texture_cube_array<f32>;
@group(1) @binding(4) var probe_reflection_sampler : sampler;

@group(1) @binding(5) var skybox_map : texture_cube<f32>;
@group(1) @binding(6) var skybox_sampler : sampler;

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

fn fog_factor(distance : f32, fog_start_distance : f32, fog_density : f32)
    -> f32 {

  let fog_distance : f32 = max(distance - fog_start_distance, 0.0f);
  let density : f32 = fog_density * 0.001;
  let exp_factor : f32 = 1.0f - exp(-density * fog_distance);
  return clamp(exp_factor, 0.0f, 1.0f);
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

  // Reflection Direction
  let R : vec3<f32> = normalize(reflect(-V, perturbed_N));
  let mip : f32 = uGlass.roughness * f32(MAX_MIP_LEVEL);

  let skybox : vec4<f32> = textureSample(skybox_map, skybox_sampler, R);
  var reflection : vec4<f32> = vec4<f32>(0.0f, 0.0f, 0.0f, 1.0f);

  // Clip based proximity
  {
    var closest_probe_index : u32 = 0u;
    var best_dist : f32 = 1e9;
    for (var i = 0u; i < ubo.probe_count.reflection; i += 1u) {
      let probe_pos = uProbeReflectionList[i].position;
      let dist = distance(vFrag, probe_pos);
      if (dist < best_dist) {
        best_dist = dist;
        closest_probe_index = i;
      }
    }

    reflection = textureSample(probe_reflection_maps, probe_reflection_sampler,
                               R, closest_probe_index);
  }

  // Weight based proximity
  //{
  //  var total_weight : f32 = 0.0;
  //  for (var i = 0u; i < ubo.probe_count.reflection; i += 1u) {
  //    let probe_pos = uProbeReflectionList[i].position;
  //    let dist = max(distance(vFrag, probe_pos), 0.001);
  //    let w = 1.0 / (dist * dist); // inverse square
  //    let refl =
  //        textureSample(probe_reflection_maps, probe_reflection_sampler, R,
  //        i);
  //
  //      reflection += refl * w;
  //      total_weight += w;
  //    }
  //
  //    reflection /= total_weight;
  //  }

  let composite = mix(skybox, reflection, reflection.a);
  let combined : vec4<f32> = mix(uGlass.color * composite, composite, f.r);

  let fog_k = fog_factor(length(vFrag - camera.position.xyz),
                         ubo.fog.start_distance, ubo.fog.density);

  let out_color = mix(combined, ubo.fog.color, fog_k);
  return out_color;
}
