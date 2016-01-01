#ifndef CBASETEXTURE_H
#define CBASETEXTURE_H

class cBaseTexture
{
public:
	cBaseTexture();
	
	int m_Tex;
	
	virtual void Render(void);
};

#endif // CBASETEXTURE_H
