#ifndef CBIGDIALOG_H
#define CBIGDIALOG_H

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QQueue>
#include <QTimer>

struct dialog_part_s
{
	QString text;
	int  talker_tex;
	bool autoadv;
};

class cBigDialog : public QObject
{
	Q_OBJECT
public:
	QTimer m_AdvTimer;
	
	explicit cBigDialog(QObject *parent = 0);
	
	QQueue<dialog_part_s *>m_DialogParts;
	QVector<QString>m_Lines; //stack up said lines here
	QString m_CurLine; //current printing line
	
	//bool m_Auto;
	int m_DrawY;
	bool m_Visible;
	
	void AddDialog(int talker_tex, QString text, bool autoadvance = false, bool start_now = false);
	void RenderDialog();
	
public slots:
	void AdvanceDialog();
};

#endif // CBIGDIALOG_H
