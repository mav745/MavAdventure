#ifndef CBASE_H
#define CBASE_H

#include <QObject>
#include <QVector2D>
#include <QString>

#include <QMap>
#include <QVector>

#include <QTableWidgetItem>

#define FL_KILLME (1<<0)

class QDataStream;

struct value_s;
struct entity_type_s;

//extern QVector<value_s *>g_DelValVec;
typedef QMap<QString, entity_type_s *> entities_map_t;
typedef QList<QString> stringlist_t;
typedef QList<float> floatlist_t;
typedef QList<int> intlist_t;

enum VALUE_TYPE
{
	EVT_INT = 0,    //0
	EVT_FLOAT,      //1
	EVT_VEC2,       //2
	EVT_COLOR,      //3
	EVT_STRING,     //4
	EVT_BOOL,       //5
	EVT_FILE,       //6
	EVT_STRINGLIST, //7
	EVT_INTLIST,    //8
	EVT_FLOATLIST,  //9
	
	EVT_MAX
};
class cEntTableValue : public QTableWidgetItem
{
	//Q_OBJECT
};

struct value_s// : public QTableWidgetItem
{
	//Q_OBJECT
//public:
	//explicit QTableWidgetItem(int type = Type);
	//explicit QTableWidgetItem(const QString &text, int type = Type);
	//explicit QTableWidgetItem(const QIcon &icon, const QString &text, int type = Type);
	//QTableWidgetItem(const QTableWidgetItem &other);
	value_s(stringlist_t items);
	value_s(floatlist_t items);
	value_s(intlist_t items);
	
	value_s(int i);
	value_s(float f);
	value_s(float x, float y);
	value_s(float r, float g, float b);
	value_s(const QString &str);
	value_s(const QByteArray &str);
	value_s(bool b);
	
	value_s(value_s *o);
	
	~value_s();
	
	QString asString();
	void fromString(const QString &str);
	//void resetMe();
	bool AskValue(const QString &fieldname);
	
	VALUE_TYPE type;
	void *data;
	
	QStringList itemlist;
};

typedef struct entity_type_s
{
	entity_type_s() {hidden=false;}
	entity_type_s(const entity_type_s *o)
	{
		hidden=false;
		QMap<QString,value_s*>::const_iterator it;
		for(it=o->fields.begin();it!=o->fields.end();it++)
			fields[it.key()] = new value_s(it.value());
	}
	~entity_type_s()
	{
		QMap<QString,value_s*>::iterator it;
		for(it=fields.begin();it!=fields.end();it++)
			delete it.value();
	}
	QString type_name;
	bool hidden;
	QMap<QString,value_s*>fields;
} entity_t;

class cBase : public QObject
{
	Q_OBJECT
public:
	explicit cBase(QObject *parent = 0);
	
	entity_t *m_pEntity;
	
	int m_Flags;
	
	QVector2D m_Mins;
	QVector2D m_Maxs;
	
	QString m_Classname;
	
	bool m_Selected;
	
	int m_Depth;
	
	virtual bool RenderMe(void);
	virtual void SaveMe(QDataStream *DS){};
	virtual void LoadMe(QDataStream *DS){};
	virtual void SetEntityType(QString type);
signals:
	
public slots:
	
};

bool depthCompare(const cBase *o1, const cBase *o2);
//int operator < (const cBase &o1, const cBase &o2);
//int operator < (const cBase *o1, const cBase *o2);

#endif // CBASE_H
