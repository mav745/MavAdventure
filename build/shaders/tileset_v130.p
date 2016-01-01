#version 130

uniform sampler2D s_TileMap;
uniform sampler2D s_TileSet;

uniform vec2 u_TileSetSize;
//uniform vec2 u_Origin;

varying vec2 v_texCoords;
varying vec2 v_scrCoords;

void main(void)
{
	vec2 texCoords = v_texCoords/16.0;
	texCoords.y = 1.0-texCoords.y;
	float idx = round(texture2D(s_TileMap,v_scrCoords).r*255.0);
	idx = mod(idx,u_TileSetSize.x*u_TileSetSize.y);
	vec2 tileCoords = 
		vec2(
			      mod(idx,u_TileSetSize.x)/u_TileSetSize.x,
			1.0-floor(idx/u_TileSetSize.x)/u_TileSetSize.y
			);
	texCoords.y  = 1.0-texCoords.y;
	texCoords.y *= 16.0/u_TileSetSize.y;
	texCoords.y  = 1.0-texCoords.y;
	texCoords.x *= 16.0/u_TileSetSize.x;
	//tileCoords.y = 1.0-tileCoords.y;
	gl_FragColor = texture2D(s_TileSet,texCoords+tileCoords);
	
	//gl_FragColor.rg = tileCoords;
	//gl_FragColor.r = idx;//(u_TileSetSize.x*u_TileSetSize.y);//tileCoords.y;
	//gl_FragColor.g = 0.0;//texCoords.x*256.0;
	//gl_FragColor.b = 0.0;//tileCoords.x;
	//gl_FragColor.a = 1.0;
}