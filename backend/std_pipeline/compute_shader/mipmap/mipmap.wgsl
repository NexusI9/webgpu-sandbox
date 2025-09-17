@group(0) @binding(0) var src_texture : texture_2d<f32>;
@group(0) @binding(1) var src_sampler : sampler;
@group(0) @binding(2) var dst_texture : texture_storage_2d<rgba8unorm, write>;

// check article:
// https://eliemichel.github.io/LearnWebGPU/basic-compute/image-processing/mipmap-generation.html
@compute @workgroup_size(8, 8) fn
    main(@builtin(global_invocation_id) id : vec3<u32>) {

  let offset = vec2<u32>(0u, 1u);
  let color = (textureLoad(src_texture, 2u * id.xy + offset.xx, 0) +
               textureLoad(src_texture, 2u * id.xy + offset.xy, 0) +
               textureLoad(src_texture, 2u * id.xy + offset.yx, 0) +
               textureLoad(src_texture, 2u * id.xy + offset.yy, 0)) *
              0.25f;

  textureStore(dst_texture, vec2<i32>(i32(id.x), i32(id.y)), color);
}
