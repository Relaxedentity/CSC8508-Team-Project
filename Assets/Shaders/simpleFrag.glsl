 #version 400 core

uniform vec4 bgColour;


in Vertex{
	vec2 TexCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
	fragColour = bgColour * vec4(1,1,1, 0.8);

}