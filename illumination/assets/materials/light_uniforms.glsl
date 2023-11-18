#ifndef light_fp
#define light_fp

#define LIGHT_DATA_PIXELS 6
/*
LIGHT DATA 6 pixels
position.x -> rgba
position.y -> rgba
position.z -> rgba

direction.xyz -> rgb (a not used)
color.xyzw -> rgba()

radius, smoothnes, specular, cutoff -> rgba()

*/

uniform lowp sampler2D DATA_TEXTURE;

uniform lowp vec4 ambient_color;
uniform lowp vec4 sunlight_color;
uniform lowp vec4 fog_color;
uniform highp vec4 fog;

uniform highp vec4 light_texture_data;
uniform highp vec4 lights_data; //lights count,radius_max,x_min,xmax
uniform highp vec4 lights_data2; //y_min,y_max,z_min,z_max

highp vec4 getData(float index) {
    float x = mod(index, light_texture_data.y) / light_texture_data.x;
    float y = (index / light_texture_data.x) / light_texture_data.y;

    return texture2D(DATA_TEXTURE, vec2(x, y));
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