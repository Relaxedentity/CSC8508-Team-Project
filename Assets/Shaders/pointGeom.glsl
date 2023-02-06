#version 330 core

uniform float particleSize = 20.0f;
uniform mat4 modelMatrix 	= mat4(1.0f);
uniform mat4 viewMatrix 	= mat4(1.0f);
uniform mat4 projMatrix 	= mat4(1.0f);
uniform mat4 shadowMatrix 	= mat4(1.0f);

layout(points) in;
layout(triangle_strip , max_vertices = 4) out;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN[];
out Vertex {
	vec4 colour;
	vec2 texCoord;
} OUT;

void main() {
	mat4 mvp 		  = (projMatrix * viewMatrix * modelMatrix);

	vec4 temp		= mvp * gl_in[ 0 ]. gl_Position;

	
		//top right
		gl_Position = temp;
		gl_Position.x += particleSize;
		gl_Position.y += particleSize;
		OUT.texCoord = vec2 (1,0);

		EmitVertex ();
		
		//Top Left
		gl_Position = temp;
		gl_Position.x -= particleSize;
		gl_Position.y += particleSize;
		OUT.texCoord = vec2 (0,0);
		EmitVertex ();

		// bottom right
		gl_Position = temp;
		gl_Position.x += particleSize;
		gl_Position.y -= particleSize;
		OUT.texCoord = vec2 (1,1);
		EmitVertex ();

		// bottom Left
		gl_Position = temp;
		gl_Position.x -= particleSize;
		gl_Position.y -= particleSize;
		OUT.texCoord = vec2 (0,1);
		EmitVertex ();
		
		EndPrimitive ();
	
}