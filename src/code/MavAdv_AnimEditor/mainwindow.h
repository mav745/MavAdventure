#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

#define CLAMP(l,x,r) (x < l ? l : (x > r ? r : x))

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
    static MainWindow * s_WND;
	static bool s_bShift;
	static bool s_bCtrl;
	static bool s_bAlt;
	static bool s_bSpace;
protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
private slots:
	void on_b_StripAdd_clicked();
	
	void on_b_StripGroup_clicked();
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
