#include "cBaseTool.h"

#include <QFrame>
#include <QVector>
#include <QVector2D>

#ifndef CTILETOOL_H
#define CTILETOOL_H

class cTileLayer;

typedef QVector<float> floatvec_t;

struct tile_s
{
	tile_s(tile_s *o)
	{
		index   = o->index  ;
		xflip   = o->xflip  ;
		yflip   = o->yflip  ;
		rotated = o->rotated;
	}

	tile_s()
	{
		xflip = yflip = rotated = false;
		index = 0;
	}
	
	int index;
	bool xflip;
	bool yflip;
	bool rotated;
};

class cTSetBrowser : public QFrame
{
	Q_OBJECT
public:
	explicit cTSetBrowser(QWidget *parent = 0);
	
	QPoint m_Ofs;
	QPoint m_Mouse;
	QPoint m_MousePrev;
	QRect m_Select;
	
	bool m_Disabled;
	
	void ProcessInput();
protected:
	void paintEvent(QPaintEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *me);
	void mouseReleaseEvent(QMouseEvent *me);
	void mouseMoveEvent(QMouseEvent *me);
};
class cTBrushBrowser : public QFrame
{
	Q_OBJECT
public:
	explicit cTBrushBrowser(QWidget *parent = 0);
	
	QPoint m_Mouse;
	QPoint m_MousePrev;
	
	bool m_Disabled;
	
	void ProcessInput();
	
	tile_s * m_Tiles[8][8];
	
	QImage *m_GridImg;
	QPixmap *m_BrushMap;
	
	void ChangeTile(int tx, int ty);
public slots:
	void clearTiles();
	void AssignMe();
	void mirrorTilesHor();
	void mirrorTilesVer();
protected:
	void keyPressEvent(QKeyEvent *e);
	void paintEvent(QPaintEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *me);
	void mouseReleaseEvent(QMouseEvent *me);
	void mouseMoveEvent(QMouseEvent *me);
};

class cTileTool : public cBaseTool
{
	Q_OBJECT
public:
	explicit cTileTool(QObject *parent = 0);
	
	int m_CurLayerIdx;
	
	QVector2D m_GrabStart;
	bool m_Grab;
	
	floatvec_t m_Verts[5];
	floatvec_t m_UVs[5];
	
	QVector<cTileLayer *> m_Layers;
	
	void UpdateLayerList(void);
	void UpdateTSetList(void);
	
	void ProcessInput();
	void DrawMe(void);
	
	void ClearALL();
	cTileLayer *AddLayer(QString fname, int depth);
signals:
	
public slots:
	
	void layerSelected(QString layer);
	void addLayer(void);
	void removeLayer(void);
	void changeLayer(void);
	void changeTSet(void);
	void changeScrFac(void);
	void toggleVisibility();
	void shiftLayer();
	void dupLayer();
};

extern cTileTool *g_pTileTool;

#endif // CTILETOOL_H
