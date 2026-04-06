struct KawaseUniform {
  texel_size: vec2<f32>,    // (1.0/width, 1.0/height)
               offset: f32, // current Kawase offset (1,2,3...)
                        _pad: f32,
                               _pad2: vec4<f32>,
};

@group(0) @binding(0) var src_texture : texture_2d<f32>;
@group(0) @binding(1) var src_sampler : sampler;
@group(0) @binding(2) var dst_texture : texture_storage_2d<rgba8unorm, write>;
@group(0) @binding(3) var<uniform> uKawase : KawaseUniform;

// Unity Shader:
// https://github.com/tomc128/urp-kawase-blur/blob/master/Assets/KawaseBlur/KawaseBlur.shader

// Other resources:
//   [] https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html
//
//   [] https://github.com/butterw/bShaders/blob/master/blurKawase_15.hlsl
//
//   [] https://community.arm.com/cfs-file/__key/communityserver-blogs-components-weblogfiles/00-00-00-20-66/siggraph2015_2D00_mmg_2D00_marius_2D00_notes.pdf
@compute @workgroup_size(8,8)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {

    let dims = textureDimensions(src_texture);

    let uv = (vec2<f32>(id.xy) + 0.5f) / vec2<f32>(dims);

    let r = uKawase.offset;
    let texel = uKawase.texel_size;

    let o = vec2<f32>(r) * texel;


    var col = textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(o.x, 0.0), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(-o.x, 0.0), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(0.0, o.y), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(0.0, -o.y), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(o.x, o.y), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(o.x, -o.y), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(-o.x, o.y), 0.0);
    col += textureSampleLevel(src_texture, src_sampler, uv + vec2<f32>(-o.x, -o.y), 0.0);
  col = col * 0.125;

  textureStore(dst_texture, vec2<i32>(id.xy), col);
}
