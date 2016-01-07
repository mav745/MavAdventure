#ifndef CBASENODE_H
#define CBASENODE_H

#include <QTreeWidgetItem>
#include <QDebug>
#include <util.h>

class QPainter;

enum PARAM_TYPE
{
	EPT_FLOAT,
	EPT_INT,
	EPT_POINTER,
	EPT_UNKNOWN
};

struct param_s
{
	QByteArray value;
	PARAM_TYPE type;
	
	param_s(int val)
	{
		value = QByteArray::number(val);
		type = EPT_INT;
	}
	param_s(float val)
	{
		value = QByteArray::number(val);
		type = EPT_FLOAT;
	}
	param_s(void *val)
	{
		value = QByteArray::number((int)val);
		type = EPT_POINTER;
	}
	
	
	static QByteArray lerp(param_s &p1, param_s &p2, float f)
	{
		if (p1.type != p2.type) return QByteArray("");
		
		if (p1.type == EPT_FLOAT)
			return QByteArray::number(p1.value.toFloat()*(1.f-f)+p2.value.toFloat()*f);
		else if (p1.type == EPT_INT)
			return QByteArray::number((int)(p1.value.toFloat()*(1.f-f)+p2.value.toFloat()*f));
		else if (p1.type == EPT_POINTER)
		{
			if (f < 0.5) return p1.value;
			return p2.value;
		}
	}
};

typedef QVector<param_s *> parmvec_t;

enum NODE_TYPE
{
	ENT_GROUP,
	ENT_STRIP,
	ENT_SOUND
};

class cBaseNode : public QTreeWidgetItem
{
	
public:
	explicit cBaseNode(int type = Type);
	
	NODE_TYPE m_Type;
	
	bool m_State;
	bool m_Visible;
	
	QString m_Name;
	
	QVector<cBaseNode*> m_FileChildren; //for file hierarchy
	
	QVector<cBaseNode*> m_Children; //for coords hierarchy
	
	QMap <QByteArray, parmvec_t *> m_ParmsMap;
	
	static cBaseNode * GetNewNode(void) {return new cBaseNode;}
	static QImage *s_pIconsImg;
	
	void DrawIcon(QPainter *p, QPoint &c);
	bool Select(const QPoint &c);
	
signals:
	
	
public slots:
};



#endif // CBASENODE_H
