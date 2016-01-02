#ifndef CDLGENTPROPS_H
#define CDLGENTPROPS_H

#include <QDialog>

#include <cBase.h>

namespace Ui {
	class cDlgEntProps;
}

#include <QList>

//struct entity_type_s;
class QTableWidgetItem;
class cBaseEntityTool;

//typedef void (*reassign_ent) (const QString &);

class cDlgEntProps : public QDialog
{
	Q_OBJECT
	
	
public:
	void FillTableFromFields(entity_t *pEnt);
	void FillList(const QStringList &ents_list);
	explicit cDlgEntProps(QWidget *parent = 0);
	~cDlgEntProps();
	
	cBase *m_pEditObj;
	cBaseEntityTool *m_pCallingTool;
	//entity_t *m_pEditEnt;
	
	void ShowWrongSel(const QString &reason);
	void ShowNormal();
	
//private:
	Ui::cDlgEntProps *ui;
	//void SetEntsList(const QStringList &ents_list);
	void SelectObject(cBase *pObj);
	void SetCallingTool(cBaseEntityTool *pTool);
	value_s *ValuePointerByRow(int row);
public slots:
	void OKHit();
	//void CancelHit();
	void entityChanged(QString newname);
	//void itemChanged(QTableWidgetItem *item);
	void itemChanged(QTableWidgetItem *item);
	void DoubleClicked(int y, int x);
protected slots:
	void closeEvent(QCloseEvent *e);
};

extern cDlgEntProps *g_pProps;

#endif // CDLGENTPROPS_H
