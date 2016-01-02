#ifndef CBACKGROUND_H
#define CBACKGROUND_H

#include "cBase.h"
#include <QString>

//#include <QImage>

class QImage;

class cBackground : public cBase
{
	Q_OBJECT
public:
	explicit cBackground(QObject *parent = 0);
	
	bool m_Visible;
	
	QImage *m_BackImg;
	
	QString m_Filename;
	QVector2D m_Origin;
	QVector2D m_ScrFac;
	QVector2D m_ScrSpd;
	bool m_HRep;
	bool m_VRep;
	int  m_Texture;
	int  m_Width ;
	int  m_Height;
	
	virtual bool RenderMe(void);
	void SaveMe(QDataStream *DS);
	void LoadMe(QDataStream *DS);
	void AssignBackground(QString fname);
signals:
	
public slots:
	
};

#endif // CBACKGROUND_H
