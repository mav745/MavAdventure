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
	
//private:
	Ui::MainWindow *ui;
	QRect GetToolFrameRefRect();
private slots:
	void on_butLines_toggled(bool checked);
	void on_butExpand_clicked();
	void on_but_ScrollEf_clicked();
	void on_pushButton_4_clicked();
};

extern MainWindow *gpMainWND;

#endif // MAINWINDOW_H
