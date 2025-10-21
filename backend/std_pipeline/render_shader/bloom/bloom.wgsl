struct VertexOut {
  @builtin(position) Position : vec4<f32>, @location(0) vUv : vec2<f32>
};

struct BloomUniform {
  threshold : f32, knee : f32, blur : u32, _pad : f32,
};

// bright.wgsl (fragment)
@group(0) @binding(0) var bloom_texture : texture_2d<f32>;
@group(0) @binding(1) var bloom_sampler : sampler;
@group(0) @binding(2) var<uniform> uBloom : BloomUniform;

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

  let texture = textureSample(bloom_texture, bloom_sampler, fragUV).rgb;

  let lum = dot(texture, vec3(0.2126, 0.7152, 0.0722));

  let threshold = 0.3f;
  let knee = 1.0f;  

  let soft = clamp((lum - threshold + knee) / (2.0 * knee), 0.0, 1.0);
  let bright = max(lum - threshold, 0.0) * soft;

  return vec4(texture * bright, 1.0);
}
