#ifndef CBASETOOL_H
#define CBASETOOL_H

#include <QObject>

class cBaseTool : public QObject
{
	Q_OBJECT
public:
	explicit cBaseTool(QObject *parent = 0);
	
	QString m_Name;
	
	virtual void DrawMe(void) {}
	
	virtual void ProcessInput(void) {}
	
signals:
	
public slots:
	
};

#endif // CBASETOOL_H
