#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;
uniform int EffectActive;

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec2 vTexture;

out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;
out vec2 textureVector;

void main()
{ 
	textureVector = vTexture;

	// Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);

	if(EffectActive == 1) {
		//Deform the model depending on the Y of the vertice
		vec3 vPosition_deform;
		if(vPosition.y < 1 || (vPosition.y > 2 && vPosition.y < 3)) vPosition_deform = vPosition + (vNormal * 0.05 * cos(vPosition.y));
		else vPosition_deform = vPosition - (vNormal * 0.05 * cos(vPosition.y));

		gl_Position = Projection * ModelView * vec4(vPosition_deform, 1.0f);
	}
	else {gl_Position = Projection * ModelView * vec4(vPosition, 1.0f);}
}
