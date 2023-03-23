#version 400 core

uniform sampler2D u_texture;
uniform float u_amount;
uniform float u_fade;
uniform float u_health;
uniform float u_time;

in vec2 v_texcoord;

out vec4 out_color;


in Vertex{
	vec2 TexCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
	vec4 center_color = texture(u_texture, v_texcoord);

  	float time = mod(u_time, u_fade * 2.0);
	float fade_factor = smoothstep(u_fade, 0.0, abs(u_fade - time));

	// Calculate offsets for RGB channels
	float offset_red = u_amount * (v_texcoord.y - 0.5);
	float offset_green = u_amount * (v_texcoord.x - 0.5);
	float offset_blue = u_amount * (0.5 - length(v_texcoord - vec2(0.5)));

	// Sample textures with offsets for RGB channels
	vec4 red_color = texture(u_texture, vec2(v_texcoord.x + offset_red, v_texcoord.y));
	vec4 green_color = texture(u_texture, vec2(v_texcoord.x, v_texcoord.y + offset_green));
	vec4 blue_color = texture(u_texture, vec2(v_texcoord.x - offset_blue, v_texcoord.y));

	// Combine colors with different weights for RGB channels
	out_color = vec4(red_color.r, green_color.g, blue_color.b, center_color.a);
	out_color = mix(center_color, out_color, fade_factor);

	// Increase aberration effect when player's health is low
	float health_factor = 1.0 - u_health;
	out_color = mix(center_color, out_color, health_factor);
}

