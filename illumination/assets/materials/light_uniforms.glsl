#ifndef light_fp
#define light_fp

/*
LIGHT DATA 6 pixels
position.x -> rgba
position.y -> rgba
position.z -> rgba

direction.xyz -> rgb (a not used)
color.xyzw -> rgba()

radius, smoothnes, specular, cutoff -> rgba()




*/

uniform lowp vec4 ambient_color;
uniform lowp vec4 sunlight_color;
uniform lowp vec4 fog_color;
uniform highp vec4 fog;

uniform highp vec4 light_texture_data;
uniform highp vec4 lights_data; //lights count,radius_max,x_min,xmax
uniform highp vec4 lights_data2; //y_min,y_max,z_min,z_max


#endif