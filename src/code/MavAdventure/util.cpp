//#define _USE_MATH_DEFINES

#include <cMainManager.h>

#include "util.h"
#include "stdlib.h"
#include "math.h"
#include <QtMath>

#include <QMap>
#include <QVector2D>
#include <QPoint>
#include <QString>
#include "iostream"

#include "cConsole.h"
#include "cGameRoom.h"

#include <QDebug>

using namespace std;

QString g_Language = "RUS";

bool g_BootsUpgrade = false;
float g_fBootsUpgTime = 0.f;


extern QMap<int, quint8> gKeysMap;
extern QVector2D gMouse;
void DBG_PRINT_S(const QString &s)
{
	DBG_PRINT(s.toUtf8().data());
}

void DBG_PRINT(const char *x)
{
	if (g_pCon) g_pCon->PrintLine(QString("DBG: ")+x);
	//system(("echo "+QString(x)).toUtf8());
}

QMap<QByteArray,cvar_t *> g_Cvars;

void UTIL_DeclareCvar(QByteArray name, QByteArray def)
{
	cvar_t *cvar = new cvar_t;
	cvar->value_def = def;
	cvar->value_s = def;
	cvar->value_f = 0.f;
	cvar->value_i = 0.f;
	cvar->value_f = def.toFloat();
	cvar->value_i = def.toInt();
	g_Cvars[name] = cvar;
}
cvar_t * UTIL_GetCvar(QByteArray name)
{
	//qDebug()<<"g_Cvars"<<g_Cvars;
	if (g_Cvars.contains(name)) return g_Cvars[name];
	
	return NULL;
}
//void UTIL_DeclareCmd(QByteArray name, void *pfunc)
//{
	
//}

QVector2D UTIL_ProjectVector(const QVector2D &v, const QVector2D &n)
{
	return n*QVector2D::dotProduct(v,n);
}
void UTIL_UseTargets(cGameRoom *pRoom, const QString &target, cBaseObject *pUser)
{
	qDebug()<<"UTIL_UseTargets with target:"<<target;
	QList<cBaseObject*>targets = pRoom->ObjectsByTargetname(target);
	for(int i=0;i<targets.size();i++)
	{
		qDebug()<<"found"<<target<<", firing";
		targets[i]->Use(pUser);
	}
}

float UTIL_FModulo(float x, float y)
{
	return x - y*floor(x/y);
}
int UTIL_RandomInt(int low, int high_not)
{
	return rand()%(high_not - low)+low;
}
float UTIL_RandomFloat(float low, float high_not)
{
	//TODO: make the real random float
	float r = UTIL_RandomInt(low*100000.f,high_not*100000.f);
	return r/100000.f;
	
//	float r = rand();
//	float dif = high_not - low;
//	while(r > dif) r -= dif;
//	r += low;
//	return r;
}
QVector2D UTIL_LengthDir(float l, float d)
{
	float a = d*M_PI/180.f;
	return QVector2D(l*cos(a),l*sin(a));
}
float UTIL_PointDirection(const QVector2D &v1, const QVector2D &v2)
{
	return 180.f*qAtan2(v2.y()-v1.y(),v2.x()-v1.x())/M_PI;
}
float UTIL_PointDirection(const QPoint &p1, const QVector2D &v2)
{
	return 180.f*qAtan2(v2.y()-p1.y(),v2.x()-p1.x())/M_PI;
}
float UTIL_PointDistance(const QVector2D &v1, const QVector2D &v2)
{
	return (v2-v1).length();
}
int UTIL_keyState(int key)
{
	if (!gKeysMap.keys().contains(key))
	{
		gKeysMap[key] = KEY_ISUP;
		//return 0;
	}
	return gKeysMap[key];
}
QVector2D UTIL_mouseCoords(void)
{
	return gMouse;
}


//template <typename T> T UTIL_Fabs(const T &value)
//{
//    if (value < (T)0) return -value;
//    return value;
//}


//float UTIL_fAbs(float f)
//{
//	if (f < 0) return -f;
//	return f;
//}


//template <typename T> T UTIL_Floor(T v)
//{
//	return (T)(int)v;
//}
