#version 400 core

uniform vec2 u_Center;
uniform float u_Radius;
uniform vec4 u_Color;

out vec4 fragColor;

void main(){
    float dist = distance(u_Center,gl_FragCoord.xy);
    if(dist > u_Radius)
        discard;
    
    fragColor = u_Color;
}