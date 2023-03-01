#version 330 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;
void main(void) {
	if(texture(diffuseTex, IN.texCoord).a==0){
		return;
	}
	fragColour = texture(diffuseTex , IN.texCoord );
}