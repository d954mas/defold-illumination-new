#ifndef light_fp
#define light_fp

/*
LIGHT DATA



*/

uniform lowp vec4 ambient_color;
uniform lowp vec4 sunlight_color;
uniform lowp vec4 fog_color;
uniform highp vec4 fog;
uniform highp vec4 lights_data; //lights count,0,x_min,xmax
uniform highp vec4 lights_data2; //y_min,y_max,z_min,z_max


#endif