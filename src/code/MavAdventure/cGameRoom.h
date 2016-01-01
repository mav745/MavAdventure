#ifndef CGAMEROOM_H
#define CGAMEROOM_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QPoint>
#include <QVector2D>
#include <QMap>

//#include <hpint.h>
#include <cBaseObject.h>
//#include <util.h>

class cBaseObject;
class cTileManager;
//class cBaseRenderable;
class cBaseMenu;
class cSegment;
class cLine;
class cTileLayer;
class cBaseTrigger;

struct roomview_s
{
	roomview_s();
	QVector2D orig;
	//int x;
	//int y;
	bool block[4];
	int block_val[4];
};
class cBackground : public cBaseObject
{
	Q_OBJECT
public:
	explicit cBackground(QObject *parent = 0) : cBaseObject(parent)
	{
		orig = QVector2D(0,0);
		velocity = QVector2D(0,0);
		vwrap = hwrap = true;
		texture = 0;
		//foreg = false;
		visible = true;
		width = height = 1;
		scr_fac = QVector2D(1,1);
		filename = "";
		m_Depth = 100000;
	}
	
	void Step();
	void Draw(void);
	void RenderMe(void) {Draw();}
	QString filename;
	
	int texture;
	
	int width;
	int height;
	
	QVector2D orig;
	QVector2D scr_fac;
	QVector2D velocity;
	
	//bool foreg;
	
	bool visible;
	
	bool vwrap;
	bool hwrap;
};
enum BLOCK_SIDE
{
	EBS_LEFT,
	EBS_RIGHT,
	EBS_UP,
	EBS_DOWN
};
class cGameRoom : public QObject
{
	Q_OBJECT
	
private:
	//QPointF m_ScrollTail;
	//QPoint m_Scroll;
	QVector2D m_Scroll;
	QVector<cBackground *>m_Foregs;
	QVector<cBaseObject *>m_pObjects;
	QMap<QString, cBaseObject *>m_ObjsByName;
	QMap<QString, cBaseObject *>m_ObjsByClassname;
	
	
public:
	explicit cGameRoom(QObject *parent = 0, const QVector2D &size = QVector2D(640,480));
	~cGameRoom();
	
	static cGameRoom * LoadFromFile(const QString &filename);
	void Unload();
	
	int m_Flags;
	
	bool m_Block[4];
	QString m_Name;
	QVector2D m_Size;
	QVector2D m_AutoScroll;
	roomview_s m_View;
	QVector<cTileLayer *>m_Layers;
	QVector<cBaseTrigger *>m_pTriggers;
	QVector<cLine *>m_Lines;
	QVector<cSegment *>m_Walls;
	QVector<cBackground *>m_Backs;
	QVector<cBaseRenderable *>m_pToRender;
	//QVector<roomview_s *>m_pViews;
	cTileManager *m_pTileMngr;
	
	bool m_Persist;
	
	void SetBlock(BLOCK_SIDE side, bool active);
	cBaseObject * CreateObject(QString name);
	void DeleteObject(cBaseObject *obj);
	void RoomUpdate(void);
	void ScrollRoom(const QVector2D &scr, bool ignore_max = false);
	
	void ObjectSetTargetname(cBaseObject *pObj, QString name);
	QList<cBaseObject *> ObjectsByTargetname(QString name);
	cBaseRenderable *RememberRenderable(cBaseRenderable *obj);
	void RemoveRenderable(cBaseRenderable *obj);
	cBackground * AddBack(QString fname,
			QVector2D orig = QVector2D(0.f,0.f),
			QVector2D speed = QVector2D(0.f,0.f),
			QVector2D scrFac = QVector2D(0.f,0.f),
			bool hwrap = false,
			bool vwrap = false,
			int depth = 10000);
	void RemoveBack(cBackground *back);
	void ChangeObjectDepth(cBaseObject *pObj, int new_depth);
	cSegment *CreateExpand(bool axial);
	void RemoveFlaged(void);
	void ExpandWalls();
	void RemoveExpand(cSegment *exp);
	void TracePlayer(const QVector2D &start, const QVector2D &end, struct traceresult_s *ptr);
	cTileLayer *AddLayer(QString fname, int depth);
	QList<cBaseObject *> ObjectsByClassname(QString name);
	cBaseObject *ObjectByTargetname(QString name);
signals:
	void roomStart();
public slots:
	
};

#endif // CGAMEROOM_H
