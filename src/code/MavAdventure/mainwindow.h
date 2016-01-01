#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
protected:
	void keyReleaseEvent(QKeyEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void resizeEvent(QResizeEvent *re);
private:
	Ui::MainWindow *ui;
	
	friend class cMainManager;
};

extern MainWindow *gpMainWND;
extern QMap<QByteArray,QByteArray> g_Args;

#endif // MAINWINDOW_H
