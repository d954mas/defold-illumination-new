#ifndef float_rgba_utils
#define float_rgba_utils

vec4 float_to_rgba(float v){
    vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc      = fract(enc);
    enc     -= enc.yzww * vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 0.0);
    return enc;
}

highp float rgba_to_float(highp vec4 rgba){
    return dot(rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0));
}

#endif