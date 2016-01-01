//#ifndef CTILEMANAGER_H
//#define CTILEMANAGER_H

//#include <QObject>
//#include <QVector>
//#include <QMap>
//#include <QPoint>
//#include <QSize>
//#include "cBaseRenderable.h"
//#include <cTileLayer.h>

//#define X_FLIPPED  0
//#define Y_FLIPPED  1
//#define ROTATED    2
//#define TILE_SOLID 3

//class QImage;

//struct tileinfo_s
//{
//	unsigned char index;
//	unsigned char flags;
//};

//class cTileManager : public QObject
//{
//	Q_OBJECT
//	QImage *GetImage(QString fname);
//public:
//	explicit cTileManager(QObject *parent = 0);
//	~cTileManager();
	
//	floatvec_t m_Verts;
//	floatvec_t m_UVs;
//	//float m_Verts[8];
//	//float m_UVs[8];
	
//	QVector<tilelayer_t *> m_Layers;
	
//	void AddLayer(
//			int depth, 
//			QString tileset_filename = "res/tilesets/test.png",
//			QString tilemap_filename = "res/maps/test_map.png");
//	void DeleteLayer(int depth);
//	void PositionLayer(int depth, const QVector2D &o);
//	QVector2D LayerPosition(int depth);
//	void HideLayer(int depth);
//	void ShowLayer(int depth);
//	bool IsLayerVisible(int depth);
//	bool LayerExists(int depth);
//	void LayerSetDepth(int depth, int new_depth);
	
//	void DrawAllLayers(void);
	
//	void ScrollAllLayers(QVector2D sc);
//	void SetLayerScrFac(int depth, const QVector2D &scrfc);
//	bool IsTileExists(QVector2D pos, int depth);
//	tilelayer_t *GetLayer(int depth);
//	int GetTileHeight(QVector2D _pos, int depth);
//	tileinfo_s GetTileInfo(QVector2D _pos, tilelayer_t *layer);
	
//	int TraceVer(QVector2D _pos, int depth, float tracedist, bool up = false);
//signals:
	
//public slots:
	
//};

//#endif // CTILEMANAGER_H
