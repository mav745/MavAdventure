#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include <stdio.h>
#include <QVector2D>
#include <QVector3D>
#include <QString>

extern FILE *read_buf;
extern FILE *write_buf;

void			READ_SET(FILE *pf);
bool			READ_BOOL();
int			READ_INT();
float			READ_FLOAT();
QVector2D	READ_COORDS();
QVector3D	READ_COLOR();
QString		READ_STRING();
template <typename T> void READ_USER_BUF(T *buf, int elem_count)
{
	fread(buf,sizeof(T),elem_count,read_buf);
}
#define RED_BULL READ_BOOL

#endif // FILEBUFFER_H
