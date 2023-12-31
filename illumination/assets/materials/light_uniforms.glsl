#ifndef light_fp
#define light_fp

#define LIGHT_DATA_PIXELS 6
#define LIGHT_RADIUS_MAX 63.0
/*
LIGHT DATA 6 pixels
position.x -> rgba
position.y -> rgba
position.z -> rgba

direction.xyz -> rgb (a not used)
color.xyzw -> rgba()

radius, smoothnes, specular, cutoff -> rgba()

*/

uniform highp sampler2D DATA_TEXTURE;

uniform lowp vec4 ambient_color;
uniform lowp vec4 sunlight_color;
uniform lowp vec4 fog_color;
uniform highp vec4 fog;

uniform highp vec4 light_texture_data;
uniform highp vec4 lights_data; //lights count
uniform highp vec4 lights_camera_data; //near, far
uniform highp vec4 clusters_data; //x_slice, y_slice, z_slice, max_lights_per_cluster
uniform highp vec4 screen_size;

highp vec4 getData(highp int index) {
    // Convert linear index to 2D pixel coordinates
    int x = index % int(light_texture_data.x);
    int y = index / int(light_texture_data.x);

    // Convert pixel coordinates to normalized coordinates
    vec2 normalizedCoords = (vec2(x, y) + 0.5) / light_texture_data.xy;

    // Sample the texture at the normalized coordinates
    return texture2D(DATA_TEXTURE, normalizedCoords);
}

highp float DecodeRGBAToFloatPosition(highp vec4 encoded) {
    encoded.rgb *= 63.0;//
    highp float intPart = round(encoded.r) * 64.0 * 64.0 + round(encoded.g) * 64.0 + round(encoded.b);
    highp float fracPart = encoded.a;
    return intPart - 131072.0 + fracPart;
    return fracPart;
}

const float phong_shininess = 16.0;
// const vec3 specular_color = vec3(1.0);
vec3 point_light2(vec3 light_color, float power, vec3 light_position, vec3 position, vec3 vnormal, float specular, vec3 view_dir)
{

    vec3 dist = light_position - position;
    vec3 direction = vec3(normalize(dist));
    float d = length(dist);

    vec3 reflect_dir = reflect(-direction, vnormal);
    float spec_dot = max(dot(reflect_dir, view_dir), 0.0);

    float irradiance = max(dot(vnormal, direction), 0.05);
    float attenuation = (1.0/(1.0 + d*power + 2.0*d*d*power*power));
    vec3 diffuse = light_color * irradiance * attenuation;

    // if (irradiance > 0.0) {
    diffuse += irradiance * attenuation * specular * pow(spec_dot, phong_shininess) * light_color; // *specular_color
    // }
    return diffuse;
}

vec3 getSpecularColor(vec3 map_specular, float light_specular, vec3 light_color, vec3 light_direction, vec3 surface_normal, vec3 view_direction) {
    if (light_specular == 0.0 || map_specular.x == 0.0) {
        return vec3(0.0);
    }

    float lambertian = max(dot(light_direction, surface_normal), 0.0);

    if (lambertian <= 0.0) {
        return vec3(0.0);
    }

    float surface_shininess = 1.0;

    vec3 reflection_direction = reflect(-light_direction, surface_normal);
    float specular_value = pow(max(dot(view_direction, reflection_direction), 0.0), surface_shininess);

    return light_color * light_specular * specular_value;
}



#endif