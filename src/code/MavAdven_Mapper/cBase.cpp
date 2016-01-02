#include "cGLManager.h"
#include "cBase.h"
#include <QDebug>
#include <QInputDialog>
#include <QColorDialog>
#include <mainwindow.h>
#include <cdlgentprops.h>
#include <QFileDialog>

QMap<QString, entity_type_s *>g_EntTypesMap;

//QVector<value_s*>g_DelValVec;

cBase::cBase(QObject *parent) :
	QObject(parent)
{
	m_Selected = false;
	m_Mins = m_Maxs = QVector2D(0,0);
	m_Flags = 0;
	m_Depth = 0;
	m_pEntity = NULL;
}

bool cBase::RenderMe()
{
//	glColor3f(0.5f,0.5f,0.0f);
//	float x1,y1,x2,y2;
//	x1 = m_Mins.x();
//	y1 = m_Mins.y();
//	x2 = m_Maxs.x();
//	y2 = m_Maxs.y();
//	MGR->DrawLine(x1,y1,x2,y1);
//	MGR->DrawLine(x2,y1,x2,y2);
//	MGR->DrawLine(x2,y2,x1,y2);
//	MGR->DrawLine(x1,y2,x1,y1);
	
	if (m_Maxs.x() < -MGR->m_View.x()		  ) return false;
	if (m_Maxs.y() < -MGR->m_View.y()		  ) return false;
	if (m_Mins.x() > -MGR->m_View.x()+MGR->m_Zoom*MGR->m_ViewSize.x()) return false;
	if (m_Mins.y() > -MGR->m_View.y()+MGR->m_Zoom*MGR->m_ViewSize.y()) return false;
	return true;
}

void cBase::SetEntityType(QString type)
{
	if (!g_EntTypesMap.contains(type)) return;
	if (m_pEntity) delete m_pEntity;
	m_pEntity = new entity_t(*g_EntTypesMap[type]);
}

bool depthCompare(const cBase *o1, const cBase *o2)
{
	return o1->m_Depth > o2->m_Depth;
}

//int operator <(const cBase &o1, const cBase &o2)
//{
//	if (o1.m_Depth < o2.m_Depth) return -1;
//	if (o1.m_Depth > o2.m_Depth) return  1;
//	return 0;
//}


//int operator < (const cBase *o1, const cBase *o2)
//{
//	if (o1->m_Depth < o2->m_Depth) return -1;
//	if (o1->m_Depth > o2->m_Depth) return  1;
//	return 0;
//}


value_s::value_s(stringlist_t items)
{
	for(int i=0;i<items.size();i++)
		itemlist.push_back(items[i]);

	//qDebug()<<itemlist;
	
	data = new QString(items[0]);
	type = EVT_STRINGLIST;
}

value_s::value_s(floatlist_t items)
{
	for(int i=0;i<items.size();i++)
		itemlist.push_back(QString::number(items[0]));//new float(items[i]));
	
	data = new float(items[0]);
	type = EVT_FLOATLIST;
}

value_s::value_s(intlist_t items)
{
	for(int i=0;i<items.size();i++)
		itemlist.push_back(QString::number(items[0]));//new int(items[i]));
	
	data = new int(items[0]);
	type = EVT_INTLIST;
}

value_s::value_s(int i)// : QTableWidgetItem(Type)
{
	data = new int(i);
	type = EVT_INT;
}

value_s::value_s(float f)// : QTableWidgetItem(Type)
{
	data = new float(f);
	type = EVT_FLOAT;
}

value_s::value_s(float x, float y)// : QTableWidgetItem(Type)
{
	//qDebug()<<"new QVector2D";
	data = new QVector2D(x,y);
	type = EVT_VEC2;
}

value_s::value_s(float r, float g, float b)// : QTableWidgetItem(Type)
{
	data = new QVector3D(r,g,b);
	type = EVT_COLOR;
}

value_s::value_s(const QString &str)// : QTableWidgetItem(Type)
{
	data = new QString(str);
	type = EVT_STRING;
}

