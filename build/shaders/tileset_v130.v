#version 130

varying vec2 v_texCoords;
varying vec2 v_scrCoords;

uniform vec2 u_Origin;
uniform vec2 u_RoomSize;

void main(void)
{
	vec2 Orign = u_Origin;
	//Orign.y = -(u_RoomSize.y-480.0) - Orign.y;
	Orign   = floor(mod(Orign,vec2(16.0)));
	Orign.y = -Orign.y;
    gl_Position = gl_ModelViewProjectionMatrix*(floor(gl_Vertex-vec4(Orign,0.0,0.0)));//ftransform();
	v_scrCoords = gl_Position.xy;
	v_scrCoords.y += (u_RoomSize.y-480.0)/240.0;
	//v_scrCoords.y = 1.0-v_scrCoords.y;
	v_scrCoords = (v_scrCoords+vec2(1.0))/vec2(2.0);
	v_scrCoords = (v_scrCoords*vec2(640.0,480.0)+u_Origin)/u_RoomSize;
	//v_scrCoords.y = 1.0-v_scrCoords.y;
	v_texCoords = gl_MultiTexCoord0.st;//gl_TexCoord[0].st;
	//v_texCoords.y = 1.0-v_texCoords.y;
}