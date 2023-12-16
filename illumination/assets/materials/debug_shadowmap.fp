//
// debug.fp
// github.com/astrochili/defold-illumination
// Copyright (c) 2022 Roman Silin
// MIT license. See LICENSE for details.
//

varying mediump vec3 model_normal;
varying mediump vec2 texture_coord;

uniform lowp sampler2D DIFFUSE_TEXTURE;

#include "/illumination/assets/materials/float_rgba_utils.glsl"


void main() {
    vec4 rgba = texture2D(DIFFUSE_TEXTURE, texture_coord.xy);
    float depth = rgba_to_float(rgba);

    gl_FragColor = vec4(depth,depth,depth,1.0);
}