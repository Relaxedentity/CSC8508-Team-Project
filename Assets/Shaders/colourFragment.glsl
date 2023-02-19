 #version 400 core

uniform float health;
uniform vec4 firstColour;
uniform vec4 secColour;

in Vertex{
	vec2 TexCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

float InverseLerp(float a, float b, float c)
{
	return (c-a)/(b-a);
}

void main(void) {
	

	float healthMask = health > IN.TexCoord.x ?  1 :  0;
	
	// create a rounded corners for health bar // create a signed distance field for a line segment
	vec2 coords = IN.TexCoord;
	coords.x *= 8;
	
	vec2 pointOnLineSeg = vec2(clamp(coords.x, 0.5, 7.5), 0.5);
	float sdf = distance(coords, pointOnLineSeg)  * 2 - 0.7;

	if(-sdf < 0)
	{discard;}
	
	float borderSdf = sdf + 0.1;

	float partDrv = fwidth(borderSdf); 
	float borderMask = 	1-clamp(borderSdf/partDrv, 0,1);

	// create a colour mask for the bar
	float tHealthColour = clamp(InverseLerp(0.2, 0.8, health), 0.0, 1.0);
	vec4 HBColour = mix(firstColour,secColour , tHealthColour);
	vec4 bgColour = vec4(0,0,0,1);
	
	vec4 finalColour = mix(bgColour, HBColour, healthMask );


	fragColour = finalColour * borderMask;
	//fragColour = vec4(sdf.xxx, 1) ;

}