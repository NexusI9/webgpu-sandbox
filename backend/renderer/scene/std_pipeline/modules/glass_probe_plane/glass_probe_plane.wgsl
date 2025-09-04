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
                    _pad : array<f32, 45>,
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
@group(1) @binding(2) var<uniform> ubo : UBO;
@group(1) @binding(3) var probe_reflection_maps : texture_2d_array<f32>;
@group(1) @binding(4) var probe_reflection_sampler : sampler;

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

fn ray_intersect_plane(ray_origin : vec3<f32>, ray_dir : vec3<f32>,
                       plane_normal : vec3<f32>, plane_signed_distance : f32)
    -> vec3<f32> {

  let denom = dot(plane_normal, ray_dir);

  if (abs(denom) < 1e-6) {
    return ray_origin;
  }

  let t = (plane_signed_distance - dot(plane_normal, ray_origin)) / denom;
  return ray_origin + t * ray_dir;
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
  // var offset : vec2<f32> = vec2<f32>(camera.position.x, camera.position.z);
  //
  //  if ((camera.mode & 2u) != 0u) {
  //    // fix position to target (lookat) if camera is Orbit mode
  //    offset.x = camera.lookat.x;
  //    offset.y = camera.lookat.z;
  //  }

  let plane_index = 0u;

  let plane = uPlaneReflectionList[plane_index];

  let ro = camera.position.xyz;
  let viewDir = normalize(vFrag - ro);

  let R = reflect(viewDir, normalize(plane.normal));

  let I = ray_intersect_plane(ro, R, plane.normal, plane.signed_distance);

  // let local = I - plane.position;
  let local = vFrag - plane.position;

  let u = dot(local, plane.tangent);
  let v = dot(local, plane.bitangent);
  var uv = (vec2<f32>(u,v) / plane.scale.xz) * 0.5f + 0.5f;

  let reflection : vec4<f32> =
                       textureSample(probe_reflection_maps,
                                     probe_reflection_sampler, uv, plane_index);

  return reflection + vec4<f32>(uv, 1.0f, 1.0f);
  //  return vec4<f32>(fract(uv), 0.0f, 1.0f);
  //   return vec4<f32>(abs(plane.bitangent), 1.0f);
  //     return vec4<f32>(1.0f, 0.0f, 0.0f, 1.0f);
}
