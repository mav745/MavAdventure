#include "cConsole.h"
#include "cMainManager.h"
#include "cGameRoom.h"
#include "util.h"
#include <windows.h>
#include <iostream>
#include <QStringList>
#include <qthread.h>
#include "QTimer"
#include <QApplication>

using namespace std;

cConsole *g_pCon;
extern QThread *g_pConThread;

QTimer *g_pConInputTimer = new QTimer;


cConsole::cConsole(QObject *parent) : QObject(parent)
{
	AllocConsole();
	freopen("CONIN$" , "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
	if (g_pCon) delete g_pCon; //fake singleton
	
	g_pCon = this;
	
	
	connect(g_pConInputTimer,SIGNAL(timeout()),this,SLOT(AskInput()));
	g_pConInputTimer->setSingleShot(false);
	g_pConInputTimer->start(100);
	m_Cmd = NULL;
}

void cConsole::PrintLine(const QString &s)
{
	Print(s);
	cout<<endl;
}
void cConsole::Print(const QString &s)
{
	cout<<s.toUtf8().data();
}

void cConsole::AskInput()
{
	cout<<"]";
	
	string cmd;
	getline(cin,cmd);
	
	if (m_Cmd) delete [] m_Cmd;
	m_Cmd = new char[cmd.size()+1];
	memset(m_Cmd,0,cmd.size()+1);
	memcpy(m_Cmd,cmd.data(),cmd.size());

	emit HasInput(QString(m_Cmd));
}


cConsole::~cConsole()
{
	
}

