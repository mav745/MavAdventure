varying vec2 v_texCoords;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	v_texCoords = gl_MultiTexCoord0.st;
}