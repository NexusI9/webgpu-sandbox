struct KawaseUniform {
  texel_size : vec2<f32>,    // (1.0/width, 1.0/height)
               offset : f32, // current Kawase offset (1,2,3...)
                        _pad : f32,
                               _pad2 : vec4<f32>,
};

@group(0) @binding(0) var src_texture : texture_2d<f32>;
@group(0) @binding(1) var src_sampler : sampler;
@group(0) @binding(2) var dst_texture : texture_storage_2d<rgba8unorm, write>;
@group(0) @binding(3) var<uniform> uKawase : KawaseUniform;

// Unity Shader:
// https://github.com/tomc128/urp-kawase-blur/blob/master/Assets/KawaseBlur/KawaseBlur.shader
@compute @workgroup_size(8, 8) fn
    main(@builtin(global_invocation_id) id : vec3<u32>) {

  let dims = textureDimensions(src_texture);

  let uv = (vec2<f32>(id.xy) + 0.5) / vec2<f32>(dims);
  let res = uKawase.texel_size;
  let i = uKawase.offset;

  // Center sample
  var col = textureSampleLevel(src_texture, src_sampler, uv, 0.0);

  // Diagonal samples
  col +=
      textureSampleLevel(src_texture, src_sampler, uv + vec2(i, i) * res, 0.0);
  col +=
      textureSampleLevel(src_texture, src_sampler, uv + vec2(i, -i) * res, 0.0);
  col +=
      textureSampleLevel(src_texture, src_sampler, uv + vec2(-i, i) * res, 0.0);
  col += textureSampleLevel(src_texture, src_sampler, uv + vec2(-i, -i) * res,
                            0.0);

  col = col / 5.0;

  textureStore(dst_texture, vec2<i32>(id.xy), col);
}
