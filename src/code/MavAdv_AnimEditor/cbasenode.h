#ifndef CBASENODE_H
#define CBASENODE_H

#include <QObject>
#include <QDebug>
#include <util.h>

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

class cBaseNode : public QObject
{
	Q_OBJECT
	
public:
	explicit cBaseNode(QObject *parent = 0);
	
	QVector<cBaseNode*> m_Children;
	QMap <QByteArray, parmvec_t *> m_ParmsMap;
signals:
	
public slots:
};



#endif // CBASENODE_H
