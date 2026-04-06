struct ColormapUniform {
    colors: array<vec4<f32>, 32>, 
    count: u32,           
};

@group(0) @binding(0) var src_texture : texture_2d<f32>;
@group(0) @binding(1) var src_sampler : sampler;
@group(0) @binding(2) var dst_texture : texture_storage_2d<rgba8unorm, write>;
@group(0) @binding(3) var<uniform> uMap : ColormapUniform;


fn luminance(c: vec3<f32>) -> f32 {
    return dot(c, vec3<f32>(0.2126, 0.7152, 0.0722));
}
  
@compute @workgroup_size(8,8)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {

    let size = textureDimensions(src_texture);
    if gid.x >= size.x || gid.y >= size.y {
        return;
    }

    let uv = vec2<i32>(gid.xy);
    let color = textureLoad(src_texture, uv, 0);

    let lum = luminance(color.rgb);  // 0..1

    let n = uMap.count;

    if n == 1u {
        textureStore(dst_texture, uv, uMap.colors[0]);
        return;
    }

    let step = 1.0f / f32(n - 1u);

    // Find segment index
    let idx_f = lum / step;
    let i = clamp(u32(idx_f), 0u, n - 2u); // segment start index
    let t = fract(idx_f); // normalized blend factor inside the segment
    let c0 = uMap.colors[i];
    let c1 = uMap.colors[i + 1u];
    let remap_color = mix(c0, c1, t);

    let out_color = vec4<f32>(remap_color.rgb, color.a);

    textureStore(dst_texture, uv, out_color);
}
