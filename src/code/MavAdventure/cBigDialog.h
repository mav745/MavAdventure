#ifndef CBIGDIALOG_H
#define CBIGDIALOG_H

#include <cBaseObject.h>

#include <QObject>

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QQueue>
#include <QTimer>

struct dialog_part_s
{
	QByteArray text;
	QByteArray talker_name;
	QString talker_texname;
	int talker_tex;
	bool autoadv;
	bool draw_left;
	//bool fastforward;
};

class cBigDialog : public QObject
{
	Q_OBJECT
public:
	static int s_MissingTex;
	static QMap<QByteArray,QByteArray> s_NamesLocale;
	
	QTimer m_AdvTimer;
	
	explicit cBigDialog(QObject *parent = 0);
	
	QVector<dialog_part_s *>m_DialogParts;
	QVector<QByteArray>m_Lines; //stack up said lines here
	QByteArray m_CurLine; //current printing line
	QByteArray m_TalkerName;
	
	int  m_TalkerTex;
	bool m_Left;
	bool m_Auto;
	bool m_ClearUp;
	int  m_DrawY;
	bool m_Visible;
	bool m_ToAdvance;
	dialog_part_s *m_CurDlgPart;
	QByteArray m_CurWord;
	void AddDialog(int talker_tex, 
			QByteArray name, 
			QByteArray text, 
			bool autoadvance = false, 
			bool start_now   = false,
			bool draw_left   = false);
	void RenderDialog();
	
	void AddDialog(
			QString talkertex_fname, 
			QByteArray name,
			QByteArray text, 
			bool autoadvance = false, 
			bool start_now   = false, 
			bool draw_left   = false);
	bool IsSkipping(dialog_part_s *part);
signals:
	void PhraseStarts();
	void PhraseStops();
	void PhraseEnds();
	void DialogEnds();
	
public slots:
	void AdvanceDialog();
	//void TestSlot(float f1) {}
	void KeyPressed(int key);
	void KeyReleased(int key);
};

extern cBigDialog *g_pBigDialog;

class cInfoDialog : public cBaseObject
{
	Q_OBJECT
public:
	explicit cInfoDialog(QObject *parent = 0);
	
	QVector<dialog_part_s>m_DialogParts;
	
	void ParseKeyValue(const QString &key, const QString &value);
	void Use(cBaseObject *pOther);
public slots:
	void DialogDone(void);
};

#endif // CBIGDIALOG_H
