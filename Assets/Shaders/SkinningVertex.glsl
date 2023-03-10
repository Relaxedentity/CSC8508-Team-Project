#version 400

uniform mat4 modelMatrix 	= mat4(1.0f);
uniform mat4 viewMatrix 	= mat4(1.0f);
uniform mat4 projMatrix 	= mat4(1.0f);
uniform mat4 shadowMatrix 	= mat4(1.0f);
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 5)in vec4 jointWeights;
layout(location = 6)in ivec4 jointIndices;

uniform vec4 		objectColour = vec4(1,1,1,1);

uniform bool hasVertexColours = false;

uniform mat4 joints [128];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));
	OUT.shadowProj 	=  shadowMatrix * vec4 ( position,1);
	vec4 localPos = vec4(position , 1.0f);
	vec4 skelPos = vec4 (0,0,0,0);
	
	OUT.normal 		= normalize ( normalMatrix * normalize ( normal ));
	for(int i = 0; i < 4; ++i) {
		int jointIndex = jointIndices[i];
		float jointWeight = jointWeights[i];
		
		skelPos += joints[jointIndex] * localPos * jointWeight;
	}
	vec3 worldPosition = ( modelMatrix * vec4 ( position ,1)). xyz ;
	OUT.worldPos 	= worldPosition;
	vec4 temp = skelPos* vec4(1,1,-1,1);
	gl_Position = mvp * vec4(temp.xyz , 1.0);
	if(hasVertexColours) {
		OUT.colour		= objectColour * colour;
	}
	OUT.texCoord = texCoord;
	OUT.colour		= objectColour;
}