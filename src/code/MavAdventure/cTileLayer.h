#ifndef CTILELAYER_H
#define CTILELAYER_H

#include "cBaseObject.h"

class QImage;

typedef QVector<float> floatvec_t;

class cTileLayer : public cBaseObject
{
	Q_OBJECT
public:
	explicit cTileLayer(QObject *parent = 0);
	~cTileLayer();
	bool m_Visible;
	
	floatvec_t *m_pVerts;
	floatvec_t *m_pUVs;
	
	QImage *m_TileMap;
	
	QString m_FName;
	
	QImage *m_TSetImg;
	int m_Texture;
	int m_TMapTexture;
	
	int m_Width;
	int m_Height;
	
	//QVector2D m_Orig;
	//QVector2D m_ScrollFactor;
	
	void AssignTileSet(QString fname);
	QPoint getCrdsByIndex(int idx);
	int getIndexByCrds(QPoint coords);

	void updateTileMapTex();
	void ShiftLayer(QVector2D sh);
	void Step();
	void RenderMe();
public slots:
	void resizeLayer(QVector2D sz);
};

#endif // CTILELAYER_H
