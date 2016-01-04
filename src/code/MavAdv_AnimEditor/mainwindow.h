#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}


#define	KS_UP (1<<0)
#define	KS_PRESSED (1<<1)
#define	KS_HOLD (1<<2)
#define	KS_RELEASED (1<<3)
#define	KS_DOWN KS_HOLD

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	static Ui::MainWindow * GetUi(void);
	static int KeyState(int key);
	void ProcessInput();
protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
private:
	static MainWindow * s_WND;
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