value_s::value_s(const QByteArray &str)
{
	data = new QString(str);
	type = EVT_FILE;
}

value_s::value_s(bool b)// : QTableWidgetItem(Type)
{
	data = new bool(b);
	type = EVT_BOOL;
}

value_s::value_s(value_s *o)
{
	data = NULL;
	type = o->type;
	fromString(o->asString());
	itemlist = o->itemlist;
}

value_s::~value_s()
{
	//qDebug()<<"deleting"<<data;
//	if (type == EVT_STRINGLIST)
//	{
//		for(int i=itemlist.size()-1;i>=0;i--)
//			delete (QString*)itemlist[i];
//	}
//	else if (type == EVT_INTLIST)
//	{
//		for(int i=itemlist.size()-1;i>=0;i--)
//			delete (int*)itemlist[i];
//	}
//	else if (type == EVT_FLOATLIST)
//	{
//		for(int i=itemlist.size()-1;i>=0;i--)
//			delete (float*)itemlist[i];
//	}
	
	if      (type == EVT_INT   || type == EVT_INTLIST  ) delete (int*  )data;
	else if (type == EVT_FLOAT || type == EVT_FLOATLIST) delete (float*)data;
	else if (type == EVT_VEC2  ) delete (QVector2D*)data;
	else if (type == EVT_COLOR ) delete (QVector3D*)data;
	else if (type == EVT_STRING || type == EVT_FILE || type == EVT_STRINGLIST) delete (QString*)data;
	else if (type == EVT_BOOL  ) delete (bool*)data;
}

QString value_s::asString()
{
	//qDebug()<<"type is"<<type;
	
	if      (type == EVT_INT   || type == EVT_INTLIST  ) return QString().number(*(int*  )data);
	else if (type == EVT_FLOAT || type == EVT_FLOATLIST) return QString().number(*(float*)data);
	else if (type == EVT_VEC2)
	{
		float x,y;
		x = ((QVector2D*)data)->x();
		y = ((QVector2D*)data)->y();
		return QString().number(x)+" "+QString().number(y);
	}
	else if (type == EVT_COLOR)
	{
		float x,y,z;
		x = ((QVector3D*)data)->x();
		y = ((QVector3D*)data)->y();
		z = ((QVector3D*)data)->z();
		return QString().number(x)+" "+QString().number(y)+" "+QString().number(z);
	}
	else if (type == EVT_STRING || type == EVT_FILE || type == EVT_STRINGLIST) 
	{
		QString &str = *(QString*)data;
		if (str.isEmpty()) return "";
		return str;
	}
	else if (type == EVT_BOOL)
	{
		if (*(bool*)data) return "TRUE";
		return "FALSE";
	}
	return "<unknown>";
}

void value_s::fromString(const QString &str)
{
	if (type == EVT_INT || type == EVT_INTLIST)
	{
		if (!data) data = new int(0);
		*(int*)data = str.toInt();
	}
	else if (type == EVT_FLOAT || type == EVT_FLOATLIST)
	{
		if (!data) data = new float(0.f);
		*(float*)data = str.toFloat();
	}
	else if (type == EVT_VEC2)
	{
		if (!data) data = new QVector2D(0,0);
		QVector2D *vec = (QVector2D*)data;
		QStringList parts = str.split(" ",QString::SkipEmptyParts);
		//qDebug()<<"parts"<<parts;
		vec->setX(parts[0].toFloat());
		vec->setY(parts[1].toFloat());
	}
	else if (type == EVT_COLOR)
	{
		if (!data) data = new QVector3D(0,0,0);
		QVector3D *vec = (QVector3D*)data;
		QStringList parts = str.split(" ",QString::SkipEmptyParts);
		vec->setX(parts[0].toFloat());
		vec->setY(parts[1].toFloat());
		vec->setZ(parts[2].toFloat());
	}
	else if (type == EVT_STRING || type == EVT_FILE || type == EVT_STRINGLIST) 
	{
		if (!data) data = new QString("");
		QString &mystr = *(QString*)data;
		mystr = str;
	}
	else if (type == EVT_BOOL)
	{
		if (!data) data = new bool(false);
		if (str == "TRUE" || str == "true" || str == "1")
			(*(bool*)data) = true;
		else
			(*(bool*)data) = false;
	}
}

