#extension GL_EXT_gpu_shader4 : enable

#define X_FLIPPED 0
#define Y_FLIPPED 1
#define ROTATED   2


uniform sampler2D s_TileMap;
uniform sampler2D s_TileSet;

uniform vec2 u_TileSetSize; //in tiles, i.e. (width/16, height/16)
//uniform vec2 u_Origin;

varying vec2 v_texCoords;
varying vec2 v_scrCoords;

bool CheckFlag(int value, int flagpos)
{
	if ((value & (1<<flagpos)) == 0) return false;
	return true;
}

void main(void)
{
	vec2 texCoords = mod(v_texCoords,vec2(1.0));
	vec4 mapData = texture2D(s_TileMap,v_scrCoords);
	
	//gl_FragColor = texture2D(s_TileMap,v_scrCoords);//texture2D(s_TileMap,v_texCoords/vec2(640.0/16.0,-480.0/16.0));
	//gl_FragColor.r *= 32.0;
	//return;
	
	int  flags   = int(round(mapData.y*256.0));
	if ( CheckFlag(flags,X_FLIPPED)) texCoords.x = 1.0-texCoords.x;
	if (!CheckFlag(flags,Y_FLIPPED)) texCoords.y = 1.0-texCoords.y;
	if ( CheckFlag(flags,  ROTATED))
	{
		float x = texCoords.x;
		texCoords.x = 1.0-texCoords.y;
		texCoords.y = x;
	}
	float idx = round(mapData.x*255.0);
	vec2 tileCoords = vec2(mod(idx,u_TileSetSize.x),floor(idx/u_TileSetSize.x)+1.0);
	tileCoords /= u_TileSetSize;
	tileCoords.y = 1.0-tileCoords.y;
	texCoords /= u_TileSetSize;
	
	// texCoords = texCoords/vec2(16.0);
	// float idx = round(mapData.x*255.0);
	// float maxIdx = u_TileSetSize.x*u_TileSetSize.y-1.0;
	// idx = mod(idx,u_TileSetSize.x*u_TileSetSize.y);
	// vec2 tileCoords = 
		// vec2(
				  // mod(idx,u_TileSetSize.x)/u_TileSetSize.x,
				// floor((maxIdx-idx)/u_TileSetSize.x)/u_TileSetSize.y
			// );
	// texCoords.y  = 1.0-texCoords.y;
	// texCoords.y *= 16.0/u_TileSetSize.y;
	// texCoords.y  = 1.0-texCoords.y;
	// texCoords.x *= 16.0/u_TileSetSize.x;

	gl_FragColor = texture2D(s_TileSet,texCoords+tileCoords);
	
	//gl_FragColor.rg = tileCoords;
	// gl_FragColor.r = idx/255.0;//texCoords.x*256.0;//(u_TileSetSize.x*u_TileSetSize.y);//tileCoords.y;
	// gl_FragColor.g = 0.0;//texCoords.y;
	// gl_FragColor.b = 0.0;//tileCoords.x;
	// gl_FragColor.a = 1.0;
}