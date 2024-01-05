uniform lowp sampler2D DIFFUSE_TEXTURE;


#include "/illumination/assets/materials/shadow/shadow_fp.glsl"
#include "/illumination/assets/materials/light_uniforms.glsl"


varying mediump vec2 var_texcoord0;
varying highp vec3 var_world_position;
varying mediump vec3 var_world_normal;
varying highp vec4 var_view_position;
varying highp vec3 var_camera_position;

void main() {
    vec4 texture_color = texture2D(DIFFUSE_TEXTURE, var_texcoord0);
    vec3 color = texture_color.rgb;
    // Defold Editor
    // if (sun_position.xyz == vec3(0)) {
    //gl_FragColor = vec4(color.rgb * vec3(0.8), 1.0);
    //  return;
    // }

    //COLOR
    vec3 illuminance_color = vec3(0);
    vec3 specular_color = vec3(0);

    vec3 surface_normal = var_world_normal;
    vec3 view_direction = normalize(var_camera_position - var_world_position);

    // Ambient
    vec3 ambient = ambient_color.rgb * ambient_color.w;
    illuminance_color = illuminance_color + ambient;


    highp float xStride = screen_size.x/clusters_data.x;
    highp float yStride = screen_size.y/clusters_data.y;
    highp float zStride = (lights_camera_data.y-lights_camera_data.x)/clusters_data.z;


    int clusterX_index = int(floor(gl_FragCoord.x/ xStride));
    int clusterY_index = int(floor(gl_FragCoord.y/ yStride));
    int clusterZ_index = int(floor(-var_view_position.z) / zStride);

    gl_FragColor = vec4(float(clusterX_index)/clusters_data.x,float(clusterY_index)/clusters_data.y,float(clusterZ_index)/clusters_data.z, texture_color.a);
}