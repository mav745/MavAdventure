#ifndef CCONSOLE_H
#define CCONSOLE_H

#include <QObject>

class cConsole : public QObject
{
	Q_OBJECT
public:
	explicit cConsole(QObject *parent = 0);
	~cConsole();
	
	char *m_Cmd;
	
	void PrintLine(const QString &s);
	void Print(const QString &s);
signals:
	void HasInput(const QString &cmd);
public slots:
	void AskInput(void);
};

extern cConsole *g_pCon;

#endif // CCONSOLE_H
