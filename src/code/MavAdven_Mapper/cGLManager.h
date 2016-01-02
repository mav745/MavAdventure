#ifndef CGLMANAGER_H
#define CGLMANAGER_H

#include <QGLWidget>
#include <QVector3D>
#include <QTimer>
#include <QVector2D>

#include <math.h>

class QTimer;
class QGLFunctions;
class QFont;
class cBase;
class cBaseTool;
class cLine;
class QOpenGLShaderProgram;

enum EDIT_MODE
{
	EEM_NOTHING = 0,
	EEM_LINES,
};

enum TEXT_ALIGN
{
	ETA_LEFT,
	ETA_CENTER,
	ETA_RIGHT
};

struct vis_elems_s
{
	vis_elems_s()
	{
		grid = axis = true;
	}

	bool grid;
	bool axis;
};

class cGLManager : public QGLWidget
{
	Q_OBJECT
public:
	//friend class cTSetBrowser;
	
	explicit cGLManager(QWidget *parent = 0);
	~cGLManager();
	
	EDIT_MODE m_EditMode;
	
	vis_elems_s m_Vis;
	
	QTimer *m_pStepTimer;
	QVector<QFont *>m_pFonts;
	QVector<QVector3D>m_Colors;
	
	QVector2D m_RoomSize;
	QVector2D m_ViewSize;
	bool m_ResizeHor;
	bool m_ResizeVer;
	
	float m_Time;
	float m_DeltaTime;
	float m_Zoom;
	QGLFunctions *m_pGLFns;
	
	QVector2D m_Mouse;
	QVector2D m_View;
	int m_GridSize;
	int m_GridTexs[5];
	
	QVector<cBase *>m_Objects;
	
	cBase *AllocObject(QString name);
	
	void drawText(int x, int y, const QString &text,TEXT_ALIGN align = ETA_LEFT);
	
	QVector2D MouseCoords(bool snapped = false);
	
	void DrawLine(QVector2D v1, QVector2D v2);
	
	inline float GridCellSize(void) {return pow(2,m_GridSize+3);}
	
	inline void drawFontPush(QFont *fnt)
	{
		m_pFonts.push_front(fnt);
	}
	inline QFont * drawFontPop(void)
	{
		if (!m_pFonts.size()) return NULL;
		
		QFont * f = m_pFonts[0];
		m_pFonts.pop_front();
		return f;
	}
	inline void drawColorPush(int r, int g, int b) 
	{
		drawColorPush(QVector3D(float(r)/255.f,float(g)/255.f,float(b)/255.f));
	}
	inline void drawColorPush(float r, float g, float b)
	{
		drawColorPush(QVector3D(r,g,b));
		glColor3f(r,g,b);
	}
	inline void drawColorPush(const QVector3D &color)
	{
		m_Colors.push_front(color);
	}
	inline QVector3D drawColorPop(void)
	{
		if (!m_Colors.size()) 
		{
			glColor3f(0,0,0);
			return QVector3D(0,0,0);
		}
		
		QVector3D c = m_Colors[0];
		glColor3f(c.x(),c.y(),c.z());
			
		m_Colors.pop_front();
		return c;
	}
	
	int KeyState(int key);
	void ObjectsByClassname(QString classname, QVector<cBase *> &vec);
	void RemoveObject(cBase *obj);
	void DrawLine(float x1, float y1, float x2, float y2);
	void FillRect(QVector2D v1, QVector2D v2);
	void OutlineRect(QVector2D v1, QVector2D v2);
	
	void SetObjectDepth(cBase *obj, int depth);
	void RegisterInput(int key, bool released = false);
	void ProcessInput();
	QOpenGLShaderProgram *CreateShader(QString vertex, QString pixel);
	void SaveAss();
	void Save();
	void Open();
	
protected:
	void wheelEvent       (QWheelEvent *we);
	void mousePressEvent  (QMouseEvent *me);
	void mouseReleaseEvent(QMouseEvent *me);
	void mouseMoveEvent   (QMouseEvent *me);
	void mouseDoubleClickEvent(QMouseEvent *me);
	void keyPressEvent    (QKeyEvent    *e);
	void keyReleaseEvent  (QKeyEvent    *e);
	
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	
signals:
	void keyPressed(int key);
	void keyReleased(int key);
	void roomSizeChanged(QVector2D sz);
	
public slots:
	void UpdateEverything(void);
	void menuAction(QAction *ac);
};


extern cGLManager *g_pMgr;
extern cBaseTool *g_CurrentTool;
//extern QVector2D gMouse;

#define MGR g_pMgr
#define GET_OBJECT(x) g_pMgr->AllocObject(#x)
#define REMOVE_OBJECT(x) g_pMgr->RemoveObject(x)
#define SET_OBJECT_DEPTH(x,y) g_pMgr->SetObjectDepth(x,y)
#define OBJECTS_BY_CLASSNAME(x,y) g_pMgr->ObjectsByClassname(#x,y)
#endif // CGLMANAGER_H
