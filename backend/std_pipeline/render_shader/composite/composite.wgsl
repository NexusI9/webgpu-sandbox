struct VertexOut {
  @builtin(position) Position : vec4<f32>, @location(0) vUv : vec2<f32>
};

struct CompositeUniform {
  bloom_intensity : f32,
                    vignette_strength : f32,
                                        vignette_radius : f32,
                                                          exposure : f32,
};

@group(0) @binding(0) var scene_texture : texture_2d<f32>;
@group(0) @binding(1) var bloom_texture : texture_2d<f32>;
@group(0) @binding(2) var linear_sampler : sampler;
@group(0) @binding(3) var<uniform> uComposite : CompositeUniform;

@vertex fn vs_main(@builtin(vertex_index) vertex_index : u32) -> VertexOut {
  var vertices = array<vec2<f32>, 3>(
      vec2<f32>(-1.0, -1.0), vec2<f32>(3.0, -1.0), vec2<f32>(-1.0, 3.0));

  var out : VertexOut;
  out.Position = vec4<f32>(vertices[vertex_index], 0.0, 1.0);
  out.vUv = vec2<f32>(0.5, -0.5) * out.Position.xy + vec2<f32>(0.5, 0.5);
  return out;
}

@fragment fn fs_main(@location(0) fragUV : vec2<f32>) ->
    @location(0) vec4<f32> {

  let scene = textureSample(scene_texture, linear_sampler, fragUV).rgb;
  let bloom = textureSample(bloom_texture, linear_sampler, fragUV).rgb;
  var color = scene + bloom * uComposite.bloom_intensity; // additive bloom

  // vignette
  let pos = fragUV * 2.0 - vec2(1.0);
  let dist = length(pos);
  let vig =
      smoothstep(uComposite.vignette_radius, 1.0, dist); // radius -> falloff
  let vignette = mix(1.0, 1.0 - uComposite.vignette_strength,
                     clamp(vig, 0.0, 1.0)); // strength

  color = color * vignette;

  // simple tone mapping + exposure
  color = vec3(1.0) - exp(-color * uComposite.exposure);
  // gamma
  color = pow(color, vec3(1.0 / 2.2));
  
  return vec4(color, 1.0);
}
