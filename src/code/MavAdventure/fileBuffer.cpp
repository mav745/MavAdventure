#include <fileBuffer.h>
#include <QDebug>

FILE *read_buf = NULL;
FILE *write_buf = NULL;
void READ_SET(FILE *pf)
{
	read_buf = pf;
}

bool READ_BOOL()
{
	bool i;
	fread(&i,sizeof(bool),1,read_buf);
	return i;
}
int READ_INT()
{
	int i;
	fread(&i,4,1,read_buf);
	return i;
}
//template <typename T> void READ_USER_BUF(T *buf, int elem_count)
//{
//	fread(buf,sizeof(T),elem_count,read_buf);
//}

float READ_FLOAT()
{
	float i;
	fread(&i,4,1,read_buf);
	return i;
}
QVector2D READ_COORDS()
{
	float x,y;
	fread(&x,4,1,read_buf);
	fread(&y,4,1,read_buf);
	return QVector2D(x,y);
}
QVector3D READ_COLOR()
{
	float r,g,b;
	fread(&r,4,1,read_buf);
	fread(&g,4,1,read_buf);
	fread(&b,4,1,read_buf);
	return QVector3D(r,g,b);
}
QString READ_STRING()
{
	int sz;
	fread(&sz,4,1,read_buf);
	//qDebug()<<"sz"<<sz;
	char *str = new char[sz+1];
	str[sz] = 0;
	fread(str,1,sz,read_buf);
	QString buf = QString(str);
	delete str;
	return buf;
}
