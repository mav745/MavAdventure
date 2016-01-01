uniform sampler2D u_Tex;

varying vec2 v_texCoords;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
    vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main(void)
{
	gl_FragColor = texture2D(u_Tex,v_texCoords);
	//return;
	
	vec3 rgbcol = gl_FragColor.rgb;
	vec3 hsvcol = rgb2hsv(rgbcol);
	float coef = 1.0-clamp(pow(2.0*abs(0.5-hsvcol.z),4.0),0.0,1.0);
	hsvcol.y *= (0.15+0.85*coef);//*1.28;
	hsvcol.y = clamp(hsvcol.y*1.1,0.0,1.0);
	hsvcol.z = clamp(hsvcol.z,0.08,1.0);
	gl_FragColor.rgb = hsv2rgb(hsvcol)+pow(1.0-coef,4.0)*vec3(0.0,0.02,0.03);//*/
	
	gl_FragColor.rgb = mix(texture2D(u_Tex,v_texCoords).rgb,gl_FragColor.rgb,vec3(0.7));
}