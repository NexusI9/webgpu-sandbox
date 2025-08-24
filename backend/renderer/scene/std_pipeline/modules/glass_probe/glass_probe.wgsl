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
  roughness : f32,
              frost_scale : f32,
                            frost_strength : f32,
                                             _pad : f32,
                                                    color : vec4<f32>,
}

const PROBE_RELFECTION_GRID_LIST_CAPACITY : u32 = 8u;
const PROBE_RELFECTION_MAX_COUNT : u32 = 3u;
const PROBE_RELFECTION_LIST_MAX_COUNT
    : u32 = PROBE_RELFECTION_GRID_LIST_CAPACITY * PROBE_RELFECTION_MAX_COUNT *
            PROBE_RELFECTION_MAX_COUNT * PROBE_RELFECTION_MAX_COUNT;

struct ProbeReflection {
  position : vec3<f32>, radius : f32
}

struct ProbeReflectionList {
  length : u32,
           entries : array<ProbeReflection, PROBE_RELFECTION_LIST_MAX_COUNT>,
}

@group(0) @binding(0) var<uniform> uViewport : Viewport;
@group(0) @binding(1) var<uniform> uCamera : Camera;
@group(0) @binding(2) var<uniform> uMesh : Mesh;
@group(0) @binding(3) var<uniform> uGlass : Glass;
@group(0) @binding(4) var<uniform> uProbeReflectionList : ProbeReflectionList;

@group(1) @binding(0) var probe_reflection_maps : texture_cube_array<f32>;
@group(1) @binding(1) var probe_reflection_sampler : sampler;

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

  // Final Matrix (Projection * View)
  var cam : mat4x4<f32> = uViewport.projection * uCamera.view;

  var output : VertexOut;
  output.Position = cam * uMesh.model * vec4<f32>(input.aPos, 1.0);
  output.vCol = input.aCol;
  output.vNorm = input.aNorm;
  output.vFrag = (uMesh.model * vec4<f32>(input.aPos, 1.0f)).xyz;
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

  let N : vec3<f32> = normalize(vNorm);

  // frost effect
  let n : f32 = perlin_noise(vUv, uGlass.frost_scale);
  let perturbed_N : vec3<f32> = normalize(N + n * uGlass.frost_strength);

  let V : vec3<f32> = normalize(uCamera.position.xyz - vFrag);

  // Fresnel
  let NdotV : f32 = max(dot(perturbed_N, V), 0.0f);
  let f0 : vec3<f32> = vec3(0.04); // dielectric default reflectance
  let f : vec3<f32> = f0 + (1.0f - f0) * pow(1.0 - NdotV, 5.0f);

  // Reflection Direction
  let R : vec3<f32> = reflect(-V, perturbed_N);
  let mip : f32 = uGlass.roughness * f32(MAX_MIP_LEVEL);

   var closest_probe_index : u32 = 4u;
   var best_dist : f32 = 1e9;
   for (var i = 0u; i < uProbeReflectionList.length; i += 1u) {
     let probe_pos = uProbeReflectionList.entries[i].position;
     let dist = distance(vFrag, probe_pos);
     if (dist < best_dist) {
       best_dist = dist;
       closest_probe_index = i;
     }
   }

  let reflection : vec4<f32> = textureSample(probe_reflection_maps,
                                            probe_reflection_sampler, R,
                                            closest_probe_index);

  // let color : vec4<f32> = mix(uGlass.color * reflection, reflection, f.r);

  
  //return vec4<f32>(
  //   vec3<f32>(f32(closest_probe_index) / f32(uProbeReflectionList.length)),
  //   1.0f);

  return reflection;
  
}
