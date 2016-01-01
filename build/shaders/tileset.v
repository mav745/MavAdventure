//#version 130

varying vec2 v_texCoords;
varying vec2 v_scrCoords;

uniform vec2 u_Origin;
uniform vec2 u_RoomSize;
uniform vec2 u_ViewSize;
uniform float u_Zoom;

void main(void)
{
	float f_Zoom = float(u_Zoom);
	vec2 Orign = u_Origin;
	//Orign.y = -(u_RoomSize.y-480.0) - Orign.y;
	Orign   = floor(mod(Orign,vec2(16.0)));
	Orign.y = -Orign.y;
    gl_Position = gl_ModelViewProjectionMatrix*(floor(gl_Vertex-vec4(Orign,0.0,0.0)));//ftransform();
	v_scrCoords = gl_Position.xy;//*vec2(f_Zoom);
	v_scrCoords.y += (u_RoomSize.y-u_ViewSize.y*f_Zoom)/(u_ViewSize.y*f_Zoom/2.0);
	//v_scrCoords.y = 1.0-v_scrCoords.y;
	v_scrCoords = (v_scrCoords+vec2(1.0))/vec2(2.0);
	v_scrCoords = (v_scrCoords*vec2(u_ViewSize.x*f_Zoom,u_ViewSize.y*f_Zoom)+u_Origin)/u_RoomSize;
	v_scrCoords.y = 1.0-v_scrCoords.y;
	v_texCoords = gl_MultiTexCoord0.st;//mod(gl_MultiTexCoord0.st,vec2(1.0));//gl_TexCoord[0].st;
	//v_texCoords.y = 1.0-v_texCoords.y;
}