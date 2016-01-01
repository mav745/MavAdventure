#include <QVector2D>

#ifndef UTIL_H
#define UTIL_H

#define XX 0
#define YY 1
#define ZZ 2

#define MGR cMainManager::g_pMainMgr

#define VEC_EXPAND QVector2D(15,30)

#define FL_KILLME   (1<<0)
#define FL_TRANSFER (1<<1)

extern QString g_Language;
extern bool  g_BootsUpgrade;
extern float g_fBootsUpgTime;

extern int butA	 ;
extern int butB	 ;
extern int butC	 ;
extern int butStart;
extern int butUp   ;
extern int butDown ;
extern int butLeft ;
extern int butRight;

extern bool g_Paused;

class cGameRoom;
class cBaseObject;

typedef struct cvar_s
{
	float      value_f;
	int        value_i;
	QByteArray value_s;
	
	QByteArray value_def;
	
	void set(QByteArray value)
	{
		value_s = value;
		value_f = value.toFloat();
		value_i = value.toInt();
	}

}cvar_t;

void UTIL_DeclareCvar(QByteArray name, QByteArray def = "0");
cvar_t * UTIL_GetCvar(QByteArray name);

//#define DBG_PRINT(x) system("echo x")

void DBG_PRINT(const char *x);
void DBG_PRINT_S(const QString &s);

float UTIL_FModulo(float x, float y);
template <class T> int UTIL_Sign(T val)
{
	return (T(0) < val) - (val < T(0));
}
void UTIL_UseTargets(cGameRoom *pRoom, const QString &target, cBaseObject *pUser = NULL);
int UTIL_RandomInt(int low, int high_not);
float UTIL_RandomFloat(float low, float high_not);
QVector2D UTIL_LengthDir(float l, float d);
float UTIL_PointDirection(const QVector2D &v1, const QVector2D &v2);
float UTIL_PointDirection(const QPoint &p1, const QVector2D &v2);
float UTIL_PointDistance(const QVector2D &v1, const QVector2D &v2);
int UTIL_keyState(int key);
//template <typename T> T UTIL_Fabs(const T &value);
QVector2D UTIL_mouseCoords(void);
QVector2D UTIL_ProjectVector(const QVector2D &v, const QVector2D &n);

//float UTIL_fAbs(float f);
//template <typename T> T UTIL_Floor(T v);
#endif // UTIL_H
