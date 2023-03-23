 #version 400 core

in vec3 position;
in vec4 colour;
in vec2 texCoord;

out Vertex {
	vec2 TexCoord;
} OUT;

void main(void) {

	
	gl_Position = vec4(position , 1.0);
	OUT.TexCoord = texCoord;

}