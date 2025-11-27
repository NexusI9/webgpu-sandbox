@group(0) @binding(0) var src_texture : texture_2d<f32>;
@group(0) @binding(1) var src_sampler : sampler;
@group(0) @binding(2) var dst_texture : texture_storage_2d<rgba8unorm, write>;

@compute @workgroup_size(8, 8)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {
    let size = textureDimensions(src_texture);

    // Bounds check
    if gid.x >= size.x || gid.y >= size.y {
        return;
    }

    // Load BGRA pixel
    let bgra: vec4<f32> = textureLoad(src_texture, vec2<i32>(gid.xy), 0);

    // Rearrange → RGBA
    let rgba: vec4<f32> = vec4<f32>(bgra.z, bgra.y, bgra.x, bgra.w);

    // Store into RGBA texture
    textureStore(dst_texture, vec2<i32>(gid.xy), rgba);
}
