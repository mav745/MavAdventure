#ifndef CMAINMANAGER_H
#define CMAINMANAGER_H

#include <QObject>
#include <QGLWidget>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.434294481903251827651
#define M_LN2      0.693147180559945309417
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M_1_PI     0.318309886183790671538
#define M_2_PI     0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.707106781186547524401

#define KEY_ISUP     (1<<0)
#define KEY_PRESSED  (1<<1)
#define KEY_RELEASED (1<<2)
#define KEY_ISDOWN   (1<<3)

class cBaseObject;
class cGameRoom;
class QTimer;
class QGLFunctions;
class QFont;
class QOpenGLShaderProgram;

extern bool g_InputActive;

enum TEXT_ALIGN
{
	ETA_LEFT,
	ETA_CENTER,
	ETA_RIGHT
};

struct custom_font_s
{
	custom_font_s(const QString &filename);
	int   width[256];
	int   height;
	float tex_width;
	float tex_height;
	QVector2D texcoords[256];
	int texture;
	
	inline int strWidth(const QByteArray &str)
	{
		int w = 0;
		for(int i=str.size()-1;i>=0;i--)
			w += width[(uchar)str[i]];
		
		return w;
	}
};

class cMainManager : public QGLWidget
{
	Q_OBJECT
	
	QTimer *m_pStepTimer;
	QVector<QFont *>m_pFonts;
	QVector<QVector3D>m_Colors;
	
public:
	explicit cMainManager(QWidget* parent=0);
	~cMainManager();
	
	QPoint m_ViewPort;
	
	QVector4D m_MiddleFade;
	QVector4D m_UpperFade;
	
	float m_Time;
	float m_DeltaTime;
	float m_FrameTime;
	QString m_NextChMap;
	cGameRoom *m_pCurrentRoom;
	QVector<cGameRoom *>m_pRooms;
	QGLFunctions *m_pGLFns;
	
	static cMainManager *g_pMainMgr;
	
	cGameRoom * AddRoom(QString name = "");
	cGameRoom * RoomByName(QString name);
	void DeleteRoom(QString name);
	void DeleteRoom(cGameRoom *room);
	
	void drawText(int x, int y, const QString &text,TEXT_ALIGN align = ETA_LEFT);
	
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
		drawColorPush(float(r)/255.f,float(g)/255.f,float(b)/255.f);
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
	
	//inline int keyState(int key);
	//inline QVector2D mouseCoords(void);
	
	static QOpenGLShaderProgram *CreateShader(QString vertex, QString pixel);
	bool IsTileExists(const QVector2D &pos, int depth);
	void drawLine(const QVector2D &v1, const QVector2D &v2);
	void drawRect(const QVector2D &v1, const QVector2D &v2);
	inline void drawRect(const QVector2D &v1, float w, float h)
	{
		drawRect(v1,v1+QVector2D(w,h));
	}
	
	void Changelevel(const QString &mapname, bool fade = true);
	void Map(const QString &mapname);
	void drawTextCustom(int x, int y, const QByteArray &text, custom_font_s *fnt,TEXT_ALIGN align = ETA_LEFT);
protected:
	void mouseMoveEvent(QMouseEvent *me);
	
	void initializeGL();
   void resizeGL(int w, int h);
   void paintGL();
	
signals:
	void keyPressed(int key);
	void keyReleased(int key);
	//void TestSignal(int a1, int a2);
public slots:
	
	void EnableInput() {g_InputActive =  true;}
	void DisableInput();
	void UpdateEverything();
	void ProcessInput(const QString &cmd);
	void ChangelevelDelayed();
};

#endif // CMAINMANAGER_H
