 #version 400 core

uniform float score;
uniform vec4 playerColour;

in Vertex{
	vec2 TexCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
	

	float progressMask = score > IN.TexCoord.x ?  1 :  0;
	
	// create a rounded corners for health bar // create a signed distance field for a line segment
	vec2 coords = IN.TexCoord;
	coords.x *= 8;
	
	vec2 pointOnLineSeg = vec2(clamp(coords.x, 0.5, 7.5), 0.5);
	float sdf = distance(coords, pointOnLineSeg)  * 2 - 1;

	if(-sdf < 0)
	{discard;}
	
	float borderSdf = sdf + 0.1;

	float partDrv = fwidth(borderSdf); 
	float borderMask = 	1-clamp(borderSdf/partDrv, 0,1);

	// create a colour mask for the bar
	vec4 progressColour = playerColour; 
	vec4 bgColour = vec4(0.149, 0.992, 0.294,1);
	
	vec4 finalColour = mix(bgColour, playerColour, progressMask );

	fragColour = finalColour * borderMask;

}