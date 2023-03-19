 #version 400 core

uniform float progress;
uniform vec4 loadedColour;

in Vertex{
	vec2 TexCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
	

    float progressMask = progress > IN.TexCoord.x ?  1 :  0;
	
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
	vec4 progressColour = loadedColour; 
	vec4 bgColour = vec4(0, 0, 0,1);
	
	vec4 finalColour = mix(bgColour, loadedColour, progressMask );

	fragColour = finalColour * borderMask;

}