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

    float axis_x = lights_data.w-lights_data.z;
    float axis_y = lights_data2.y-lights_data2.x;
    float axis_z = lights_data2.w-lights_data2.z;


    highp float xStride = screen_size.x/clusters_data.x;
    highp float yStride = screen_size.y/clusters_data.y;
    highp float zStride = clusters_data.z;


    int clusterX_index = int(floor(gl_FragCoord.x/ xStride));
    int clusterY_index = int(floor(gl_FragCoord.y/ yStride));
    int clusterZ_index = int(floor(-var_view_position.z) / zStride);



    float clusterID = round(float(clusterX_index) +
    float(clusterY_index) * clusters_data.x +
    float(clusterZ_index) * clusters_data.x * clusters_data.y);

    highp int cluster_tex_idx = int(round(lights_data.x*float(LIGHT_DATA_PIXELS) + clusterID * (1.0+clusters_data.w)));
    int num_lights = int(round(rgba_to_float(getData(cluster_tex_idx))*clusters_data.w));

    for (int i = 0; i < num_lights; ++i) {
        highp int light_tex_idx = cluster_tex_idx +1 + i;
        int lightIdx = int(round(rgba_to_float(getData(light_tex_idx))*lights_data.x));
        // lightIdx = i;
       // if (num_lights!= int(lights_data.x)){
         //   gl_FragColor = vec4(1.0,0.0,0.0, 1.0);
       //     return;
      //  }
        //if (lightIdx != i){
        //    break;
       // }
        //if (lightIdx>460){
          //  gl_FragColor = vec4(1,0,0,1);
            //return;
        //}

        highp int lightIndex = lightIdx * LIGHT_DATA_PIXELS;
        float x = lights_data.z + rgba_to_float(getData(lightIndex))*axis_x;
        float y = lights_data2.x + rgba_to_float(getData(lightIndex+1))*axis_y;
        float z = lights_data2.z + rgba_to_float(getData(lightIndex+2))*axis_z;
        // vec3 spotDirection = getData(lightIndex+3).xyz;
        vec4 lightColorData = getData(lightIndex+4);
        vec4 lightData = getData(lightIndex+5);

        vec3 lightPosition = vec3(x, y, z);
        float lightRadius = lightData.x*lights_data.y;
        float lightSmoothness = lightData.y;
        float lightSpecular = lightData.z;
        float lightCutoff = lightData.w;


        float lightDistance = length(lightPosition - var_world_position);
        if (lightDistance > lightRadius) {
            // Skip this light source because of distance
            continue;
        }

        vec3 lightColor = lightColorData.rgb* lightColorData.a;
        vec3 lightDirection = normalize(lightPosition - var_world_position);
        vec3 lightIlluminanceColor = point_light2(lightColor.rgb, lightSmoothness, lightPosition, var_world_position, var_world_normal
        , lightSpecular, view_direction);



        if (lightCutoff < 1.0) {
            vec3 spotDirection = getData(lightIndex+3).xyz* 2.0 - vec3(1.0);
            float spot_theta = dot(lightDirection, normalize(spotDirection));

            float spot_cutoff = lightCutoff * 2.0 - 1.0;

            if (spot_theta <= spot_cutoff) {
                continue;
            }

            if (lightSmoothness > 0.0) {
                float spot_cutoff_inner = (spot_cutoff + 1.0) * (1.0 - lightSmoothness) - 1.0;
                float spot_epsilon = spot_cutoff_inner - spot_cutoff;
                float spot_intensity = clamp((spot_cutoff - spot_theta) / spot_epsilon, 0.0, 1.0);

                lightIlluminanceColor = lightIlluminanceColor * spot_intensity;;
            }
        }

        illuminance_color = illuminance_color + lightIlluminanceColor;

        //
    }


    //REGION SHADOW -----------------
    // shadow map
    vec4 depth_proj = var_texcoord0_shadow / var_texcoord0_shadow.w;
    float shadow = shadow_calculation(depth_proj.xyzw);
    vec3 shadow_color = shadow_color.xyz*shadow_color.w*(sunlight_color.w) * shadow;

    vec3 diff_light = vec3(0);
    diff_light += max(direct_light(sunlight_color.rgb, sun_position.xyz, var_world_position.xyz, var_world_normal, shadow_color)*sunlight_color.w, 0.0);
    diff_light += vec3(illuminance_color.xyz);

    color.rgb = color.rgb * (min(diff_light, 1.0));

    // Fog
    float dist = abs(var_view_position.z);
    float fog_max = fog.y;
    float fog_min = fog.x;
    float fog_factor = clamp((fog_max - dist) / (fog_max - fog_min) + fog_color.a, 0.0, 1.0);
    color = mix(fog_color.rgb, color, fog_factor);


    gl_FragColor = vec4(color, texture_color.a);

    //float colorz = floor(-var_view_position.z-camNear)/clusters_data.z;
  //  gl_FragColor = vec4(float(clusterX_index)/clusters_data.x,float(clusterY_index)/clusters_data.y,float(clusterZ_index)/1.0, texture_color.a);
   // gl_FragColor = vec4(float(clusterZ_index)/5.0,float(clusterZ_index)/5.0,float(clusterZ_index)/5.0, texture_color.a);
    //gl_FragColor = vec4(clusterZ_index/10.0,color.g,color.b, texture_color.a);
}