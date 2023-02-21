 #version 400 core

uniform sampler2D 	mainTex;

in Vertex{
	vec2 TexCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
	
	//if (texture(mainTex, IN.TexCoord).a <f)
	//{
	//	discard;
	//}

	fragColour = texture(mainTex, IN.TexCoord) * vec4(0,0,1,1);

	//fragColour = vec4(IN.TexCoord, 1,1);



}