//void value_s::resetMe()
//{
//	if (type == EVT_INT  || type == EVT_INTLIST)
//	{
//		memset(data,0,sizeof(int));
//	}
//	else if (type == EVT_FLOAT || type == EVT_FLOATLIST)
//	{
//		memset(data,0,sizeof(float));
//	}
//	else if (type == EVT_VEC2)
//	{
//		memset(data,0,sizeof(QVector2D));
//	}
//	else if (type == EVT_COLOR)
//	{
//		memset(data,0,sizeof(QVector3D));
//	}
//	else if (type == EVT_STRING || type == EVT_FLOATLIST) 
//	{
//		QString &mystr = *(QString*)data;
//		mystr.clear();
//	}
//	else if (type == EVT_BOOL)
//	{
//		memset(data,0,sizeof(bool));
//	}
//}

bool value_s::AskValue(const QString &fieldname)
{
	bool handled = true;
	if (type == EVT_INT)
	{
		if (!data) data = new int(0);
		*(int*)data = QInputDialog::getInt(gpMainWND,"INT",fieldname,*(int*)data);
	}
	else if (type == EVT_FLOAT)
	{
		if (!data) data = new float(0.f);
		*(float*)data = QInputDialog::getDouble(gpMainWND,"Float",fieldname,*(float*)data,
															 -2147483647.0, 2147483647.0,3);
	}
	else if (type == EVT_VEC2)
	{
		if (!data) data = new QVector2D(0,0);
		QVector2D *vec = (QVector2D*)data;
		vec->setX(QInputDialog::getDouble(gpMainWND,"Vector2D X",fieldname,vec->x(),
													 -2147483647.0, 2147483647.0,3));
		vec->setY(QInputDialog::getDouble(gpMainWND,"Vector2D Y",fieldname,vec->y(),
													 -2147483647.0, 2147483647.0,3));
	}
	else if (type == EVT_COLOR)
	{
		if (!data) data = new QVector3D(0,0,0);
		QVector3D *vec = (QVector3D*)data;
		//QStringList parts = str.split(" ",QString::SkipEmptyParts);
		QColor color = QColorDialog::getColor(QColor(vec->x(),vec->y(),vec->z()));
		vec->setX(color.red  ());
		vec->setY(color.green());
		vec->setZ(color.blue ());
	}
	else if (type == EVT_STRING) 
	{
		handled = false;
//		if (!data) data = new QString("");
//		QString &mystr = *(QString*)data;
//		mystr = str;
	}
	else if (type == EVT_FILE) 
	{
		QString fname = QFileDialog::getOpenFileName(
					gpMainWND,"Game levels",
					QDir::currentPath(),"*.* (*.*)");
		if (!fname.isEmpty())
		{
			fname.remove(QDir::currentPath());
			//if (!data) data = new QString("");
			QString &mystr = *(QString*)data;
			mystr = fname;
		}
	}
	else if (type == EVT_BOOL)
	{
		bool ok;
		QStringList items;
		items<<"TRUE"<<"FALSE";
		QString sel = QInputDialog::getItem(gpMainWND,"BOOL",fieldname,items,!(*(bool*)data),false,&ok);
		if (ok)
		{
			if (sel == "TRUE")
				*(bool*)data = true;
			else
				*(bool*)data = false;
		}
	}
	else if (type == EVT_STRINGLIST ||
				type == EVT_FLOATLIST ||
				type == EVT_INTLIST)
	{
		bool ok;
		QString sel = QInputDialog::getItem(
							  gpMainWND,"Value List",fieldname,
							  itemlist,itemlist.indexOf(asString()),
							  false,&ok);
		if (ok)
			fromString(sel);
	}
	g_pProps->FillTableFromFields(g_pProps->m_pEditObj->m_pEntity);
	
	return handled;
}
