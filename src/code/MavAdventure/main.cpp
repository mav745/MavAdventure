#include <windows.h>
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

QMap<QByteArray,QByteArray> g_Args;
int main(int argc, char *argv[])
{
	QByteArray args;
	for(int i=1;i<argc;i++)
	{
		//qDebug()<<argv[i];
		args.append(argv[i]);
		args += ' ';
		//args += argv[i]+' ';
	}
	args.resize(args.size() -1);
	//qDebug()<<"args:"<<args;
	QList<QByteArray> parts = args.split('-');
	//qDebug()<<"parts:"<<parts;
	for(int i=0;i<parts.size();i++)
	{
		if (parts[i].isEmpty())
			continue;
		
		QList<QByteArray>arg_value = parts[i].split(' ');
		if (arg_value.size())
		{
			if (arg_value.size() == 1)
				g_Args[arg_value[0]] = "NULL";
			else //>1
			{
				QByteArray val = arg_value[1];
				for(int j=2;j<arg_value.size();j++)
					val += ' ' + arg_value[j];
				
				g_Args[arg_value[0]] = val;
			}
		}
	}
	//qDebug()<<"g_Args:"<<g_Args;
	
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	
	return a.exec();
}
